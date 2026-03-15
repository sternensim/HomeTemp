#include "rtp_sender.h"

#include "esphome/core/log.h"
#include "esphome/core/hal.h"       // millis()
#include "esphome/core/helpers.h"   // random_uint32()

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>

// lwIP on ESP-IDF provides the POSIX socket API
#include <lwip/sockets.h>
#include <lwip/netdb.h>

namespace esphome {
namespace rtp_sender {

static const char *const TAG = "rtp_sender";

// ─── Component lifecycle ──────────────────────────────────────────────────────

void RtpSender::setup() {
  ssrc_ = random_uint32();

  // Register data callback. The microphone calls this from the I2S task
  // every time a chunk of audio is ready.
  mic_->add_data_callback([this](const std::vector<uint8_t> &data) {
    this->on_mic_data_(data);
  });

  // Keep the microphone running continuously even when voice_assistant is idle.
  // ESPHome's I2S microphone uses a start-count, so this is safe alongside
  // voice_assistant which also calls start()/stop().
  mic_->start();

  ESP_LOGI(TAG, "RTP sender ready → rtsp://%s:%u%s  %u Hz mono",
           host_.c_str(), port_, path_.c_str(), sample_rate_);
}

void RtpSender::dump_config() {
  ESP_LOGCONFIG(TAG, "RTP Sender (RTSP publish):");
  ESP_LOGCONFIG(TAG, "  Target : rtsp://%s:%u%s", host_.c_str(), port_, path_.c_str());
  ESP_LOGCONFIG(TAG, "  Format : L16 / %u Hz / mono", sample_rate_);
}

void RtpSender::loop() {
  RtpState st = state_.load();
  if (st != RtpState::IDLE) return;

  uint32_t now = millis();
  if (now - last_attempt_ms_ < RETRY_MS) return;
  last_attempt_ms_ = now;

  state_.store(RtpState::CONNECTING);

  if (rtsp_connect_() && rtsp_announce_() && rtsp_setup_() && rtsp_record_()) {
    state_.store(RtpState::RECORDING);
    ESP_LOGI(TAG, "RTSP publish active");
  } else {
    disconnect_();
  }
}

// ─── Public control ───────────────────────────────────────────────────────────

void RtpSender::stop_streaming() {
  ESP_LOGI(TAG, "Stopping RTP stream");
  disconnect_();
  mic_->stop();
  state_.store(RtpState::STOPPED);
}

void RtpSender::start_streaming() {
  ESP_LOGI(TAG, "Starting RTP stream");
  mic_->start();
  state_.store(RtpState::IDLE);    // loop() will initiate RTSP handshake
  last_attempt_ms_ = 0;            // connect immediately
}

// ─── RTSP handshake ───────────────────────────────────────────────────────────

bool RtpSender::rtsp_connect_() {
  sock_.store(-1);

  int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd < 0) {
    ESP_LOGE(TAG, "socket() failed errno=%d", errno);
    return false;
  }

  // Resolve hostname (mediamtx service name on the Docker network)
  struct hostent *he = ::gethostbyname(host_.c_str());
  if (!he) {
    ESP_LOGE(TAG, "DNS failed for '%s'", host_.c_str());
    ::close(fd);
    return false;
  }

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port_);
  ::memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

  // Non-blocking connect with timeout to avoid freezing the main loop
  int flags = ::fcntl(fd, F_GETFL, 0);
  ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  int ret = ::connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  if (ret < 0 && errno != EINPROGRESS) {
    ESP_LOGE(TAG, "connect() failed errno=%d", errno);
    ::close(fd);
    return false;
  }

  if (ret != 0) {
    // Wait for connection with timeout using select()
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(fd, &wset);
    struct timeval tv;
    tv.tv_sec  = CONNECT_TIMEOUT_MS / 1000;
    tv.tv_usec = (CONNECT_TIMEOUT_MS % 1000) * 1000;

    int sel = ::select(fd + 1, nullptr, &wset, nullptr, &tv);
    if (sel <= 0) {
      ESP_LOGE(TAG, "connect() timed out after %u ms", CONNECT_TIMEOUT_MS);
      ::close(fd);
      return false;
    }

    // Check for connect error
    int err = 0;
    socklen_t len = sizeof(err);
    ::getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (err != 0) {
      ESP_LOGE(TAG, "connect() async error: %d", err);
      ::close(fd);
      return false;
    }
  }

  // Switch back to blocking for RTSP handshake
  ::fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

  sock_.store(fd);
  ESP_LOGD(TAG, "TCP connected to %s:%u", host_.c_str(), port_);
  return true;
}

std::string RtpSender::build_sdp_() {
  char buf[320];
  // Dynamic payload type 96 with explicit rtpmap so mediamtx knows the
  // actual sample rate (48 000 Hz) and channel count (mono = 1).
  ::snprintf(buf, sizeof(buf),
    "v=0\r\n"
    "o=- %lu %lu IN IP4 0.0.0.0\r\n"
    "s=BirdMic\r\n"
    "c=IN IP4 0.0.0.0\r\n"
    "t=0 0\r\n"
    "m=audio 0 RTP/AVP 96\r\n"
    "a=rtpmap:96 L16/%u/1\r\n"
    "a=control:trackID=0\r\n",
    static_cast<unsigned long>(ssrc_),
    static_cast<unsigned long>(ssrc_),
    sample_rate_);
  return std::string(buf);
}

bool RtpSender::rtsp_announce_() {
  std::string sdp = build_sdp_();
  char msg[768];
  ::snprintf(msg, sizeof(msg),
    "ANNOUNCE rtsp://%s:%u%s RTSP/1.0\r\n"
    "CSeq: %u\r\n"
    "Content-Type: application/sdp\r\n"
    "Content-Length: %u\r\n"
    "\r\n"
    "%s",
    host_.c_str(), port_, path_.c_str(),
    cseq_++,
    static_cast<unsigned>(sdp.size()),
    sdp.c_str());

  std::string response;
  return tcp_send_blocking_(msg, ::strlen(msg)) && rtsp_read_response_(response);
}

bool RtpSender::rtsp_setup_() {
  char msg[256];
  ::snprintf(msg, sizeof(msg),
    "SETUP rtsp://%s:%u%s/trackID=0 RTSP/1.0\r\n"
    "CSeq: %u\r\n"
    "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n"
    "\r\n",
    host_.c_str(), port_, path_.c_str(),
    cseq_++);

  std::string response;
  if (!tcp_send_blocking_(msg, ::strlen(msg)) || !rtsp_read_response_(response))
    return false;

  // Extract Session header from SETUP response — required for RECORD
  session_id_ = parse_session_(response);
  if (session_id_.empty()) {
    ESP_LOGE(TAG, "SETUP response missing Session header");
    return false;
  }
  ESP_LOGD(TAG, "RTSP Session: %s", session_id_.c_str());
  return true;
}

bool RtpSender::rtsp_record_() {
  char msg[320];
  ::snprintf(msg, sizeof(msg),
    "RECORD rtsp://%s:%u%s RTSP/1.0\r\n"
    "CSeq: %u\r\n"
    "Session: %s\r\n"
    "Range: npt=0.000-\r\n"
    "\r\n",
    host_.c_str(), port_, path_.c_str(),
    cseq_++,
    session_id_.c_str());

  std::string response;
  if (!tcp_send_blocking_(msg, ::strlen(msg)) || !rtsp_read_response_(response))
    return false;

  // Switch socket to non-blocking so audio sends never stall the main loop
  int fd = sock_.load();
  if (fd >= 0) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }
  return true;
}

bool RtpSender::tcp_send_blocking_(const char *data, size_t len) {
  int fd = sock_.load();
  if (fd < 0) return false;

  // Ensure socket is blocking for RTSP control messages
  int flags = ::fcntl(fd, F_GETFL, 0);
  ::fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

  size_t sent = 0;
  while (sent < len) {
    int n = ::send(fd, data + sent, len - sent, 0);
    if (n <= 0) {
      ESP_LOGE(TAG, "tcp_send_ error errno=%d", errno);
      return false;
    }
    sent += n;
  }
  return true;
}

bool RtpSender::rtsp_read_response_(std::string &response) {
  int fd = sock_.load();
  if (fd < 0) return false;

  // Apply a 5-second receive timeout for control messages
  struct timeval tv = {5, 0};
  ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  char buf[512] = {};
  int n = ::recv(fd, buf, sizeof(buf) - 1, 0);
  if (n <= 0) {
    ESP_LOGE(TAG, "rtsp_read_response_: no response (errno=%d)", errno);
    return false;
  }
  buf[n] = '\0';

  if (::strncmp(buf, "RTSP/1.0 200", 12) != 0) {
    ESP_LOGE(TAG, "RTSP unexpected response: %.120s", buf);
    return false;
  }

  response.assign(buf, n);
  return true;
}

std::string RtpSender::parse_session_(const std::string &response) {
  // Look for "Session: <id>" header. The id may be followed by ";timeout=..."
  const char *key = "Session: ";
  size_t pos = response.find(key);
  if (pos == std::string::npos) {
    // Try case-insensitive (some servers use "Session:" with varying case)
    key = "session: ";
    pos = response.find(key);
  }
  if (pos == std::string::npos) return {};

  size_t start = pos + ::strlen(key);
  size_t end = response.find_first_of(";\r\n", start);
  if (end == std::string::npos) end = response.size();
  return response.substr(start, end - start);
}

// ─── Audio path ───────────────────────────────────────────────────────────────
//
// Called from the ESPHome I2S audio task (not the main loop task).
// Keep allocations off the heap — use the static buffers in the header.

void RtpSender::on_mic_data_(const std::vector<uint8_t> &data) {
  // Snapshot shared state into locals for thread safety.
  // If disconnect_() runs concurrently, we may send on a stale fd —
  // that produces EBADF which triggers the reconnect path safely.
  if (state_.load() != RtpState::RECORDING) return;
  int fd = sock_.load();
  if (fd < 0) return;

  // I2S delivers 32-bit frames (ICS-43434: 24-bit audio in the upper 24 bits,
  // zero-padded in the lower 8 bits, signed 2's complement, little-endian).
  // Convert to signed 16-bit by taking the upper 16 bits of each 32-bit word.
  size_t n = data.size() / 4;
  if (n == 0 || n > MAX_SAMPLES) return;  // guard against oversized chunks

  const int32_t *src = reinterpret_cast<const int32_t *>(data.data());
  for (size_t i = 0; i < n; i++) {
    pcm_buf_[i] = static_cast<int16_t>(src[i] >> 16);
  }

  send_rtp_(pcm_buf_, n, fd);
}

void RtpSender::send_rtp_(const int16_t *samples, size_t n, int fd) {
  // Packet layout (all big-endian):
  //   [0]    '$'          RTSP interleave marker
  //   [1]    0            channel 0 = RTP data
  //   [2-3]  length       RTP packet length in bytes (12 header + payload)
  //   [4]    0x80         RTP: V=2, P=0, X=0, CC=0
  //   [5]    96           RTP: M=0, PT=96 (dynamic L16/sample_rate_/1)
  //   [6-7]  seq          RTP sequence number
  //   [8-11] timestamp    RTP timestamp (increments by sample count)
  //   [12-15] ssrc        Sync source ID
  //   [16+]  payload      Big-endian signed 16-bit PCM samples

  size_t payload_len = n * 2;
  size_t rtp_len     = 12 + payload_len;
  size_t frame_len   = 4  + rtp_len;

  if (frame_len > RTP_BUF_SIZE) return;  // should never happen given MAX_SAMPLES

  // RTSP interleave header
  rtp_buf_[0] = '$';
  rtp_buf_[1] = 0;
  rtp_buf_[2] = static_cast<uint8_t>(rtp_len >> 8);
  rtp_buf_[3] = static_cast<uint8_t>(rtp_len & 0xFF);

  // RTP fixed header
  rtp_buf_[4] = 0x80;
  rtp_buf_[5] = 96;
  rtp_buf_[6] = static_cast<uint8_t>(rtp_seq_ >> 8);
  rtp_buf_[7] = static_cast<uint8_t>(rtp_seq_ & 0xFF);

  uint32_t ts_be = __builtin_bswap32(rtp_ts_);
  ::memcpy(rtp_buf_ + 8, &ts_be, 4);

  uint32_t ssrc_be = __builtin_bswap32(ssrc_);
  ::memcpy(rtp_buf_ + 12, &ssrc_be, 4);

  // Payload: host-endian int16 → big-endian bytes
  uint8_t *dst = rtp_buf_ + 16;
  for (size_t i = 0; i < n; i++) {
    int16_t s = samples[i];
    dst[i * 2]     = static_cast<uint8_t>(s >> 8);
    dst[i * 2 + 1] = static_cast<uint8_t>(s & 0xFF);
  }

  // Non-blocking send — drop the packet silently if the TCP buffer is full.
  // A dropped audio packet is harmless for BirdNET-Go (it buffers many frames).
  int sent = ::send(fd, rtp_buf_, frame_len, MSG_DONTWAIT);
  if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
    // Real error (including EBADF from race) — signal main loop to reconnect
    ESP_LOGW(TAG, "RTP send error errno=%d, will reconnect", errno);
    state_.store(RtpState::IDLE);
  }

  rtp_seq_++;
  rtp_ts_ += static_cast<uint32_t>(n);  // clock ticks = samples (rate = sample_rate_)
}

void RtpSender::disconnect_() {
  int fd = sock_.exchange(-1);   // atomically grab and invalidate
  if (fd >= 0) {
    ::close(fd);
  }
  state_.store(RtpState::IDLE);
  cseq_  = 1;
  session_id_.clear();
  ESP_LOGW(TAG, "RTSP disconnected — retrying in %u s", RETRY_MS / 1000);
}

}  // namespace rtp_sender
}  // namespace esphome
