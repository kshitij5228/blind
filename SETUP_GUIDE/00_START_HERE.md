# 🚀 YAHAN SE SHURU KARO BRO | START HERE

**Interactive AI Vision Assistant for Blind Users**

Ye guide tumhe **zero se lekar pura system setup** karne mein madad karegi. Ekdum baby steps mein, line by line.

---

## 📚 Setup Order - Isi Sequence Mein Karo

In files ko **isi order mein** padho aur follow karo:

### 1️⃣ Pehla Step - Kya Kya Chahiye
📄 **[01_REQUIREMENTS.md](01_REQUIREMENTS.md)**
- Kya kya saman chahiye (Hardware List)
- Kaun se software chahiye (Software List)
- Total kitna kharcha aayega

### 2️⃣ Dusra Step - Software Install Karo
📄 **[02_SOFTWARE_INSTALLATION.md](02_SOFTWARE_INSTALLATION.md)**
- Python kaise install karein (Windows/Linux/Mac)
- Git kaise install karein
- PlatformIO kaise install karein (ESP32 ke liye)
- Har step detail mein

### 3️⃣ Teesra Step - Hardware Jodo
📄 **[03_HARDWARE_ASSEMBLY.md](03_HARDWARE_ASSEMBLY.md)**
- ESP32 Master mein wire kaise lagaye
- ESP32-CAM mein wire kaise lagaye
- Microphone (INMP441) kaise jode
- Speaker (PCM5102A) kaise jode
- Diagram ke saath explained

### 4️⃣ Chautha Step - Backend Setup
📄 **[04_BACKEND_SETUP.md](04_BACKEND_SETUP.md)**
- Code kaise download karein
- Python Packages kaise install karein
- Google Gemini API Key kaise le
- Configuration file kaise banaye
- Server kaise chalu karein

### 5️⃣ Panchwa Step - Firmware Upload
📄 **[05_FIRMWARE_UPLOAD.md](05_FIRMWARE_UPLOAD.md)**
- ESP32 ko Computer se kaise connect karein
- Firmware kaise upload karein
- WiFi Settings kaise dale
- Troubleshooting

### 6️⃣ Chhata Step - Testing
📄 **[06_TESTING.md](06_TESTING.md)**
- System kaise test karein
- Common Problems aur Solutions
- Performance Tips

### 7️⃣ Satwan Step - Daily Use
📄 **[07_USAGE_GUIDE.md](07_USAGE_GUIDE.md)**
- Device kaise chalu karein
- Snapshot Mode kaise use karein
- Conversation Mode kaise use karein
- Battery Management

---

## ⚡ Quick Start (Agar Tum Experienced Ho)

Agar tumhe thoda technical knowledge hai to seedha ye karo:

```bash
# 1. Repository clone karo
git clone <repository-url>
cd blind

# 2. Backend setup karo
cd backend
python -m venv venv
venv\Scripts\activate  # Windows
source venv/bin/activate  # Linux/Mac
pip install -r requirements.txt
cp .env.example .env
# Edit .env file with your keys

# 3. Server chalu karo
python -m uvicorn server.app:app --host 0.0.0.0 --port 8000

# 4. Firmware upload karo (dusri terminal mein)
cd ../firmware/esp32_master
# Edit secrets.h with WiFi credentials
# Upload using PlatformIO
```

---

## 🆘 Help Chahiye?

- **Common Errors**: Dekho [06_TESTING.md](06_TESTING.md)
- **Hardware Issues**: Dekho [03_HARDWARE_ASSEMBLY.md](03_HARDWARE_ASSEMBLY.md)
- **Software Issues**: Dekho [02_SOFTWARE_INSTALLATION.md](02_SOFTWARE_INSTALLATION.md)

---

## 📞 Contact

Issues ya questions ke liye GitHub Issues par post karo.

---

**Ab Shuru Karo:** 👉 [01_REQUIREMENTS.md](01_REQUIREMENTS.md)
