# Quick Start Checklist

Follow this step-by-step checklist to get your Home Environmental Monitor up and running.

---

## Pre-Build: What You Have

- [ ] Raspberry Pi (3B+ or newer) ready to use
- [ ] microSD card (32GB recommended, A1 class)
- [ ] USB power supply (5V, 2A+)
- [ ] Computer with internet for downloading
- [ ] WiFi network with password handy
- [ ] This project folder: `HomeAutomation/`

---

## Phase 1: Software Setup (Day 1 — ~2 hours)

### Step 1: Install Home Assistant OS

- [ ] Download Balena Etcher: https://www.balena.io/etcher/
- [ ] Download HA OS image for your Pi: https://www.home-assistant.io/installation/
- [ ] Flash microSD card with HA OS
- [ ] Insert SD into Raspberry Pi and power on
- [ ] Wait 5 minutes for first boot
- [ ] Find Pi's IP address (check router DHCP list)
- [ ] Open browser: `http://homeassistant.local:8123` (or use IP)
- [ ] Create user account (username + password)
- [ ] Complete onboarding setup

### Step 2: Install ESPHome Add-on

- [ ] In Home Assistant: **Settings → Add-ons → Add-on Store**
- [ ] Search for "ESPHome" and install
- [ ] Start ESPHome add-on
- [ ] Enable "Start on boot" toggle
- [ ] Open ESPHome dashboard: **http://homeassistant.local:6052**
- [ ] Verify dashboard is empty (no devices yet)

### Step 3: Set Up ESPHome Configurations

- [ ] Connect to Pi via SSH or File Editor add-on
- [ ] Navigate to `/config/esphome/`
- [ ] Copy this project's `esphome/` folder contents to Pi
- [ ] Edit `/config/esphome/secrets.yaml`:
  - [ ] Replace `YOUR_WIFI_SSID` with your WiFi name
  - [ ] Replace `YOUR_WIFI_PASSWORD` with your WiFi password
  - [ ] Generate random API key: `python3 -c "import secrets; print(secrets.token_hex(16))"`
  - [ ] Generate OTA password: Use any random string (15+ characters)
- [ ] Save `secrets.yaml`
- [ ] Refresh ESPHome dashboard
- [ ] Verify three device cards appear:
  - [ ] Living Room Node
  - [ ] Bedroom Node
  - [ ] Office Node

---

## Phase 2: Build & Flash First Node (Day 2-3 — ~4-6 hours)

### Step 4: Order Hardware

- [ ] 1× ESP32-WROOM-32 dev board ($4)
- [ ] 1× BME680 breakout board ($7)
- [ ] 1× SSD1306 0.96" OLED ($1.50)
- [ ] 1× TP4056 charger module ($0.80)
- [ ] 1× 3.7V 1200mAh LiPo battery ($4)
- [ ] 1× 5V 100mA solar panel ($3)
- [ ] Breadboard, wires, capacitors ($3-5)
- [ ] **Total for 1 node:** ~$23-25

### Step 5: Prototype on Breadboard

- [ ] Read: [02-NODE-SETUP.md](docs/02-NODE-SETUP.md)
- [ ] Layout breadboard:
  - [ ] ESP32 power (VIN to +5V, GND to -)
  - [ ] BME680 connections (I2C on GPIO21/GPIO22, power 3.3V)
  - [ ] OLED connections (I2C on GPIO21/GPIO22, power 3.3V)
  - [ ] Capacitor across power (smooth voltage)

### Step 6: Flash via USB

- [ ] Read: [03-FIRMWARE-FLASHING.md](docs/03-FIRMWARE-FLASHING.md)
- [ ] Connect ESP32 to computer via USB
- [ ] In ESPHome dashboard: Click "Living Room Node" card
- [ ] Click "INSTALL" → "Connect via USB"
- [ ] Select ESP32 COM port from popup
- [ ] Wait for flashing to complete (1-3 minutes)
- [ ] Watch for "Successfully installed" message

### Step 7: Verify in Home Assistant

- [ ] Wait 30 seconds for node to boot
- [ ] Open Home Assistant
- [ ] Go to **Settings → Devices & Services**
- [ ] Look for **"Living Room Node"** under ESPHome
- [ ] Click to see sensors:
  - [ ] Temperature
  - [ ] Humidity
  - [ ] Pressure
  - [ ] IAQ Score
  - [ ] CO2 Equivalent
  - [ ] VOC Equivalent

### Step 8: Test OLED Display

- [ ] Check physical OLED on breadboard
- [ ] Should show:
  - [ ] Room name ("Living Room")
  - [ ] Current temperature
  - [ ] Current humidity
  - [ ] Current pressure
  - [ ] Current IAQ score
- [ ] All values should update every 5 seconds

### Step 9: Test Power Circuit

- [ ] Connect TP4056 to solar panel (red to IN+, black to IN-)
- [ ] Connect LiPo to TP4056 (red to OUT+, black to OUT-)
- [ ] Connect TP4056 OUT to ESP32 VIN
- [ ] Place in sunlight or under bright lamp
- [ ] Verify TP4056 LED turns red (charging)
- [ ] Measure LiPo voltage: should increase slowly

---

## Phase 3: Build Remaining Nodes (Day 4-5 — ~3-4 hours per node)

### Step 10: Build Node 2 (Bedroom)

- [ ] Order same hardware as Node 1
- [ ] Breadboard prototype (same wiring)
- [ ] Flash `room-node-2.yaml` via USB
- [ ] Verify appears as "Bedroom Node" in HA
- [ ] Check all sensors populate
- [ ] Test power circuit

### Step 11: Build Node 3 (Office)

- [ ] Order same hardware as Node 1
- [ ] Breadboard prototype (same wiring)
- [ ] Flash `room-node-3.yaml` via USB
- [ ] Verify appears as "Office Node" in HA
- [ ] Check all sensors populate
- [ ] Test power circuit

---

## Phase 4: Finalize Enclosures (Day 6 — ~2 hours + print time)

### Step 12: 3D Print Enclosures

- [ ] Read: [ENCLOSURE-DESIGN.md](hardware/ENCLOSURE-DESIGN.md)
- [ ] Find STL files or design your own
- [ ] Slice in Cura/PrusaSlicer:
  - [ ] Layer height: 0.2mm
  - [ ] Infill: 10%
  - [ ] Generate supports
- [ ] Print 3 enclosures (or 1-2 if time-limited)
- [ ] Post-process: Remove supports, sand edges

### Step 13: Assemble into Enclosures

- [ ] Solder components to stripboard (or use breadboard if prototype)
- [ ] Install into 3D printed case:
  - [ ] Mount ESP32 with standoffs
  - [ ] Glue OLED behind display window
  - [ ] Glue BME680 to bottom vent slot
- [ ] Route cables through designated ports
- [ ] Optional: Glue acrylic window over display

---

## Phase 5: Dashboard & Automations (Day 7 — ~1-2 hours)

### Step 14: Create Dashboard

- [ ] Read: [04-DASHBOARD-SETUP.md](docs/04-DASHBOARD-SETUP.md)
- [ ] In Home Assistant: **Dashboards → Create Dashboard**
- [ ] Name: "Home Environment Monitor"
- [ ] Enter edit mode (pencil icon)
- [ ] Add cards:
  - [ ] Gauge cards for IAQ scores (all 3 rooms)
  - [ ] Gauge cards for temperatures (all 3 rooms)
  - [ ] Entities cards for humidity/pressure/etc.
  - [ ] History graph for 24h trends
- [ ] Save dashboard

### Step 15: Create Automations

- [ ] Go to **Settings → Automations & Scenes**
- [ ] Create automation: "Alert on Poor IAQ"
  - [ ] Trigger: IAQ > 150
  - [ ] Action: Create persistent notification
- [ ] Create automation: "Alert on High Temperature"
  - [ ] Trigger: Temperature > 28°C
  - [ ] Action: Create persistent notification
- [ ] Create automation: "Alert on High Humidity"
  - [ ] Trigger: Humidity > 70%
  - [ ] Action: Create persistent notification
- [ ] Test automations (click play icon)
- [ ] Verify notifications appear

### Step 16: Test All Sensors

- [ ] Dashboard shows all 18 entities (6 sensors × 3 nodes)
- [ ] All values update every 5 minutes (sleep cycle)
- [ ] History graphs populate after 1 hour
- [ ] Automations trigger when tested

---

## Phase 6: Deploy to Rooms (Day 8)

### Step 17: Place Nodes

- [ ] Decide room locations:
  - [ ] Living Room (high traffic area)
  - [ ] Bedroom (away from direct sunlight)
  - [ ] Office (away from heat sources)
- [ ] Mount each node:
  - [ ] Wall mount with screws (if case has tabs)
  - [ ] Or place on shelf/table
  - [ ] Sensor vent slots pointing downward (prevent water)
  - [ ] OLED facing observer for visibility
- [ ] Route power cables:
  - [ ] USB to nearby outlet (if not using solar)
  - [ ] Or place solar panel on window sill

### Step 18: Connect Power

- [ ] Plug each node into USB power (temporary)
- [ ] OR connect to battery + solar (standalone)
- [ ] Wait 30 seconds for WiFi connection
- [ ] Verify all three nodes show "Online" in ESPHome
- [ ] Verify all sensors showing values in HA dashboard

### Step 19: Live Monitoring

- [ ] Open HA dashboard on phone/tablet/computer
- [ ] Observe readings from all rooms
- [ ] Check OLED displays on each node (should show live data)
- [ ] Watch trends over next 24 hours

---

## Final Verification Checklist

### Hardware
- [ ] All 3 nodes powered and connected
- [ ] OLED displays show live readings
- [ ] Sensors respond to environment (temp changes, humidity changes)
- [ ] WiFi signal strong (RSSI > -70 dBm)
- [ ] Battery charging if solar panel present

### Software
- [ ] All nodes appear in Home Assistant
- [ ] All 18 sensors show values (not "Unavailable")
- [ ] Dashboard displays all readings
- [ ] History graphs show 24-hour trends
- [ ] Automations trigger on test
- [ ] Mobile app notifications work (optional)

### Functionality
- [ ] Deep sleep working (node offline for 5 min, then reconnects)
- [ ] OTA update works (update via ESPHome without USB)
- [ ] Temperature offset calibrated (matches other thermometer)
- [ ] IAQ accuracy improving (shows 3/3 after 1+ hours)

---

## Troubleshooting Quick Links

**Problem during build?** Check:
1. [05-TROUBLESHOOTING.md](docs/05-TROUBLESHOOTING.md) — comprehensive troubleshooting guide
2. [02-NODE-SETUP.md](docs/02-NODE-SETUP.md) — wiring issues
3. [03-FIRMWARE-FLASHING.md](docs/03-FIRMWARE-FLASHING.md) — flashing issues

**Common issues:**
- Node won't power → Check USB cable (must be data cable)
- OLED blank → Check I2C address (0x3C default)
- Sensors unavailable → Wait 5 min for node to wake from sleep
- WiFi won't connect → Check credentials in `secrets.yaml`

---

## Maintenance & Updates

### Monthly
- [ ] Check battery voltage (should be ~3.7V)
- [ ] Verify all sensors still reporting
- [ ] Check for WiFi disconnections in logs

### Every 3 Months
- [ ] Review historical data for trends
- [ ] Clean sensor vents (dust can affect readings)
- [ ] Update ESPHome if new features available

### Yearly
- [ ] Replace batteries if not holding charge
- [ ] Calibrate temperature reading vs. reference thermometer
- [ ] Clean solar panels of dust/debris

---

## After Completion: Fun Ideas

✓ **Mobile app integration** — Install Home Assistant Companion App on phone
✓ **Voice control** — Add Google Home / Alexa integration
✓ **Data export** — Export history to CSV for analysis
✓ **Custom alerts** — Automate fan, humidifier, or ventilation system
✓ **Wall display** — Add 3.5" TFT screen to Raspberry Pi for kiosk mode
✓ **Additional sensors** — Add CO2 sensor (MH-Z19B) or particle sensor (PMS5003)

---

## Estimated Timeline

| Phase | Task | Time | Cumulative |
|-------|------|------|-----------|
| 1 | Software setup | 2 hrs | 2 hrs |
| 2 | Build & flash 1 node | 4-6 hrs | 6-8 hrs |
| 3 | Build nodes 2-3 | 6-8 hrs | 12-16 hrs |
| 4 | Enclosures | 2-6 hrs | 14-22 hrs |
| 5 | Dashboard & automations | 1-2 hrs | 15-24 hrs |
| 6 | Deploy to rooms | 1 hr | 16-25 hrs |
| **Total** | | | **~1-2 weeks** |

*Note: Assumes some 3D printing time in parallel, some days waiting for parts.*

---

## Success! 🎉

Once you reach "Phase 6: Deploy to Rooms," your system is **fully operational**!

You now have:
✓ Real-time environmental monitoring in your home
✓ Cloud-free (all data stays local)
✓ 24/7 automated alerts on poor conditions
✓ Beautiful Home Assistant dashboard
✓ Scalable (can add more nodes anytime)

**Enjoy your smart home!**

---

**Questions?** See [README.md](README.md) or [05-TROUBLESHOOTING.md](docs/05-TROUBLESHOOTING.md)
