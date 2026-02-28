# 3D Printed Enclosure Design

This document covers the design and printing of enclosures for your sensor nodes.

---

## Design Principles

### Critical Requirements

1. **Ventilation:**
   - BME680 must have air access for accurate readings
   - Slots or holes for air circulation
   - **Face downward to prevent water ingress** (if indoor)
   - Minimum 10mm² vent area per sensor

2. **Thermal Management:**
   - ESP32 self-heats ~2-3°C
   - Avoid tight enclosures trapping heat
   - Air gaps help dissipate heat

3. **Cable Routing:**
   - Slot for USB charging cable (during development)
   - Slot for solar panel JST connector
   - Slot for battery JST connector

4. **OLED Visibility:**
   - Display window must be transparent or clear
   - Avoid reflections (matte plastic better than glossy)
   - Hole diameter: 55mm × 35mm (for 0.96" OLED)

5. **Mounting:**
   - Wall mount: Tabs or holes for M4 screws
   - Table mount: Flat bottom with rubber feet (optional)
   - Height: 60-80mm (compact, not too tall)

---

## Reference Design Dimensions

### Outer Dimensions
```
Length:  70 mm (houses PCB length)
Width:   60 mm (houses PCB width)
Height:  80 mm (includes solar + battery stack)

Wall thickness: 2 mm (strong enough for PLA)
Tolerance: ±0.5 mm
```

### Ventilation Specifications

**Sensor (BME680) side** (bottom when mounted):
- Slot dimensions: 60mm wide × 8mm tall
- Location: Bottom edge of enclosure
- Pattern: Horizontal slot (doesn't trap water)
- Optionally: Add 4× 5mm diameter holes for better airflow

**Alternative (top slot + bottom holes):**
- Top slot: 1/3 of way down
- Bottom holes: 8× 5mm holes in checkerboard pattern
- Allows convection circulation

### OLED Window

**Display size:** 27mm × 27mm (visible area)
**Window hole:** 30mm × 30mm (slight clearance)
**Bezel thickness:** 2mm (frame around hole)

Optional: Use clear acrylic window insert glued into hole

### Cable Routing

**Top rear** (hidden when mounted):
- 2× slot: USB charging (8mm wide × 12mm tall)
- 2× slot: Solar JST connector (6mm × 10mm)
- 2× slot: Battery JST connector (6mm × 10mm)

---

## Assembly Instructions

### Before Printing

1. **Choose material:**
   - **PLA:** Easiest, biodegradable, not UV resistant
   - **PETG:** Stronger, outdoor-safe, slightly harder to print
   - **ABS:** Strongest, warps more, requires heated bed

2. **Choose print settings:**
   - **Layer height:** 0.2mm (balance speed/quality)
   - **Infill:** 10% (saves filament, still sturdy)
   - **Support:** Yes (for overhangs)
   - **Bed temp:** 200°C (PLA), 230°C (PETG)
   - **Nozzle temp:** 210°C (PLA), 250°C (PETG)

3. **Estimated print time & filament:**
   - Single enclosure: ~3 hours, ~25-30g filament
   - Three enclosures: ~9 hours total
   - Cost: ~$2-3 per enclosure in filament

### Printing

1. **Slice with Cura/PrusaSlicer:**
   - Orient: Display facing up
   - Generate supports for interior cable slots
   - Preview: Check for collisions

2. **Print:**
   - Monitor first layer (critical)
   - Watch for adhesion issues
   - Check halfway through for quality

3. **Post-print:**
   - Remove supports carefully
   - Clean excess support material
   - Sand rough edges with 120-grit sandpaper
   - Optional: Prime with filler primer for smooth finish

### Assembly into Enclosure

1. **Install PCB:**
   - Mount ESP32 on small PCB or standoffs
   - Screw down with M3 standoffs
   - Ensure display window aligned with OLED

2. **Install sensors:**
   - Glue BME680 to bottom slot area (sensor facing down)
   - Glue OLED behind display window (use hot glue)
   - Position: Center, flush with window

3. **Cable management:**
   - Route USB through top slot
   - Route solar/battery JST connectors through rear slots
   - Use small zip ties to organize

4. **Optional: Install clear window:**
   - Glue transparent acrylic sheet in display hole
   - Prevents dust from entering display
   - Makes OLED more visible in bright light

5. **Seal enclosure:**
   - Apply silicone sealant around sensor vents
   - **DO NOT seal solar vent** (blocks airflow)
   - Allows moisture escape while preventing rain

---

## DIY Design (No STL)

If you don't want to print, you can 3D model yourself in:
- **Fusion 360** (free for personal use)
- **FreeCAD** (open source)
- **TinkerCAD** (web-based, beginner-friendly)

### Basic Box Design Steps

1. **Create outer box:**
   - Dimensions: 70 × 60 × 80 mm
   - Wall thickness: 2 mm

2. **Cut OLED window:**
   - Front face: 30 × 30 mm hole, centered

3. **Add vents:**
   - Bottom edge: 60 × 8 mm slot (horizontal)
   - OR: 8× 5mm holes arranged in pattern

4. **Add cable ports:**
   - Rear top: 3 small slots (USB, solar JST, battery JST)
   - Use cylinder or cube, subtract from body

5. **Add mounting tabs:**
   - Back face: 4× M4 screw holes
   - Tabs: 10mm tall above main body

6. **Round edges (optional):**
   - 1mm fillet on all edges (looks better)

---

## Alternative: No Enclosure (Open Design)

For testing/development, you can run **without enclosure**:

**Pros:**
- No printing needed
- Better airflow (more accurate sensors)
- Easier to debug/adjust wiring

**Cons:**
- Dust can collect on sensors
- Not suitable for outdoor deployment
- Looks messy

**Setup:**
- Mount on breadboard or small PCB
- Place on desk near measuring device
- Use zip ties to keep wires organized

---

## Weatherproofing (Optional, for Outdoor Nodes)

If deploying outdoors:

1. **Material:** Use PETG or ABS (more UV resistant than PLA)

2. **Sealing:**
   - Apply silicone sealant to all joints
   - Use O-rings on cable ports
   - Drill drain holes in bottom (let water escape)

3. **Ventilation still needed:**
   - Keep BME680 vent slot open
   - Use hydrophobic membrane (lets air through, blocks water)
   - Available: 3M Gortex or PTFE membranes

4. **Solar mounting:**
   - Angle panel ~30° from horizontal
   - Face south (northern hemisphere)
   - Direct sunlight minimum 4 hours/day

5. **Lightning protection (optional):**
   - Add diode on solar input (protects from reverse voltage)
   - Ground metal mounting bracket
   - Not critical unless in storm-prone area

---

## Filament Cost Calculation

**Per enclosure (PLA):**
- Infill 10%, no supports: ~20g
- Cost: $0.40-0.60 per enclosure

**Three enclosures:**
- Total filament: ~60g
- Total cost: $1.50-2.00

**Plus optional:**
- Acrylic window sheets: $5 per 2 sheets
- Silicone sealant: $3 per tube
- Mounting hardware (screws, standoffs): $2-3

---

## Recommended STL Resources

You can find pre-made enclosure STLs on:

1. **Thingiverse:** https://www.thingiverse.com/
   - Search: "ESP32 sensor enclosure"
   - Many options, check ratings/reviews

2. **Printables:** https://www.printables.com/
   - Free and paid models
   - Often better quality than Thingiverse

3. **Custom design services:**
   - **Fiverr:** Commission custom design (~$20-50)
   - **Freelancer:** Similar rates
   - Provide dimensions and requirements

---

## Quick Enclosure: Cardboard Version (Temporary)

For initial testing **before printing**:

1. **Use cardboard box:**
   - Cut to size (~80 × 70 × 60mm)
   - Hot glue sides together

2. **Cut windows:**
   - Display window: 30mm square on front
   - Sensor vent: 60 × 8mm slot on bottom

3. **Mount components inside:**
   - Hot glue or double-sided tape
   - Keep wires tidy

4. **Cardboard isn't weatherproof**, but fine for indoor testing

---

## Enclosure Optimization

### Reduce Print Time
- Lower infill to 5% (still sturdy enough)
- Increase layer height to 0.3mm
- Print without supports (add internal ribs instead)

### Improve Quality
- Use finer layer height (0.1mm) for visible surfaces
- Add textured finish (sand after printing)
- Paint with acrylic paint for better look

### Save Filament
- Thin walls to 1.5mm (if still rigid with supports)
- Minimize support material placement
- Use tree supports (Cura plugin) instead of linear

---

## Checklist

- [ ] Enclosure design finalized (dimensions approved)
- [ ] STL file sliced in Cura/PrusaSlicer
- [ ] Print settings verified (temp, speed, supports)
- [ ] First enclosure printed successfully
- [ ] Post-processing complete (supports removed, edges sanded)
- [ ] PCB/sensors installed and tested
- [ ] Ventilation confirmed working (OLED showing readings)
- [ ] Cable routing clean and organized
- [ ] All three enclosures printed and assembled
- [ ] Optional: Acrylic windows installed
- [ ] Optional: Silicone sealing applied

---

## Next Steps

Once enclosure is complete:
→ Continue with final assembly and deployment

---

## Design Files

If you want to start from scratch, create these in your CAD tool:

**Fusion 360 template workflow:**
1. Sketch → Draw rectangle 70 × 60mm (front face)
2. Pad → Extrude 80mm height (outer box)
3. Sketch on front → Circle 30mm (display window)
4. Pocket → Cut display window 2mm deep
5. Create slots for vents and cables using Sketch + Pocket
6. Add mounting tabs (Pad on back)
7. Export as STL

**FreeCAD workflow:**
1. Part → Create box (70 × 60 × 80)
2. Subtractive features → Pocket (display window)
3. Add Pocket for vents
4. Add Pad for mounting tabs
5. File → Export as STL
