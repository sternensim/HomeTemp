#pragma once

#include "esphome/core/component.h"
#include "esphome/components/microphone/microphone.h"

#include <atomic>
#include <string>
#include <cstdint>

namespace esphome {
namespace rtp_sender {

// Maximum samples the I2S callback can deliver per chunk.
// 1024 covers the largest typical ESPHome DMA buffer.
static constexpr size_t MAX_SAMPLES = 1024;

// Maximum samples per RTP packet — keeps the RTP packet ≤ 1440 bytes
// (mediamtx limit).  700 samples × 2 bytes = 1400 payload + 12 header = 1412.
static constexpr size_t RTP_MAX_SAMPLES = 700;

// RTSP interleave header (4) + RTP fixed header (12) + payload
static constexpr size_t RTP_BUF_SIZE = 4 + 12 + RTP_MAX_SAMPLES * 2;

enum class RtpState : uint8_t {
  IDLE,        // waiting / retry back-off
  CONNECTING,  // TCP + RTSP handshake in progress
  RECORDING,   // actively sending RTP frames
  STOPPED,     // explicitly stopped (thermal protection / user request)
};

class RtpSender : public Component {
 public:
  // Called by generated code (from __init__.py)
  void set_microphone(microphone::Microphone *mic) { mic_ = mic; }
  void set_host(const std::string &host) { host_ = host; }
  void set_port(uint16_t port) { port_ = port; }
  void set_path(const std::string &path) { path_ = path; }
  void set_sample_rate(uint32_t rate) { sample_rate_ = rate; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  // Public control — callable from YAML lambdas via id(rtp_sender_id)
  void stop_streaming();
  void start_streaming();

 protected:
  // Config
  microphone::Microphone *mic_{nullptr};
  std::string host_;
  uint16_t port_{8554};
  std::string path_{"/bird"};
  uint32_t sample_rate_{48000};

  // TCP socket + state (accessed from I2S task and main loop)
  std::atomic<int> sock_{-1};
  std::atomic<RtpState> state_{RtpState::IDLE};
  uint32_t last_attempt_ms_{0};
  static constexpr uint32_t RETRY_MS = 15000;
  static constexpr uint32_t CONNECT_TIMEOUT_MS = 5000;

  // RTSP sequencing
  uint16_t cseq_{1};
  std::string session_id_;   // parsed from SETUP response

  // RTP sequencing
  uint16_t rtp_seq_{0};
  uint32_t rtp_ts_{0};
  uint32_t ssrc_{0};
  uint32_t rtp_packet_count_{0};
  uint32_t rtp_octet_count_{0};

  // RTCP Sender Report — keeps the mediamtx session alive
  uint32_t last_rtcp_ms_{0};
  static constexpr uint32_t RTCP_INTERVAL_MS = 5000;

  // Static buffers — no heap allocation in the audio path
  int16_t  pcm_buf_[MAX_SAMPLES];
  uint8_t  rtp_buf_[RTP_BUF_SIZE];

  // Partial-write tracking — if send() can't push the entire RTP frame
  // in one call, we save the offset and flush the remainder on the next
  // callback before building a new packet.
  size_t pending_offset_{0};
  size_t pending_len_{0};

  // RTSP handshake steps
  bool rtsp_connect_();
  bool rtsp_announce_();
  bool rtsp_setup_();
  bool rtsp_record_();
  bool tcp_send_blocking_(const char *data, size_t len);
  bool rtsp_read_response_(std::string &response);
  std::string build_sdp_();
  std::string parse_session_(const std::string &response);

  // Audio pipeline
  void on_mic_data_(const std::vector<uint8_t> &data);
  void send_rtp_(const int16_t *samples, size_t n, int fd);
  void send_rtcp_sr_();
  void disconnect_();
};

}  // namespace rtp_sender
}  // namespace esphome
