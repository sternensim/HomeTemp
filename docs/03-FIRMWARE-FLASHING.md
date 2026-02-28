# Step 3: Firmware Flashing & OTA Updates

This guide covers initial firmware flashing and over-the-air (OTA) updates for your ESP32 nodes.

---

## Part A: Initial Flashing (USB Required)

The first flash of each node MUST be done via USB. After that, all updates can be OTA.

### Method 1: ESPHome Web Installer (Easiest)

1. **Connect ESP32 to computer via USB:**
   - Use a data cable (not charge-only)
   - Windows: Install CP2104 driver if needed
   - macOS/Linux: Usually works out of the box

2. **Go to ESPHome dashboard:**
   - Open `http://homeassistant.local:6052` or Home Assistant → ESPHome

3. **Select your node (e.g., "Living Room Node"):**
   - Click the device card

4. **Click the three-dot menu → "Install":**
   - Choose **"Wirelessly"** or **"Connect via USB"**
   - If **"Connect via USB"** appears, click it (easiest)
   - Browser may ask for permission to access USB

5. **Select USB device:**
   - A popup appears listing COM ports
   - Find your ESP32 (usually shows as "CP2104" or "CH340G")
   - Click to select

6. **Installation begins:**
   - Progress bar shows flashing
   - Takes 1-3 minutes
   - Watch for "Installed successfully" message

7. **Node reboots and connects to WiFi:**
   - OLED powers on
   - Should appear in Home Assistant within 30 seconds
   - Go to **Settings → Devices & Services** to verify

**Troubleshooting USB install:**
- Browser doesn't show USB port → Not a data cable, try another
- Permission denied → Run browser as admin, refresh, retry
- Device not found → Install silabs CP2104 driver (Google "CP2104 driver Windows")

---

### Method 2: Manual Flashing via esptool.py (Advanced)

If web installer doesn't work:

1. **Install esptool.py:**
   ```bash
   pip install esptool
   ```

2. **Download compiled firmware from ESPHome:**
   - In ESPHome dashboard, click device → menu → "Edit"
   - Click "COMPILE"
   - After compile, click "DOWNLOAD BINARY"
   - Save the `.bin` file

3. **Flash using esptool:**
   ```bash
   esptool.py --chip esp32 --port COM5 --baud 460800 write_flash -z 0x1000 firmware.bin
   ```
   - Replace `COM5` with your actual COM port (or `/dev/ttyUSB0` on Linux)
   - Replace `firmware.bin` with downloaded filename

4. **Monitor serial output:**
   ```bash
   esptool.py --port COM5 --baud 115200 read_mac
   ```
   - Should print MAC address if communication works

---

## Part B: Over-the-Air (OTA) Updates

After initial USB flash, all future updates are wireless!

### OTA Update Process

1. **In ESPHome dashboard:**
   - Click on device card (e.g., "Living Room Node")

2. **Make code changes (optional):**
   - Edit YAML config if you want to change settings
   - Save in file editor

3. **Click "INSTALL":**
   - Node must be powered and connected to WiFi
   - You'll see "Preparing..." then progress bar

4. **Installation happens wirelessly:**
   - Node receives firmware over WiFi
   - Reboots automatically
   - Reconnects to WiFi within 30 seconds
   - Shows "Online" status in ESPHome dashboard

**If OTA fails:**
- Check WiFi signal: node may have weak connection
- Restart node: Turn off power briefly
- Recompile firmware: Click "COMPILE" again
- Try again: Click "INSTALL"

---

## Part C: Common Configuration Changes

### Change Sleep Duration (Battery Life vs. Update Frequency)

Default: 5 minutes sleep, 30 seconds awake

**Edit in ESPHome YAML:**
```yaml
deep_sleep:
  run_duration: 30s      # Time to stay awake
  sleep_duration: 5min   # Time to sleep
```

**Examples:**
- Battery critical? Use `sleep_duration: 15min` (extends to ~10 days)
- Want more frequent updates? Use `sleep_duration: 1min` (battery ~2 days)
- Ultra-low power? Use `sleep_duration: 30min` (battery ~30 days)

After changing, compile and push OTA update.

### Adjust Display Brightness

If OLED is too bright or dim:

**In YAML display section:**
```yaml
display:
  - platform: ssd1306_i2c
    brightness: 100%    # Range: 0-100%
```

Change to:
- `brightness: 50%` for dimmer
- `brightness: 25%` for very dim (nighttime friendly)
- `brightness: 100%` for maximum brightness

### Change Temperature Offset

If readings are consistently off by a few degrees (self-heating):

```yaml
bme680_bsec:
  temperature_offset: 2.0   # Add 2°C to reading
```

Change to:
- `temperature_offset: 0.0` if readings are accurate
- `temperature_offset: -1.0` if sensor reads too high
- `temperature_offset: 3.0` if sensor reads too low

### Change I2C Address

If your BME680 or OLED uses a different address:

```yaml
bme680_bsec:
  address: 0x77    # BME680 primary address
```

Or OLED:
```yaml
display:
  - platform: ssd1306_i2c
    address: 0x3C   # Change to 0x3D if needed
```

---

## Part D: Monitoring & Debugging

### View Live Logs

1. **In ESPHome dashboard:**
   - Click device card → "Logs" button (if device is online)
   - Shows real-time serial output

2. **Watch for:**
   - WiFi connection attempts
   - Sensor readings (temp, humidity, IAQ)
   - Deep sleep transitions
   - OTA updates

### Common Log Messages

```
[I][main:019]: Running deep sleep...          ← Good, node going to sleep
[E][component:214]: Component wifi took a long time...  ← WiFi slow, signal weak
[I][esphome.api:048]: Hello from living room  ← Node identified to HA
[W][bme680_bsec:033]: IAQ accuracy low (0/3)  ← Sensor still calibrating
```

### Check Sensor Status in Home Assistant

1. Go to **Settings → Devices & Services → Devices**
2. Find "Living Room Node" (and other nodes)
3. Click to see all entities
4. Each sensor shows:
   - Current value
   - Last update timestamp
   - Edit icon (to adjust names/units)

---

## Part E: Troubleshooting

### Node Goes Offline After Flash

**Common causes:**
1. **WiFi disconnect** → Move router closer or extend antenna
2. **WiFi credentials wrong** → Check `secrets.yaml`, re-flash
3. **Power cycling** → Check power supply stability
4. **Node in deep sleep** → This is normal! Will wake in 5 minutes and reconnect

**Solution:**
- Wait 5 minutes (node sleeps)
- Check Home Assistant shows "Online"
- If still offline: Power off, wait 5s, power on

### OTA Update Gets Stuck

**If stuck on "Preparing..." for >2 minutes:**
1. Unplug power from node
2. Wait 10 seconds
3. Power back on
4. Try OTA again

### After OTA, Sensors Show "Unavailable"

**Node needs a moment to recalibrate:**
- Give it 5 minutes (one sleep cycle)
- Sensor calibration takes time
- Pressure/IAQ take longer than temp/humidity

### OLED Goes Black But Node Still Online

1. Check display brightness setting (might be 0%)
2. Check if display component crashed (check logs)
3. Power cycle node

---

## Part F: Batch Updates (All Nodes at Once)

Once you've tested a configuration on one node, update all three:

1. **Copy working YAML to other nodes:**
   - Edit `room-node-2.yaml` to match node-1 (except room names)
   - Edit `room-node-3.yaml` similarly

2. **Push updates in sequence:**
   - Click "INSTALL" on node-1, wait for complete
   - Click "INSTALL" on node-2, wait for complete
   - Click "INSTALL" on node-3, wait for complete

3. **Verify all three show "Online"** in ESPHome dashboard

---

## Part G: Backing Up Configurations

### Automated Backup (Home Assistant)

Home Assistant automatically backs up daily. To access:

1. **Settings → System → Backups**
2. View all automatic snapshots
3. Download manually if needed

### Manual YAML Backup

Keep a copy of your working YAML files:

```bash
# From your computer
scp -r root@homeassistant.local:/config/esphome ~/backup-esphome
```

---

## Checklist: Flashing Complete

- [ ] All three nodes flashed via USB initially
- [ ] Each node appears "Online" in ESPHome dashboard
- [ ] Each node shows sensors in Home Assistant
- [ ] OTA update tested successfully
- [ ] Backup of YAML configs saved locally
- [ ] Sleep duration optimized for battery life
- [ ] Display brightness adjusted
- [ ] Temperature offsets calibrated

---

## Next Steps

Once firmware is stable:
→ Continue to **[Step 4: Dashboard & Automations Setup](04-DASHBOARD-SETUP.md)**
