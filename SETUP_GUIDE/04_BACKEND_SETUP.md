# 🖥️ Backend Setup Bhai | Python Server Kaise Banaye

**Line by line, har command explain ke saath**

---

## 📁 Step 1: Code Download Karo

### Option A: Git Se Clone Karo (Recommended)

#### Step 1.1: Folder Banao
```cmd
# Command Prompt kholo (Windows Key + R, type "cmd")

# Desktop par jao
cd Desktop

# Check karo kahan ho
cd
# Output: C:\Users\KSHITIJ\Desktop
```

#### Step 1.2: Repository Clone Karo
```cmd
# Git clone command (apna repository URL dalo)
git clone https://github.com/your-username/blind.git

# Clone hone mein 10-20 seconds lagenge

# Check karo folder bana ya nahi
dir
# "blind" folder dikhna chahiye

# Folder mein jao
cd blind

# Andar kya hai dekho
dir
# Output: backend, firmware, docs, README.md, etc.
```

### Option B: ZIP Download Karo (agar Git kaam nahi kar raha)

1. Browser mein repository kholo
2. Green "Code" button click karo
3. "Download ZIP" select karo
4. ZIP file Desktop par extract karo
5. Folder ka name "blind" rakho
6. Command Prompt mein: `cd Desktop\blind`

---

## 🐍 Step 2: Python Virtual Environment Banao

### Why Virtual Environment?
- Har project ke alag packages rahte hain
- System Python ko mess nahi hota
- Clean uninstall ho sakta hai

### Step 2.1: Backend Folder Mein Jao
```cmd
# Already in C:\Users\KSHITIJ\Desktop\blind
cd backend

# Check karo kahan ho
cd
# Output: C:\Users\KSHITIJ\Desktop\blind\backend
```

### Step 2.2: Virtual Environment Create Karo
```cmd
# Virtual environment create karo (name: venv)
python -m venv venv

# Wait karo 30-60 seconds
# Ek naya folder "venv" banega

# Check karo
dir
# "venv" folder dikhna chahiye
```

### Step 2.3: Virtual Environment Activate Karo

**Windows:**
```cmd
# Activate command
venv\Scripts\activate

# Successful hone par command prompt badal jayega:
# (venv) C:\Users\KSHITIJ\Desktop\blind\backend>
#  ^^^^^ yeh prefix dikhega

# ✅ Agar (venv) dikh raha hai to activated hai
```

**Linux/Mac:**
```bash
source venv/bin/activate
```

---

## 📦 Step 3: Python Packages Install Karo

### Step 3.1: Requirements File Check Karo
```cmd
# File exist karti hai ya nahi
type requirements.txt

# Output mein saari packages ki list dikhni chahiye:
# fastapi
# uvicorn
# speechrecognition
# vosk
# gtts
# pydub
# etc...
```

### Step 3.2: Packages Install Karo
```cmd
# Saari packages ek saath install hongi
pip install -r requirements.txt

# ⏰ Yeh 5-10 minutes lagega
# Internet speed par depend karta hai

# Output mein bahut saari lines aayengi:
# Collecting fastapi...
# Downloading fastapi-0.x.x...
# Installing collected packages: ...
# Successfully installed ...

# ✅ Last mein "Successfully installed" dikhna chahiye
```

### Step 3.3: Installation Verify Karo
```cmd
# Installed packages ki list dekho
pip list

# Output mein hone chahiye:
# fastapi       0.x.x
# uvicorn       0.x.x
# speechrecognition x.x.x
# vosk          x.x.x
# gtts          x.x.x
# google-generativeai x.x.x
# ... aur bhi bahut saare
```

---

## 🔑 Step 4: API Keys Setup Karo

### Step 4.1: Google Gemini API Key Lo

1. **Browser mein jao**: https://makersuite.google.com/app/apikey
2. **Google Account se Login karo**
3. "**Create API Key**" button click karo
4. "**Create API key in new project**" select karo
5. API Key generate hogi (40-50 characters long)
6. 📋 **Copy karo** (Ctrl+C)
7. ⚠️ Yeh key secret hai, kisi ko mat dikhao

### Step 4.2: .env File Banao

#### Method 1: Command Line Se
```cmd
# .env.example se copy karo
copy .env.example .env

# .env file ban gayi
# Output: 1 file(s) copied.
```

#### Method 2: Manually (agar copy command kaam nahi kare)
1. Notepad kholo
2. File → Open
3. Navigate to: `C:\Users\KSHITIJ\Desktop\blind\backend`
4. File type: "All Files (*.*)"
5. `.env.example` file open karo
6. File → Save As
7. File name: `.env` (dot se shuru, no extension)
8. Save type: "All Files"
9. Save click karo

### Step 4.3: .env File Edit Karo

```cmd
# Notepad se .env file kholo
notepad .env
```

**File mein yeh dikhega:**
```bash
# Google Gemini API Configuration
GEMINI_API_KEY=your_gemini_api_key_here
GEMINI_MODEL=gemini-1.5-flash

# Server Configuration
SERVER_HOST=0.0.0.0
SERVER_PORT=8000
MAX_IMAGE_SIZE_MB=10
MAX_AUDIO_SIZE_MB=10

# Audio Configuration
AUDIO_FORMAT=wav
AUDIO_SAMPLE_RATE=16000
AUDIO_CHANNELS=1

# Vosk Models (Free offline STT)
VOSK_MODEL_PATH_EN=backend/models/vosk_models/vosk-model-small-en-us-0.15
VOSK_MODEL_PATH_HI=backend/models/vosk_models/vosk-model-small-hi-0.22

# Rate Limiting
RATE_LIMIT_PER_MINUTE=30
```

**Edit karo:**
1. `GEMINI_API_KEY=your_gemini_api_key_here` ki jagah
2. Apni actual API key paste karo
3. Example: `GEMINI_API_KEY=AIzaSyABCDEF1234567890abcdefGHIJKLMNOP`
4. **Save karo** (Ctrl+S)
5. Notepad **band karo**

✅ **Check**: .env file mein real API key hai

---

## 📥 Step 5: Vosk Models Download Karo

### Why Vosk?
- **Free** offline speech recognition
- Internet ki jarurat nahi STT ke liye
- English aur Hindi support karta hai

### Step 5.1: Models Folder Banao
```cmd
# Backend folder mein hona chahiye
# C:\Users\KSHITIJ\Desktop\blind\backend

# Models folder structure banao
mkdir models
cd models
mkdir vosk_models
cd vosk_models

# Check current path
cd
# Output: C:\Users\KSHITIJ\Desktop\blind\backend\models\vosk_models
```

### Step 5.2: English Model Download Karo

1. **Browser mein jao**: https://alphacephei.com/vosk/models
2. "**vosk-model-small-en-us-0.15**" khojo
3. Download link click karo (~40 MB)
4. ZIP file download hogi: `vosk-model-small-en-us-0.15.zip`
5. ZIP file ko extract karo
6. Extracted folder ko **copy karo**
7. Paste karo: `C:\Users\KSHITIJ\Desktop\blind\backend\models\vosk_models\`

### Step 5.3: Hindi Model Download Karo

1. Same website par "**vosk-model-small-hi-0.22**" khojo
2. Download karo (~42 MB)
3. Extract karo
4. Copy-paste karo same folder mein

### Step 5.4: Verify Models
```cmd
# vosk_models folder mein check karo
cd C:\Users\KSHITIJ\Desktop\blind\backend\models\vosk_models
dir

# Output mein 2 folders hone chahiye:
# vosk-model-small-en-us-0.15
# vosk-model-small-hi-0.22

# English model check karo
cd vosk-model-small-en-us-0.15
dir
# "am" folder aur conf files honi chahiye

# Back to backend folder
cd C:\Users\KSHITIJ\Desktop\blind\backend
```

---

## 🔊 Step 6: FFmpeg Install Karo (Audio Processing Ke Liye)

### Windows ke liye:

#### Step 6.1: Download Karo
1. Jao: https://www.gyan.dev/ffmpeg/builds/
2. "ffmpeg-release-essentials.zip" download karo (~70 MB)
3. ZIP extract karo

#### Step 6.2: PATH Mein Add Karo
1. Extracted folder mein `bin` folder khojo
2. Path copy karo: `C:\ffmpeg\bin` (example)
3. **Windows Key + R** dabao
4. Type karo: `sysdm.cpl` aur Enter
5. "Advanced" tab
6. "Environment Variables" button
7. "System variables" mein "Path" select karo
8. "Edit" click karo
9. "New" click karo
10. FFmpeg bin path paste karo
11. OK, OK, OK

#### Step 6.3: Verify Karo
```cmd
# Nayi Command Prompt kholo
ffmpeg -version

# Output: ffmpeg version x.x.x ...
```

### Linux ke liye:
```bash
sudo apt install ffmpeg -y
```

### macOS ke liye:
```bash
brew install ffmpeg
```

---

## ✅ Step 7: Backend Test Karo

### Step 7.1: Virtual Environment Active Karo (agar nahi hai)
```cmd
cd C:\Users\KSHITIJ\Desktop\blind\backend
venv\Scripts\activate

# (venv) dikhna chahiye
```

### Step 7.2: Test Script Run Karo
```cmd
# Simple test karo ki imports kaam kar rahe hain
python -c "import fastapi; import google.generativeai; print('✅ All imports successful')"

# Output: ✅ All imports successful
```

### Step 7.3: Server Start Karo (Test)
```cmd
# Server start karo
python -m uvicorn server.app:app --host 0.0.0.0 --port 8000

# Output dekho:
# INFO:     Started server process
# INFO:     Uvicorn running on http://0.0.0.0:8000 (Press CTRL+C to quit)
# INFO:     Application startup complete.

# ✅ Yeh sab dikhe to server running hai!
```

### Step 7.4: Browser Mein Test Karo
1. Browser kholo
2. Jao: `http://localhost:8000/docs`
3. ✅ Swagger UI (API documentation) dikhna chahiye
4. Test endpoint: `/health` par click karo → "Try it out" → "Execute"
5. Response: `{"status": "healthy"}`

### Step 7.5: Server Band Karo
- Command Prompt mein: **Ctrl+C** dabao
- Server stop ho jayega

---

## 🎯 Agla Step

Backend setup complete? Ekdum mast!

👉 **Ab Jao**: [05_FIRMWARE_UPLOAD.md](05_FIRMWARE_UPLOAD.md)

ESP32 mein firmware upload karo!
