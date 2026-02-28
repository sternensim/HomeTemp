# Home Environmental Monitor — Out-of-the-Box Build Plan

## Context
Build a fully self-contained, cheap DIY system to monitor **temperature, humidity, pressure, and air quality** in 2–3 rooms. Each room gets a custom sensor node (ESP32 + BME680 + OLED) that reports to **Home Assistant** running on an existing Raspberry Pi. Each node has a local OLED display AND data feeds into an HA Lovelace dashboard. Nodes run on LiPo battery with solar trickle charging as fallback. Budget: **< $100** (Raspberry Pi already owned).

---

## 1. System Architecture

```
[Room Node 1]                 [Room Node 2]                [Room Node 3]
ESP32 + BME680 + OLED         ESP32 + BME680 + OLED        ESP32 + BME680 + OLED
LiPo + Solar + TP4056         LiPo + Solar + TP4056        LiPo + Solar + TP4056
        |                             |                            |
        +------------- WiFi (ESPHome Native API) ----------------+
                                      |
                          [Raspberry Pi — Home Assistant OS]
                          ESPHome Add-on (node management + OTA)
                          HA Lovelace Dashboard (web browser)
                                      |
                          [Any browser / tablet / phone]
```

---

## 2. Hardware — Sensor Node (per room × 2–3)

| Component | Part | ~Price (AliExpress) | Notes |
|---|---|---|---|
| Microcontroller | ESP32-WROOM-32 dev board | $3–5 | Built-in WiFi + BLE, 3.3V logic |
| Environmental sensor | BME680 breakout (I2C) | $5–8 | Temp + Humidity + Pressure + VOC gas index |
| Local display | SSD1306 0.96" OLED (I2C) | $1–2 | 128×64px, 3.3V safe, tiny and low power |
| LiPo charger | TP4056 with protection (Micro-USB) | $0.50–1 | Handles charging + overdischarge protection |
| Battery | 3.7V 1200–2000mAh LiPo | $3–5 | JST-PH 2.0 connector |
| Solar panel | 5V 100–200mA epoxy panel | $2–4 | Connects to TP4056 IN+ / IN- |
| Enclosure | 3D-printed case (PLA/PETG) | ~$0.50 filament | Ventilated slots for sensor accuracy |
| Misc | PCB headers, Dupont wires, decoupling caps | $1–2 | |

**Node total: ~$15–27**
**3 nodes: ~$45–81**

### Why BME680?
The BME680 (Bosch) gives all 4 metrics (temp, humidity, pressure, VOC index) from a single I2C address, saving cost and wiring over separate sensor chips. The gas resistance reading is used as a TVOC/air quality proxy and ESPHome has native support for the BSEC library.

### Why ESP32 over ESP8266?
- Dual-core, faster for BSEC sensor library (floating point intensive)
- More GPIO, built-in hall sensor, better deep sleep current spec
- Still ~$3–5 on AliExpress

---

## 3. Hardware — Central / Display

| Component | Part | ~Price |
|---|---|---|
| Central hub | Raspberry Pi (already owned) | $0 |
| microSD | 32GB A1 class card | ~$5–8 |
| Wall display | Any spare tablet/phone or 3.5" RPi TFT (optional) | $0–15 |

**Central total: $5–23**

> For the wall display, the cheapest approach is using an existing Android tablet or phone in **kiosk mode** running the Home Assistant companion app or a browser pointed at `http://homeassistant.local:8123`. No additional hardware required.

---

## 4. Wiring Diagram (per node)

```
                         ┌──────────────────────────────────┐
                         │           ESP32 Dev Board         │
Solar Panel 5V ─────────►│ VIN (5V)                         │
                 TP4056  │ GND ◄──── GND (common)           │
LiPo 3.7V ──────────────►│ 3.3V ──► BME680 VCC             │
                         │          BME680 GND ──► GND       │
                         │ GPIO21 ──► SDA ──┬── BME680 SDA  │
                         │ GPIO22 ──► SCL ──┼── BME680 SCL  │
                         │                  └── OLED SDA/SCL │
                         │ 3.3V ──► OLED VCC                │
                         │ GND  ──► OLED GND                │
                         └──────────────────────────────────┘

Power path:
  Solar → TP4056 CHARGE IN → LiPo battery
  LiPo → TP4056 OUT → ESP32 VIN (via 5V boost or direct if dev board has boost)
  Note: Most ESP32 dev boards have an onboard LDO (AMS1117) that accepts 3.7–5V on VIN
```

---

## 5. Software Architecture

### 5.1 Node Firmware — ESPHome

ESPHome runs on the ESP32 nodes. You write a **YAML config** — ESPHome generates and compiles the firmware and handles HA integration automatically.

**Key ESPHome features used:**
- `bme680_bsec` component — reads all 4 BME680 metrics including calibrated IAQ (Indoor Air Quality) score
- `display` component with `ssd1306_i2c` driver — draws readings on OLED
- `deep_sleep` component — puts ESP32 into deep sleep between readings (~10µA)
- `wifi` component — connects to local WiFi
- `api` component — native encrypted API to Home Assistant (no MQTT needed)
- `ota` component — firmware updates over WiFi from HA ESPHome add-on

### 5.2 Central Hub — Home Assistant OS

Install **Home Assistant OS** (HAOS) on the Raspberry Pi microSD. This is the recommended install method and gives you the Supervisor (for add-ons).

**Add-ons to install:**
1. **ESPHome** — manages node configs, compiles firmware, pushes OTA updates
2. **File Editor** (or Studio Code Server) — edit ESPHome YAML files in browser
3. Optional: **Mosquitto MQTT** — only needed if you want MQTT logging alongside native API

**Home Assistant integrations (auto-discovered):**
- ESPHome — all nodes appear as HA devices automatically when powered on

### 5.3 Dashboard — HA Lovelace

Create a custom Lovelace dashboard. Recommended cards (available in HACS or built-in):
- `gauge` card — IAQ score with color zones (green/yellow/red)
- `history-graph` card — 24h trend lines for temp/humidity
- `entities` card — current readings grouped by room
- `mini-graph-card` (HACS) — compact inline graphs per sensor

---

## 6. ESPHome Node Config (template)

```yaml
# esphome/room_node_1.yaml

esphome:
  name: room-node-1
  friendly_name: Living Room Node

esp32:
  board: esp32dev

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:
  encryption:
    key: !secret api_key

ota:
  - platform: esphome
    password: !secret ota_password

logger:

bme680_bsec:
  address: 0x77
  temperature_offset: 2.0        # compensate for self-heating
  iaq_mode: static               # static = room not actively ventilated
  supply_voltage: 3.3V
  operating_age: 28d

sensor:
  - platform: bme680_bsec
    temperature:
      name: "Temperature"
    pressure:
      name: "Pressure"
    humidity:
      name: "Humidity"
    iaq:
      name: "IAQ Score"
    co2_equivalent:
      name: "CO2 Equivalent"
    breath_voc_equivalent:
      name: "VOC Equivalent"

display:
  - platform: ssd1306_i2c
    model: "SSD1306 128x64"
    address: 0x3C
    update_interval: 5s
    lambda: |-
      it.printf(0, 0,  id(font_s), "Living Room");
      it.printf(0, 16, id(font_m), "%.1f°C  %.0f%%", id(temp).state, id(humid).state);
      it.printf(0, 36, id(font_s), "%.0f hPa", id(press).state);
      it.printf(0, 52, id(font_s), "IAQ: %.0f", id(iaq).state);

font:
  - file: "gfonts://Roboto"
    id: font_m
    size: 16
  - file: "gfonts://Roboto"
    id: font_s
    size: 12

deep_sleep:
  run_duration: 30s          # stay awake 30s to send readings + update OLED
  sleep_duration: 5min       # sleep 5 minutes between readings
```

> Replicate this config for each room node, changing only `name`, `friendly_name`, and the display label string.

---

## 7. Power Budget & Solar Sizing

| State | Current Draw | Duration per 5-min cycle |
|---|---|---|
| Deep sleep | ~10 µA | ~270s |
| Active (WiFi connect + read + transmit) | ~150 mA avg | ~25s |
| OLED on (active) | +20 mA | ~25s |

**Average current per cycle:**
`(170mA × 25s + 0.01mA × 270s) / 300s ≈ 14.2 mA average`

**Battery life (1200 mAh):**
`1200 / 14.2 ≈ 84 hours ≈ 3.5 days` (no solar)

**Solar to sustain indefinitely:**
`14.2 mA × 3.7V = 52.5 mW needed`
→ A **5V 100mA (0.5W)** panel near a window provides ~50–100 mW in normal daylight → sufficient to extend battery life dramatically (weeks/indefinitely with reasonable light)

> **Tip:** Increase `sleep_duration` to 10–15min in battery-critical setups — battery life scales nearly linearly with sleep time.

---

## 8. Build Steps (ordered)

1. **Install Home Assistant OS** on Raspberry Pi SD card → first boot, configure account
2. **Install ESPHome add-on** in HA → open ESPHome dashboard
3. **Create node YAML configs** in ESPHome (one per room) using the template above
4. **Flash first node via USB** (first flash must be wired; subsequent updates are OTA)
5. **Wire up prototype node** on breadboard: ESP32 + BME680 + SSD1306 + TP4056
6. **Power on and verify** node appears in HA under Devices & Integrations
7. **Design and 3D print enclosure** with ventilation slots facing down/sideways
8. **Solder final version** onto stripboard or custom PCB
9. **Add solar panel + LiPo** and test charging circuit with multimeter
10. **Deploy to rooms**, place out of direct sunlight (avoid sensor temp error from solar heat)
11. **Build HA Lovelace dashboard** with gauge + history graph cards per room
12. **Configure HA automations** (e.g. notify if IAQ > 150 = unhealthy)

---

## 9. Budget Summary

| Item | Qty | Unit Cost | Total |
|---|---|---|---|
| ESP32-WROOM-32 dev board | 3 | $4 | $12 |
| BME680 I2C module | 3 | $7 | $21 |
| SSD1306 0.96" OLED | 3 | $1.50 | $4.50 |
| TP4056 charger module | 3 | $0.80 | $2.40 |
| 3.7V 1200mAh LiPo | 3 | $4 | $12 |
| 5V 100mA solar panel | 3 | $3 | $9 |
| Misc (wire, headers, caps) | — | — | $5 |
| 32GB microSD for HA | 1 | $7 | $7 |
| **Total** | | | **~$73** |

> Remaining ~$27 headroom for 3D print filament, a spare node, or an optional 3.5" wall TFT display (~$12–15) connected directly to the Raspberry Pi via SPI.

---

## 10. Home Assistant Dashboard Layout (Lovelace YAML sketch)

```yaml
title: Home Environment
views:
  - title: Overview
    cards:
      - type: horizontal-stack
        cards:
          - type: gauge
            name: Living Room IAQ
            entity: sensor.living_room_node_iaq_score
            min: 0
            max: 500
            segments:
              - from: 0   color: "#00c853"  # Good
              - from: 100 color: "#ffd600"  # Average
              - from: 150 color: "#ff6d00"  # Little bad
              - from: 200 color: "#d50000"  # Bad
          # repeat for each room...
      - type: history-graph
        entities:
          - sensor.living_room_node_temperature
          - sensor.bedroom_node_temperature
        hours_to_show: 24
```

---

## 11. Recommended Component Sources

| Source | Use | Notes |
|---|---|---|
| **AliExpress** | All components | 2–4 week delivery, significantly cheaper |
| **LCSC Electronics** | Components in bulk | Great for passive components |
| **Amazon** | If you need parts quickly | 2–3× more expensive but fast |
| **JLCPCB / PCBWay** | Custom PCB if desired | 5 boards for ~$5 + shipping |

---

## 12. Optional Enhancements (post-MVP)

- **CO2 sensor**: Add MH-Z19B (~$15 each) — real CO2, not estimated — would push per-node cost up ~$15
- **Particulate matter**: PMS5003 (~$15) for PM2.5/PM10 — useful near kitchens
- **HA Notification**: Automate mobile push alerts when IAQ degrades
- **Node-RED** (HA add-on): Complex automation flows with visual editor
- **Custom PCB**: Design a compact PCB in KiCad, order from JLCPCB (~$5/5 boards)
- **E-Ink wall display**: Waveshare 4.2" e-ink (~$15) as an ultra-low-power central display

---

## 13. Verification & Testing

- [ ] Each node appears in HA under **Settings → Devices & Services → ESPHome**
- [ ] All 6 sensors (temp, humidity, pressure, IAQ, CO2eq, VOC) populate with values
- [ ] OLED updates every 5s while awake
- [ ] Node enters deep sleep (WiFi drops from router after ~30s of being awake)
- [ ] Node reconnects and publishes new values after 5min sleep
- [ ] Solar panel charges LiPo (TP4056 charge LED active in daylight)
- [ ] HA Lovelace dashboard shows all rooms with history graphs
- [ ] OTA firmware update succeeds from ESPHome add-on without USB cable
