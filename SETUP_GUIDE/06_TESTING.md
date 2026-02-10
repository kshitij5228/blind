# ✅ Testing Aur Troubleshooting Bhai

**System ko test kaise karein aur problems kaise solve karein**

---

## 🔄 Step 1: Backend Server Chalu Karo

### Terminal 1: Backend Server

```cmd
# Backend folder mein jao
cd C:\Users\KSHITIJ\Desktop\blind\backend

# Virtual environment activate karo
venv\Scripts\activate

# Server start karo
python -m uvicorn server.app:app --host 0.0.0.0 --port 8000 --reload

# Output dekho:
# INFO: Uvicorn running on http://0.0.0.0:8000
# INFO: Application startup complete
```

✅ **Server running hai**

**Server running rahna chahiye, is terminal ko band mat karo!**

---

## 🧪 Step 2: Basic API Test Karo

### Browser Se Test Karo

1. Browser kholo
2. Jao: `http://localhost:8000/docs`
3. Swagger UI dikhega

### Health Check Test:
1. `/health` endpoint expand karo
2. "Try it out" click karo
3. "Execute" click karo
4. ✅ Response: `{"status": "healthy"}`

---

## 🎤 Step 3: Hardware Button Test Karo

### Serial Monitor Mein Dekho (ESP32 Master)

**Test 1: Short Press (Snapshot Mode)**
1. Hardware button ko **1 second** dabao aur chhodo
2. Serial Monitor mein output:
```
Button pressed
State changed to: 3 (CAPTURING)
Requesting image capture from ESP32-CAM...
Capture successful
State changed to: 4 (UPLOADING)
Uploading to backend...
Upload successful, playing response
Audio playback complete
State changed to: 0 (IDLE)
```

✅ **Snapshot mode kaam kar raha hai**

**Test 2: Long Press (Conversation Mode)**
1. Button ko **3+ seconds** dabaye rakho
2. Serial Monitor:
```
Button pressed
Recording audio...
Recording stopped. Recorded 48044 bytes
State changed to: 3 (CAPTURING)
Requesting image capture...
Capture successful
Uploading to backend...
Upload successful (with audio)
Audio playback complete
```

✅ **Conversation mode kaam kar raha hai**

---

## 🔊 Step 4: Audio Test Karo

### Microphone Test:
1. Long press button
2. Speaker mein **bolo**: "What do you see?"
3. Release button
4. Serial Monitor mein:
```
Transcribed query: 'What do you see?' (language: en)
```

✅ **Microphone aur STT kaam kar rahe hain**

### Speaker Test:
1. Koi bhi mode mein button press karo
2. Server response aane ke baad
3. Speaker se **audio sunai deni chahiye**

✅ **Speaker aur TTS kaam kar rahe hain**

---

## 📸 Step 5: Camera Test Karo

### ESP32-CAM Serial Monitor:
```
Capture command received
Initializing camera...
Camera capture OK! Image size: 35840 bytes
Sending image in 18 chunks...
Chunk 0/18 sent, ACK received
Chunk 1/18 sent, ACK received
...
All chunks sent successfully
```

✅ **Camera kaam kar raha hai**

### Backend Terminal Mein:
```
INFO: Received analyze request
INFO: Session ID: 246f28ab-1a2b...
INFO: Mode: snapshot
INFO: Image size: 35840 bytes
Gemini response: I see a wooden chair in front of you, approximately 3 feet away...
```

✅ **Image backend mein pahunch raha hai aur Gemini analyze kar raha hai**

---

## 🔍 Step 6: End-to-End Test Karo

### Complete Workflow Test:

1. **Camera ke saamne koi object rakho** (pen, book, cup, etc.)
2. **Short press button** (snapshot mode)
3. **Wait** 5-10 seconds
4. **Speaker se suno** AI ka response

**Expected Flow:**
```
User: [Press button]
Device: [Captures image]
Device: [Sends to backend]
Backend: [STT → Gemini → TTS]
Device: [Plays audio response]
Speaker: "I see a red pen on the table in front of you..."
```

✅ **Pura system kaam kar raha hai bhai!**

---

## ❌ Common Problems Aur Solutions

### Problem 1: WiFi Connect Nahi Ho Raha

**Symptoms:**
```
Connecting to Wi-Fi: Home_WiFi
..................................
Wi-Fi connection failed!
```

**Solutions:**
- [ ] `secrets.h` mein SSID aur password check karo (spelling, case-sensitive)
- [ ] WiFi **2.4GHz** hai? (5GHz kaam nahi karega)
- [ ] Router reboot karo
- [ ] ESP32 ke paas WiFi router rakho (signal strength)
- [ ] WiFi mein special characters (`@`, `#`, etc.) hain? Escape karo

### Problem 2: Backend Connection Failed

**Symptoms:**
```
HTTP error: -1
ya
Connection refused
```

**Solutions:**
- [ ] Backend server running hai? (Terminal mein check karo)
- [ ] `secrets.h` mein correct IP address hai?
- [ ] Firewall backend port 8000 ko block kar raha hai?
  - Windows: Firewall settings → Allow port 8000
- [ ] ESP32 aur Computer same WiFi par hain?

**IP Address Phir Se Check Karo:**
```cmd
ipconfig
# IPv4 Address dekho
```

### Problem 3: Speaker Se Sound Nahi Aa Rahi

**Symptoms:**
- Speaker se koi sound nahi aa rahi

**Solutions:**
- [ ] Speaker wiring check karo (PCM5102A LOUT/ROUT)
- [ ] Speaker volume (agar adjustable hai)
- [ ] PCM5102A ko 3.3V power mil raha hai?
- [ ] Serial Monitor mein "Audio playback complete" dikh raha hai?
- [ ] Speaker kaam kar raha hai? (Dusre device se test karo)

### Problem 4: Microphone Recording Nahi Kar Raha

**Symptoms:**
```
Transcribed query: '' (language: en)
ya
Recording stopped. Recorded 0 bytes
```

**Solutions:**
- [ ] INMP441 wiring check karo (6 pins)
- [ ] INMP441 ko power mil raha hai? (3.3V)
- [ ] I2S pins correct hain? (GPIO32, GPIO25, GPIO33)
- [ ] Microphone ke paas bol rahe ho? (1-2 feet distance)
- [ ] Silent environment mein test karo

### Problem 5: Camera Kaam Nahi Kar Raha

**Symptoms:**
```
Camera init failed
ya
Capture failed with status: 0x01
```

**Solutions:**
- [ ] ESP32-CAM ko separate power do (USB cable)
- [ ] Camera connector properly connected hai?
- [ ] Firmware upload properly hua?
- [ ] UART wiring check karo (TX-RX cross connection)
- [ ] ESP32-CAM reset karo (RST button)

### Problem 6: Gemini API Error

**Symptoms:**
```
Gemini analysis error: API key not valid
ya
429 Too Many Requests
```

**Solutions:**
- [ ] `.env` file mein API key correct hai?
- [ ] API key ke aage-piche extra spaces nahi hain?
- [ ] Google AI Studio mein quota check karo
- [ ] API key regenerate karo aur update karo
- [ ] Rate limit: 60 requests/minute, slow down karo

### Problem 7: Upload Fail Ho Raha

**Symptoms:**
```
esptool.py failed
ya
Serial port not found
```

**Solutions:**
- [ ] USB cable properly connected hai?
- [ ] COM port available hai? (Device Manager check karo)
- [ ] USB driver installed hai?
- [ ] Dusra USB port try karo
- [ ] ESP32 ko unplug/plug karo
- [ ] PlatformIO ko restart karo

### Problem 8: Build Errors Aa Rahe

**Symptoms:**
```
Compilation error: ...
ya
Linking failed: ...
```

**Solutions:**
- [ ] `secrets.h` file exist karti hai?
- [ ] PlatformIO extension properly installed hai?
- [ ] Code mein syntax errors nahi hain?
- [ ] `.pio` folder delete karke phir se build karo
- [ ] VS Code restart karo

---

## 🔧 Advanced Troubleshooting

### Serial Monitor Mein Debug Info Dekho

**ESP32 Master Serial Monitor:**
- Connection status
- Button events
- Audio recording stats
- Upload status
- HTTP response codes

**ESP32-CAM Serial Monitor:**
- Camera initialization
- Image capture status
- UART communication
- Chunk transfer progress

**Backend Terminal:**
- API requests received
- STT transcription results
- Gemini API calls
- TTS generation
- Error stack traces

### Log Levels Badha Do

**.env Mein:**
```
LOG_LEVEL=DEBUG
```

Restart server:
```cmd
Ctrl+C  # Stop server
python -m uvicorn server.app:app --host 0.0.0.0 --port 8000 --reload --log-level debug
```

Ab detailed logs dikhenge

---

## 📊 Performance Optimization Tips

### Battery Life Badhao:
- WiFi power save mode enable karo
- LED brightness kam karo
- Unused peripherals disable karo
- Deep sleep implement karo (advanced)

### Response Time Kam Karo:
- WiFi signal strength improve karo
- Backend server powerful machine par run karo
- Image resolution kam karo (quality vs speed)
- Gemini model "flash" use karo (already set)

### Audio Quality Sudharo:
- Sample rate 16kHz par rakho
- Noise-free environment
- Microphone ko mouth ke paas rakho
- Speaker volume adjust karo

---

## ✅ Final Checklist

Sab kuch kaam kar raha hai?

- [ ] Backend server starts without errors
- [ ] ESP32 Master connects to WiFi
- [ ] ESP32-CAM initializes camera
- [ ] Button short press works (snapshot)
- [ ] Button long press works (conversation)
- [ ] Microphone records audio
- [ ] Camera captures image
- [ ] Backend processes requests
- [ ] Gemini API responds
- [ ] Speaker plays audio responses
- [ ] Complete workflow successful

### Sab ✅ Hai Bhai?

---

## 🎯 Agla Step

Testing complete? Mast!

👉 **Ab Jao**: [07_USAGE_GUIDE.md](07_USAGE_GUIDE.md)

Daily use ke instructions padho!
