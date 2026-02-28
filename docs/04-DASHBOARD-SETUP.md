# Step 4: Dashboard & Automations Setup

This guide covers creating your Home Assistant dashboard and automations to monitor and alert on environmental conditions.

---

## Part A: Create Lovelace Dashboard

Home Assistant's Lovelace UI is the visual dashboard for monitoring your sensors.

### Method 1: Create Dashboard via UI (Recommended)

1. **Open Home Assistant:**
   - Go to `http://homeassistant.local:8123`
   - Log in

2. **Create new dashboard:**
   - Click the **Dashboards** icon (left sidebar)
   - Click **Create Dashboard**
   - Give it a name: "Home Environment Monitor"
   - Click **Create**

3. **Add cards to dashboard:**
   - Click the **pencil icon** (top right) to enter edit mode
   - Click **+ Add Card**
   - Choose card type

### Popular Card Types for Environmental Monitoring

#### Gauge Card (IAQ Score)

Shows a visual gauge with color zones.

1. Click **+ Add Card**
2. Search for **"Gauge"**
3. Select **"Gauge"**
4. Fill in:
   - **Entity:** `sensor.living_room_node_iaq_score`
   - **Name:** "IAQ Score"
   - **Min:** 0
   - **Max:** 500
   - **Segments** (color zones):
     - 0-50: Green (#00c853) — Good
     - 50-100: Yellow (#ffd600) — Average
     - 100-150: Orange (#ff6d00) — Little Bad
     - 150+: Red (#d50000) — Bad

5. Click **Save**
6. Repeat for Bedroom and Office nodes

#### Entities Card (Quick Overview)

Shows all sensor readings in a list.

1. Click **+ Add Card**
2. Search for **"Entities"**
3. Select **"Entities"**
4. **Title:** "Living Room Sensors"
5. Click **Entities** field, then search and add:
   - Temperature
   - Humidity
   - Pressure
   - CO2 Equivalent
   - IAC Score

6. Click **Save**

#### History Graph Card (24-Hour Trends)

Shows sensor history over time.

1. Click **+ Add Card**
2. Search for **"History-stats"** or **"Custom:mini-graph-card"** (if HACS installed)
3. Add entities to chart:
   - Temperature (all rooms)
   - Humidity (all rooms)
   - IAQ Score (all rooms)

---

### Method 2: Import Pre-Built Dashboard (YAML)

Use the provided `lovelace-dashboard.yaml`:

1. **Copy dashboard YAML to HA config:**
   - SSH to Pi or use File Editor add-on
   - Navigate to `/config/`
   - Create file: `ui-lovelace.yaml`
   - Paste content from `home-assistant/lovelace-dashboard.yaml`

2. **Enable in Home Assistant:**
   - Edit `/config/configuration.yaml`
   - Add: `lovelace: !include ui-lovelace.yaml`
   - Restart HA: **Settings → System → Restart**

3. **Dashboard should now appear:**
   - Click **Dashboards** icon in sidebar
   - Click **Home Environment Monitor**

---

## Part B: Set Up Automations

Automations notify you when conditions are unhealthy.

### Method 1: Create Automations via UI

1. **Open automations:**
   - Go to **Settings → Automations & Scenes**
   - Click **Create Automation**

2. **Create IAQ Alert Automation:**
   - **Name:** "Alert: Poor Air Quality — Living Room"
   - **Trigger:** Numeric State
     - Entity: `sensor.living_room_node_iaq_score`
     - Condition: **above** 150
   - **Action:** Create Notification
     - Title: "Poor Air Quality"
     - Message: "IAQ score is {{ states('sensor.living_room_node_iaq_score') }}. Open a window!"
   - **Save**

3. **Repeat for all rooms** with different entities

### Method 2: Import Pre-Built Automations (YAML)

1. **Enable YAML automations:**
   - Edit `/config/configuration.yaml`
   - Add: `automation: !include automations.yaml`
   - Restart HA

2. **Create automations.yaml:**
   - Copy content from `home-assistant/automations.yaml`
   - Place at `/config/automations.yaml`
   - Restart HA

3. **View automations:**
   - Go to **Settings → Automations & Scenes**
   - All automations from YAML appear here

---

## Part C: Common Automation Triggers

### Temperature Thresholds

```yaml
trigger:
  platform: numeric_state
  entity_id: sensor.living_room_node_temperature
  above: 28  # Too hot
  # or: below: 16  # Too cold
```

### Humidity Thresholds

```yaml
trigger:
  platform: numeric_state
  entity_id: sensor.living_room_node_humidity
  above: 70  # Too humid, risk of mold
  # or: below: 30  # Too dry, uncomfortable
```

### IAQ Score Thresholds

```yaml
trigger:
  platform: numeric_state
  entity_id: sensor.living_room_node_iaq_score
  above: 150  # Poor air quality
```

### Time-Based Automation (Morning Report)

```yaml
trigger:
  platform: time
  at: "07:00:00"  # Every morning at 7 AM

action:
  service: persistent_notification.create
  data:
    title: "Morning Environment Report"
    message: >
      Living Room: {{ states('sensor.living_room_node_temperature') }}°C,
      {{ states('sensor.living_room_node_humidity') }}% humidity,
      IAQ: {{ states('sensor.living_room_node_iaq_score') }}
```

---

## Part D: Notification Options

### In-Home Notifications (Persistent)

Shows blue notification banner in HA UI:

```yaml
action:
  service: persistent_notification.create
  data:
    title: "Air Quality Alert"
    message: "IAQ score is unhealthy"
    notification_id: "iaq_alert"  # Unique ID
```

Clear notification:
```yaml
action:
  service: persistent_notification.dismiss
  data:
    notification_id: "iaq_alert"
```

### Mobile Notifications (If using HA Companion App)

```yaml
action:
  service: notify.mobile_app_your_phone
  data:
    title: "Air Quality Alert"
    message: "IAQ is poor. Open a window!"
```

### HA Cloud / Webhooks

For external services (Slack, Discord, etc.), see HA documentation on webhooks and IFTTT.

---

## Part E: Dashboard Layout Tips

### Organize by Room

Create separate view tabs:

```yaml
views:
  - title: Living Room
    icon: mdi:sofa
    cards:
      # Living room cards here

  - title: Bedroom
    icon: mdi:bed
    cards:
      # Bedroom cards here

  - title: Office
    icon: mdi:desk
    cards:
      # Office cards here
```

### Show Historical Data

Use **mini-graph-card** (from HACS) for compact graphs:

```yaml
type: custom:mini-graph-card
title: "Temperature (24h)"
entities:
  - sensor.living_room_node_temperature
hours_to_show: 24
points_per_hour: 2
show:
  graph: line
```

### Add Badges (Quick Status)

Small status indicators at top of view:

```yaml
badges:
  - entity: binary_sensor.living_room_node_status
    name: "Living Room"
  - entity: sensor.living_room_node_iaq_score
    name: "IAQ"
```

---

## Part F: Advanced: Scripts & Automations

### Script: Notify All Poor Air Quality

Create a script that runs when ANY room has poor IAQ:

```yaml
# In scripts.yaml or via UI
notify_poor_air_quality:
  alias: "Notify: Poor Air Quality in Any Room"
  sequence:
    - service: persistent_notification.create
      data:
        title: "Poor Air Quality"
        message: |
          Living Room: {{ states('sensor.living_room_node_iaq_score') }}
          Bedroom: {{ states('sensor.bedroom_node_iaq_score') }}
          Office: {{ states('sensor.office_node_iaq_score') }}
          Please open windows to improve ventilation.
```

Then use in multiple automations:

```yaml
trigger:
  platform: numeric_state
  entity_id:
    - sensor.living_room_node_iaq_score
    - sensor.bedroom_node_iaq_score
    - sensor.office_node_iaq_score
  above: 150

action:
  service: script.notify_poor_air_quality
```

### Conditional Automations (Only Alert During Night)

```yaml
trigger:
  platform: numeric_state
  entity_id: sensor.living_room_node_humidity
  above: 70

condition:
  - condition: time
    after: "22:00:00"
    before: "08:00:00"

action:
  service: persistent_notification.create
  data:
    title: "High Humidity at Night"
    message: "Humidity is {{ states('sensor.living_room_node_humidity') }}%"
```

---

## Part G: Troubleshooting Dashboard Issues

### Sensors Show "Unavailable"

**Likely causes:**
1. Node is in deep sleep (normal, will show available after waking)
2. WiFi disconnected
3. Node lost connection to HA

**Solution:**
- Wait 5 minutes (one sleep cycle)
- Check WiFi signal: **Settings → Devices → Node → Network**
- Restart node via ESPHome dashboard

### History Graph Empty

**Data not yet recorded:**
1. Recorder must be enabled (default in HA)
2. Give it 24 hours to collect data
3. Check: **Settings → Automations → Recorder logs**

**Solution:**
- Configure `recorder` component in `configuration.yaml`:
  ```yaml
  recorder:
    purge_keep_days: 30
  ```

### Card Not Updating

1. Refresh page: `Ctrl+Shift+R` (hard refresh)
2. Check entity name matches exactly:
   - Use **Settings → Devices → Sensors** to copy exact name
3. Edit card, verify entity is correct

### Dashboard Not Loading

1. If using YAML-based dashboard:
   - Check YAML syntax (use online YAML validator)
   - Verify file path is correct
   - Restart HA: **Settings → System → Restart**

2. If created via UI:
   - Hard refresh: `Ctrl+Shift+R`
   - Clear browser cache

---

## Part H: Performance Tips

### Reduce Update Frequency (Save Resources)

If your Pi is slow, reduce sensor update frequency:

**In ESPHome YAML:**
```yaml
sensor:
  - platform: bme680_bsec
    temperature:
      update_interval: 30s  # Default is 10s
```

Or increase sleep time:
```yaml
deep_sleep:
  sleep_duration: 15min  # Instead of 5min
```

### Limit History (Save Disk Space)

```yaml
# In configuration.yaml
recorder:
  purge_keep_days: 7  # Keep only 1 week instead of 30 days
```

---

## Part I: Useful HA Links

- **Entities card docs:** https://www.home-assistant.io/dashboards/entities/
- **Gauge card docs:** https://www.home-assistant.io/dashboards/gauge/
- **Automation docs:** https://www.home-assistant.io/automations/
- **ESPHome integration:** https://www.home-assistant.io/integrations/esphome/
- **HACS (add more cards):** https://hacs.xyz/

---

## Checklist: Dashboard Complete

- [ ] Lovelace dashboard created with overview, history, and system views
- [ ] All three nodes show sensor values
- [ ] Gauge cards display with color zones
- [ ] History graphs show 24-hour trends
- [ ] Automations trigger on poor IAQ
- [ ] Notifications display when conditions are unhealthy
- [ ] Mobile app notifications working (if available)
- [ ] Temperature and humidity automations configured

---

## Final Verification

1. **All sensors visible:**
   - Go to **Settings → Devices & Services**
   - Each node shows 7-8 entities

2. **Dashboard populated:**
   - All rooms visible with live readings
   - Graphs show history

3. **Automations working:**
   - Manually trigger automation (click play icon)
   - Verify notification appears

4. **Nodes online:**
   - All three nodes show "Online" in ESPHome dashboard
   - WiFi signal shows (-70dBm or better)

---

**Congratulations!** Your system is now fully operational. All hardware and software is ready. Time to buy components and deploy!

→ Next: **Order hardware from budget-summary in plan, assemble nodes, deploy to rooms.**
