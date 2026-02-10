# 🔧 Hardware Assembly Bhai | Wiring Kaise Karein

**Ekdum dhire dhire, har wire kahan lagegi**

---

## ⚠️ Shuru Karne Se Pehle

1. ✅ Saara saman table par rakho
2. ✅ Achi roshni mein kaam karo
3. ✅ ESP32 ko USB se **connect mat** karo abhi
4. ⚠️ Wiring ke dauran power OFF rakho

---

## 📌 ESP32 Master - Pin Diagram Samjho

```
ESP32 DevKit V1 - Top View

                    ┌────────────┐
              3V3 ─┤1        30├─ GND
              EN  ─┤2        29├─ GPIO23
          GPIO36  ─┤3        28├─ GPIO22
          GPIO39  ─┤4        27├─ GPIO1 (TX)
          GPIO34  ─┤5        26├─ GPIO3 (RX)
          GPIO35  ─┤6        25├─ GPIO21
          GPIO32  ─┤7        24├─ GND
          GPIO33  ─┤8        23├─ GPIO19
          GPIO25  ─┤9        22├─ GPIO18
          GPIO26  ─┤10       21├─ GPIO5
          GPIO27  ─┤11       20├─ GPIO17 (TX2)
          GPIO14  ─┤12       19├─ GPIO16 (RX2)
          GPIO12  ─┤13       18├─ GPIO4
          GND     ─┤14       17├─ GPIO0
          GPIO13  ─┤15       16├─ GPIO2
                    └────────────┘
```

---

## 🎤 Part 1: INMP441 Microphone Jodo

### INMP441 ke Pins:
- **VDD** = Power (3.3V)
- **GND** = Ground
- **SD** = Serial Data
- **WS** = Word Select (Left/Right)
- **SCK** = Serial Clock
- **L/R** = Left/Right Select

### Connections:

| INMP441 Pin | → | ESP32 Pin | Wire Color (suggest) |
|-------------|---|-----------|---------------------|
| VDD | → | 3V3 | **Red** |
| GND | → | GND | **Black** |
| SD | → | GPIO32 | Blue |
| WS | → | GPIO25 | Green |
| SCK | → | GPIO33 | Yellow |
| L/R | → | GND | **Black** |

### Step-by-Step:

1. **INMP441 ko breadboard mein lagao**
   - Breadboard ke kisi ek end mein insert karo
   - Sare pins alag-alag rows mein hone chahiye

2. **Power connections (RED aur BLACK wires)**
   - Red jumper: INMP441 VDD → ESP32 3V3
   - Black jumper: INMP441 GND → ESP32 GND
   - Black jumper: INMP441 L/R → ESP32 GND (Left channel select)

3. **Data connections**
   - Blue jumper: INMP441 SD → ESP32 GPIO32
   - Green jumper: INMP441 WS → ESP32 GPIO25
   - Yellow jumper: INMP441 SCK → ESP32 GPIO33

✅ **Check**: Total 6 wires lagne chahiye INMP441 par

---

## 🔊 Part 2: PCM5102A DAC Module Jodo

### PCM5102A ke Important Pins:
- **VIN** = Power (3.3V ya 5V)
- **GND** = Ground
- **BCK** = Bit Clock
- **DIN** = Data In
- **LCK** = Left/Right Clock (Word Select)
- **FMT**, **XMT**, **FLT** = Configuration pins

### Connections:

| PCM5102A Pin | → | ESP32 Pin | Wire Color |
|--------------|---|-----------|-----------|
| VIN | → | 3V3 | **Red** |
| GND | → | GND | **Black** |
| BCK | → | GPIO26 | Orange |
| DIN | → | GPIO22 | Purple |
| LCK | → | GPIO27 | Brown |
| FMT | → | GND | **Black** |
| XMT | → | 3V3 | **Red** |

### Configuration Jumpers (PCM5102A board par):
- **H1L** (FLT): Solder karo ya jumper lagao
- **H2L** (DEMP): Leave open (kuch nahi)
- **H3L** (XSMT): 3V3 se connect
- **H4L** (FMT): GND se connect

### Step-by-Step:

1. **PCM5102A ko breadboard mein lagao**
   - INMP441 se dur, dusri side mein

2. **Power connections**
   - Red wire: PCM5102A VIN → ESP32 3V3
   - Black wire: PCM5102A GND → ESP32 GND

3. **Audio data connections**
   - Orange wire: PCM5102A BCK → ESP32 GPIO26
   - Purple wire: PCM5102A DIN → ESP32 GPIO22
   - Brown wire: PCM5102A LCK → ESP32 GPIO27

4. **Configuration pins**
   - Black wire: PCM5102A FMT → ESP32 GND
   - Red wire: PCM5102A XMT → ESP32 3V3

5. **Speaker connection**
   - PCM5102A LOUT → Speaker Positive (+)
   - PCM5102A GND → Speaker Negative (-)

✅ **Check**: Total 7 wires + 2 speaker wires

---

## 🔘 Part 3: Button Aur LED Jodo

### Button Connection:

| Component | → | ESP32 Pin | Wire Color |
|-----------|---|-----------|-----------|
| Button Pin 1 | → | GPIO15 | Koi bhi color |
| Button Pin 2 | → | GND | **Black** |

**Note: Internal Pull-up use karenge code mein, external resistor nahi chahiye**

### LED Connection:

```
ESP32 GPIO2 ──────┬─── 220Ω Resistor ─── LED (+) ─── LED (-) ─── GND
```

| Component | → | Connection |
|-----------|---|------------|
| GPIO2 | → | Resistor ka ek end |
| Resistor dusra end | → | LED ka Long leg (+) |
| LED Short leg (-) | → | GND |

### Step-by-Step:

1. **Button lagao**
   - Breadboard mein button insert karo
   - Ek pin se wire leke GPIO15 par
   - Dusre pin se wire leke GND par

2. **LED Circuit banao**
   - Resistor (220Ω) breadboard mein insert karo
   - Ek wire: GPIO2 → Resistor ka ek end
   - LED ka long leg (+) → Resistor ka dusra end
   - LED ka short leg (-) → GND

✅ **Check**: Button = 2 wires, LED = 2 wires + 1 resistor

---

## 📡 Part 4: ESP32-CAM UART Connection

### UART Pins (ESP32 Master ↔ ESP32-CAM):

| ESP32 Master | → | ESP32-CAM | Kya Hai |
|--------------|---|-----------|---------|
| GPIO17 (TX2) | → | U0RXD | Master data bhejta |
| GPIO16 (RX2) | → | U0TXD | Master data receive karta |
| GND | → | GND | Common ground |

⚠️ **Important**: ESP32-CAM ko alag se power do (USB cable)

### Step-by-Step:

1. **UART Cross connection**
   - Wire: ESP32 Master GPIO17 → ESP32-CAM U0RXD (pin marked RX)
   - Wire: ESP32 Master GPIO16 → ESP32-CAM U0TXD (pin marked TX)

2. **Common Ground**
   - Wire: ESP32 Master GND → ESP32-CAM GND

3. **ESP32-CAM Power**
   - ESP32-CAM ko apni USB cable se power do
   - Ya 5V aur GND wires separate se

✅ **Check**: Total 3 wires between ESP32 Master aur CAM

---

## 🔋 Part 5: Power Connections

### Development (Testing) ke liye:
- ESP32 Master: USB cable se computer
- ESP32-CAM: USB cable se computer (ya FTDI programmer)

### Final Product ke liye:
- Power Bank ka output split karo
- Ek cable ESP32 Master ko
- Ek cable ESP32-CAM ko

---

## ✅ Final Assembly Checklist

### ESP32 Master par Total Connections:

```
Power:
☐ 3V3 se 3 devices (INMP441, PCM5102A, PCM5102A-XMT)
☐ GND multiple connections

Audio Input (INMP441):
☐ GPIO32 - SD (data)
☐ GPIO25 - WS (word select)
☐ GPIO33 - SCK (clock)

Audio Output (PCM5102A):
☐ GPIO26 - BCK (bit clock)
☐ GPIO22 - DIN (data in)
☐ GPIO27 - LCK (word select)

User Interface:
☐ GPIO15 - Button
☐ GPIO2 - LED

UART (ESP32-CAM):
☐ GPIO17 (TX2) - to CAM RX
☐ GPIO16 (RX2) - to CAM TX
```

### Wire Count Check:
- **INMP441**: 6 wires
- **PCM5102A**: 7 wires
- **Speaker**: 2 wires
- **Button**: 2 wires
- **LED**: 2 wires + 1 resistor
- **UART to CAM**: 3 wires
- **Total**: ~22 connections

---

## 🎯 Agla Step

Hardware assembly complete? Mast!

👉 **Ab Jao**: [04_BACKEND_SETUP.md](04_BACKEND_SETUP.md)

Backend software setup karo!
