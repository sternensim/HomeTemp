#include "rtp_sender.h"

#include "esphome/core/log.h"
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
  if (state_ != RtpState::IDLE) return;

  uint32_t now = millis();
  if (now - last_attempt_ms_ < RETRY_MS) return;
  last_attempt_ms_ = now;

  state_ = RtpState::CONNECTING;

  if (rtsp_connect_() && rtsp_announce_() && rtsp_setup_() && rtsp_record_()) {
    state_ = RtpState::RECORDING;
    ESP_LOGI(TAG, "RTSP publish active");
  } else {
    disconnect_();
  }
}

// ─── RTSP handshake ───────────────────────────────────────────────────────────

bool RtpSender::rtsp_connect_() {
  sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock_ < 0) {
    ESP_LOGE(TAG, "socket() failed errno=%d", errno);
    return false;
  }

  // Resolve hostname (mediamtx service name on the Docker network)
  struct hostent *he = ::gethostbyname(host_.c_str());
  if (!he) {
    ESP_LOGE(TAG, "DNS failed for '%s'", host_.c_str());
    ::close(sock_);
    sock_ = -1;
    return false;
  }

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port_);
  ::memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

  // Blocking connect (timeout via SO_RCVTIMEO on the subsequent reads)
  if (::connect(sock_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    ESP_LOGE(TAG, "connect() failed errno=%d", errno);
    ::close(sock_);
    sock_ = -1;
    return false;
  }

  // After handshake we switch to non-blocking for the audio path
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

  return tcp_send_blocking_(msg, ::strlen(msg)) && rtsp_read_ok_();
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

  return tcp_send_blocking_(msg, ::strlen(msg)) && rtsp_read_ok_();
}

bool RtpSender::rtsp_record_() {
  char msg[192];
  ::snprintf(msg, sizeof(msg),
    "RECORD rtsp://%s:%u%s RTSP/1.0\r\n"
    "CSeq: %u\r\n"
    "\r\n",
    host_.c_str(), port_, path_.c_str(),
    cseq_++);

  if (!tcp_send_blocking_(msg, ::strlen(msg)) || !rtsp_read_ok_())
    return false;

  // Switch socket to non-blocking so audio sends never stall the main loop
  int flags = ::fcntl(sock_, F_GETFL, 0);
  ::fcntl(sock_, F_SETFL, flags | O_NONBLOCK);
  return true;
}

bool RtpSender::tcp_send_blocking_(const char *data, size_t len) {
  // Ensure socket is blocking for RTSP control messages
  int flags = ::fcntl(sock_, F_GETFL, 0);
  ::fcntl(sock_, F_SETFL, flags & ~O_NONBLOCK);

  size_t sent = 0;
  while (sent < len) {
    int n = ::send(sock_, data + sent, len - sent, 0);
    if (n <= 0) {
      ESP_LOGE(TAG, "tcp_send_ error errno=%d", errno);
      return false;
    }
    sent += n;
  }
  return true;
}

bool RtpSender::rtsp_read_ok_() {
  // Apply a 5-second receive timeout for control messages
  struct timeval tv = {5, 0};
  ::setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  char buf[512] = {};
  int n = ::recv(sock_, buf, sizeof(buf) - 1, 0);
  if (n <= 0) {
    ESP_LOGE(TAG, "rtsp_read_ok_: no response (errno=%d)", errno);
    return false;
  }

  if (::strncmp(buf, "RTSP/1.0 200", 12) != 0) {
    buf[n] = '\0';
    ESP_LOGE(TAG, "RTSP unexpected response: %.120s", buf);
    return false;
  }
  return true;
}

// ─── Audio path ───────────────────────────────────────────────────────────────
//
// Called from the ESPHome I2S audio task (not the main loop task).
// Keep allocations off the heap — use the static buffers in the header.

void RtpSender::on_mic_data_(const std::vector<uint8_t> &data) {
  if (state_ != RtpState::RECORDING || sock_ < 0) return;

  // I2S delivers 32-bit frames (ICS-43434: 24-bit audio in the upper 24 bits,
  // zero-padded in the lower 8 bits, signed 2's complement, little-endian).
  // Convert to signed 16-bit by taking the upper 16 bits of each 32-bit word.
  size_t n = data.size() / 4;
  if (n == 0 || n > MAX_SAMPLES) return;  // guard against oversized chunks

  const int32_t *src = reinterpret_cast<const int32_t *>(data.data());
  for (size_t i = 0; i < n; i++) {
    pcm_buf_[i] = static_cast<int16_t>(src[i] >> 16);
  }

  send_rtp_(pcm_buf_, n);
}

void RtpSender::send_rtp_(const int16_t *samples, size_t n) {
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
  int sent = ::send(sock_, rtp_buf_, frame_len, MSG_DONTWAIT);
  if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
    // Real error — signal the main loop to reconnect
    ESP_LOGW(TAG, "RTP send error errno=%d, will reconnect", errno);
    state_ = RtpState::IDLE;
    ::close(sock_);
    sock_ = -1;
  }

  rtp_seq_++;
  rtp_ts_ += static_cast<uint32_t>(n);  // clock ticks = samples (rate = sample_rate_)
}

void RtpSender::disconnect_() {
  if (sock_ >= 0) {
    ::close(sock_);
    sock_ = -1;
  }
  state_ = RtpState::IDLE;
  cseq_  = 1;
  ESP_LOGW(TAG, "RTSP disconnected — retrying in %u s", RETRY_MS / 1000);
}

}  // namespace rtp_sender
}  // namespace esphome
