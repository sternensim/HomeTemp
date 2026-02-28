# Bill of Materials (BOM)

Complete shopping list for building 3 environmental sensor nodes.

---

## Complete System (3 Nodes + 1 Hub)

### Microcontroller & Sensors

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| ESP32-WROOM-32 dev board | 3 | $4.00 | $12.00 | AliExpress | Built-in WiFi + BLE |
| BME680 I2C breakout | 3 | $7.00 | $21.00 | AliExpress | Temp + Humidity + Pressure + IAQ |
| SSD1306 0.96" OLED I2C | 3 | $1.50 | $4.50 | AliExpress | 128×64 pixel display |
| **Subtotal** | | | **$37.50** | | |

### Power & Charging

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| TP4056 LiPo charger module | 3 | $0.80 | $2.40 | AliExpress | USB charging + protection |
| 3.7V 1200mAh LiPo (JST-PH) | 3 | $4.00 | $12.00 | AliExpress | Energy dense, rechargeable |
| 5V 100mA solar panel | 3 | $3.00 | $9.00 | AliExpress | Epoxy type, outdoor-safe |
| **Subtotal** | | | **$23.40** | | |

### Wiring & Assembly

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| Breadboard (830 tie-point) | 1 | $2.00 | $2.00 | AliExpress | For prototyping |
| Dupont jumper wires (40-pin set) | 2 | $1.50 | $3.00 | AliExpress | M-M, M-F, F-F assortment |
| PCB headers & JST connectors | 1 | $3.00 | $3.00 | LCSC/AliExpress | Breakaway headers + JST |
| 100µF electrolytic capacitor | 5 | $0.10 | $0.50 | LCSC | Power smoothing (pack of 10) |
| 10µF ceramic capacitor | 5 | $0.05 | $0.25 | LCSC | Decoupling (pack of 10) |
| 10kΩ resistor (1/4W) | 5 | $0.01 | $0.05 | LCSC | Pull-up (pack of 100) |
| Stranded hookup wire (22AWG) | 1 | $2.00 | $2.00 | Local | For soldering |
| **Subtotal** | | | **$10.80** | | |

### Optional: Final Assembly

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| PCB (stripboard/perfboard) | 3 | $1.00 | $3.00 | LCSC/AliExpress | For soldering (alternative to breadboard) |
| M3 brass standoffs (pack of 20) | 1 | $3.00 | $3.00 | AliExpress | Board mounting |
| M3 screws assortment | 1 | $2.00 | $2.00 | Local | For mounting standoffs |
| Thermal silicone pads | 1 | $2.00 | $2.00 | AliExpress | Optional heatsink material |
| **Subtotal** | | | **$10.00** | | |

### Optional: 3D Printing

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| PLA filament (1kg roll) | 1 | $15.00 | $15.00 | Local/Amazon | 3 enclosures = ~60-80g |
| Acrylic sheet (clear) | 1 | $5.00 | $5.00 | Amazon | For display windows (optional) |
| Silicone sealant | 1 | $3.00 | $3.00 | Local | For weatherproofing (optional) |
| **Subtotal** | | | **$23.00** | | |

### Central Hub (Home Assistant)

| Part | Quantity | Unit Cost | Total | Supplier | Notes |
|------|----------|-----------|-------|----------|-------|
| Raspberry Pi (if buying) | 1 | $45-80 | $45-80 | Local/Amazon | Already owned in this project |
| microSD 32GB A1 class | 1 | $7.00 | $7.00 | Amazon | For HA OS installation |
| USB power supply (3A+) | 1 | $10.00 | $10.00 | Local | For Pi (may already have) |
| Ethernet cable (optional) | 1 | $3.00 | $3.00 | Local | Recommended over WiFi |
| **Subtotal** | | | **$25.00** | | |

---

## Summary by Phase

### Phase 1: Prototype (Breadboard)
**One complete node, testing only**

| Category | Cost |
|----------|------|
| Microcontroller + Sensors | $12.50 |
| Power | $7.80 |
| Wiring & parts | $10.80 |
| **Prototype Total** | **$31.10** |

### Phase 2: Final Build (All 3 Nodes)
**3 complete, soldered nodes ready for deployment**

| Category | Cost |
|----------|------|
| Microcontroller + Sensors | $37.50 |
| Power | $23.40 |
| Wiring & assembly | $20.80 |
| PCB boards & soldering supplies | ~$5.00 |
| **Final Hardware Total** | **$86.70** |

### Phase 3: Enclosures & Display
**3D printed cases + optional accessories**

| Category | Cost |
|----------|------|
| 3D printing (filament) | $1.50-3.00 |
| Acrylic windows | $3.50 (per 3) |
| Silicone sealant | $1.00 (per 3) |
| **Enclosure Total** | **$6.00-7.50** |

### Phase 4: Central Hub Setup
**Home Assistant on Raspberry Pi**

| Category | Cost |
|----------|------|
| microSD card | $7.00 |
| USB power | ~$0.00 (likely already have) |
| Ethernet cable | $3.00 |
| **HA Setup Total** | **$10.00** |

---

## Grand Total by Scenario

### Scenario A: Minimal (Breadboard, Testing Only)
- 1 node prototype, testing
- **Total: $31.10**

### Scenario B: Recommended (3 Final Nodes + HA)
- 3 complete soldered nodes
- Home Assistant setup
- **Total: $96.70 - $98.20**

### Scenario C: Complete (With Enclosures + Accessories)
- 3 complete soldered nodes in 3D printed cases
- Home Assistant setup
- Optional: acrylic windows, sealant
- **Total: $103.70 - $107.70**

---

## Sourcing by Supplier

### AliExpress (Cheapest, 2-4 week shipping)
| Item | Link |
|------|------|
| ESP32-WROOM-32 | Search: "ESP32 dev board" |
| BME680 breakout | Search: "BME680 I2C" |
| SSD1306 OLED | Search: "SSD1306 0.96 OLED I2C" |
| TP4056 charger | Search: "TP4056 LiPo charger" |
| LiPo battery | Search: "3.7V 1200mAh LiPo JST" |
| Solar panel | Search: "5V 100mA solar panel" |
| Breadboard + wires | Search: "breadboard kit" |
| PCB headers | Search: "PCB breakaway headers" |
| Enclosure (optional) | Search: "ESP32 case 3D printable" or print yourself |

**Tips:**
- Order all parts at once to reduce shipping costs
- Add 2-3 week buffer for delivery
- Buy slightly more components than needed (practice soldering!)

### LCSC (Faster, better for passive components)
| Item | Link |
|------|------|
| Capacitors | Search: "100µF capacitor", "10µF ceramic" |
| Resistors | Search: "10kΩ resistor 1/4W" |
| PCB boards | Search: "perfboard 2.54mm pitch" |
| Headers | Search: "straight header 2.54mm" |

**Tips:**
- Often has faster shipping (1-2 weeks)
- Good for bulk passive components
- Free shipping at certain thresholds

### Amazon (Fastest, more expensive)
| Item | Notes |
|------|-------|
| Breadboard kits | 2-day delivery, premium cost |
| Soldering supplies | Solder, flux, pump |
| USB cables | Data cables (charge-only won't work) |
| Power supplies | Often already have |
| microSD cards | Kingston A1 or SanDisk |

### Local/Hobbyist Shops
| Item | Notes |
|------|-------|
| Soldering iron | Weller or Hakko recommended |
| Solder | 60/40 or lead-free |
| Multimeter | For testing + debugging |
| Wire strippers | Essential for clean assembly |

---

## Cost Breakdown

### Per Node (For 3 Nodes)

| Component | % of Cost |
|-----------|-----------|
| Microcontroller (ESP32) | 13% |
| Environmental sensor (BME680) | 23% |
| Display (OLED) | 5% |
| Power system (TP4056 + battery + solar) | 48% |
| Wiring + assembly + misc | 11% |
| **Total per node** | **100%** (~$29) |

**Largest cost drivers:**
1. Power system (battery + solar) = ~$8 per node
2. BME680 sensor = ~$7 per node
3. Everything else = ~$14 per node

---

## Cost Optimization Tips

### Save Money
- Buy all 3 nodes at once → Combined shipping discount
- Skip solar panels initially → Add later, saves $9
- Use breadboard instead of soldering → Saves ~$5 per node
- Buy generic resistor/capacitor packs → Cheaper in bulk
- 3D print enclosure yourself → Saves ~$5 per node vs. buying

### No Compromise on Quality
- ❌ Don't cheap out on power supply (unstable power = headaches)
- ❌ Don't use charge-only USB cables (won't work for programming)
- ❌ Don't skip capacitors (brownout/resets)
- ✓ Do invest in good solder + soldering iron (saves rework)
- ✓ Do buy from established sellers (BME680 counterfeits exist)

---

## Expansion Parts (For Future Upgrades)

If you want to add features later:

| Feature | Part | Cost |
|---------|------|------|
| Real CO2 sensor | MH-Z19B | $15-20 |
| Particle sensor | PMS5003 | $15-20 |
| Extra node | 1× set of above | $25-30 |
| Wall display | 3.5" RPi TFT | $12-15 |
| Voice control | Google Home mini | $20 |
| Automation | Relay module + smart plug | $5-10 |

---

## Storage & Spares

Recommended to buy extra:

- [ ] 1 extra ESP32 (failures happen during flashing)
- [ ] 1 extra BME680 (sensor can fail)
- [ ] 1 extra OLED (display can break)
- [ ] 2× extra LiPo batteries (battery management needed)
- [ ] Resistor/capacitor assortment (for experiments)
- [ ] Dupont wire assortment (wires break frequently)

**Extra cost: ~$30-40**
**Total with spares: ~$130-150**

---

## Checklist Before Ordering

- [ ] Read the plan: [HomeEncironmentalMonitorPlan.md](../HomeEncironmentalMonitorPlan.md)
- [ ] Verify you have a Raspberry Pi (or budget $45-80)
- [ ] Confirm WiFi password (needed for configuration)
- [ ] Pick your rooms (where will nodes go?)
- [ ] Decide on enclosures (3D print vs. cardboard vs. none)
- [ ] Decide on solar panels (optional, adds cost)
- [ ] Check you have soldering equipment (or buy budget kit)
- [ ] Add all parts to cart (AliExpress or LCSC)
- [ ] Verify shipping to your location (some sellers don't ship worldwide)
- [ ] Proceed with order!

---

## Expected Delivery Timeline

**AliExpress (standard shipping):**
- Order day: Day 0
- Arrive: Day 14-28 (varies by region)
- **Ready to build: Week 2-4**

**LCSC (DHL express):**
- Order day: Day 0
- Arrive: Day 3-7
- **Ready to build: Week 1**

**Amazon Prime:**
- Order day: Day 0
- Arrive: Day 1-2
- **Ready to build: Immediately**

---

## Spreadsheet Template

Save this as a CSV to track your orders:

```csv
Component,Qty,Unit Cost,Total Cost,Supplier,Order Date,Arrival Date,Status
ESP32-WROOM-32,3,4.00,12.00,AliExpress,2026-02-26,2026-03-12,Ordered
BME680,3,7.00,21.00,AliExpress,2026-02-26,2026-03-12,Ordered
SSD1306 OLED,3,1.50,4.50,AliExpress,2026-02-26,2026-03-12,Ordered
TP4056,3,0.80,2.40,AliExpress,2026-02-26,2026-03-12,Ordered
LiPo Battery,3,4.00,12.00,AliExpress,2026-02-26,2026-03-12,Ordered
Solar Panel,3,3.00,9.00,AliExpress,2026-02-26,2026-03-12,Ordered
Breadboard Kit,1,3.50,3.50,AliExpress,2026-02-26,2026-03-12,Ordered
microSD Card,1,7.00,7.00,Amazon,2026-02-26,2026-02-27,Delivered
,,TOTAL,71.40
```

---

## Summary

**For a complete, working system:**
- **Minimum budget:** $96.70 (without enclosures)
- **Recommended budget:** $110 (with enclosures)
- **With spares:** $140-150

**Best value:** Order entire kit from AliExpress at once → ~2-3 week wait, significant savings vs. piecemeal purchasing.

Good luck with your build! 🎉

---

**Next:** [QUICK-START.md](../QUICK-START.md) — Follow step-by-step instructions once hardware arrives.
