#pragma once

#include "esphome/core/component.h"
#include "esphome/components/microphone/microphone.h"

#include <atomic>
#include <string>
#include <cstdint>

namespace esphome {
namespace rtp_sender {

// Maximum samples per audio callback chunk (512 is typical for ESPHome I2S).
// Sized for 1024 to be safe. At 48 kHz that is ~21 ms per packet.
static constexpr size_t MAX_SAMPLES = 1024;

// RTSP interleave header (4) + RTP fixed header (12) + payload (MAX_SAMPLES * 2 bytes)
static constexpr size_t RTP_BUF_SIZE = 4 + 12 + MAX_SAMPLES * 2;

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

  // Static buffers — no heap allocation in the audio path
  int16_t  pcm_buf_[MAX_SAMPLES];
  uint8_t  rtp_buf_[RTP_BUF_SIZE];

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
  void disconnect_();
};

}  // namespace rtp_sender
}  // namespace esphome
