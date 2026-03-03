# Home Environmental Monitor — Complete Build Guide

**A budget-friendly DIY system to monitor temperature, humidity, pressure, and air quality (IAQ) in multiple rooms.**

---

## Quick Start

### What You're Building

3 sensor nodes (ESP32 + BME680 + OLED) + 1 central display station (TENSTAR 2.8" LCD) + 1 central hub (Raspberry Pi + Home Assistant OS)

**Features:**
- Real-time environmental monitoring (temperature, humidity, pressure, air quality)
- Local OLED display on each sensor node
- Central 2.8" colour touchscreen showing all rooms at a glance (LVGL UI)
- Long-term data storage and graphing via InfluxDB + Grafana (optional)
- Cloud-free (everything stays on your home network)
- WiFi-enabled with deep sleep for battery efficiency
- Solar charging (optional, extends battery life)
- Home Assistant integration for dashboards and automations

**Budget:** ~$75 for hardware (3 nodes) + ~$15 for display station

---

## Folder Structure

```
HomeAutomation/
├── README.md                          ← You are here
├── HomeEncironmentalMonitorPlan.md    ← Original architecture plan
│
├── esphome/
│   ├── secrets.yaml                   ← WiFi credentials (KEEP SECRET!)
│   ├── room-node-1.yaml               ← Living Room config
│   ├── room-node-2.yaml               ← Bedroom config
│   ├── room-node-3.yaml               ← Office config
│   ├── kitchen-dht11.yaml             ← Kitchen config (DHT11 sensor)
│   └── display-station.yaml           ← TENSTAR 2.8" LCD central display
│
├── home-assistant/
│   ├── configuration-additions.yaml   ← HA config snippets
│   ├── lovelace-dashboard.yaml        ← Dashboard UI config
│   ├── automations.yaml               ← Alerts & triggers
│   └── influxdb-grafana.yaml          ← Long-term storage setup guide
│
├── hardware/
│   └── ENCLOSURE-DESIGN.md            ← 3D printing guide
│
├── docs/
│   ├── 01-HA-SETUP.md                 ← Install Home Assistant OS
│   ├── 02-NODE-SETUP.md               ← Breadboard & wiring
│   ├── 03-FIRMWARE-FLASHING.md        ← ESP32 flashing guide
│   ├── 04-DASHBOARD-SETUP.md          ← Create UI & automations
│   └── 05-TROUBLESHOOTING.md          ← Debug guide
│
└── scripts/
    └── (Optional helper scripts)
```

---

## Getting Started: Step-by-Step

### Phase 1: Setup Home Assistant (Software First)

**Time: 1-2 hours**

1. **Install Home Assistant OS on Raspberry Pi**
   - Follow: [01-HA-SETUP.md](docs/01-HA-SETUP.md)
   - Install ESPHome add-on
   - Upload node configurations

2. **Prepare ESPHome configs**
   - Copy `esphome/` folder to Pi
   - Edit `secrets.yaml` with your WiFi credentials
   - Configs ready for nodes

### Phase 2: Build First Node (Prototype)

**Time: 2-3 hours + flashing**

1. **Breadboard prototype**
   - Follow: [02-NODE-SETUP.md](docs/02-NODE-SETUP.md)
   - Wire: ESP32 + BME680 + OLED + TP4056
   - Test on breadboard first

2. **Flash firmware via USB**
   - Follow: [03-FIRMWARE-FLASHING.md](docs/03-FIRMWARE-FLASHING.md)
   - Flash `room-node-1.yaml` to first ESP32
   - Verify appears in Home Assistant

3. **Test sensors**
   - Check OLED displays readings
   - Verify all 6 sensors in Home Assistant
   - Temperature, humidity, pressure, IAQ, CO2, VOC

### Phase 3: Finalize & Replicate Nodes

**Time: 3-4 hours per node**

1. **Solder final version**
   - Move from breadboard to soldered PCB/stripboard
   - Add TP4056 + LiPo + solar panel

2. **Test power circuit**
   - Verify TP4056 charging
   - Test battery voltage
   - Confirm solar panel working

3. **3D print enclosure**
   - Follow: [ENCLOSURE-DESIGN.md](hardware/ENCLOSURE-DESIGN.md)
   - Print PLA case with ventilation
   - Assemble into final form

4. **Repeat for nodes 2 & 3**
   - Build and flash remaining nodes
   - Quick process after first node

### Phase 4: Create Dashboard & Automations

**Time: 1 hour**

1. **Build Home Assistant dashboard**
   - Follow: [04-DASHBOARD-SETUP.md](docs/04-DASHBOARD-SETUP.md)
   - Add gauge cards for IAQ
   - Add history graphs
   - Add system monitoring

2. **Create automations**
   - Alert when IAQ > 150 (unhealthy)
   - Alert on temperature extremes
   - Alert on high humidity

3. **Test everything**
   - Verify all sensors visible
   - Trigger test automation
   - Check mobile notifications (optional)

### Phase 5: Central Display Station

**Time: 1 hour**

The display station is an always-on TENSTAR ESP32 2.8" LCD (aka "Cheap Yellow Display") that shows live temperature, humidity, and IAQ for all 3 rooms on a single 320×240 colour screen.

1. **Flash the firmware**
   - Flash `esphome/display-station.yaml` to the TENSTAR board via USB
   - It will appear in Home Assistant automatically

2. **Verify entity IDs**
   - Open HA → Developer Tools → States
   - Confirm the entity IDs match the ones in `display-station.yaml`
   - Default pattern: `sensor.room_node_1_living_room_temperature`

3. **Calibrate the touch screen** *(optional)*
   - Set `logger: level: DEBUG` in `display-station.yaml`, reflash
   - Watch ESPHome logs while touching the screen corners
   - Update `calibration_x_min/max` and `calibration_y_min/max` with the raw values

4. **Adjust display orientation if needed**
   - If the image is upside down, change `rotation: 90` to `rotation: 270`

**Display layout:**

```
┌─────────────────────────────────────────────────────┐
│  Home Environment Monitor                           │
├──────────────┬────────────┬──────────┬──────────────┤
│ Living Room  │  24.5°     │  45%     │  IAQ 50      │
├──────────────┼────────────┼──────────┼──────────────┤
│ Bedroom      │  22.1°     │  52%     │  IAQ 45      │
├──────────────┼────────────┼──────────┼──────────────┤
│ Office       │  23.8°     │  48%     │  IAQ 62      │
├─────────────────────────────────────────────────────┤
│  Connected to Home Assistant                        │
└─────────────────────────────────────────────────────┘
```

---

### Phase 6: Long-Term Storage with InfluxDB + Grafana *(optional)*

**Time: 30–60 minutes**

Home Assistant's built-in recorder keeps 10 days of raw data. For months or years of history with flexible graphs, add InfluxDB and Grafana. Full instructions in [`home-assistant/influxdb-grafana.yaml`](home-assistant/influxdb-grafana.yaml).

**Quick summary:**

1. **Install add-ons** in HA → Add-on Store:
   - `InfluxDB` (Community Add-ons)
   - `Grafana` (Community Add-ons)

2. **Set up InfluxDB**
   - Open its Web UI → run the setup wizard
   - Organization: `home-assistant` / Bucket: `homeassistant`
   - Generate an All-Access API token, add it to `secrets.yaml` as `influxdb_token`

3. **Add to `configuration.yaml`** — copy the `influxdb:` block from `influxdb-grafana.yaml` and restart HA

4. **Connect Grafana** → Connections → Data Sources → InfluxDB
   - URL: `http://a0d7b954-influxdb:8086`
   - Query language: `Flux`
   - Paste your token

5. **Build dashboards** using the example Flux queries in `influxdb-grafana.yaml`:
   - Multi-room temperature/humidity/IAQ time series
   - Daily min/max overlays
   - Current-value stat panels

**Data retention comparison:**

| Storage | Raw retention | Long-term |
|---------|--------------|-----------|
| HA Recorder | 10–30 days | Hourly averages only |
| InfluxDB | Set per bucket (e.g. 1 year) | Full resolution forever |

> For 3 rooms × 7 sensors × 5-minute readings: ~50–100 MB per year in InfluxDB.

---

### Phase 7: Deploy

**Time: 30 minutes**

1. **Place nodes in rooms**
   - Living room, bedroom, office (or your rooms)
   - Expose sensor vents (slot pointing down)
   - Position away from direct sunlight

2. **Plug in power**
   - Each node has solar charging (optional)
   - Or USB power until solar charges battery

3. **Monitor from Home Assistant**
   - All readings appear in dashboard
   - Automations trigger on thresholds

---

## What You Need

### Hardware to Buy

| Item | Qty | Unit Cost | Notes |
|------|-----|-----------|-------|
| ESP32-WROOM-32 dev board | 3 | $4 | Built-in WiFi + BLE |
| BME680 breakout (I2C) | 3 | $7 | Temp + Humidity + Pressure + IAQ |
| SSD1306 OLED 0.96" | 3 | $1.50 | 128×64 display |
| TP4056 charger module | 3 | $0.80 | LiPo charging circuit |
| 3.7V 1200mAh LiPo | 3 | $4 | Battery |
| 5V 100mA solar panel | 3 | $3 | Trickle charging |
| Breadboard + wires | — | $2 | Prototyping |
| Dupont jumper wires | — | $1 | 40-pin assortment |
| Misc (caps, headers) | — | $2 | Decoupling, JST headers |
| **Total** | | **~$75** | For 3 complete nodes |

**Already have:**
- Raspberry Pi (any model 3B+ or newer)
- microSD card (32GB recommended)
- USB power supplies
- Computer for flashing

### Tools & Software

**Software (all free):**
- Home Assistant OS (free)
- ESPHome (free, open source)
- Balena Etcher (free, for SD card flashing)
- esptool.py (free, Python package)

**Hardware tools:**
- Soldering iron + solder (for final nodes)
- Multimeter (for testing)
- Breadboard (for prototyping)
- USB cable (for programming ESP32)
- 3D printer (optional, for enclosure)

---

## Quick Reference

### Key Files & What They Do

| File | Purpose |
|------|---------|
| `esphome/secrets.yaml` | **KEEP SECRET!** WiFi credentials & API keys |
| `esphome/room-node-*.yaml` | Node firmware configs — Living Room, Bedroom, Office (BME680) |
| `esphome/kitchen-dht11.yaml` | Kitchen node firmware (DHT11 — temp + humidity only) |
| `esphome/display-station.yaml` | Central display firmware (TENSTAR 2.8" LCD) |
| `home-assistant/lovelace-dashboard.yaml` | Dashboard UI layout |
| `home-assistant/automations.yaml` | Alerts & triggers |
| `home-assistant/configuration-additions.yaml` | HA config additions |
| `home-assistant/influxdb-grafana.yaml` | InfluxDB + Grafana setup guide & Flux queries |

### GPIO Pin Usage

```
ESP32 Pins Used:
GPIO21 → SDA (I2C data)    ← BME680 + OLED share this
GPIO22 → SCL (I2C clock)   ← BME680 + OLED share this
GPIO2  → Status LED (optional)
VIN    ← Power (3.7-5V from LiPo/USB)
GND    ← Ground (common to all)
3.3V   → Sensor power (BME680, OLED)
```

### I2C Addresses

```
BME680:   0x77 (primary), 0x76 (secondary)
OLED:     0x3C (primary), 0x3D (secondary)
```

Check module silkscreen if different!

---

## Common Tasks

### Change WiFi SSID/Password

1. Edit `esphome/secrets.yaml`
2. Update `wifi_ssid` and `wifi_password`
3. OTA update each node via ESPHome dashboard

### Adjust Deep Sleep Duration (Battery vs. Frequency)

```yaml
# In room-node-*.yaml
deep_sleep:
  sleep_duration: 5min    # Change to 10min, 15min, etc.
```

**Trade-off:** Longer sleep = longer battery life but less frequent readings

### Change Temperature Offset (If Readings Off)

```yaml
bme680_bsec:
  temperature_offset: 2.0   # Add 2°C to all readings
```

### Adjust OLED Brightness

```yaml
display:
  brightness: 75%   # Range: 0-100%
```

### View Live Logs

- ESPHome dashboard → click node → "Logs" button
- See WiFi connections, sensor readings, errors

---

## Troubleshooting Quick Links

**Problem?** See [05-TROUBLESHOOTING.md](docs/05-TROUBLESHOOTING.md)

Quick checks:
1. **Node won't power on** → Check USB cable & power supply
2. **No WiFi connection** → Check credentials in `secrets.yaml`
3. **Sensors show "Unavailable"** → Wait 5 min (node might be sleeping)
4. **OLED blank** → Check I2C address (usually `0x3C`)
5. **No automations triggering** → Check entity names in automations

---

## Architecture Overview

```
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│   Living Room   │  │     Bedroom     │  │     Office      │
│   Node (ESP32)  │  │   Node (ESP32)  │  │   Node (ESP32)  │
│   + BME680      │  │   + BME680      │  │   + BME680      │
│   + OLED        │  │   + OLED        │  │   + OLED        │
│   + LiPo + Solar│  │   + LiPo + Solar│  │   + LiPo + Solar│
└────────┬────────┘  └────────┬────────┘  └────────┬────────┘
         │                    │                     │
         └────────────────────┼─────────────────────┘
                              │
                     WiFi (ESPHome API)
                              │
    ┌─────────────────────────┴──────────────────────────┐
    │       Raspberry Pi — Home Assistant OS             │
    │       + ESPHome add-on  (manages nodes)            │
    │       + InfluxDB add-on (long-term storage)        │
    │       + Grafana add-on  (historical graphs)        │
    │       + Automations     (alerts & triggers)        │
    └──────────────┬─────────────────────────────────────┘
                   │
         ┌─────────┴──────────┐
         │                    │
    ┌────▼──────────┐   ┌─────▼──────────────────────┐
    │  HA Dashboard │   │  Display Station           │
    │  (any browser)│   │  TENSTAR ESP32 2.8" LCD    │
    │  tablet/phone │   │  320×240 LVGL touchscreen  │
    └───────────────┘   │  always-on, wall-mounted   │
                        └────────────────────────────┘
```

---

## Next Steps

1. **Read the original plan:** [HomeEncironmentalMonitorPlan.md](HomeEncironmentalMonitorPlan.md)
2. **Start with Phase 1:** [01-HA-SETUP.md](docs/01-HA-SETUP.md)
3. **Questions?** See [05-TROUBLESHOOTING.md](docs/05-TROUBLESHOOTING.md)

---

## Learning Resources

- **ESPHome documentation:** https://esphome.io/
- **Home Assistant documentation:** https://www.home-assistant.io/
- **Home Assistant Community:** https://community.home-assistant.io/
- **ESP32 Datasheet:** https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- **BME680 Datasheet:** https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors-bme680/

---

## Support & Issues

**If something doesn't work:**
1. Check [05-TROUBLESHOOTING.md](docs/05-TROUBLESHOOTING.md) for your specific issue
2. Check Home Assistant logs: **Settings → System → Logs**
3. Check ESPHome logs: **ESPHome dashboard → Device → Logs**
4. Search community: https://community.home-assistant.io/

---

## Tips for Success

✓ **Breadboard first, solder second** — Test everything before soldering
✓ **Backup your configs** — Save YAML files locally
✓ **Start simple** — Get one node working perfectly before building three
✓ **Test power circuits carefully** — Multimeter is your friend
✓ **Give sensors time to warm up** — IAQ accuracy improves over hours
✓ **Monitor WiFi signal** — Weak signals cause disconnects

---

## File Versions & Updates

**Last updated:** March 2026
**Plan version:** 1.1 — added display station + InfluxDB/Grafana
**ESPHome version:** Compatible with 2024.12+
**Home Assistant:** Compatible with 2024.12+

---

**Happy building! 🏡**
