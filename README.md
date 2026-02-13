# Interactive AI Vision Assistant for the Blind

An AI-powered assistive device that helps blind and visually impaired individuals understand their surroundings through voice interaction and computer vision.

## 🌟 Features

- **Dual-Mode Operation:**
  - **Snapshot Mode:** Quick scene description with a short button press
  - **Conversation Mode:** Ask specific questions about your environment with long press

- **Multilingual Support:**
  - English
  - Hindi (हिंदी)

- **Advanced AI Integration:**
  - Free speech-to-text (SpeechRecognition + Vosk offline)
  - Google Gemini 1.5 Flash for multimodal vision analysis
  - gTTS for natural text-to-speech
  - Contextual conversation with memory

- **Dual-MCU Architecture:**
  - ESP32 Master handles audio, Wi-Fi, and user interaction
  - ESP32-CAM manages camera and SD card buffering
  - Efficient memory management prevents OOM errors

## 🎯 System Architecture

```
┌─────────────┐      UART      ┌──────────────┐
│  ESP32-CAM  │ ←────────────→ │ ESP32 Master │
│  + Camera   │                │ + Audio I/O  │
│  + SD Card  │                │ + Wi-Fi      │
└─────────────┘                └──────┬───────┘
                                      │ HTTPS
                                      ↓
                              ┌───────────────┐
                              │ Python Backend│
                              │  ├─ STT       │
                              │  ├─ Gemini AI │
                              │  └─ TTS       │
                              └───────────────┘
```

## 📁 Project Structure

```
blind/
├── firmware/
│   ├── esp32_master/          # Primary ESP32 firmware
│   │   ├── main.cpp           # Main firmware code
│   │   ├── config.h           # Pin and system configuration
│   │   └── secrets.h.example  # Wi-Fi and server credentials template
│   ├── esp32_cam/             # ESP32-CAM firmware
│   │   ├── main.cpp           # Camera and SD card handling
│   │   └── config.h           # Camera and UART configuration
│   └── libraries/             # Shared utilities
├── backend/
│   ├── server/                # FastAPI server
│   │   ├── app.py             # Main application
│   │   ├── handlers.py        # Request handlers
│   │   └── middleware.py      # Logging and error handling
│   ├── services/              # AI services
│   │   ├── stt_service.py     # Speech-to-text
│   │   ├── gemini_service.py  # Gemini AI integration
│   │   └── tts_service.py     # Text-to-speech
│   ├── models/                # Data models
│   │   └── session.py         # Session management
│   ├── requirements.txt       # Python dependencies
│   └── .env.example           # Environment variables template
├── docs/
│   ├── hardware_setup.md      # Hardware assembly guide
│   ├── firmware_setup.md      # Firmware programming guide
│   ├── backend_setup.md       # Backend installation guide
│   ├── user_guide.md          # User operation manual
│   ├── protocol.md            # UART protocol specification
│   └── api_spec.md            # REST API documentation
└── tests/
    ├── firmware_tests/        # Firmware unit tests
    └── backend_tests/         # Backend service tests
```

## 🔧 Hardware Requirements

### Components
- 1x ESP32 DevKit V1 (with PSRAM recommended)
- 1x ESP32-CAM (AI-Thinker)
- 1x INMP441 I2S MEMS Microphone
- 1x PCM5102A I2S DAC Module
- 1x Push Button
- 1x Micro SD Card (8GB+, Class 10)
- 5V Power Supply (2A minimum)
- Jumper wires and breadboard

### Estimated Cost
~$40-60 USD total

## 🚀 Quick Start

### 1. Hardware Setup
Follow the detailed [Hardware Setup Guide](docs/hardware_setup.md) to:
- Assemble the ESP32 modules
- Connect audio components (microphone and speaker)
- Wire UART communication between MCUs
- Insert SD card and camera

### 2. Firmware Setup
Follow the [Firmware Setup Guide](docs/firmware_setup.md) to:
- Install Arduino IDE and ESP32 board support
- Configure Wi-Fi and server credentials
- Upload firmware to both ESP32 modules
- Test individual components

### 3. Backend Setup
Follow the [Backend Setup Guide](docs/backend_setup.md) to:
- Install Python 3.9+ and dependencies
- Configure API keys (Gemini)
- Start the backend server
- Test API endpoints

### 4. Usage
See the [User Guide](docs/user_guide.md) for:
- Operating the device
- Understanding button interactions
- Language support
- Best practices

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Hardware Setup](docs/hardware_setup.md) | Complete hardware assembly instructions |
| [Firmware Setup](docs/firmware_setup.md) | ESP32 programming guide |
| [Backend Setup](docs/backend_setup.md) | Python server installation |
| [User Guide](docs/user_guide.md) | How to use the device |
| [UART Protocol](docs/protocol.md) | Communication protocol specification |
| [API Specification](docs/api_spec.md) | REST API reference |

## 🎮 How to Use

### Snapshot Mode (Short Press)
1. Press and release button quickly (<0.5s)
2. Device captures image and describes the scene
3. Listen to audio response

**Example Response:**
> "There is a wooden chair about 3 feet directly in front of you. A table is at your 2 o'clock position, approximately 4 feet away."

### Conversation Mode (Long Press)
1. Press and hold button
2. Ask your question while holding
3. Release button when done
4. Listen to personalized answer

**Example:**
- **You:** "What color is this shirt?"
- **AI:** "The shirt is dark blue with white stripes."

## 🌐 Multilingual Support

The system automatically detects your language and responds accordingly:

- **English:** Speak naturally, AI responds in English
- **हिंदी (Hindi):** हिंदी में बोलें, जवाब भी हिंदी में मिलेगा

## 🔑 API Keys Required
### Google Gemini (Vision AI)
Get API key at [ai.google.dev](https://ai.google.dev/)

## 🛠️ Technology Stack

### Hardware
- ESP32 (dual modules)
- OV2640 Camera
- INMP441 I2S Microphone
- PCM5102A I2S DAC

### Firmware
- Arduino/ESP-IDF
- UART Binary Protocol
- I2S Audio Processing

### Backend
- Python 3.9+
- FastAPI (web framework)
- SpeechRecognition + Vosk (STT - 100% free)
- Google Gemini 1.5 Flash (Vision AI)
- gTTS (TTS)
- Redis (session storage)

## 📊 Performance

| Metric | Target | Typical |
|--------|--------|---------|
| Snapshot Mode Latency | <5s | 3-5s |
| Conversation Mode Latency | <8s | 5-8s |
| Image Quality | SVGA | 800x600 |
| Audio Quality | 44.1kHz | 16-bit |
| Session Memory | Last 10 | 10 interactions |

## 🔒 Privacy & Security

- Images processed transiently, not stored permanently
- Audio recordings deleted after transcription
- Sessions expire after 30 minutes
- No personal data retention
- Local processing where possible

**Note:** For production use, implement HTTPS and authentication.

## ⚠️ Safety Notice

**This device is an assistance tool, NOT a replacement for:**
- Guide dogs
- White canes
- Human assistance
- Professional mobility training

Always use traditional mobility aids in conjunction with this device.

## 🐛 Troubleshooting

### Hardware Issues
- **ESP32-CAM won't boot:** Use stronger power supply (1A+)
- **No audio:** Check I2S pin connections
- **UART errors:** Verify TX/RX crossover, common ground

### Software Issues
- **Wi-Fi won't connect:** Check 2.4GHz network, credentials
- **API errors:** Verify API keys in `.env` file
- **Slow responses:** Check internet connection, server resources

See documentation for detailed troubleshooting.

## 🤝 Contributing

Contributions are welcome! Areas for improvement:
- Additional language support
- Improved object detection algorithms
- Offline mode capabilities
- Mobile app integration
- Voice-only interaction mode

## 📝 License

This project is open-source and available under the MIT License.

## 🙏 Acknowledgments
- Google for Gemini 1.5 Flash API
- ESP32 community for hardware support
- gTTS for free text-to-speech
- Accessibility community for feedback

## 📧 Contact & Support

- Documentation: See `/docs` folder
- Issues: Report via GitHub Issues
- Questions: Check User Guide and API Spec

## 🗺️ Roadmap

### Phase 1 (Current)
- ✅ Basic vision assistance
- ✅ Multilingual support (English, Hindi, Marathi)
- ✅ Dual-mode operation

### Phase 2 (Planned)
- [ ] Additional languages
- [ ] Offline mode for basic descriptions
- [ ] Object tracking
- [ ] Navigation assistance

### Phase 3 (Future)
- [ ] Mobile app integration
- [ ] Wearable form factor
- [ ] Integration with smart home devices
- [ ] Community marketplace for extensions

## 🎓 Educational Use

This project is ideal for:
- IoT and embedded systems courses
- AI/ML application development
- Accessibility technology research
- Maker community projects

---

**Built with ❤️ for the blind and visually impaired community**

For detailed setup instructions, start with the [Hardware Setup Guide](docs/hardware_setup.md).
