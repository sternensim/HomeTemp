# Step 1: Home Assistant Installation & Setup

This guide walks you through setting up Home Assistant OS on your Raspberry Pi and installing the ESPHome add-on.

## Prerequisites

- Raspberry Pi (3B+, 4, or 5) with Ethernet or WiFi capability
- microSD card (32GB recommended, A1 class)
- USB card reader
- Computer with internet access
- Home Assistant Installer (download from https://www.home-assistant.io/installation/)

---

## Part A: Install Home Assistant OS on Raspberry Pi

### Step 1.1: Download & Flash SD Card

1. **Download Home Assistant OS image for your Raspberry Pi:**
   - Go to: https://www.home-assistant.io/installation/raspberry-pi
   - Download the image for your Pi model (e.g., Raspberry Pi 4)

2. **Flash the image to microSD:**
   - Windows: Use **Balena Etcher** (https://www.balena.io/etcher/)
   - macOS: Use **Balena Etcher** or `dd` command
   - Linux: Use **Balena Etcher** or `dd` command

   **Using Balena Etcher:**
   - Insert microSD into card reader
   - Open Balena Etcher
   - Select the downloaded `.img.xz` file
   - Select the microSD card
   - Click "Flash" and wait (~5-10 minutes)
   - Eject card when complete

### Step 1.2: Boot Home Assistant

1. Insert microSD into Raspberry Pi
2. Power on the Pi (use official 5V 3A power supply)
3. Wait 3-5 minutes for first boot (blue LED activity indicates setup)
4. Check router DHCP list or use ARP scanner to find Pi's IP

   **Find Pi's IP:**
   ```bash
   # Linux/macOS
   arp-scan -l | grep -i raspberry

   # Windows (Command Prompt as Admin)
   arp -a
   # Look for device starting with "dc:a6:32" (Raspberry Pi OUI)
   ```

5. Open browser and navigate to: `http://homeassistant.local:8123`
   - If that doesn't work, try: `http://<PI_IP>:8123`

### Step 1.3: Initial Setup

1. **Create account:**
   - Set username (e.g., "admin") and password
   - Choose onboarding options (you can skip analytics)

2. **Configure location & system:**
   - Set timezone to your location
   - Keep default settings for now

3. **Home Assistant should now be running!**

---

## Part B: Install & Configure ESPHome Add-on

### Step 2.1: Install ESPHome Add-on

1. In Home Assistant UI, go to: **Settings → Add-ons → Add-on Store**

2. Search for **"ESPHome"**

3. Click the ESPHome add-on and select **"Install"**

4. Wait for installation (~2-3 minutes)

5. Once installed, click **"Start"** to launch the service

6. Enable **"Start on boot"** toggle (so ESPHome restarts after HA reboots)

### Step 2.2: Access ESPHome Dashboard

1. In Home Assistant, go to **Settings → Add-ons → ESPHome**
2. Click the **"Open Web UI"** button (or navigate to `http://homeassistant.local:6052`)
3. You should see the ESPHome dashboard (empty, no devices yet)

### Step 2.3: Install File Editor Add-on (Optional but Recommended)

1. Go to **Settings → Add-ons → Add-on Store**
2. Search for **"File Editor"** or **"Studio Code Server"**
3. Click and **"Install"**
4. Start the add-on
5. This allows you to edit ESPHome YAML files directly in the browser

---

## Part C: Create ESPHome Secrets File

ESPHome uses a `secrets.yaml` file to store sensitive WiFi credentials and API keys.

### Step 3.1: Create secrets.yaml in ESPHome folder

1. Open **File Editor** (or SSH to Pi and edit directly)
2. Navigate to `/config/esphome/`
3. Create new file: `secrets.yaml`
4. Copy content from this project's `esphome/secrets.yaml`
5. **Fill in your values:**

```yaml
wifi_ssid: "YOUR_ACTUAL_WIFI_SSID"
wifi_password: "YOUR_ACTUAL_WIFI_PASSWORD"

# Generate random API key (run in terminal):
# python3 -c "import secrets; print(secrets.token_hex(16))"
api_key: "00000000000000000000000000000000"

ota_password: "your_secure_password_here"
```

**To generate secure random values:**
- SSH into your Pi (or use HA terminal add-on)
- Run: `python3 -c "import secrets; print(secrets.token_hex(16))"`
- Copy the output to `api_key` and `ota_password`

---

## Part D: Upload ESPHome Node Configs

### Step 4.1: Add Node Configurations to ESPHome

You have two options:

**Option 1: Upload via File Editor (Easy)**
1. Open **File Editor** add-on
2. Navigate to `/config/esphome/`
3. Click "Create new file" button
4. Create files:
   - `room-node-1.yaml`
   - `room-node-2.yaml`
   - `room-node-3.yaml`
5. Copy content from this project's `esphome/room-node-*.yaml` files
6. Save each file

**Option 2: Upload via SCP/SSH**
```bash
# From your computer with SSH enabled on Pi
scp esphome/room-node-*.yaml root@homeassistant.local:/config/esphome/
```

---

## Part E: Verify ESPHome Setup

1. Go back to ESPHome dashboard
2. You should now see three device cards:
   - **Living Room Node** (room-node-1)
   - **Bedroom Node** (room-node-2)
   - **Office Node** (room-node-3)

3. Each card shows:
   - A status indicator (orange = not yet running)
   - "COMPILE" button (to compile firmware)
   - Device name and version

**If you don't see the cards:**
- Refresh the browser (`Ctrl+Shift+R`)
- Check that the YAML files are in `/config/esphome/` with correct names
- Check Home Assistant logs: **Settings → System → Logs**

---

## Part F: Connect Raspberry Pi to Network

### Ensure Reliable WiFi/Ethernet Connection

1. **Preferred: Ethernet connection**
   - Connect Raspberry Pi directly to router with Ethernet cable
   - More stable for a central hub
   - No WiFi interference

2. **Alternative: WiFi**
   - During HA setup, connect to your WiFi network
   - 5GHz band recommended for reliability

3. **Verify connection:**
   - Go to **Settings → System → About**
   - Confirm "Home Assistant" URL shows `http://homeassistant.local:8123`

---

## Troubleshooting

### HA won't start
- Check power supply (needs 3A @ 5V for Pi 4)
- Wait 5+ minutes on first boot
- Check router DHCP for device "homeassistant"

### Can't access UI at homeassistant.local
- Try IP address directly (find with `arp-scan` or check router)
- Ensure Pi is on same network as your computer
- Restart mDNS: `sudo systemctl restart avahi-daemon` (SSH)

### ESPHome add-on won't start
- Check disk space: **Settings → System → Storage**
- Restart Home Assistant: **Settings → System → Restart**
- Check logs: **Settings → Add-ons → ESPHome → Logs**

---

## Next Steps

Once HA is running and ESPHome dashboard shows all three nodes:
→ Continue to **[Step 2: Node Setup & Wiring](02-NODE-SETUP.md)**
