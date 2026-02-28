# Step 5: Comprehensive Troubleshooting Guide

---

## Table of Contents

1. [Hardware Troubleshooting](#hardware-troubleshooting)
2. [WiFi & Connectivity Issues](#wifi--connectivity-issues)
3. [Sensor Issues](#sensor-issues)
4. [Display Issues](#display-issues)
5. [Power & Battery Issues](#power--battery-issues)
6. [Home Assistant Integration](#home-assistant-integration)
7. [ESPHome Firmware Issues](#esphome-firmware-issues)

---

## Hardware Troubleshooting

### ESP32 Won't Power On

**Symptoms:** No LED lights, device unresponsive

**Causes & Solutions:**
1. **Bad power supply:**
   - Try USB power directly from computer
   - Measure voltage: should be 5V on VIN pin
   - Use at least 2A power supply

2. **Reversed power:**
   - Check polarity: Red (+) to VIN, Black (-) to GND
   - Never connect backwards (destroys board)

3. **Damaged board:**
   - Check for visible burns or damage
   - Try on another breadboard location
   - Replace if visibly damaged

### Breadboard Not Making Connections

**Symptoms:** Devices powered but sensors don't read

**Causes & Solutions:**
1. **Loose wires:**
   - Push all Dupont connectors firmly into breadboard
   - Try different holes if connection unstable
   - Clean breadboard contacts with pencil eraser

2. **Bad Dupont wires:**
   - Test by connecting LED to power (should light)
   - If LED doesn't light, wire is broken
   - Replace broken wires

3. **Breadboard worn out:**
   - Very old breadboards lose contact pressure
   - Test with multimeter (should show <1Ω continuity)
   - Replace if > 5 years old

### I2C Devices Not Detected

**Symptoms:** BME680 or OLED not responding, "I2C scan found no devices"

**Causes & Solutions:**

1. **Wrong I2C address:**
   - BME680: Usually `0x77`, sometimes `0x76`
   - OLED: Usually `0x3C`, sometimes `0x3D`
   - Check module silkscreen or datasheet
   - Try scanning: Enable I2C scan in ESPHome logs

   ```yaml
   i2c:
     scan: true
   logger:
     level: DEBUG
   ```

   Then check logs for detected addresses.

2. **Wires not connected:**
   - SDA to GPIO21: Verify wire path
   - SCL to GPIO22: Verify wire path
   - GND connected: Must be common ground to all devices
   - 3.3V from ESP32: Should be supplying sensors

3. **Pull-up resistors (advanced):**
   - Add 10kΩ resistors from SDA/SCL to 3.3V
   - Especially if cable runs > 1 meter
   - But most breakouts have internal pull-ups

4. **Bad sensor breakout:**
   - Test with multimeter: should show resistive path SDA/SCL to GND
   - If open circuit, breakout is damaged
   - Contact seller for replacement

**Debug ESP32 I2C:**
```yaml
i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true        # Scan and log found devices
  frequency: 100kHz # Slow down if having issues
```

---

## WiFi & Connectivity Issues

### Node Won't Connect to WiFi

**Symptoms:** ESP32 boots but never shows "Connected" to AP

**Causes & Solutions:**

1. **Wrong WiFi credentials:**
   - Check `secrets.yaml` spelling exactly
   - WiFi is case-sensitive
   - No spaces at beginning/end
   - Test with known device first

2. **WiFi too far away:**
   - Move ESP32 within 3 meters of router
   - Check WiFi signal strength in logs
   - RSSI < -80 dBm = weak, try moving closer

3. **WiFi band mismatch:**
   - Some older ESP32 don't support 5GHz
   - Use 2.4GHz WiFi band
   - Check router settings, force 2.4GHz only

4. **Router not broadcasting SSID:**
   - Some routers hide SSID
   - ESPHome needs it visible to connect
   - Temporarily enable SSID broadcast

5. **Too many WiFi networks:**
   - Router scanning > 10 networks slow to connect
   - Move to less congested location or change WiFi channel
   - Router settings: Switch from WiFi channel 1 to 6 or 11

**Fix in ESPHome:**
```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  power_save_mode: none  # Try turning off power save
  fast_connect: true     # Skip some checks
```

### Node Connects but Drops Frequently

**Symptoms:** Connects for 30s, then disconnects

**Causes & Solutions:**

1. **Weak signal:**
   - Move router closer
   - Check RSSI: should be > -70 dBm
   - Add WiFi range extender

2. **Interference:**
   - Move away from microwave, cordless phones
   - Try different WiFi channel (1, 6, 11 best for 2.4GHz)
   - Metal enclosure can block WiFi

3. **Power issue causing resets:**
   - If brownout detected, power supply unstable
   - Add larger capacitor (220µF) across power rails
   - Use 3A+ power supply

**Improve stability:**
```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  # Improve connection stability
  power_save_mode: none
  reboot_timeout: 15min
  ap:
    ssid: "ESP32-Fallback"
    password: "fallback123"
```

### WiFi Password Wrong After Flash

**Symptoms:** "WiFi credentials invalid" error

**Solution:**
1. Check `secrets.yaml` in `/config/esphome/`
2. Verify exact WiFi name and password
3. No extra spaces or special characters
4. Recompile and flash

---

## Sensor Issues

### BME680 Reads All Zeros or No Data

**Symptoms:** Temperature shows 0.0, humidity 0, IAQ unavailable

**Causes & Solutions:**

1. **Sensor not detected on I2C:**
   - See "I2C Devices Not Detected" section above
   - Check address (usually 0x77)
   - Verify SDA/SCL connected

2. **Sensor needs warmup:**
   - First boot takes 30+ minutes for calibration
   - IAQ accuracy: 0/3 → 3/3 takes time
   - Let device run for 1 hour before testing

3. **BSEC library not loaded:**
   - ESPHome must compile BSEC (takes extra space)
   - Check logs for "BSEC initialized"
   - If error, node may be out of RAM

**Fix:**
```yaml
bme680_bsec:
  address: 0x77
  state_save_interval: 6h  # Save calibration
```

### IAQ Accuracy Low (0/3 or 1/3)

**Symptoms:** IAQ score available but shows "accuracy low"

**Normal behavior:**
- New sensor needs 30 minutes warmup time
- Full accuracy takes 1-7 days
- BSEC auto-calibrates from air quality history

**Speed up:**
- Keep sensor on continuously (longer warm-up = faster calibration)
- Or: Run with longer `operating_age: 28d` if available

### Pressure Fluctuates Wildly

**Symptoms:** Pressure jumps ±20 hPa between readings

**Causes:**
1. **Sensor moving or vibrating:**
   - Mount securely to reduce vibration
   - Avoid placing near active speakers/fans

2. **Temperature fluctuation:**
   - Large temp changes cause pressure fluctuation
   - Pressure is temperature-dependent

3. **Altitude change:**
   - If moving node up/down, pressure changes
   - Calibrate sea-level pressure in HA

### Humidity Always Shows 0% or 100%

**Symptoms:** Humidity pegged at extreme values

**Causes:**
1. **Sensor saturated:**
   - Move to drier/less humid location
   - Ensure ventilation slots not blocked
   - Point slots downward to prevent water ingress

2. **I2C communication failing intermittently:**
   - Same as BME680 not detected above
   - Check I2C wiring

---

## Display Issues

### OLED Screen Blank or Shows Garbage

**Symptoms:** Nothing displays, or random pixels lit

**Causes & Solutions:**

1. **Wrong I2C address:**
   - OLED usually `0x3C`, sometimes `0x3D`
   - In YAML: `address: 0x3C`
   - Check module documentation

2. **SDA/SCL reversed:**
   - Make sure GPIO21 = SDA, GPIO22 = SCL
   - Swapping them causes display corruption

3. **Insufficient power to OLED:**
   - OLED needs 3.3V clean supply
   - Add 10µF capacitor near OLED VCC

4. **Display burned out:**
   - OLED lifespan is ~30k hours
   - If new, likely wiring issue
   - If old, may need replacement

**Verify display is detected:**
```yaml
logger:
  level: DEBUG
i2c:
  scan: true
```

Check logs for `0x3C` address found.

### OLED Flickers or Shows Old Data

**Symptoms:** Display flashes, shows previous readings

**Causes:**
1. **Update interval too fast:**
   ```yaml
   display:
     update_interval: 5s  # Try 10s or 15s
   ```

2. **I2C bus overloaded:**
   - Too many devices on I2C
   - Usually not issue with 2 devices (BME680 + OLED)
   - Slow down I2C frequency:
   ```yaml
   i2c:
     frequency: 100kHz  # Instead of 400kHz
   ```

### Display Backlight Not Adjustable

**Symptoms:** OLED always full brightness or always off

**In YAML:**
```yaml
display:
  brightness: 75%  # Range 0-100%
```

After OTA update, brightness should change.

---

## Power & Battery Issues

### Constant Brownout / Resets

**Symptoms:** ESP32 reboots every few seconds, brown-out detector triggered

**Causes:**
1. **Weak power supply:**
   - Use official 5V 2A+ supply
   - USB from computer may only be 500mA
   - Too many devices drawing power

2. **Bad USB cable:**
   - Use short, quality USB cable
   - Test with different cable

3. **Loose connections:**
   - All power connections must be firm
   - Tug on wires to verify
   - Resolder if breadboard connection bad

4. **Too much current draw:**
   - Active WiFi + OLED running = ~200mA peak
   - Add 220µF capacitor across power to smooth

**Immediate fixes:**
- Add **220µF electrolytic capacitor** across VIN/GND
- Use quality 3A power supply
- Verify all connections are solid

### Battery Drains Too Fast

**Symptoms:** LiPo dies in 1-2 days instead of 3-5

**Causes:**
1. **Deep sleep not working:**
   - Check logs: should show "Entering deep sleep"
   - If no message, deep_sleep component disabled
   - WiFi stays on = 200mA continuous = few hours drain

2. **Sleep time too short:**
   - Default 5 minutes = ~14 cycles/hour
   - Try 15 minutes = 4 cycles/hour
   - Battery life scales ~linearly with sleep time

3. **Solar not charging:**
   - TP4056 LED should be red (charging) in sunlight
   - Measure solar panel voltage: should be ~5.5V+
   - Check if panel is broken/dirty

**Optimize for battery:**
```yaml
deep_sleep:
  run_duration: 20s    # Shorter awake time
  sleep_duration: 15min  # Longer sleep time
  # Result: 14.2 mA avg * 15 min = battery life ~10 days
```

### TP4056 Never Charges

**Symptoms:** Charge LED never lights, battery stays at 3.0V

**Causes:**
1. **No solar panel voltage:**
   - Multimeter: Solar IN+ to IN- should read 5V+
   - Check if panel is working (test with another panel)
   - Clean panel (dust blocks light)

2. **TP4056 IN terminals loose:**
   - Resolder solar connections
   - Check continuity with multimeter

3. **Battery already full:**
   - TP4056 only charges if battery < 4.1V
   - Discharge battery slightly (run device longer)
   - Then place in sunlight

4. **TP4056 overcurrent protection tripped:**
   - Battery short circuit detected
   - Check battery leads aren't touching
   - Replace battery if shorted internally

---

## Home Assistant Integration

### Node Doesn't Appear in Home Assistant

**Symptoms:** ESPHome shows device online, but HA shows "Unavailable"

**Causes & Solutions:**

1. **ESPHome API encryption mismatch:**
   - Check `api_key` in secrets.yaml matches what ESPHome compiled with
   - Recompile and flash if key changed

2. **Node name has spaces:**
   - ESPHome name must be lowercase, no spaces
   - Use `room-node-1`, not `Room Node 1`

3. **ESPHome add-on not connected to HA:**
   - Restart ESPHome add-on: **Settings → Add-ons → ESPHome → Restart**
   - Restart Home Assistant: **Settings → System → Restart**

4. **Network isolation:**
   - Node and HA must be on same WiFi/Ethernet
   - Check: Both on 192.168.x.x (or same subnet)
   - Not behind different firewalls

**Fix:**
1. In HA: **Settings → System → Logs**, search for "esphome"
2. Look for errors during API connection
3. If encryption error, recompile with new key

### Sensors Show "Unavailable"

**Symptoms:** Sensors exist but show no value

**Causes:**
1. **Node in deep sleep (normal):**
   - Happens every 5 minutes
   - Returns "Available" when wakes up
   - Wait 5 minutes to see update

2. **WiFi disconnected:**
   - Check ESPHome shows "Online"
   - Check WiFi RSSI (signal strength)

3. **Sensor not detected by node:**
   - Check ESPHome logs for sensor errors
   - Verify I2C address correct

### History Not Showing

**Symptoms:** Graphs empty, no historical data

**Causes:**

1. **Recorder not enabled:**
   ```yaml
   # In configuration.yaml
   recorder:
   ```

2. **Not enough time passed:**
   - Home Assistant needs 1+ readings to show history
   - Gives it 1 hour minimum

3. **Database corrupted:**
   - Go to **Settings → System → Maintenance**
   - Click "Purge" to clear database
   - Takes a few minutes

---

## ESPHome Firmware Issues

### Compilation Fails

**Symptoms:** "ERROR: Compilation failed" or timeout

**Causes:**
1. **Out of disk space:**
   - SSH to HA: `df -h`
   - If /root < 100MB free, clean up
   - HA: **Settings → System → Storage**

2. **YAML syntax error:**
   - Check indentation (2 spaces, not tabs)
   - No special characters
   - Validate at: https://www.yamllint.com/

3. **Component not supported:**
   - Some components unavailable on ESP32
   - Check component docs

**Common YAML errors:**
```yaml
# Wrong (tab character):
sensor:
→ platform: bme680_bsec    # ← This is a TAB

# Correct (2 spaces):
sensor:
  - platform: bme680_bsec  # ← This is 2 SPACES
```

### OTA Update Fails / Timeout

**Symptoms:** "Preparing..." for > 2 minutes, then times out

**Causes:**
1. **WiFi signal weak:**
   - RSSI should be > -70 dBm
   - Move router closer
   - OTA needs stable connection

2. **Node in deep sleep:**
   - Force wake first:
     - ESPHome dashboard → Device → Force Wake button
     - Or: Power cycle node

3. **Memory insufficient:**
   - Node running out of RAM
   - Try simpler YAML (remove extra sensors/automations)

### After OTA Update, Node Won't Start

**Symptoms:** Node keeps rebooting after OTA, "Reset Reason: Exception"

**Causes:**
1. **Firmware incompatible:**
   - YAML error in latest config
   - Roll back: Compile previous version, flash via USB

2. **Out of memory:**
   - Firmware too large for ESP32
   - Remove non-essential sensors/display features

3. **Corrupted flash:**
   - Erase and reflash:
   ```bash
   esptool.py --chip esp32 --port COM5 erase_flash
   # Then reflash firmware
   ```

---

## Still Stuck?

### Debug Data to Collect

When asking for help, provide:
1. ESPHome device logs (full output)
2. Home Assistant system logs (error messages)
3. Your YAML config (with secrets removed)
4. Wiring photo (breadboard layout)
5. Exact error message or symptom

### Community Resources

- **ESPHome docs:** https://esphome.io/
- **Home Assistant forums:** https://community.home-assistant.io/
- **ESPHome Discord:** https://discord.gg/KhAMQ2w
- **Home Assistant Discord:** https://discord.gg/home-assistant

---

## Prevention Tips

1. **Backup working configs:**
   ```bash
   cp -r /config/esphome ~/esphome-backup
   ```

2. **Test changes on one node first before rolling out to all three**

3. **Keep power supply clean:**
   - Use quality USB cables
   - Add capacitors near power inputs
   - Regular solder inspections

4. **Firmware version management:**
   - Note working ESP32 core version
   - Don't auto-update if system stable

5. **Monitor battery health:**
   - Check LiPo voltage monthly
   - Replace if bloated or not holding charge

---

**Still need help?** Check the other guides:
- [HA Setup](01-HA-SETUP.md)
- [Node Setup](02-NODE-SETUP.md)
- [Firmware Flashing](03-FIRMWARE-FLASHING.md)
- [Dashboard Setup](04-DASHBOARD-SETUP.md)
