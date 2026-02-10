# 💻 Software Install Karo Bhai | Software Installation

**Ekdum shuru se, har step detail mein**

---

## 📥 Step 1: Python Install Karo

### Windows ke liye:

#### Step 1.1: Download Karo
1. Browser kholo (Chrome/Edge)
2. Jao: https://www.python.org/downloads/
3. "Download Python 3.11.x" par click karo (yellow button)
4. File download hogi (~30 MB)

#### Step 1.2: Install Karo
1. Downloaded file par **double-click** karo
2. ⚠️ **Bahut Jaruri**: Niche "**Add Python to PATH**" ko ✅ tick karo
3. "**Install Now**" par click karo
4. Wait karo 2-3 minutes
5. "Setup was successful" dikhega
6. "Close" par click karo

#### Step 1.3: Check Karo Python Install Hua Ya Nahi
1. **Windows Key + R** dabao
2. Type karo: `cmd` aur Enter dabao
3. Black window khulegi (Command Prompt)
4. Type karo: `python --version`
5. Enter dabao
6. ✅ Agar dikhe "Python 3.11.x" to sahi hai
7. ❌ Agar error aaye to Python phir se install karo (PATH tick karna bhul gaye)

### Linux (Ubuntu) ke liye:

```bash
# Terminal kholo (Ctrl+Alt+T)

# Step 1: Update karo
sudo apt update

# Step 2: Python install karo
sudo apt install python3.11 python3-pip python3-venv -y

# Step 3: Check karo
python3 --version
# Output: Python 3.11.x
```

### macOS ke liye:

```bash
# Terminal kholo

# Step 1: Homebrew install karo (agar nahi hai)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Step 2: Python install karo
brew install python@3.11

# Step 3: Check karo
python3 --version
```

---

## 📥 Step 2: Git Install Karo

### Windows ke liye:

#### Step 2.1: Download Karo
1. Browser mein jao: https://git-scm.com/download/win
2. "64-bit Git for Windows Setup" automatically download hoga
3. File size: ~50 MB

#### Step 2.2: Install Karo
1. Downloaded file par **double-click** karo
2. "Next" par click karo (saare options default rakho)
3. **Important screen**: "Choosing the default editor"
   - Select karo: "Use Visual Studio Code as Git's default editor"
   - Next click karo
4. Baaki sab screens par "Next" click karte jao
5. Last mein "Install" click karo
6. Wait karo 2-3 minutes
7. "Finish" click karo

#### Step 2.3: Check Karo
1. **Nayi** Command Prompt kholo (purani band karo)
2. Type karo: `git --version`
3. Enter dabao
4. ✅ Output: "git version 2.x.x"

### Linux ke liye:

```bash
# Install karo
sudo apt install git -y

# Check karo
git --version
```

### macOS ke liye:

```bash
# Install karo
brew install git

# Check karo
git --version
```

---

## 📥 Step 3: VS Code Install Karo

### Windows ke liye:

#### Step 3.1: Download Karo
1. Jao: https://code.visualstudio.com/
2. "Download for Windows" par click karo
3. File download hogi (~100 MB)

#### Step 3.2: Install Karo
1. Downloaded file par double-click karo
2. "I accept the agreement" ko select karo
3. "Next" click karo
4. **Important screen**: "Select Additional Tasks"
   - ✅ Tick karo: "Add to PATH"
   - ✅ Tick karo: "Create a desktop icon"
   - Next click karo
5. "Install" click karo
6. Wait karo 2-3 minutes
7. ✅ "Launch Visual Studio Code" ko tick rakho
8. "Finish" click karo
9. VS Code khul jayega

### Linux ke liye:

```bash
# Download aur Install
sudo snap install --classic code

# ya
sudo apt install code -y
```

### macOS ke liye:

```bash
brew install --cask visual-studio-code
```

---

## 🔌 Step 4: PlatformIO Extension Install Karo

**VS Code khula hona chahiye**

#### Step 4.1: Extensions Panel Kholo
1. VS Code mein dekho left side mein icons
2. **Extensions icon** par click karo (ya **Ctrl+Shift+X** dabao)
3. Search box dikhega

#### Step 4.2: PlatformIO Khojo
1. Search box mein type karo: `platformio ide`
2. Enter dabao
3. Pehla result hoga: "**PlatformIO IDE**" by PlatformIO

#### Step 4.3: Install Karo
1. "**Install**" button par click karo
2. Installing... dikhega (wait karo 3-5 minutes)
3. ✅ Install complete hone par "Reload Required" dikhega
4. "**Reload Now**" par click karo
5. VS Code restart hoga

#### Step 4.4: Check Karo
1. VS Code restart hone ke baad
2. Left side mein ek naya icon dikhega: 👽 **PlatformIO** (alien head)
3. ✅ Yeh dikhe to installation successful

---

## 🔌 Step 5: USB Drivers Install Karo (ESP32 ke liye)

### Windows ke liye:

#### Step 5.1: CP210x Driver (Most ESP32 boards)
1. Jao: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. "CP210x Windows Drivers" download karo
3. Zip file extract karo
4. `CP210xVCPInstaller_x64.exe` run karo
5. "Install" click karo
6. "Success" dikhega

#### Step 5.2: CH340 Driver (Kuch ESP32 boards)
1. Jao: https://sparks.gogo.co.nz/ch340.html
2. "Windows" section mein download link click karo
3. Downloaded file extract karo
4. "SETUP.exe" run karo
5. "INSTALL" click karo

#### Step 5.3: Check Karo
1. ESP32 ko USB se connect karo
2. Device Manager kholo (Windows Key + X, phir "Device Manager")
3. "Ports (COM & LPT)" expand karo
4. ✅ Dikhna chahiye: "Silicon Labs CP210x USB to UART Bridge (COM3)" ya "USB-SERIAL CH340 (COM3)"
5. ❌ Agar "Unknown Device" dikhe to driver phir se install karo

### Linux ke liye:

```bash
# Usually no driver needed, but user ko dialout group mein add karo
sudo usermod -a -G dialout $USER

# Logout aur Login karo phir se
```

### macOS ke liye:

```bash
# Usually works automatically
# Agar nahi to CP210x driver Silicon Labs website se install karo
```

---

## ✅ Final Check - Sab Kuch Install Hua?

Command Prompt/Terminal mein ye commands run karo:

```bash
# Python check
python --version
# Expected: Python 3.11.x

# Pip check
pip --version
# Expected: pip 23.x.x

# Git check
git --version
# Expected: git version 2.x.x

# VS Code check
code --version
# Expected: 1.8x.x
```

### Sab kuch ✅ dikh raha hai?

---

## 🎯 Agla Step

Saare software install ho gaye? Badiya!

👉 **Ab Jao**: [03_HARDWARE_ASSEMBLY.md](03_HARDWARE_ASSEMBLY.md)

Hardware ko jodna seekho!
