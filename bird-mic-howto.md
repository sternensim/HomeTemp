# Bird Microphone — Setup Guide
### ESP32 + ICS-43434 → ESPHome → mediamtx → BirdNET-Go (all via Docker Compose)

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│  docker-compose (same host)                                  │
│                                                             │
│  ┌──────────────┐  native API  ┌──────────────────────┐    │
│  │   ESPHome    │◄────────────►│  Home Assistant      │    │
│  │  container   │              │  container (8123)    │    │
│  └──────────────┘              └──────────────────────┘    │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  ESP32 (bird-mic)                                    │   │
│  │  voice_assistant ──────────────────────► HA Assist  │   │
│  │  UDP/RTP sender ──────────────────────► mediamtx    │   │
│  └─────────────────────────────────────────────────────┘   │
│                          │                                   │
│                    UDP :5004                                 │
│                          ▼                                   │
│  ┌──────────────────────────┐  RTSP stream                  │
│  │  mediamtx container      │─────────────────────────┐    │
│  │  rtsp://.../bird (8554)  │                          │    │
│  └──────────────────────────┘                          ▼    │
│                                         ┌─────────────────┐ │
│                                         │  BirdNET-Go     │ │
│                                         │  container(8080)│ │
│                                         └─────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

**Two audio paths from the ESP32:**
- **voice_assistant → HA**: for voice commands and HA dashboard sensors (already in the ESPHome YAML)
- **UDP/RTP → mediamtx → BirdNET-Go**: dedicated bird identification stream (configured below)

---

## 1. Hardware

### Parts
| Part | Notes |
|---|---|
| ESP32-WROOM / ESP32-DevKit | Any generic ESP32 board |
| ICS-43434 I2S microphone breakout | ~3 € on AliExpress/LCSC |

### Wiring

```
ICS-43434     ESP32
─────────     ─────────────
BCLK    →     GPIO26
WS      →     GPIO25
DOUT    →     GPIO34   ← input-only pin, ideal for mic data
VDD     →     3V3
GND     →     GND
L/R     →     GND      ← ties to left channel
```

> Tip: Keep wires short (< 10 cm) to avoid picking up switching noise from the ESP32.

---

## 2. Flash the ESPHome firmware

```bash
# From the HomeAutomation repo root:
esphome run esphome/bird-mic.yaml
```

On first flash use USB; after that OTA works automatically.

Confirm the device shows up in **Settings → Devices & Services → ESPHome** in Home Assistant.

---

## 3. Set the Docker host IP in secrets

The ESP32 needs to know the LAN IP of the machine running docker-compose so it can
publish audio to mediamtx. Edit [esphome/secrets.yaml](esphome/secrets.yaml):

```yaml
mediamtx_host: "192.168.1.10"   # ← replace with your Docker host's LAN IP
```

> Find your host IP with `ip route get 1` (Linux) or `ipconfig` (Windows) — look for
> the address on the same subnet as your WiFi network.
> Do **not** use `localhost` or `127.0.0.1` — the ESP32 is a separate device.

The custom `rtp_sender` component ([esphome/components/rtp_sender/](esphome/components/rtp_sender/))
is already wired into [esphome/bird-mic.yaml](esphome/bird-mic.yaml) and will be compiled
automatically when you flash. No further YAML edits are needed.

---

## 4. Start the Docker stack

```bash
# From the HomeAutomation repo root:
docker-compose up -d mediamtx birdnet-go
```

Check both containers are running:
```bash
docker-compose ps
docker-compose logs -f birdnet-go
```

---

## 5. Configure BirdNET-Go audio source

Open the BirdNET-Go web UI at `http://<host-ip>:8080`.

1. Go to **Settings → Audio → Source**
2. Select **RTSP**
3. Enter the stream URL (mediamtx is on the same Docker network, so use the service name):
   ```
   rtsp://mediamtx:8554/bird
   ```
4. Set sample rate to **48000 Hz**, channels **mono**
5. Save and restart BirdNET-Go:
   ```bash
   docker-compose restart birdnet-go
   ```

---

## 6. Configure BirdNET-Go location and species filter

In the BirdNET-Go web UI → **Settings → Location**:

| Setting | Value |
|---|---|
| Latitude | Your coordinates (e.g. `48.1351`) |
| Longitude | Your coordinates (e.g. `11.5820`) |
| Locale | Your region language for species names |

This restricts detections to birds plausibly present at your location.

---

## 7. Tune audio settings

These are set in [esphome/bird-mic.yaml](esphome/bird-mic.yaml):

| Setting | Location in YAML | Notes |
|---|---|---|
| Gain | `volume_multiplier: 1.2` | Increase if birds are quiet (try 2.0–3.0 outdoors) |
| Noise suppression | `noise_suppression_level: 0` | Keep at 0 — suppression removes bird calls |
| Auto gain | `auto_gain: 0dBFS` | Leave at 0 for consistent amplitude |
| Thermal shutdown | `above: 80.0` in the interval block | Lower to 70 if the ESP32 runs hot |

---

## 8. Verify it works

1. Check mediamtx has an active publisher:
   ```bash
   docker-compose logs mediamtx | grep "bird"
   # Should show: "bird: someone is publishing"
   ```
2. Open BirdNET-Go → **Live detections** tab.
3. Play a bird call recording near the microphone — you should see a detection within a few seconds.
4. HA → **Developer Tools → States** → filter `bird_mic` to see sensor values.

---

## Troubleshooting

| Symptom | Fix |
|---|---|
| mediamtx shows no publisher | Check ESP32 → Docker host network route; verify UDP port 5004 is not firewalled |
| BirdNET-Go shows "stream not available" | `docker-compose logs mediamtx` — confirm a publisher connected before BirdNET-Go started |
| No detections despite audio | Increase `volume_multiplier`; check L/R pin is wired to GND on ICS-43434 |
| BirdNET-Go web UI unreachable | Port 8080 conflict — change host port in docker-compose: `"8081:8080"` |
| ESP32 keeps rebooting | Check 3V3 current — ICS-43434 needs a stable 3V3 rail |
| GPIO34 boot error | GPIO34 is input-only; nothing should drive it at boot |
| `mediamtx` container exits | Check `mediamtx/mediamtx.yml` is mounted correctly; run `docker-compose logs mediamtx` |




my links:

https://github.com/tphakala/birdnet-go/discussions/1260

https://github.com/Sukecz/birdnetgo-esp32-rtsp-mic

https://github.com/tphakala/birdnet-go 

https://github.com/jpmurray/esp32-audio-streamer

https://birdnet.cornell.edu/?lang=de

https://github.com/Nachtzuster/BirdNET-Pi