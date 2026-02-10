# Firmware Setup Guide

## Interactive AI Vision Assistant - Firmware Programming

This guide provides detailed instructions for programming the ESP32 Master and ESP32-CAM modules.

---

## Prerequisites

### Software Requirements
- Arduino IDE 2.x or PlatformIO
- USB cable (compatible with your ESP32 boards)
- Serial terminal program (Arduino Serial Monitor, PuTTY, or similar)

### Knowledge Requirements
- Basic understanding of Arduino IDE or PlatformIO
- Familiarity with ESP32 programming
- Understanding of serial communication

---

## Setup Arduino IDE

### Step 1: Install Arduino IDE
1. Download Arduino IDE 2.x from [arduino.cc](https://www.arduino.cc/en/software)
2. Install the IDE following the installer instructions
3. Launch Arduino IDE

### Step 2: Add ESP32 Board Support
1. Go to **File → Preferences**
2. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Click **OK**
4. Go to **Tools → Board → Boards Manager**
5. Search for "esp32"
6. Install "esp32 by Espressif Systems" (version 2.0.x or later)
7. Wait for installation to complete

### Step 3: Install Required Libraries

Install the following libraries via **Tools → Manage Libraries**:

#### For ESP32 Master:
- **ESP32** (installed with board support)
- **WiFi** (built-in)
- **HTTPClient** (built-in)
- **driver/i2s** (built-in)

#### For ESP32-CAM:
- **ESP32** (installed with board support)
- **esp_camera** (included in ESP32 board package)
- **SD_MMC** (built-in)
- **FS** (built-in)

**Note:** Most libraries are built-in with the ESP32 board package. No additional installation needed.

---

## Programming ESP32 Master

### Step 1: Prepare the Code

1. Navigate to `firmware/esp32_master/` directory
2. Copy `secrets.h.example` to `secrets.h`
3. Edit `secrets.h` with your credentials:
   ```cpp
   #define WIFI_SSID "Your_WiFi_SSID"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define SERVER_URL "http://your-server-ip:8000"
   #define API_ENDPOINT "/api/v1/analyze"
   ```

### Step 2: Configure Arduino IDE

1. Connect ESP32 Master to computer via USB
2. Select board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Configure board settings:
   - **Upload Speed:** 115200
   - **CPU Frequency:** 240MHz
   - **Flash Frequency:** 80MHz
   - **Flash Mode:** QIO
   - **Flash Size:** 4MB (or your board's flash size)
   - **Partition Scheme:** Default 4MB with spiffs
   - **PSRAM:** Enabled (if your board has PSRAM)
   - **Port:** Select the correct COM port

### Step 3: Compile and Upload

1. Open `main.cpp` in Arduino IDE
2. Click **Verify** (checkmark icon) to compile
3. Fix any compilation errors
4. Click **Upload** (arrow icon)
5. Wait for upload to complete
6. Open **Serial Monitor** (Tools → Serial Monitor)
7. Set baud rate to **115200**
8. Press **Reset** button on ESP32
9. Verify startup messages appear

### Expected Output:
```
ESP32 Master Starting...
I2S initialized
UART initialized
Connecting to Wi-Fi: Your_SSID
.....
Wi-Fi connected!
IP address: 192.168.1.xxx
Session ID: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
System ready!
```

---

## Programming ESP32-CAM

### Important Notes for ESP32-CAM:
- ESP32-CAM does **not** have built-in USB-to-Serial converter
- You need an **FTDI adapter** or **MB Programmer** to upload code
- GPIO0 must be connected to GND during upload, then disconnected for normal operation

### Step 1: Connect FTDI/MB Programmer

#### Using FTDI Adapter:
| FTDI Pin | ESP32-CAM Pin |
|----------|---------------|
| 5V | 5V |
| GND | GND |
| TX | U0R (RX) |
| RX | U0T (TX) |

#### Using MB Programmer:
Simply insert ESP32-CAM into the MB Programmer socket.

### Step 2: Enter Upload Mode

#### For FTDI Adapter:
1. Connect GPIO0 to GND
2. Press Reset button
3. GPIO0 can be disconnected after upload starts

#### For MB Programmer:
1. Ensure jumper is set to programming mode
2. Press reset button on programmer

### Step 3: Configure Arduino IDE

1. Select board: **Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM**
2. Configure settings:
   - **Upload Speed:** 115200
   - **CPU Frequency:** 240MHz
   - **Flash Frequency:** 80MHz
   - **Flash Mode:** QIO
   - **Flash Size:** 4MB
   - **Partition Scheme:** Huge APP (3MB No OTA)
   - **PSRAM:** Enabled
   - **Port:** Select the correct COM port

### Step 4: Compile and Upload

1. Open `firmware/esp32_cam/main.cpp` in Arduino IDE
2. Click **Verify** to compile
3. Fix any compilation errors
4. Click **Upload**
5. Wait for upload to complete
6. **Disconnect GPIO0 from GND** (if using FTDI)
7. Press **Reset** button
8. Open Serial Monitor at **115200 baud**

### Expected Output:
```
Camera initialized
SD Card Type: SDHC
SD Card Size: 15872MB
SD Card initialized
ESP32-CAM Ready
```

---

## Testing the System

### Test 1: ESP32-CAM Standalone
1. Power on ESP32-CAM
2. Watch for LED blink pattern (3 quick blinks)
3. Check SD card initialization in Serial Monitor
4. Verify camera initialization

### Test 2: ESP32 Master Standalone
1. Power on ESP32 Master
2. Wait for Wi-Fi connection
3. Press button briefly - should see state changes in Serial Monitor
4. Verify I2S initialization messages

### Test 3: UART Communication
1. Power on both ESP32s
2. Connect UART pins as per hardware guide
3. Upload UART test sketch to both boards
4. Verify messages are exchanged

### Test 4: End-to-End Test
1. Ensure backend server is running (see [Backend Setup](backend_setup.md))
2. Power on both ESP32s
3. Wait for Wi-Fi connection
4. Short press button (snapshot mode)
5. Watch Serial Monitor for:
   - Image capture request
   - Image transfer via UART
   - HTTP upload to backend
   - Audio response playback

---

## Troubleshooting

### Upload Failed / No Serial Port
- **Solution:** Install CP210x or CH340 USB drivers
- **Solution:** Try different USB cable
- **Solution:** Disconnect other USB devices
- **Solution:** Restart Arduino IDE

### ESP32-CAM Upload Failed
- **Solution:** Ensure GPIO0 is connected to GND during upload
- **Solution:** Press reset button before upload
- **Solution:** Reduce upload speed to 921600 or 460800
- **Solution:** Use MB Programmer instead of FTDI

### Compilation Errors
- **Error:** `esp_camera.h: No such file or directory`
  - **Solution:** Select correct board (AI Thinker ESP32-CAM)
  - **Solution:** Update ESP32 board package to 2.0.x or later

- **Error:** `PSRAM not found`
  - **Solution:** Enable PSRAM in board settings
  - **Solution:** Use board with PSRAM

### Runtime Errors

#### ESP32 Master:
- **Wi-Fi won't connect:**
  - Check SSID and password in `secrets.h`
  - Verify Wi-Fi network is 2.4GHz (ESP32 doesn't support 5GHz)
  - Check router settings

- **No audio recording:**
  - Verify INMP441 connections
  - Check I2S pin assignments
  - Test with known-good microphone

- **No audio playback:**
  - Verify PCM5102A connections
  - Check I2S pin assignments
  - Ensure XMT pin is HIGH

#### ESP32-CAM:
- **Camera init failed:**
  - Check camera ribbon cable connection
  - Try different camera module
  - Verify power supply is adequate (min 500mA)

- **SD card failed:**
  - Format SD card as FAT32
  - Try different SD card
  - Check SD card pins

- **UART communication failed:**
  - Verify TX/RX crossover connection
  - Check baud rate (115200)
  - Ensure common ground

---

## Advanced Configuration

### Adjusting Camera Settings
Edit `firmware/esp32_cam/config.h`:
- `JPEG_QUALITY`: Lower = better quality, larger file (range: 1-63)
- `CAMERA_FRAME_SIZE`: Adjust resolution (SVGA, XGA, etc.)

### Adjusting Audio Settings
Edit `firmware/esp32_master/config.h`:
- `MIC_SAMPLE_RATE`: Recording sample rate (default: 16000 Hz)
- `DAC_SAMPLE_RATE`: Playback sample rate (default: 44100 Hz)

### Adjusting UART Settings
Edit configuration files:
- `UART_BAUD_RATE`: Increase for faster image transfer (max 921600)
- `CHUNK_SIZE`: Adjust chunk size for reliability vs. speed

---

## Updating Firmware

### Over-the-Air (OTA) Updates
For production deployment, implement OTA updates:
1. Add OTA library to code
2. Configure OTA password
3. Upload firmware via Wi-Fi

### USB Updates
1. Connect board via USB
2. Follow upload procedure above
3. No need to disconnect components (unless using GPIO0)

---

## Serial Debugging

### Enable Verbose Logging
Add to your code:
```cpp
#define DEBUG_LEVEL 3  // 0=None, 1=Error, 2=Warning, 3=Info, 4=Debug
```

### Monitor Multiple Serial Ports
Use tools like:
- **Termite** (Windows)
- **screen** (Linux/Mac)
- **PuTTY** (All platforms)

To monitor both ESP32s simultaneously, use two serial terminals.

---

## Performance Optimization

### Memory Usage
- Monitor heap usage: `ESP.getFreeHeap()`
- Use PSRAM for large buffers
- Free buffers after use

### Power Consumption
- Enable deep sleep when idle
- Reduce Wi-Fi transmit power
- Optimize LED usage

---

## Next Steps

After successful firmware upload:
1. Test each component individually
2. Perform integrated system test
3. Refer to [User Guide](user_guide.md) for operation
4. Monitor system logs for any issues
5. Fine-tune settings based on performance

---

## Additional Resources

- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [ESP32-CAM Getting Started](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/)
- [Arduino ESP32 GitHub](https://github.com/espressif/arduino-esp32)
