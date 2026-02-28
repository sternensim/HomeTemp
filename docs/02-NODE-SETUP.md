# Step 2: Sensor Node Hardware Setup & Wiring

This guide covers breadboard prototyping, proper wiring, and assembly of your environmental sensor nodes.

---

## Hardware Checklist (per node × 3)

- [ ] ESP32-WROOM-32 development board
- [ ] BME680 environmental sensor breakout board (I2C)
- [ ] SSD1306 0.96" OLED display (I2C, 128×64)
- [ ] TP4056 LiPo charger module with protection
- [ ] 3.7V 1200-2000mAh LiPo battery (JST-PH connector)
- [ ] 5V 100-200mA solar panel (with JST or similar connector)
- [ ] Breadboard (for prototyping)
- [ ] Dupont wires (M-M, M-F assortment)
- [ ] 100µF electrolytic capacitor (smoothing)
- [ ] Stranded wire for permanent soldering

---

## Part A: Understanding the Pinout

### ESP32 Dev Board Key Pins

```
ESP32-WROOM-32 (30-pin variant)
┌─────────────────────────────────────────┐
│ USB    3.3V   GPIO34  GPIO35  GPIO32    │
│ GND    EN     GPIO23  GPIO22  GPIO21    │  ← GPIO22 (SCL), GPIO21 (SDA)
│ VIN    D36    GPIO15  GPIO02  GPIO13    │
│ D39    D35    GPIO14  GPIO27  GPIO12    │
│ D38    D34    GPIO26  GPIO25  GPIO33    │
│ D37    D33    GND     GPIO19  GPIO18    │
│ RST    D32    5V      GPIO05  GPIO17    │
│ VIN    GND    GND     GPIO04  GPIO16    │
│ 5V     D31    3.3V    GPIO02  GPIO00    │
└─────────────────────────────────────────┘
```

### Key Pins for This Project

| Pin | Purpose | Notes |
|-----|---------|-------|
| **GPIO21** | SDA (I2C Data) | BME680 + OLED both use I2C |
| **GPIO22** | SCL (I2C Clock) | Shared between sensors |
| **VIN** | Power Input | 3.7V-5V from TP4056 or USB |
| **3.3V** | 3.3V Output | Powers sensors (BME680, OLED) |
| **GND** | Ground | Common ground (all devices) |
| **GPIO2** | Status LED | (onboard blue LED, optional) |

---

## Part B: Prototype on Breadboard (First Node)

### Step 1: Breadboard Layout

**Recommended layout (viewing breadboard from top):**

```
┌─────────────────────────────────────────────────────┐
│ Power Rail (left)                                   │
│ + 3.3V + + + + + + + + + +                          │
│ - GND  - - - - - - - - - -                          │
│                                                     │
│ ESP32 (left side)                                   │
│ ┌─────────────────┐                                │
│ │ VIN (5V) ───→ + │ Power column                   │
│ │ GND ──────→ -  │ Power column                   │
│ │ GPIO21 ────→ SDA line                            │
│ │ GPIO22 ────→ SCL line                            │
│ │ 3.3V ───────→ + │ Power column                   │
│ └─────────────────┘                                │
│                                                     │
│ SDA/SCL lines (center)                              │
│ ├─ GPIO21 ─[100Ω]─┬─ BME680 SDA                    │
│ │                 ├─ OLED SDA                      │
│ │                 └─ 3.3V (pullup)                 │
│                                                     │
│ ├─ GPIO22 ─[100Ω]─┬─ BME680 SCL                    │
│ │                 ├─ OLED SCL                      │
│ │                 └─ 3.3V (pullup)                 │
│                                                     │
│ BME680 (center)                                     │
│ ┌──────────┐                                       │
│ │ VCC → 3.3V│                                       │
│ │ GND → GND │                                       │
│ │ SDA → line                                       │
│ │ SCL → line                                       │
│ └──────────┘                                       │
│                                                     │
│ OLED (right side)                                   │
│ ┌──────────┐                                       │
│ │ VCC → 3.3V│                                       │
│ │ GND → GND │                                       │
│ │ SDA → line                                       │
│ │ SCL → line                                       │
│ └──────────┘                                       │
│                                                     │
│ Capacitor (smoothing)                               │
│ 100µF between VIN(+) and GND(-)                    │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Step 2: Wire the Breadboard

**Follow this order:**

1. **Power connections (critical):**
   - Plug ESP32 dev board into breadboard (vertically, leaving space)
   - Connect VIN pin to **positive power rail**
   - Connect GND pin to **negative power rail**
   - Add 100µF capacitor across power rails (smooth power)
   - Connect 3.3V pin to **positive 3.3V column**

2. **I2C pull-up resistors (optional but recommended):**
   - BME680 and OLED have internal pull-ups, but adding external 10kΩ resistors is safer
   - Add two 10kΩ resistors from SDA/SCL lines to 3.3V
   - OR skip this step (devices usually work without)

3. **BME680 connections:**
   - VCC → 3.3V
   - GND → negative rail
   - SDA → GPIO21 (blue wire)
   - SCL → GPIO22 (yellow wire)

4. **OLED connections:**
   - VCC → 3.3V
   - GND → negative rail
   - SDA → GPIO21 (same blue wire as BME680)
   - SCL → GPIO22 (same yellow wire as BME680)

5. **Power source (temporary):**
   - Connect USB cable to ESP32 (powers via USB 5V regulator)
   - OR if using TP4056: connect TP4056 OUT+ to VIN, OUT- to GND

---

## Part C: First Flash & Test

### Step 3: Flash Firmware via ESPHome

1. **Connect ESP32 to computer via USB:**
   - Use USB-C or Micro-USB cable (data cable, not charge-only)
   - LED should light on board

2. **In ESPHome dashboard:**
   - Click card for **Room Node 1**
   - Click the **three-dot menu** → **Edit** (or "COMPILE")

3. **Verify secrets.yaml is correct:**
   - Ensure `wifi_ssid`, `wifi_password`, `api_key`, and `ota_password` are set

4. **Compile firmware:**
   - Click **COMPILE** button
   - Wait 1-2 minutes for compilation
   - You'll see log output

5. **Flash to device:**
   - After compilation, a download button appears
   - Plug ESP32 into your computer (USB)
   - Click **INSTALL**
   - Select **"Connect via USB"** (if available)
   - Select the COM port of the ESP32
   - Flashing begins (takes 1-3 minutes)
   - Watch for "Ready to serve" message

### Step 4: Verify Node Appears in Home Assistant

1. **Node should connect to WiFi automatically:**
   - Watch ESPHome logs for WiFi connection
   - Watch Home Assistant for new device discovery

2. **In Home Assistant:**
   - Go to **Settings → Devices & Services**
   - Look for **"Living Room Node"** under ESPHome
   - Check that all sensors appear:
     - Temperature
     - Humidity
     - Pressure
     - IAQ Score
     - CO2 Equivalent
     - VOC Equivalent

3. **On the OLED display:**
   - You should see live readings updating
   - Room name, temperature, humidity, pressure, IAQ score

**If something fails:**
→ See troubleshooting section at end

---

## Part D: Wiring TP4056 Charging Circuit

Once prototype works, add the charging circuit.

### Step 5: TP4056 Module Connections

The TP4056 has five pins (standard layout):

```
         ┌─────────┐
    2K   │ TP4056  │
    GND  │         │   IN+ (5V from solar)
    GND  │         │   IN- (GND)
    GND  │         │   OUT+ (to ESP32 VIN)
         └─────────┘   OUT- (to GND)
```

**Typical pinout (check your module):**

| Pin | Function | Connect To |
|-----|----------|-----------|
| IN+ | Solar input (5V) | Solar panel positive |
| IN- | Solar ground | Solar panel negative, main GND |
| OUT+ | Battery charge output (3.7V) | LiPo+ (red), ESP32 VIN via diode |
| OUT- | Battery charge ground | LiPo- (black), main GND |
| GND | (if present) | Common ground |

### Step 6: Complete Power Circuit

```
                          ┌──────────────┐
Solar Panel (5V 100mA) ──→ │              │
                          │  TP4056      │
GND ─────────────────────→ │  Charger     │
                          └──────────────┘
                           OUT+ (3.7V)
                             │
                          [Schottky diode optional]
                             ↓
         ┌──────────────────────────────────┐
         │  LiPo 1200mAh 3.7V (JST-PH)       │
         │  +  RED  ──→ ESP32 VIN            │
         │  -  BLACK ──→ GND                 │
         └──────────────────────────────────┘
                             ↓
                      ┌────────────────┐
                      │  ESP32 Dev     │
                      │  VIN (powers   │
                      │  from LiPo)    │
                      └────────────────┘
```

**Detailed steps:**

1. **Solder LiPo battery wires:**
   - Red wire to TP4056 OUT+
   - Black wire to TP4056 OUT- AND common GND

2. **Solder ESP32 power:**
   - TP4056 OUT+ → ESP32 VIN (or through Schottky diode if solar connected directly)
   - Common GND → ESP32 GND

3. **Connect solar panel:**
   - Red (+5V) → TP4056 IN+
   - Black (GND) → TP4056 IN-

4. **Test charging circuit:**
   - Place solar panel in sunlight (or under bright lamp)
   - TP4056 should show charging LED (red)
   - Measure voltage at LiPo: should be ~3.7V

---

## Part E: Assemble into Enclosure (3D Printed)

### Step 7: Design & Print Enclosure

See **[ENCLOSURE-DESIGN.md](ENCLOSURE-DESIGN.md)** for:
- STL files and dimensions
- Ventilation slot sizing (crucial for sensor accuracy!)
- Assembly instructions
- Cable routing

---

## Checklist: Node 1 Complete

- [ ] Breadboard prototype powers on
- [ ] OLED display shows readings
- [ ] WiFi connects (check HA device list)
- [ ] All 6 sensors report values in HA
- [ ] TP4056 charging circuit tested with multimeter
- [ ] LiPo battery charging in sunlight
- [ ] Enclosure printed and assembled
- [ ] Final assembly soldered (not breadboard)

---

## Repeat for Nodes 2 & 3

Once Node 1 is working:

1. Build Node 2 on breadboard (same process)
2. Flash `room-node-2.yaml` via ESPHome
3. Verify it appears in HA as "Bedroom Node"
4. Build Node 3 and flash `room-node-3.yaml`

---

## Troubleshooting

### OLED is blank or shows garbage
- Check I2C address: should be `0x3C` (some modules use `0x3D`)
- Verify SDA/SCL wiring
- In ESPHome YAML, check: `address: 0x3C`

### BME680 not detecting
- I2C address is typically `0x77`, sometimes `0x76`
- Check wiring: SDA/SCL to GPIO21/GPIO22
- Enable I2C scan in logs to confirm device presence

### WiFi won't connect
- Double-check SSID and password in `secrets.yaml`
- Move ESP32 closer to router
- Check if WiFi uses 2.4GHz (some older ESP32 don't support 5GHz well)

### Node won't flash via USB
- Try different USB cable (must be data cable)
- Install CP2104 driver (Windows): https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- Try different USB port on computer

### Brownout detector triggered
- Indicates power is unstable (likely bad connection)
- Verify all wires firmly inserted
- Add larger capacitor (220µF) across power rails
- Check USB power supply is at least 2A

---

## Next Steps

Once all three nodes are flashing and reporting to HA:
→ Continue to **[Step 3: Firmware Flashing & OTA Updates](03-FIRMWARE-FLASHING.md)**
