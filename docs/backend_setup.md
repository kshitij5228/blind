# Backend Setup Guide

## Interactive AI Vision Assistant - Backend Server Configuration

This guide provides detailed instructions for setting up and running the Python backend server.

---

## Prerequisites

### System Requirements
- Python 3.9 or later
- pip (Python package manager)
- 2GB RAM minimum (4GB recommended)
- Internet connection for API services
- Optional: Redis server for session management

### Operating System
- Linux (Ubuntu 20.04+ recommended)
- macOS 10.15+
- Windows 10/11

---

## Installation Steps

### Step 1: Install Python

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install python3 python3-pip python3-venv
```

#### macOS:
```bash
brew install python@3.9
```

#### Windows:
1. Download Python from [python.org](https://www.python.org/downloads/)
2. Run installer and check "Add Python to PATH"
3. Verify installation: `python --version`

### Step 2: Clone or Download Project

Navigate to the backend directory:
```bash
cd backend
```

### Step 3: Create Virtual Environment

#### Linux/macOS:
```bash
python3 -m venv venv
source venv/bin/activate
```

#### Windows:
```bash
python -m venv venv
venv\Scripts\activate
```

You should see `(venv)` in your terminal prompt.

### Step 4: Install Dependencies

```bash
pip install --upgrade pip
pip install -r requirements.txt
```

This will install:
- FastAPI (web framework)
- Uvicorn (ASGI server)
- SpeechRecognition (free Google Web Speech API)
- Vosk (offline STT)
- Google Generative AI (for Gemini)
- gTTS (text-to-speech)
- pyttsx3 (offline TTS fallback)
- Redis (session management)
- Other utilities

---

## API Keys Configuration

### Step 1: Obtain API Keys

#### Google Gemini API Key
1. Go to [ai.google.dev](https://ai.google.dev/)
2. Click "Get API key"
3. Create or select a project
4. Generate API key
5. Copy the key

### Step 2: Configure Environment Variables

1. Copy the example environment file:
   ```bash
   cp .env.example .env
   ```

2. Edit `.env` file with your favorite text editor:
   ```bash
   nano .env
   # or
   vim .env
   # or use any text editor
   ```

3. Fill in your API keys:
   ```
   GEMINI_API_KEY=your-gemini-key-here
   VOSK_MODEL_PATH_EN=backend/models/vosk_models/vosk-model-small-en-us-0.15
   VOSK_MODEL_PATH_HI=backend/models/vosk_models/vosk-model-small-hi-0.22
   VOSK_MODEL_PATH_MR=backend/models/vosk_models/vosk-model-small-mr-0.1
   ```

4. Configure other settings:
   ```
   # Redis (optional - leave empty for in-memory storage)
   REDIS_URL=redis://localhost:6379
   
   # Server settings
   SERVER_HOST=0.0.0.0
   SERVER_PORT=8000
   LOG_LEVEL=INFO
   
   # Session configuration
   SESSION_TTL=1800
   
   # Rate limiting
   MAX_REQUESTS_PER_MINUTE=10
   ```

---

## Optional: Redis Setup

Redis provides persistent session storage and better performance. It's optional - the system will use in-memory storage if Redis is not available.

### Install Redis

#### Ubuntu/Debian:
```bash
sudo apt install redis-server
sudo systemctl start redis
sudo systemctl enable redis
```

#### macOS:
```bash
brew install redis
brew services start redis
```

#### Windows:
1. Download Redis for Windows from [GitHub](https://github.com/microsoftarchive/redis/releases)
2. Extract and run `redis-server.exe`

### Test Redis Connection
```bash
redis-cli ping
# Should respond with: PONG
```

### Configure Redis URL
In `.env` file:
```
REDIS_URL=redis://localhost:6379
```

---

## Running the Server

### Development Mode

#### Start the server:
```bash
cd backend
source venv/bin/activate  # On Windows: venv\Scripts\activate
python server/app.py
```

Or using uvicorn directly:
```bash
uvicorn server.app:app --host 0.0.0.0 --port 8000 --reload
```

The `--reload` flag enables auto-restart on code changes (development only).

### Expected Output:
```
Starting Interactive AI Vision Assistant Backend...
Using SpeechRecognition (Google Web Speech) as primary STT backend
Vosk offline STT initialized as fallback
Gemini 1.5 Flash initialized successfully
Using gTTS as primary TTS backend
Connected to Redis at redis://localhost:6379
All services initialized successfully!
INFO:     Started server process [12345]
INFO:     Waiting for application startup.
INFO:     Application startup complete.
INFO:     Uvicorn running on http://0.0.0.0:8000 (Press CTRL+C to quit)
```

### Test the Server

Open browser and visit:
- Health check: `http://localhost:8000/api/v1/health`
- API documentation: `http://localhost:8000/docs`

---

## Production Deployment

### Option 1: Direct Deployment

1. **Install supervisor** (for auto-restart):
   ```bash
   sudo apt install supervisor
   ```

2. **Create supervisor config** (`/etc/supervisor/conf.d/blind-assistant.conf`):
   ```ini
   [program:blind-assistant]
   directory=/path/to/backend
   command=/path/to/backend/venv/bin/python server/app.py
   user=youruser
   autostart=true
   autorestart=true
   stderr_logfile=/var/log/blind-assistant.err.log
   stdout_logfile=/var/log/blind-assistant.out.log
   environment=PATH="/path/to/backend/venv/bin"
   ```

3. **Start service**:
   ```bash
   sudo supervisorctl reread
   sudo supervisorctl update
   sudo supervisorctl start blind-assistant
   ```

### Option 2: Docker Deployment

1. **Create Dockerfile** (`backend/Dockerfile`):
   ```dockerfile
   FROM python:3.9-slim
   
   WORKDIR /app
   
   COPY requirements.txt .
   RUN pip install --no-cache-dir -r requirements.txt
   
   COPY . .
   
   EXPOSE 8000
   
   CMD ["python", "server/app.py"]
   ```

2. **Build and run**:
   ```bash
   docker build -t blind-assistant-backend .
   docker run -d -p 8000:8000 --env-file .env blind-assistant-backend
   ```

### Option 3: Cloud Deployment

#### Deploy to AWS EC2:
1. Launch EC2 instance (t2.medium or larger)
2. SSH into instance
3. Follow installation steps above
4. Configure security group to allow port 8000
5. Use Elastic IP for static IP address

#### Deploy to Google Cloud Run:
1. Containerize application with Docker
2. Push to Google Container Registry
3. Deploy to Cloud Run
4. Configure environment variables

#### Deploy to Heroku:
1. Create `Procfile`:
   ```
   web: python server/app.py
   ```
2. Commit code to git
3. Deploy: `git push heroku main`

---

## SSL/HTTPS Configuration

For production, use HTTPS for secure communication.

### Option 1: Nginx Reverse Proxy

1. **Install Nginx**:
   ```bash
   sudo apt install nginx
   ```

2. **Configure Nginx** (`/etc/nginx/sites-available/blind-assistant`):
   ```nginx
   server {
       listen 80;
       server_name your-domain.com;
       
       location / {
           proxy_pass http://localhost:8000;
           proxy_set_header Host $host;
           proxy_set_header X-Real-IP $remote_addr;
       }
   }
   ```

3. **Enable SSL with Let's Encrypt**:
   ```bash
   sudo apt install certbot python3-certbot-nginx
   sudo certbot --nginx -d your-domain.com
   ```

### Option 2: Built-in SSL (Development Only)

Generate self-signed certificate:
```bash
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
```

Run with SSL:
```bash
uvicorn server.app:app --host 0.0.0.0 --port 8000 --ssl-keyfile key.pem --ssl-certfile cert.pem
```

---

## Testing the Backend

### Test 1: Health Check
```bash
curl http://localhost:8000/api/v1/health
```

Expected response:
```json
{
  "status": "healthy",
  "services": {
    "stt": "available",
    "gemini": "available",
    "tts": "available",
    "session": "available"
  }
}
```

### Test 2: Analyze Endpoint (with sample files)

Create test request:
```bash
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=test-session-123" \
  -F "mode=snapshot" \
  -F "image=@test_image.jpg" \
  --output response.mp3
```

Play the audio response to verify TTS.

### Test 3: Session Management

Create session and retrieve:
```bash
# Make a request (creates session)
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=test-session-456" \
  -F "mode=snapshot" \
  -F "image=@test_image.jpg" \
  --output /dev/null

# Retrieve session
curl http://localhost:8000/api/v1/session/test-session-456
```

---

## Monitoring and Logging

### View Logs

The server logs include:
- Request timestamps and processing times
- Service timings (STT, Gemini, TTS)
- Error messages and stack traces

Logs are printed to stdout. In production, redirect to a file:
```bash
python server/app.py >> /var/log/blind-assistant.log 2>&1
```

### Monitor Performance

Key metrics to track:
- Request latency (target: <5s for snapshot, <8s for conversation)
- API call success rates
- Memory usage
- API costs

### Set Up Monitoring (Optional)

Use tools like:
- **Prometheus + Grafana** for metrics
- **Sentry** for error tracking
- **AWS CloudWatch** if deployed on AWS

---

## Troubleshooting

### Server won't start

**Error:** `ModuleNotFoundError: No module named 'fastapi'`
- **Solution:** Activate virtual environment and install dependencies
- **Solution:** Run `pip install -r requirements.txt`

**Error:** `Address already in use`
- **Solution:** Change port in `.env` file
- **Solution:** Kill process using port 8000: `sudo lsof -ti:8000 | xargs kill -9`

### API Key Errors
**Error:** `Gemini API quota exceeded`
- **Solution:** Check your Gemini API usage at ai.google.dev
- **Solution:** Wait for quota reset or upgrade plan

### Audio Issues

**Error:** `gTTS synthesis failed`
- **Solution:** Check internet connection
- **Solution:** Verify firewall allows outbound connections
- **Solution:** Fallback to pyttsx3 will be used automatically

**Error:** `No audio returned`
- **Solution:** Check TTS service logs
- **Solution:** Test with simple text input

### Performance Issues

**Slow response times:**
- Check network latency to API services
- Monitor CPU/memory usage
- Consider upgrading server resources
- Enable Redis for session caching

---

## API Cost Optimization

### Free Tier Usage
- **SpeechRecognition:** Free, ~50 requests/day per IP (no API key)
- **Vosk:** Free, unlimited offline usage
- **Gemini API:** Free quota available
- **gTTS:** Completely free, no API key required

### Cost Monitoring
Track your API usage:
- Gemini usage: ai.google.dev

### Optimization Tips
1. Cache TTS outputs for common phrases
2. Implement request rate limiting
3. Use smaller Gemini models if available

---

## Next Steps

After backend is running:
1. Note the server IP address and port
2. Update ESP32 Master firmware with server URL
3. Test end-to-end system integration
4. Refer to [User Guide](user_guide.md) for operation
5. Monitor logs for any issues

---

## Additional Resources

- [FastAPI Documentation](https://fastapi.tiangolo.com/)
- [Google Gemini Documentation](https://ai.google.dev/docs)
- [Redis Documentation](https://redis.io/documentation)
