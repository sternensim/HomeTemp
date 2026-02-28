# Docker Setup Guide (Alternative to Home Assistant OS)

**Use this guide if you want to run Home Assistant in Docker instead of on bare Raspberry Pi OS.**

---

## Prerequisites

You need:
- Raspberry Pi (3B+ or newer) with Raspberry Pi OS installed
- Docker and Docker Compose installed
- This project folder: `HomeAutomation/`
- About 5GB free disk space

---

## Step 1: Install Docker & Docker Compose

SSH into your Raspberry Pi and run:

```bash
# Install Docker
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh

# Add your user to docker group (avoid sudo for every command)
sudo usermod -aG docker $USER
newgrp docker

# Verify installation
docker --version
docker-compose --version
```

---

## Step 2: Prepare Project Folders

On your Pi, clone or copy the `HomeAutomation` project:

```bash
# If cloning from GitHub/repo
git clone <your-repo-url> ~/HomeAutomation
cd ~/HomeAutomation

# Or if copying manually, ensure folder structure is:
# HomeAutomation/
# ├── docker-compose.yml
# ├── .env.example
# ├── esphome/
# │   ├── secrets.yaml
# │   ├── room-node-1.yaml
# │   ├── room-node-2.yaml
# │   └── room-node-3.yaml
# └── home-assistant/
#     ├── config/
#     └── automations.yaml
```

---

## Step 3: Configure Environment

```bash
cd ~/HomeAutomation

# Copy example env file
cp .env.example .env

# Edit for your setup (timezone, ports, etc.)
nano .env
```

**Key settings to review:**
- `TZ`: Your timezone (e.g., `Europe/Berlin`, `America/New_York`)
- `HA_PORT`: Home Assistant web UI port (default 8123)
- `ESPHOME_PORT`: ESPHome dashboard port (default 6052)

---

## Step 4: Create Home Assistant Config Directory

```bash
# Create config folder for Home Assistant
mkdir -p home-assistant/config

# Copy automations and configuration files
cp home-assistant/automations.yaml home-assistant/config/
cp home-assistant/configuration-additions.yaml home-assistant/config/

# Home Assistant will auto-create configuration.yaml on first run
```

---

## Step 5: Configure ESPHome Secrets

Edit your ESPHome credentials:

```bash
nano esphome/secrets.yaml
```

Replace with your actual WiFi credentials:

```yaml
wifi_ssid: "YOUR_WIFI_SSID"
wifi_password: "YOUR_WIFI_PASSWORD"
api_key: "YOUR_API_KEY"
ota_password: "YOUR_OTA_PASSWORD"
```

Generate a secure API key:

```bash
python3 -c "import secrets; print(secrets.token_hex(16))"
```

---

## Step 6: Start Services

```bash
cd ~/HomeAutomation

# Start all services (home-assistant, esphome, portainer)
docker-compose up -d

# Check status
docker-compose ps
```

**Expected output:**
```
CONTAINER ID  IMAGE                           STATUS
abc123        homeassistant/home-assistant    Up 2 seconds
def456        esphome/esphome                 Up 2 seconds
ghi789        portainer/portainer-ce          Up 2 seconds
```

---

## Step 7: Access Home Assistant

### First Time Setup

1. Open browser: `http://<pi-ip>:8123` or `http://homeassistant.local:8123`
2. Wait 2-3 minutes for Home Assistant to initialize (first boot is slow)
3. Create user account (username + password)
4. Complete onboarding setup
5. Go to **Settings → Add-ons** (you won't have add-ons in Docker version)

---

## Step 8: Configure ESPHome Integration

Since ESPHome runs as separate container, you need to connect it to Home Assistant:

### Option A: Manual Integration (Recommended)

1. Open Home Assistant: **Settings → Devices & Services**
2. Click **Create Integration**
3. Search for **ESPHome**
4. Enter ESPHome server address: `esphome:6052`
   - (Container name `esphome` resolves internally on Docker network)
5. Click **Connect**

### Option B: Auto-Discovery

If auto-discovery works, ESPHome devices should appear automatically after ~1 minute.

---

## Step 9: Configure Home Assistant

Copy required configuration additions to your `configuration.yaml`:

```bash
cat home-assistant/config/configuration-additions.yaml
```

Add these sections to:
`~/HomeAutomation/home-assistant/config/configuration.yaml`

Sections to add:
- `automation: !include automations.yaml`
- `logger:` settings
- `recorder:` settings
- `homeassistant:` unit definitions

**Restart Home Assistant after changes:**

```bash
docker-compose restart homeassistant
```

---

## Step 10: Upload ESPHome Configs

ESPHome dashboard is at: `http://<pi-ip>:6052`

1. Open ESPHome dashboard
2. Verify three device cards appear:
   - Living Room Node
   - Bedroom Node
   - Office Node
3. If not showing, check logs:

```bash
docker-compose logs esphome
```

---

## Step 11: Flash First Node (USB)

**For initial USB flashing (before WiFi works):**

First, enable USB device access in Docker. Uncomment in `docker-compose.yml`:

```yaml
devices:
  - /dev/ttyUSB0:/dev/ttyUSB0    # First USB device
```

Then restart:

```bash
docker-compose restart esphome
```

Now connect ESP32 via USB and flash via ESPHome dashboard.

---

## Daily Management Commands

### View Logs

```bash
# All services
docker-compose logs -f

# Home Assistant only
docker-compose logs -f homeassistant

# ESPHome only
docker-compose logs -f esphome

# Last 50 lines
docker-compose logs --tail 50 homeassistant
```

### Stop/Start Services

```bash
# Stop all
docker-compose down

# Start all
docker-compose up -d

# Restart specific service
docker-compose restart homeassistant
```

### Backup Configuration

```bash
# Backup all configs
tar -czf homeautomation-backup-$(date +%Y%m%d).tar.gz \
  esphome/ home-assistant/config/

# List backups
ls -lh *.tar.gz
```

### Update Docker Images

```bash
# Pull latest images
docker-compose pull

# Restart services with new images
docker-compose up -d
```

---

## Troubleshooting

### Home Assistant Won't Start

```bash
docker-compose logs homeassistant | tail -20
```

Common issues:
1. **Disk full:** `df -h` to check
2. **Port conflict:** Change `HA_PORT` in `.env`
3. **Permission error:** Ensure user is in docker group: `groups $USER`

### ESPHome Can't See Nodes

```bash
docker-compose logs esphome | grep -i error
```

Check:
1. ESPHome network on same Docker network (automatic)
2. ESPHome integration configured in HA
3. WiFi credentials correct in `secrets.yaml`

### Nodes Won't Connect to WiFi

```bash
# Check ESPHome logs
docker-compose logs esphome

# Restart ESPHome service
docker-compose restart esphome
```

Verify:
1. `secrets.yaml` has correct WiFi SSID/password
2. WiFi is 2.4GHz (not 5GHz only)
3. Node is close to router

### Port Already in Use

If port 8123 or 6052 is already used, change in `.env`:

```bash
HA_PORT=8124
ESPHOME_PORT=6053
```

Then restart:

```bash
docker-compose up -d
```

---

## Comparison: Docker vs Home Assistant OS

| Aspect | Docker | HA OS |
|--------|--------|-------|
| **Setup time** | 30-45 min | 15 min |
| **Complexity** | Medium | Easy |
| **Linux knowledge needed** | Yes | No |
| **Flexibility** | High (run other services) | Low (HA only) |
| **Updates** | Manual (`docker-compose pull`) | Automatic |
| **Add-ons** | Limited (must run separately) | Full support |
| **Resource usage** | Lighter | Heavier |
| **Backup/Restore** | Easy (copy folders) | Easier (snapshot) |

---

## Optional: Add More Services

You can extend `docker-compose.yml` with other services:

### InfluxDB (Time-series database)

```yaml
influxdb:
  image: influxdb:2.6
  container_name: influxdb
  restart: unless-stopped
  ports:
    - "8086:8086"
  volumes:
    - ./influxdb/data:/var/lib/influxdb2
  networks:
    - homeautomation
```

### Grafana (Data visualization)

```yaml
grafana:
  image: grafana/grafana:latest
  container_name: grafana
  restart: unless-stopped
  ports:
    - "3000:3000"
  volumes:
    - ./grafana/data:/var/lib/grafana
  networks:
    - homeautomation
  depends_on:
    - influxdb
```

Then update `docker-compose up -d` to restart with new services.

---

## Next Steps

1. **Continue with Phase 2:** Build first node ([02-NODE-SETUP.md](02-NODE-SETUP.md))
2. **Flash first node:** Follow [03-FIRMWARE-FLASHING.md](03-FIRMWARE-FLASHING.md)
3. **Create dashboard:** Follow [04-DASHBOARD-SETUP.md](04-DASHBOARD-SETUP.md)
4. **Troubleshooting:** See [05-TROUBLESHOOTING.md](05-TROUBLESHOOTING.md)

---

## Useful Docker Compose Commands Reference

```bash
# View containers
docker ps

# View full Docker compose status
docker-compose ps

# View specific service logs (live)
docker-compose logs -f <service>

# Enter container shell
docker-compose exec <service> /bin/bash

# Show resource usage
docker stats

# Clean up unused images/volumes
docker system prune

# Remove everything (careful!)
docker-compose down -v
```

---

## Support

**Issues with Docker setup?**
- Check [05-TROUBLESHOOTING.md](05-TROUBLESHOOTING.md) for common errors
- View Docker logs: `docker-compose logs -f`
- Check Docker daemon: `sudo journalctl -u docker -n 50`

---

**Next:** Proceed to [02-NODE-SETUP.md](02-NODE-SETUP.md) when ready to build first node.
