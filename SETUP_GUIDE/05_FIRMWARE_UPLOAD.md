# 🔧 Firmware Upload Karo | ESP32 Programming

**ESP32 mein code kaise upload karein - ekdum step by step**

---

## 🔌 Step 1: Hardware Connect Karo

### Step 1.1: ESP32 Master Connect Karo
1. **USB Micro cable** lo
2. ESP32 Master board mein plug karo
3. Cable dusra end computer ke USB port mein
4. ✅ ESP32 par **power LED** chalu hogi (usually red)

### Step 1.2: Connection Check Karo
```cmd
# Device Manager kholo (Windows Key + X → Device Manager)
# "Ports (COM & LPT)" expand karo

# ✅ Dikhna chahiye:
# "Silicon Labs CP210x USB to UART Bridge (COM3)"
# ya
# "USB-SERIAL CH340 (COM5)"

# COM number note karo (COM3, COM5, etc.)
```

**Agar dikhai nahi de raha:**
- USB cable change karo (kuch cables data transfer nahi karte)
- Driver install karo ([02_SOFTWARE_INSTALLATION.md](02_SOFTWARE_INSTALLATION.md) Step 5 dekho)
- USB port change karo

---

## 📂 Step 2: Firmware Folder VS Code Mein Kholo

### Step 2.1: VS Code Open Karo
1. VS Code launch karo
2. File → Open Folder
3. Navigate to: `C:\Users\KSHITIJ\Desktop\blind\firmware\esp32_master`
4. "Select Folder" click karo
5. Folder VS Code mein open hoga

### Step 2.2: PlatformIO Workspace Initialize Karo
1. **Left sidebar** mein **PlatformIO icon** (alien 👽) click karo
2. "PIO Home" → "Open" click karo
3. PlatformIO home page khulega
4. Close kar do (abhi kaam nahi hai)

---

## 🔐 Step 3: WiFi Credentials Configure Karo

### Step 3.1: secrets.h File Banao

#### VS Code Mein:
1. Left panel mein files dikhenge
2. `secrets.h.example` file dikhegi
3. Us par **right-click** karo
4. "Copy" select karo
5. Folder par right-click → "Paste"
6. File ka name `secrets.h` mein rename karo

#### Ya Command Line Se:
```cmd
cd C:\Users\KSHITIJ\Desktop\blind\firmware\esp32_master
copy secrets.h.example secrets.h
```

### Step 3.2: secrets.h Edit Karo

VS Code mein `secrets.h` file open karo:

**File mein yeh dikhega:**
```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi credentials
#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"

// Backend server URL
#define SERVER_URL "http://192.168.1.100:8000"
#define API_ENDPOINT "/analyze"

#endif
```

**Edit karo:**

1. `"your_wifi_name"` ki jagah apna WiFi name dalo
   - Example: `"Home_WiFi_2.4GHz"`
   - ⚠️ **5GHz nahi**, only **2.4GHz** WiFi

2. `"your_wifi_password"` ki jagah WiFi password dalo
   - Example: `"MyWiFiPassword123"`

3. `SERVER_URL` mein apne computer ka IP address dalo

**Apna IP Address Kaise Find Karein:**

**Windows:**
```cmd
ipconfig

# Output mein "IPv4 Address" khojo
# Example: 192.168.1.105
# Yeh IP use karo
```

**Linux/Mac:**
```bash
ifconfig
# ya
ip addr show
```

**Updated secrets.h Example:**
```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi credentials
#define WIFI_SSID "Home_WiFi_2.4GHz"
#define WIFI_PASSWORD "MySecurePass123"

// Backend server URL
#define SERVER_URL "http://192.168.1.105:8000"
#define API_ENDPOINT "/analyze"

#endif
```

4. **Save karo** (Ctrl+S)

---

## 🏗️ Step 4: Build (Compile) Karo Firmware

### Step 4.1: PlatformIO Build Command

**Method 1: VS Code Bottom Toolbar**
1. VS Code ke **bottom bar** mein dekho
2. ✅ Checkmark icon dikhega: **"Build"**
3. Us par **click** karo
4. Terminal khulega aur build start hoga

**Method 2: PlatformIO Sidebar**
1. Left mein PlatformIO icon (👽) click karo
2. "Project Tasks" expand karo
3. "esp32dev" expand karo
4. "General" expand karo
5. "Build" par click karo

### Step 4.2: Build Process Monitor Karo

Terminal mein output dikhega:

```
Building in release mode
Compiling .pio/build/esp32dev/src/main.cpp.o
Linking .pio/build/esp32dev/firmware.elf
Building .pio/build/esp32dev/firmware.bin
RAM:   [====      ]  40.2% (used 131828 bytes)
Flash: [======    ]  58.3% (used 764125 bytes)
========================= [SUCCESS] Took 45.23 seconds =========================
```

✅ **Success Hone Par:**
- Last line mein `[SUCCESS]` dikhega
- No errors

❌ **Agar Error Aaye:**
- Red text mein errors dikhenge
- Common errors aur solutions [06_TESTING.md](06_TESTING.md) mein dekho

---

## 📤 Step 5: ESP32 Mein Firmware Upload Karo

### Step 5.1: Upload Karo

**Method 1: Bottom Toolbar**
1. Bottom bar mein **→** (arrow/upload) icon click karo

**Method 2: PlatformIO Sidebar**
1. PlatformIO → Project Tasks → esp32dev → General → Upload

### Step 5.2: Upload Process

```
Configuring upload protocol...
Looking for upload port...
Auto-detected: COM3
Uploading .pio/build/esp32dev/firmware.bin

esptool.py v4.5.1
Serial port COM3
Connecting........_____....
Chip is ESP32-D0WDQ6 (revision v1.0)
Features: WiFi, BT, Dual Core, 240MHz
Crystal is 40MHz
MAC: 24:6f:28:ab:cd:ef

Writing at 0x00010000... (100 %)
Wrote 764128 bytes (490234 compressed) at 0x00010000 in 43.2 seconds

Hash of data verified.

Leaving...
Hard resetting via RTS pin...
========================= [SUCCESS] Took 52.67 seconds =========================
```

✅ **Success Hone Par:**
- `[SUCCESS]` message
- ESP32 automatically restart hoga

### Step 5.3: Serial Monitor Kholo (Dekhne Ke Liye)

**Bottom toolbar mein:**
- **Plug icon** (Serial Monitor) click karo

**Output Dekho:**
```
ESP32 Master Starting...
I2S initialized
UART initialized
PSRAM found, using it for buffers
Connecting to Wi-Fi: Home_WiFi_2.4GHz
.....
Wi-Fi connected!
IP address: 192.168.1.150
Session ID: 246f28ab-1a2b-3c4d-5e6f-7890abcdef12
System ready!
```

✅ **Yeh sab dikhe to ekdum perfect!**

---

## 🎥 Step 6: ESP32-CAM Mein Firmware Upload Karo

### Step 6.1: ESP32 Master Ko Disconnect Karo
- USB cable nikalo ESP32 Master se
- Hardware wiring waise hi rahne do

### Step 6.2: ESP32-CAM Folder Open Karo
1. VS Code → File → Open Folder
2. Navigate: `C:\Users\KSHITIJ\Desktop\blind\firmware\esp32_cam`
3. Open karo

### Step 6.3: ESP32-CAM Ko Programming Mode Mein Connect Karo

**ESP32-CAM Programming Mode:**
1. ESP32-CAM par **GPIO0** aur **GND** ko short karo
   - Jumper wire use karo
   - Ya button press karo (agar lagaya hai)
2. USB cable connect karo (via FTDI programmer)
3. Power LED chalu hogi

### Step 6.4: Build Aur Upload Karo
```
PlatformIO → Build (✅ icon)
Wait for success

PlatformIO → Upload (→ icon)
Wait for success
```

### Step 6.5: Normal Mode Mein Switch Karo
1. **GPIO0** jumper remove karo
2. ESP32-CAM ko **reset** karo (RST button ya power cycle)
3. Serial Monitor kholo
4. Output dekho:
```
ESP32-CAM Ready
Camera initialized
Waiting for commands...
```

---

## 🔗 Step 7: Dono ESP32 Saath Mein Connect Karo

1. **ESP32-CAM** USB cable mein rahne do (power ke liye)
2. **ESP32 Master** bhi USB cable se connect karo
3. **UART wires** connected hone chahiye (hardware assembly se)
4. Dono ka **Serial Monitor** open karo alag-alag instances mein

---

## ✅ Verification Checklist

### ESP32 Master:
- [ ] WiFi connected
- [ ] IP address mila
- [ ] Session ID generated
- [ ] "System ready!" dikha

### ESP32-CAM:
- [ ] Camera initialized
- [ ] "Waiting for commands" dikha
- [ ] UART se Master se connected

### Communication Test:
1. ESP32 Master ke Serial Monitor mein **button press simulate** karo
2. Ya hardware button dabao
3. Master: "Requesting image capture..." dikhna chahiye
4. CAM: "Capture command received" dikhna chahiye

---

## 🎯 Agla Step

Firmware upload complete? Badiya bhai!

👉 **Ab Jao**: [06_TESTING.md](06_TESTING.md)

System ko test karna seekho!
