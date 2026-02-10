# API Specification

## Interactive AI Vision Assistant - Backend REST API

This document defines the REST API endpoints for the backend server.

---

## Base URL

```
http://your-server-ip:8000
```

For production with SSL:
```
https://your-domain.com
```

---

## Authentication

Currently, the API does not require authentication. Session-based access control is implemented using session IDs.

**Future Enhancement:** Consider adding API key authentication for production deployments.

---

## Common Headers

### Request Headers
```
Content-Type: multipart/form-data
```

### Response Headers
```
Content-Type: audio/mpeg (for audio responses)
X-Process-Time: <processing_time_in_seconds>
X-Response-Text: <text_response_preview>
X-Detected-Language: <language_code>
```

---

## Endpoints

### 1. Root Endpoint

**GET /**

Returns basic service information.

#### Response
```json
{
  "service": "Interactive AI Vision Assistant API",
  "version": "1.0.0",
  "status": "running"
}
```

---

### 2. Health Check

**GET /api/v1/health**

Check the health status of all backend services.

#### Response (200 OK)
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

#### Response (500 Internal Server Error)
```json
{
  "status": "unhealthy",
  "services": {
    "stt": "unavailable",
    "gemini": "available",
    "tts": "available",
    "session": "available"
  },
  "error": "STT service initialization failed"
}
```

---

### 3. Analyze Image (Main Endpoint)

**POST /api/v1/analyze**

Process an image with optional audio query and return audio response.

#### Request

**Content-Type:** `multipart/form-data`

**Form Fields:**

| Field      | Type   | Required    | Description                                      |
|------------|--------|-------------|--------------------------------------------------|
| session_id | string | Yes         | Unique session identifier (UUID)                 |
| mode       | string | Yes         | "snapshot" or "conversation"                     |
| image      | file   | Yes         | JPEG image file (max 10MB)                       |
| audio      | file   | Conditional | WAV audio file (required if mode="conversation") |

**Example using curl:**
```bash
# Snapshot mode
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=550e8400-e29b-41d4-a716-446655440000" \
  -F "mode=snapshot" \
  -F "image=@photo.jpg" \
  --output response.mp3

# Conversation mode
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=550e8400-e29b-41d4-a716-446655440000" \
  -F "mode=conversation" \
  -F "image=@photo.jpg" \
  -F "audio=@question.wav" \
  --output response.mp3
```

#### Response (200 OK)

**Content-Type:** `audio/mpeg`

Returns MP3 audio stream with AI-generated response.

**Headers:**
```
X-Response-Text: There is a wooden chair directly in front...
X-Detected-Language: en
X-Process-Time: 4.23
Content-Disposition: attachment; filename=response.mp3
```

**Body:** Binary MP3 audio data

#### Response (400 Bad Request)

```json
{
  "detail": "Invalid mode. Must be 'snapshot' or 'conversation'"
}
```

Possible error messages:
- "Invalid mode. Must be 'snapshot' or 'conversation'"
- "Image must be JPEG format"
- "Audio must be WAV format"
- "Missing required field: session_id"

#### Response (429 Too Many Requests)

**Content-Type:** `audio/mpeg`

Returns audio message: "Rate limit exceeded. Please wait before making another request."

**Headers:**
```
X-Rate-Limit-Exceeded: true
```

#### Response (500 Internal Server Error)

**Content-Type:** `audio/mpeg`

Returns audio error message in detected language.

**Headers:**
```
X-Error-Message: <error_description>
X-Error-Type: <exception_type>
```

---

### 4. Reset Session

**POST /api/v1/session/reset**

Clear all conversation history and settings for a session.

#### Request

**Content-Type:** `application/x-www-form-urlencoded`

**Form Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| session_id | string | Yes | Session ID to reset |

**Example:**
```bash
curl -X POST http://localhost:8000/api/v1/session/reset \
  -F "session_id=550e8400-e29b-41d4-a716-446655440000"
```

#### Response (200 OK)
```json
{
  "status": "success",
  "session_id": "550e8400-e29b-41d4-a716-446655440000",
  "message": "Session reset successfully"
}
```

#### Response (500 Internal Server Error)
```json
{
  "detail": "Failed to reset session"
}
```

---

### 5. Get Session Information

**GET /api/v1/session/{session_id}**

Retrieve information about a specific session.

#### Path Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| session_id | string | Session ID to retrieve |

**Example:**
```bash
curl http://localhost:8000/api/v1/session/550e8400-e29b-41d4-a716-446655440000
```

#### Response (200 OK)
```json
{
  "session_id": "550e8400-e29b-41d4-a716-446655440000",
  "created_at": "2026-02-06T10:30:00.000Z",
  "last_activity": "2026-02-06T10:45:00.000Z",
  "detected_language": "en",
  "chat_history": [
    {
      "timestamp": "2026-02-06T10:30:15.000Z",
      "user_query": "[Snapshot Mode]",
      "detected_language": "en",
      "image_path": "temp_550e8400-e29b-41d4-a716-446655440000.jpg",
      "ai_response": "There is a wooden table directly in front of you..."
    },
    {
      "timestamp": "2026-02-06T10:32:20.000Z",
      "user_query": "What color is it?",
      "detected_language": "en",
      "image_path": "temp_550e8400-e29b-41d4-a716-446655440000.jpg",
      "ai_response": "The table is dark brown..."
    }
  ]
}
```

#### Response (404 Not Found)
```json
{
  "detail": "Session not found"
}
```

---

## Data Models

### Session Object
```typescript
{
  session_id: string;           // UUID
  created_at: string;           // ISO 8601 timestamp
  last_activity: string;        // ISO 8601 timestamp
  detected_language: string;    // "en" | "hi" | "mr" | null
  chat_history: Interaction[];  // Array of interactions (max 10)
}
```

### Interaction Object
```typescript
{
  timestamp: string;            // ISO 8601 timestamp
  user_query: string;           // User's question or "[Snapshot Mode]"
  detected_language: string;    // "en" | "hi" | "mr"
  image_path: string;           // Temporary image path
  ai_response: string;          // AI-generated response text
}
```

---

## Error Codes

| HTTP Code | Meaning | Common Causes |
|-----------|---------|---------------|
| 200 | Success | Request processed successfully |
| 400 | Bad Request | Invalid parameters, wrong file format |
| 404 | Not Found | Session not found |
| 429 | Too Many Requests | Rate limit exceeded (10 req/min) |
| 500 | Internal Server Error | Service failure, API error |

---

## Rate Limiting

### Limits
- **10 requests per minute** per session
- **Concurrent requests:** Unlimited (but throttled per session)

### Headers
No rate limit headers are currently returned, but may be added in future versions:
```
X-RateLimit-Limit: 10
X-RateLimit-Remaining: 5
X-RateLimit-Reset: 1675701600
```

### Behavior on Limit Exceeded
- Returns 429 status code
- Audio response: "Rate limit exceeded. Please wait before making another request."
- Session is not affected; wait 1 minute before retry

---

## Request Size Limits

| Resource | Limit | Notes |
|----------|-------|-------|
| Image File | 10 MB | JPEG only |
| Audio File | 5 MB | WAV only, 16kHz recommended |
| Total Request | 15 MB | Combined multipart payload |

---

## Response Times

### Target Latencies (95th percentile)

| Mode | Target | Typical Range |
|------|--------|---------------|
| Snapshot | <5s | 3-5 seconds |
| Conversation | <8s | 5-8 seconds |

### Breakdown
- STT (Conversation only): 1-2s
- Gemini Analysis: 2-4s
- TTS Generation: 1-2s
- Network/Processing: <1s

---

## Audio Formats

### Input Audio (WAV)
- **Format:** PCM WAV
- **Sample Rate:** 16000 Hz (recommended)
- **Bit Depth:** 16-bit
- **Channels:** Mono (1 channel)
- **Encoding:** Linear PCM

### Output Audio (MP3)
- **Format:** MP3
- **Bitrate:** 128 kbps
- **Sample Rate:** 44100 Hz (from gTTS)
- **Channels:** Mono or Stereo (depending on TTS backend)

---

## Image Formats

### Input Image (JPEG)
- **Format:** JPEG/JPG
- **Max Resolution:** No hard limit (recommended: 1024x768)
- **Color Space:** RGB
- **Orientation:** Any (AI handles rotation)

### Quality Recommendations
- **Min:** 320x240 (QVGA)
- **Recommended:** 800x600 (SVGA)
- **Max:** 1600x1200 (UXGA)

Higher resolution = better detail but longer processing time.

---

## Language Support

### Supported Languages

| Code | Language | STT | Gemini | TTS |
|------|----------|-----|--------|-----|
| en | English | ✓ | ✓ | ✓ |
| hi | Hindi | ✓ | ✓ | ✓ |
| mr | Marathi | ✓ | ✓ | ✓ |

### Language Detection
- Automatic detection via STT (SpeechRecognition or Vosk)
- Language persists in session
- Can be overridden by speaking in different language

---

## Session Management

### Session Lifecycle
1. **Creation:** Automatic on first request with new session_id
2. **Active:** Updated on each interaction
3. **Expiration:** 30 minutes of inactivity
4. **Cleanup:** Automatic garbage collection

### Session Storage
- **Development:** In-memory (lost on restart)
- **Production:** Redis (persistent, with TTL)

---

## CORS Policy

### Allowed Origins
- **Development:** `*` (all origins)
- **Production:** Specific domains (configure in deployment)

### Allowed Methods
- GET, POST, OPTIONS

### Allowed Headers
- Content-Type, Authorization, X-Requested-With

---

## WebSocket Support

Currently not supported. All interactions are via HTTP REST.

**Future Enhancement:** Consider WebSocket for real-time streaming.

---

## API Versioning

Current version: **v1**

Future versions will be accessible via:
```
/api/v2/analyze
/api/v3/analyze
```

---

## Example Client Implementations

### Python Client
```python
import requests

def analyze_image(session_id, image_path, mode="snapshot", audio_path=None):
    url = "http://localhost:8000/api/v1/analyze"
    
    files = {
        'image': ('image.jpg', open(image_path, 'rb'), 'image/jpeg')
    }
    
    data = {
        'session_id': session_id,
        'mode': mode
    }
    
    if mode == "conversation" and audio_path:
        files['audio'] = ('audio.wav', open(audio_path, 'rb'), 'audio/wav')
    
    response = requests.post(url, files=files, data=data)
    
    if response.status_code == 200:
        with open('response.mp3', 'wb') as f:
            f.write(response.content)
        return True
    else:
        print(f"Error: {response.status_code}")
        return False
```

### JavaScript Client
```javascript
async function analyzeImage(sessionId, imageFile, mode, audioFile = null) {
  const formData = new FormData();
  formData.append('session_id', sessionId);
  formData.append('mode', mode);
  formData.append('image', imageFile);
  
  if (mode === 'conversation' && audioFile) {
    formData.append('audio', audioFile);
  }
  
  const response = await fetch('http://localhost:8000/api/v1/analyze', {
    method: 'POST',
    body: formData
  });
  
  if (response.ok) {
    const audioBlob = await response.blob();
    const audioUrl = URL.createObjectURL(audioBlob);
    // Play audio
    const audio = new Audio(audioUrl);
    audio.play();
  } else {
    console.error('Error:', response.status);
  }
}
```

---

## Testing

### Health Check Test
```bash
curl http://localhost:8000/api/v1/health
```

### Analyze Endpoint Test
```bash
# Prepare test files
echo "Test" | espeak -w test_audio.wav
wget https://via.placeholder.com/640x480.jpg -O test_image.jpg

# Test snapshot mode
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=test-123" \
  -F "mode=snapshot" \
  -F "image=@test_image.jpg" \
  --output response.mp3

# Test conversation mode
curl -X POST http://localhost:8000/api/v1/analyze \
  -F "session_id=test-123" \
  -F "mode=conversation" \
  -F "image=@test_image.jpg" \
  -F "audio=@test_audio.wav" \
  --output response.mp3
```

---

## Security Considerations

### Current Implementation
- No authentication (suitable for local/trusted networks)
- No encryption (HTTP only)
- Session IDs are user-provided (trust-based)

### Production Recommendations
1. **Add Authentication:** API keys or OAuth2
2. **Enable HTTPS:** Use SSL/TLS certificates
3. **Validate Input:** File type, size, content validation
4. **Rate Limiting:** Per-IP or per-API-key limits
5. **Input Sanitization:** Prevent injection attacks
6. **Secure Session IDs:** Server-generated UUIDs

---

## Performance Optimization

### Caching
- TTS responses for common phrases
- Session data in Redis
- Static content with CDN

### Scaling
- Horizontal scaling with load balancer
- Separate STT/TTS workers
- Database connection pooling

---

## Monitoring

### Metrics to Track
- Request count (by endpoint, status code)
- Response times (p50, p95, p99)
- Error rates
- API costs (Gemini only - STT and TTS are free)
- Session count and lifetime

### Logging
All requests are logged with:
- Timestamp
- Client IP
- Endpoint
- Status code
- Processing time

---

## Future API Enhancements

### Planned Features
- Batch processing endpoint
- WebSocket for streaming
- Video analysis support
- Custom voice selection
- Response caching API
- Analytics endpoint

---

## Support

For API issues or questions:
- Check server logs
- Review [Backend Setup Guide](backend_setup.md)
- Test with provided examples
- Verify API key configuration
