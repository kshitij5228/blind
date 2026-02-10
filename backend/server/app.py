"""
FastAPI Server Application

Main server application for the Interactive AI Vision Assistant backend.
Provides REST API endpoints for image analysis and audio response.
"""

import os
import io
from typing import Optional
from fastapi import FastAPI, UploadFile, File, Form, HTTPException
from fastapi.responses import StreamingResponse, JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager

# Import services and handlers
from services.stt_service import init_stt_service
from services.gemini_service import init_gemini_service
from services.tts_service import init_tts_service
from models.session import init_session_manager
from server.handlers import (
    handle_analyze_request,
    handle_session_reset,
    handle_get_session,
    generate_error_audio
)
from server.middleware import (
    RequestLoggingMiddleware,
    RateLimitMiddleware,
    global_exception_handler,
    get_cors_config
)

# Load environment variables
from dotenv import load_dotenv
load_dotenv()

# ============================================================================
# Application Lifespan Management
# ============================================================================

@asynccontextmanager
async def lifespan(app: FastAPI):
    """Application lifespan manager for startup/shutdown"""
    # Startup
    print("Starting Interactive AI Vision Assistant Backend...")
    
    # Initialize services
    gemini_key = os.getenv("GEMINI_API_KEY")
    redis_url = os.getenv("REDIS_URL")
    
    init_stt_service()  # Now using free backends (SpeechRecognition + Vosk)
    init_gemini_service(gemini_key)
    init_tts_service()
    init_session_manager(redis_url)
    
    print("All services initialized successfully!")
    
    yield
    
    # Shutdown
    print("Shutting down backend...")

# ============================================================================
# FastAPI Application
# ============================================================================

app = FastAPI(
    title="Interactive AI Vision Assistant API",
    description="Backend API for blind assistance with vision and conversation",
    version="1.0.0",
    lifespan=lifespan
)

# ============================================================================
# Middleware Configuration
# ============================================================================

# Add CORS middleware
cors_config = get_cors_config()
app.add_middleware(
    CORSMiddleware,
    **cors_config
)

# Add request logging middleware
app.add_middleware(RequestLoggingMiddleware)

# Add rate limiting middleware (10 requests per minute per session)
app.add_middleware(RateLimitMiddleware, max_requests=10, window_seconds=60)

# Add global exception handler
app.add_exception_handler(Exception, global_exception_handler)

# ============================================================================
# API Endpoints
# ============================================================================

@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "service": "Interactive AI Vision Assistant API",
        "version": "1.0.0",
        "status": "running"
    }

@app.get("/api/v1/health")
async def health_check():
    """Health check endpoint"""
    return {
        "status": "healthy",
        "services": {
            "stt": "available",
            "gemini": "available",
            "tts": "available",
            "session": "available"
        }
    }

@app.post("/api/v1/analyze")
async def analyze(
    session_id: str = Form(...),
    mode: str = Form(...),
    image: UploadFile = File(...),
    audio: Optional[UploadFile] = File(None)
):
    """
    Main analysis endpoint
    
    Accepts multipart/form-data with:
    - session_id: User session ID
    - mode: "snapshot" or "conversation"
    - image: JPEG image file
    - audio: WAV audio file (optional, for conversation mode)
    
    Returns:
    - Audio stream (MP3 format) with AI response
    """
    try:
        # Get file size limits from environment or use defaults
        MAX_IMAGE_SIZE_MB = float(os.getenv("MAX_IMAGE_SIZE_MB", "5"))
        MAX_AUDIO_SIZE_MB = float(os.getenv("MAX_AUDIO_SIZE_MB", "10"))
        
        # Validate mode
        if mode not in ["snapshot", "conversation"]:
            raise HTTPException(status_code=400, detail="Invalid mode. Must be 'snapshot' or 'conversation'")
        
        # Validate image format and size
        if not image.filename.lower().endswith(('.jpg', '.jpeg')):
            raise HTTPException(status_code=400, detail="Image must be JPEG format")
        
        # Check image content type
        if image.content_type and not image.content_type.startswith('image/jpeg'):
            raise HTTPException(status_code=400, detail="Image must be JPEG format")
        
        # Read and validate image size
        image_data = await image.read()
        image_size_mb = len(image_data) / (1024 * 1024)
        if image_size_mb > MAX_IMAGE_SIZE_MB:
            raise HTTPException(
                status_code=413,
                detail=f"Image too large: {image_size_mb:.2f}MB (max: {MAX_IMAGE_SIZE_MB}MB)"
            )
        
        # Validate JPEG magic bytes
        if len(image_data) < 2 or not (image_data[0] == 0xFF and image_data[1] == 0xD8):
            raise HTTPException(status_code=400, detail="Invalid JPEG file")
        
        # Validate audio format and size if provided
        audio_data = None
        if audio:
            if not audio.filename.lower().endswith('.wav'):
                raise HTTPException(status_code=400, detail="Audio must be WAV format")
            
            # Check audio content type
            if audio.content_type and not audio.content_type.startswith('audio/'):
                raise HTTPException(status_code=400, detail="Audio must be WAV format")
            
            # Read and validate audio size
            audio_data = await audio.read()
            audio_size_mb = len(audio_data) / (1024 * 1024)
            if audio_size_mb > MAX_AUDIO_SIZE_MB:
                raise HTTPException(
                    status_code=413,
                    detail=f"Audio too large: {audio_size_mb:.2f}MB (max: {MAX_AUDIO_SIZE_MB}MB)"
                )
            
            # Validate WAV magic bytes (RIFF header)
            if len(audio_data) < 4 or audio_data[:4] != b'RIFF':
                raise HTTPException(status_code=400, detail="Invalid WAV file")
        
        # Process request
        audio_response, response_text, detected_language = await handle_analyze_request(
            session_id=session_id,
            mode=mode,
            image_data=image_data,
            audio_data=audio_data
        )
        
        if not audio_response:
            raise HTTPException(status_code=500, detail="Failed to generate audio response")
        
        # Return audio stream (WAV format for I2S DAC compatibility)
        return StreamingResponse(
            io.BytesIO(audio_response),
            media_type="audio/wav",
            headers={
                "X-Response-Text": response_text[:200],  # First 200 chars for debugging
                "X-Detected-Language": detected_language,
                "Content-Disposition": f"attachment; filename=response.wav"
            }
        )
    
    except HTTPException:
        raise
    except Exception as e:
        print(f"Error in analyze endpoint: {e}")
        # Generate error audio
        error_audio = generate_error_audio(str(e), "en")
        if error_audio:
            return StreamingResponse(
                io.BytesIO(error_audio),
                media_type="audio/wav",
                status_code=500
            )
        else:
            raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/v1/session/reset")
async def reset_session(session_id: str = Form(...)):
    """
    Reset a user session
    
    Args:
        session_id: Session ID to reset
    
    Returns:
        Success confirmation
    """
    try:
        success = handle_session_reset(session_id)
        return {
            "status": "success" if success else "failed",
            "session_id": session_id,
            "message": "Session reset successfully"
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/session/{session_id}")
async def get_session(session_id: str):
    """
    Retrieve session information
    
    Args:
        session_id: Session ID to retrieve
    
    Returns:
        Session data or 404 if not found
    """
    try:
        session_data = handle_get_session(session_id)
        
        if session_data:
            return JSONResponse(content=session_data)
        else:
            raise HTTPException(status_code=404, detail="Session not found")
    
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# ============================================================================
# Run Server
# ============================================================================

if __name__ == "__main__":
    import uvicorn
    
    # Get configuration from environment
    host = os.getenv("SERVER_HOST", "0.0.0.0")
    port = int(os.getenv("SERVER_PORT", 8000))
    log_level = os.getenv("LOG_LEVEL", "info").lower()
    
    print(f"Starting server on {host}:{port}")
    
    uvicorn.run(
        "app:app",
        host=host,
        port=port,
        log_level=log_level,
        reload=False
    )
