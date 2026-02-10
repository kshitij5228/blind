# Backend Setup Guide

## Overview

The backend server for the Interactive AI Vision Assistant provides:
- **Speech-to-Text (STT)**: Converts user voice queries to text (FREE - no API costs!)
- **AI Vision**: Analyzes camera images and answers questions using Google Gemini
- **Text-to-Speech (TTS)**: Converts AI responses to audio for the user
- **Session Management**: Tracks conversation context across multiple interactions

## Prerequisites

- Python 3.9 or higher
- pip (Python package manager)
- Redis server (optional, for distributed deployments)
- Internet connection (for AI vision and online STT)

## Installation

### 1. Install Python Dependencies

```bash
cd backend
pip install -r requirements.txt
```

### 2. Download Vosk Models (for offline STT)

The system uses **completely FREE** speech-to-text services:
- **Primary**: SpeechRecognition with Google Web Speech API (free, no API key required)
- **Fallback**: Vosk (offline, unlimited free usage)

Download Vosk models for all supported languages (~120 MB total):

```bash
# Create models directory
mkdir -p models/vosk_models
cd models/vosk_models

# Download English model (~40 MB)
curl -L -O https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip
unzip vosk-model-small-en-us-0.15.zip
rm vosk-model-small-en-us-0.15.zip

# Download Hindi model (~42 MB)
curl -L -O https://alphacephei.com/vosk/models/vosk-model-small-hi-0.22.zip
unzip vosk-model-small-hi-0.22.zip
rm vosk-model-small-hi-0.22.zip

cd ../..
```

See [models/vosk_models/README.md](models/vosk_models/README.md) for detailed instructions.

### 3. Configure Environment Variables

Copy the example environment file and configure it:

```bash
cp .env.example .env
```

Edit `.env` and set the required values:

```env
# ============================================================================
# API Keys
# ============================================================================

# Google Gemini API Key (REQUIRED for AI vision)
# Get your free key from: https://makersuite.google.com/app/apikey
GEMINI_API_KEY=your_gemini_api_key_here

# ============================================================================
# STT Configuration (FREE - No API keys needed!)
# ============================================================================

# Vosk model paths (for offline STT fallback)
VOSK_MODEL_PATH_EN=backend/models/vosk_models/vosk-model-small-en-us-0.15
VOSK_MODEL_PATH_HI=backend/models/vosk_models/vosk-model-small-hi-0.22
VOSK_MODEL_PATH_MR=backend/models/vosk_models/vosk-model-small-mr-0.1

# ============================================================================
# TTS Configuration (FREE)
# ============================================================================

# Using Google TTS (gTTS) - completely free, no API key required

# ============================================================================
# Session Management
# ============================================================================

# Redis URL (optional, uses in-memory storage if not set)
REDIS_URL=redis://localhost:6379/0

# Session timeout in seconds (default: 300 = 5 minutes)
SESSION_TIMEOUT=300

# ============================================================================
# Server Configuration
# ============================================================================

# Server host and port
HOST=0.0.0.0
PORT=8000

# Enable debug mode (default: false)
DEBUG=false

# Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
LOG_LEVEL=INFO
```

### 4. Start Redis (Optional)

If using Redis for session management:

```bash
# Install Redis (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install redis-server

# Start Redis
sudo systemctl start redis
sudo systemctl enable redis

# Verify Redis is running
redis-cli ping  # Should return "PONG"
```

On Windows, use [Redis for Windows](https://github.com/microsoftarchive/redis/releases) or Docker.

## Running the Server

### Development Mode

```bash
cd backend
python server/app.py
```

The server will start on `http://0.0.0.0:8000`

### Production Mode (with Uvicorn)

```bash
uvicorn server.app:app --host 0.0.0.0 --port 8000 --workers 4
```

### Docker Deployment

```bash
# Build image
docker build -t vision-assistant-backend .

# Run container
docker run -d \
  -p 8000:8000 \
  -e GEMINI_API_KEY=your_key \
  -v $(pwd)/models:/app/models \
  --name vision-backend \
  vision-assistant-backend
```

## API Endpoints

### Health Check

```bash
GET /
```

Returns server status and available services.

### Speech-to-Text

```bash
POST /stt
Content-Type: multipart/form-data

{
  "audio_file": <WAV file, 16kHz, mono>
}
```

Returns:
```json
{
  "text": "transcribed text",
  "language": "en|hi|mr"
}
```

### Vision Query

```bash
POST /vision
Content-Type: multipart/form-data

{
  "image_file": <JPEG image>,
  "query": "What do you see?",
  "language": "en|hi|mr",
  "session_id": "optional_session_id"
}
```

Returns:
```json
{
  "response": "AI-generated description",
  "session_id": "session_id_for_context"
}
```

### Text-to-Speech

```bash
POST /tts
Content-Type: application/json

{
  "text": "Text to convert to speech",
  "language": "en|hi|mr"
}
```

Returns: MP3 audio file

## Cost Analysis

### ✅ FREE Services (No API costs!)

1. **Speech-to-Text (STT)**:
   - **Primary**: Google Web Speech API via SpeechRecognition library
     - Cost: **$0.00** (completely free, no API key required)
     - Limitations: ~50 requests/day per IP (sufficient for personal use)
   - **Fallback**: Vosk offline recognition
     - Cost: **$0.00** (completely free, unlimited usage)
     - Models: ~120 MB one-time download
   - **Total STT cost**: **$0.00/month** ✅

2. **Text-to-Speech (TTS)**:
   - Google TTS (gTTS) library
   - Cost: **$0.00** (completely free, no API key required)
   - Quality: Natural-sounding voices for all languages

### 💰 Paid Services

1. **AI Vision**: Google Gemini Pro Vision
   - Free tier: 60 requests/minute
   - Paid tier: $0.002/image after free tier
   - Typical usage: **$0-5/month** (for moderate use)

2. **Redis** (Optional):
   - Self-hosted: **$0.00**
   - Cloud (Redis Labs): Free tier available, then $5-10/month
   - Alternative: In-memory storage (no cost, but sessions lost on restart)

### 📊 Total Monthly Costs

| Service | Cost |
|---------|------|
| **STT (SpeechRecognition + Vosk)** | **$0.00** ✅ |
| **TTS (gTTS)** | **$0.00** ✅ |
| **Vision (Gemini)** | $0-5 (free tier + light paid usage) |
| **Redis** | $0-10 (optional) |
| **Total** | **$0-15/month** |

**Key Advantage**: Unlimited voice transcription with no API costs! 🎉

## Testing

### Run Unit Tests

```bash
cd backend
pytest tests/
```

### Manual Testing

1. Test STT:
```bash
curl -X POST http://localhost:8000/stt \
  -F "audio_file=@test_audio.wav"
```

2. Test Vision:
```bash
curl -X POST http://localhost:8000/vision \
  -F "image_file=@test_image.jpg" \
  -F "query=What do you see?" \
  -F "language=en"
```

3. Test TTS:
```bash
curl -X POST http://localhost:8000/tts \
  -H "Content-Type: application/json" \
  -d '{"text":"Hello world","language":"en"}' \
  --output test_output.mp3
```

## Troubleshooting

### STT Issues

1. **SpeechRecognition errors**:
   - Check internet connection
   - Verify audio format (WAV, 16kHz, mono)
   - Rate limiting: Wait a few minutes between requests

2. **Vosk model not found**:
   - Download models as per [models/vosk_models/README.md](models/vosk_models/README.md)
   - Check paths in `.env` file
   - Ensure models are extracted (not just downloaded ZIP files)

3. **Poor transcription accuracy**:
   - Ensure clear audio with minimal background noise
   - Check microphone quality
   - Verify correct language is being detected

### Vision API Issues

1. **Gemini API errors**:
   - Verify API key is correct
   - Check API quota at [Google AI Studio](https://makersuite.google.com/)
   - Ensure image is valid JPEG format

2. **Rate limiting**:
   - Free tier: 60 requests/minute
   - Add delays between requests if hitting limits

### Server Issues

1. **Port already in use**:
   ```bash
   # Kill process using port 8000
   lsof -ti:8000 | xargs kill -9
   ```

2. **Module not found**:
   ```bash
   # Reinstall dependencies
   pip install -r requirements.txt
   ```

## Performance Optimization

1. **Enable Redis**: Faster session lookups and persistence
2. **Use multiple workers**: `--workers 4` for production
3. **Preload Vosk models**: Models are loaded at startup for fast inference
4. **Cache frequently used prompts**: Reduce AI API calls

## Security

1. **API Keys**: Never commit `.env` file to version control
2. **CORS**: Configure allowed origins in production
3. **Rate Limiting**: Implement per-IP rate limiting for public deployments
4. **HTTPS**: Use reverse proxy (nginx) with SSL certificates for production

## Support

For issues or questions:
- Check documentation in `docs/` folder
- Review API specification in `api_spec.md`
- Check hardware setup in `../hardware/README.md`
