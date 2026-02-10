# 📋 Kya Kya Chahiye Bro | Requirements

---

## 🔧 Hardware Ki List

### ✅ Jaruri Components (Must Have)

| **Item** | **Quantity** | **Approx Price** | **Where to Buy** |
|----------|-------------|------------------|------------------|
| **ESP32 DevKit V1** (Master) | 1 | ₹350-500 | Amazon, Robu.in |
| **ESP32-CAM** (with OV2640) | 1 | ₹450-600 | Amazon, Robu.in |
| **INMP441 Microphone** | 1 | ₹200-300 | Amazon |
| **PCM5102A DAC Module** | 1 | ₹250-400 | Amazon |
| **Speaker (8Ω, 3W)** | 1 | ₹150-250 | Local Electronics |
| **Push Button** | 1 | ₹10-20 | Local Electronics |
| **LED** (any color) | 1 | ₹5 | Local Electronics |
| **Resistor** (220Ω) | 1 | ₹2 | Local Electronics |
| **Breadboard** | 1 | ₹100-150 | Amazon |
| **Jumper Wires** (M-M, M-F) | 40 pcs | ₹100-150 | Amazon |
| **USB Micro Cable** (2 pcs) | 2 | ₹100-200 | Amazon |
| **Power Bank** (10000mAh+) | 1 | ₹800-1500 | Amazon |

**Total Kharcha Lagega**: ₹2,500 - ₹4,000

---

## 💻 Software Ki List

### Windows PC ke liye:

| **Software** | **Version** | **Size** | **Free?** |
|-------------|-------------|----------|-----------|
| **Python** | 3.9 या higher | ~30 MB | ✅ Yes |
| **Git** | Latest | ~50 MB | ✅ Yes |
| **VS Code** | Latest | ~100 MB | ✅ Yes |
| **PlatformIO Extension** | Latest | ~500 MB | ✅ Yes |

### Linux/Mac ke liye:
Same as above (installation steps alag honge)

---

## 🌐 Internet Aur API Requirements

### 1. **WiFi Connection**
- Speed: Minimum 2 Mbps
- Type: 2.4 GHz (ESP32 5GHz support nahi karta)
- Stable connection chahiye

### 2. **Google Gemini API Key**
- **Free Tier**: 60 requests/minute
- **Cost**: Ekdum FREE hai bhai
- **Signup**: [Google AI Studio](https://makersuite.google.com/app/apikey)

### 3. **Optional (advanced features ke liye)**
- OpenAI API (Free version mein use nahi hota)
- Azure Account (Free version mein use nahi hota)

---

## 🔋 Power Kaise Denge

### Development ke time:
- **ESP32 Master**: USB se power (Computer se)
- **ESP32-CAM**: USB se power (Computer se)

### Final Product ke liye:
- **Power Bank**: 5V/2A output
- **ESP32 Master**: 500mA average
- **ESP32-CAM**: 300mA average (Camera on)
- **Total**: ~800mA-1A

**Battery Kitni Der Chalegi:**
- 10000mAh Power Bank
- Runtime: ~10-12 hours

---

## 🖥️ Computer Kaisa Chahiye

### Minimum:
- **Processor**: Intel i3 ya equivalent
- **RAM**: 4 GB
- **Storage**: 5 GB free space
- **OS**: Windows 10/11, Ubuntu 20.04+, macOS 10.15+

### Recommended (Better hai):
- **Processor**: Intel i5 ya higher
- **RAM**: 8 GB
- **Storage**: 10 GB free space
- **USB Ports**: 2 available

---

## 📦 Shopping List Banao

### Step 1: Online order karo (3-5 days mein aa jayega)
```
☐ ESP32 DevKit V1 (1 pc)
☐ ESP32-CAM with OV2640 (1 pc)
☐ INMP441 Microphone Module (1 pc)
☐ PCM5102A DAC Module (1 pc)
☐ Breadboard (830 points)
☐ Jumper Wires Set
☐ USB Micro Cables (2 pcs)
☐ Power Bank (10000mAh)
```

### Step 2: Local Electronics Shop se khareedo
```
☐ Push Button (1 pc)
☐ LED (koi bhi color, 1 pc)
☐ Resistor 220Ω (1 pc)
☐ Speaker 8Ω 3W (1 pc)
```

---

## ✅ Checklist - Sab Kuch Aa Gaya?

Saman aane ke baad check karo:

### Hardware:
- [ ] ESP32 Master board
- [ ] ESP32-CAM board
- [ ] INMP441 Mic (6 pins)
- [ ] PCM5102A DAC (9 pins)
- [ ] Speaker with wires
- [ ] Button
- [ ] LED
- [ ] Resistor
- [ ] Breadboard
- [ ] Jumper wires (kaafi quantity)
- [ ] 2 USB cables
- [ ] Power bank

### Computer:
- [ ] Internet connection
- [ ] 2 free USB ports
- [ ] Admin access (software install karne ke liye)

---

## 🎯 Agla Step

Saara saman mil gaya? Perfect!

👉 **Ab Jao**: [02_SOFTWARE_INSTALLATION.md](02_SOFTWARE_INSTALLATION.md)

Software installation shuru karo!
