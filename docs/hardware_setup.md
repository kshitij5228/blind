# Hardware Setup Guide

## Interactive AI Vision Assistant - Hardware Assembly

This guide provides detailed instructions for assembling the hardware components of the Interactive AI Vision Assistant.

---

## Components Required

### ESP32 Master Module
- 1x ESP32 DevKit V1 (with PSRAM recommended)
- 1x INMP441 I2S MEMS Microphone
- 1x PCM5102A I2S DAC Module
- 1x Push Button (momentary switch)
- 1x 10kΩ Resistor (for pull-up, if button doesn't have internal)
- 1x LED (optional, for status indication)
- 1x 220Ω Resistor (for LED)

### ESP32-CAM Module
- 1x ESP32-CAM (AI-Thinker)
- 1x OV2640 Camera (usually included with ESP32-CAM)
- 1x Micro SD Card (8GB or larger, Class 10 recommended)
- 1x MB Programmer or FTDI adapter (for initial programming)

### Power Supply
- 1x 5V Power Supply (2A minimum)
- Power distribution board or breadboard

### Connectivity
- Jumper wires (male-to-male, male-to-female)
- Breadboard (for prototyping) or custom PCB

---

## Pin Connection Tables

### ESP32 Master Connections

#### INMP441 Microphone
| INMP441 Pin | ESP32 Pin | Description |
|-------------|-----------|-------------|
| VDD | 3.3V | Power supply |
| GND | GND | Ground |
| SCK | GPIO26 | I2S Serial Clock |
| WS | GPIO25 | I2S Word Select |
| SD | GPIO33 | I2S Serial Data |
| L/R | GND | Left channel (connect to GND) |

#### PCM5102A DAC
| PCM5102A Pin | ESP32 Pin | Description |
|--------------|-----------|-------------|
| VIN | 5V | Power supply |
| GND | GND | Ground |
| BCK | GPIO14 | I2S Bit Clock |
| LCK (WS) | GPIO27 | I2S Word Select |
| DIN | GPIO12 | I2S Data Input |
| SCK | GND | System Clock (tie to GND for auto-config) |
| FMT | GND | Format selection (GND = I2S) |
| XMT | 3.3V | Soft mute control (3.3V = unmuted) |

#### Button
| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| Button (one side) | GPIO4 | Digital input |
| Button (other side) | GND | Ground |
| Pull-up resistor | Built-in | Use internal pull-up |

#### Status LED (Optional)
| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| LED (Anode via resistor) | GPIO2 | Digital output |
| LED (Cathode) | GND | Ground |

#### ESP32-CAM UART Connection
| ESP32 Master | ESP32-CAM | Description |
|--------------|-----------|-------------|
| GPIO16 (RX2) | GPIO1 (TX) | UART receive |
| GPIO17 (TX2) | GPIO3 (RX) | UART transmit |
| GND | GND | Common ground |

---

### ESP32-CAM Connections

#### Camera (OV2640)
The camera is connected via the dedicated camera interface on the ESP32-CAM board. No external wiring needed - the camera module plugs directly into the board connector.

#### SD Card
Insert the Micro SD card into the SD card slot on the ESP32-CAM board. No external wiring needed.

#### UART to ESP32 Master
| ESP32-CAM Pin | ESP32 Master Pin | Description |
|---------------|------------------|-------------|
| GPIO1 (TX) | GPIO16 (RX2) | UART transmit |
| GPIO3 (RX) | GPIO17 (TX2) | UART receive |
| GND | GND | Common ground |

#### Power Supply
| ESP32-CAM Pin | Power Source | Description |
|---------------|--------------|-------------|
| 5V | 5V Power Supply | Power input |
| GND | GND | Ground |

**Important:** ESP32-CAM requires a stable 5V power supply with sufficient current (minimum 500mA, recommended 1A).

---

## Assembly Instructions

### Step 1: Prepare the Breadboard

1. Place both ESP32 boards on the breadboard with adequate spacing
2. Ensure power rails are connected to your 5V power supply
3. Connect all GND pins to the common ground rail

### Step 2: ESP32 Master Assembly

1. **Connect INMP441 Microphone:**
   - Connect VDD to 3.3V rail
   - Connect GND to ground rail
   - Connect SCK to GPIO26
   - Connect WS to GPIO25
   - Connect SD to GPIO33
   - Connect L/R to GND (configures as left channel)

2. **Connect PCM5102A DAC:**
   - Connect VIN to 5V rail
   - Connect GND to ground rail
   - Connect BCK to GPIO14
   - Connect LCK to GPIO27
   - Connect DIN to GPIO12
   - Connect SCK to GND
   - Connect FMT to GND
   - Connect XMT to 3.3V

3. **Connect Button:**
   - Connect one side of button to GPIO4
   - Connect other side to GND
   - The internal pull-up resistor will be enabled in firmware

4. **Connect Status LED (Optional):**
   - Connect LED anode through 220Ω resistor to GPIO2
   - Connect LED cathode to GND

### Step 3: ESP32-CAM Setup

1. **Insert SD Card:**
   - Format SD card as FAT32
   - Insert into SD card slot on ESP32-CAM

2. **Connect Camera:**
   - Carefully connect OV2640 camera module to the board connector
   - Ensure ribbon cable is properly seated

3. **Power Connection:**
   - Connect 5V pin to 5V power rail
   - Connect GND to ground rail

### Step 4: Inter-Module UART Connection

1. Connect ESP32 Master GPIO16 to ESP32-CAM GPIO1
2. Connect ESP32 Master GPIO17 to ESP32-CAM GPIO3
3. Ensure both boards share common ground

### Step 5: Power Connections

1. Connect 5V power supply to the power rails
2. **Important:** Do not power the ESP32-CAM via USB and external power simultaneously
3. Verify all ground connections are secure

---

## Wiring Diagram

```
                    [5V Power Supply]
                           |
                    +------+------+
                    |             |
              [ESP32 Master]  [ESP32-CAM]
                    |             |
        +-----------+--------+    |
        |           |        |    |
   [INMP441]  [PCM5102A] [Button] [SD Card]
                                   [Camera]
    
    UART Connection:
    ESP32 Master GPIO16 (RX2) <---> ESP32-CAM GPIO1 (TX)
    ESP32 Master GPIO17 (TX2) <---> ESP32-CAM GPIO3 (RX)
    
    Common GND connection between all components
```

---

## Testing Connections

### Basic Power Test
1. Connect power supply (without ESP32s inserted)
2. Verify 5V and 3.3V rails with multimeter
3. Check for shorts between power and ground

### ESP32 Master Test
1. Insert ESP32 Master into breadboard
2. Power on and check for LED (if connected)
3. Upload a simple blink sketch to verify programming

### ESP32-CAM Test
1. Insert ESP32-CAM into breadboard
2. Power on and check for red power LED
3. Upload a simple test sketch using MB programmer

### UART Test
1. Upload test firmware to both boards
2. Send simple messages between boards
3. Verify communication using Serial Monitor

---

## Troubleshooting

### ESP32-CAM won't boot
- **Issue:** Brown-out detector triggered
- **Solution:** Use a more powerful 5V supply (minimum 1A)
- **Solution:** Add 100µF capacitor across 5V and GND near ESP32-CAM

### No audio from PCM5102A
- **Issue:** Incorrect I2S configuration
- **Solution:** Verify BCK, LCK, and DIN connections
- **Solution:** Check SCK is tied to GND for auto-config
- **Solution:** Ensure XMT is pulled HIGH (3.3V)

### INMP441 not recording
- **Issue:** Incorrect I2S pin assignment
- **Solution:** Double-check SCK, WS, and SD pin connections
- **Solution:** Verify L/R pin is connected to GND

### UART communication fails
- **Issue:** Crossed TX/RX connections
- **Solution:** TX of one board should connect to RX of the other
- **Solution:** Verify common ground connection
- **Solution:** Check baud rate matches (115200)

### SD Card not detected
- **Issue:** SD card format or connection
- **Solution:** Format SD card as FAT32
- **Solution:** Try a different SD card (Class 10 recommended)
- **Solution:** Check SD card is fully inserted

---

## Safety Notes

1. **Never** connect/disconnect components while power is on
2. Always use appropriate current-rated power supply
3. Avoid touching components during operation
4. Use proper electrostatic discharge (ESD) precautions
5. Keep liquids away from electronics
6. Ensure adequate ventilation for components

---

## Recommended Enclosure

For a portable blind assistance device:

1. Use a sturdy enclosure with camera opening
2. Mount button on top for easy access
3. Add speaker output (connect to PCM5102A line out)
4. Include power switch and LED indicators
5. Ensure microphone port is unobstructed
6. Consider weather-resistant design for outdoor use

---

## Component Sourcing

### Recommended Suppliers
- **ESP32 Modules:** Amazon, AliExpress, Adafruit, SparkFun
- **INMP441:** Amazon, Adafruit
- **PCM5102A:** Amazon, AliExpress
- **General Components:** Digi-Key, Mouser, Newark

### Estimated Cost
- ESP32 Master: $8-12
- ESP32-CAM: $10-15
- INMP441 Microphone: $5-8
- PCM5102A DAC: $5-10
- Miscellaneous (button, LED, wires, SD card): $10-15
- **Total:** ~$40-60 USD

---

## Next Steps

After completing hardware assembly:
1. Refer to [Firmware Setup Guide](firmware_setup.md) for programming instructions
2. Test each module independently before integration
3. Follow [Backend Setup Guide](backend_setup.md) to configure the server
4. Consult [User Guide](user_guide.md) for operation instructions
