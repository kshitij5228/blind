"""
Middleware and Error Handling Module

Provides global exception handling, request logging, and rate limiting.
"""

import time
import traceback
from fastapi import Request, Response
from fastapi.responses import StreamingResponse
from starlette.middleware.base import BaseHTTPMiddleware
from collections import defaultdict
from datetime import datetime, timedelta
import io

from services.tts_service import get_tts_service

# ============================================================================
# Request Logging Middleware
# ============================================================================

class RequestLoggingMiddleware(BaseHTTPMiddleware):
    """Middleware for logging request details and processing time"""
    
    async def dispatch(self, request: Request, call_next):
        # Log request start
        start_time = time.time()
        
        # Extract request metadata
        client_ip = request.client.host if request.client else "unknown"
        method = request.method
        path = request.url.path
        
        print(f"[{datetime.utcnow().isoformat()}] {method} {path} from {client_ip}")
        
        # Process request
        try:
            response = await call_next(request)
            
            # Log response
            process_time = time.time() - start_time
            status_code = response.status_code
            
            print(f"[{datetime.utcnow().isoformat()}] {method} {path} - Status: {status_code} - Time: {process_time:.2f}s")
            
            # Add processing time header
            response.headers["X-Process-Time"] = str(process_time)
            
            return response
            
        except Exception as e:
            # Log error
            print(f"[{datetime.utcnow().isoformat()}] ERROR in {method} {path}: {str(e)}")
            traceback.print_exc()
            raise

# ============================================================================
# Rate Limiting Middleware
# ============================================================================

class RateLimitMiddleware(BaseHTTPMiddleware):
    """Simple rate limiting middleware"""
    
    def __init__(self, app, max_requests: int = 10, window_seconds: int = 60):
        """
        Initialize rate limiter
        
        Args:
            app: FastAPI application
            max_requests: Maximum requests per window
            window_seconds: Time window in seconds
        """
        super().__init__(app)
        self.max_requests = max_requests
        self.window_seconds = window_seconds
        self.request_counts = defaultdict(list)
    
    async def dispatch(self, request: Request, call_next):
        # Get session ID from form data or query params
        session_id = None
        
        if request.method == "POST":
            # Try to get session_id from query params first
            session_id = request.query_params.get("session_id")
            
            # If not in query params, try to extract from multipart form data
            if not session_id:
                content_type = request.headers.get("content-type", "")
                if "multipart/form-data" in content_type:
                    try:
                        # Clone the body stream for peeking
                        body = await request.body()
                        # Simple extraction: look for session_id field in form data
                        body_str = body.decode('utf-8', errors='ignore')
                        import re
                        match = re.search(r'name="session_id"\r?\n\r?\n([^\r\n]+)', body_str)
                        if match:
                            session_id = match.group(1).strip()
                    except Exception as e:
                        print(f"Could not extract session_id from form: {e}")
        
        if not session_id:
            # Use IP address as fallback
            session_id = request.client.host if request.client else "unknown"
        
        # Check rate limit
        now = datetime.utcnow()
        cutoff_time = now - timedelta(seconds=self.window_seconds)
        
        # Remove old requests
        self.request_counts[session_id] = [
            req_time for req_time in self.request_counts[session_id]
            if req_time > cutoff_time
        ]
        
        # Check if limit exceeded
        if len(self.request_counts[session_id]) >= self.max_requests:
            print(f"Rate limit exceeded for session: {session_id}")
            
            # Generate error audio
            tts = get_tts_service()
            error_text = "Rate limit exceeded. Please wait before making another request."
            audio_data = tts.synthesize(error_text, "en", "wav")
            
            if audio_data:
                return StreamingResponse(
                    io.BytesIO(audio_data),
                    media_type="audio/wav",
                    status_code=429,
                    headers={"X-Rate-Limit-Exceeded": "true"}
                )
            else:
                return Response(
                    content="Rate limit exceeded",
                    status_code=429,
                    media_type="text/plain"
                )
        
        # Add current request
        self.request_counts[session_id].append(now)
        
        # Process request
        response = await call_next(request)
        return response

# ============================================================================
# Global Exception Handler
# ============================================================================

async def global_exception_handler(request: Request, exc: Exception) -> Response:
    """
    Handle all unhandled exceptions
    
    Args:
        request: FastAPI request
        exc: Exception that occurred
    
    Returns:
        Error response with audio feedback
    """
    # Log full error
    print(f"UNHANDLED EXCEPTION: {str(exc)}")
    traceback.print_exc()
    
    # Try to determine language from request
    language = "en"
    try:
        if hasattr(request, "query_params"):
            language = request.query_params.get("language", "en")
    except:
        pass
    
    # Generate error audio
    tts = get_tts_service()
    
    error_messages = {
        "en": "Sorry, an unexpected error occurred. Please try again.",
        "hi": "क्षमा करें, एक अप्रत्याशित त्रुटि हुई। कृपया पुनः प्रयास करें।"
    }
    
    error_text = error_messages.get(language, error_messages["en"])
    audio_data = tts.synthesize(error_text, language, "wav")
    
    if audio_data:
        return StreamingResponse(
            io.BytesIO(audio_data),
            media_type="audio/wav",
            status_code=500,
            headers={
                "X-Error-Message": str(exc),
                "X-Error-Type": type(exc).__name__
            }
        )
    else:
        return Response(
            content=f"Error: {str(exc)}",
            status_code=500,
            media_type="text/plain"
        )

# ============================================================================
# Service Timing Decorator
# ============================================================================

def log_service_time(service_name: str):
    """
    Decorator to log service processing time
    
    Args:
        service_name: Name of the service
    """
    def decorator(func):
        async def async_wrapper(*args, **kwargs):
            start_time = time.time()
            result = await func(*args, **kwargs)
            elapsed_time = time.time() - start_time
            print(f"[TIMING] {service_name}: {elapsed_time:.2f}s")
            return result
        
        def sync_wrapper(*args, **kwargs):
            start_time = time.time()
            result = func(*args, **kwargs)
            elapsed_time = time.time() - start_time
            print(f"[TIMING] {service_name}: {elapsed_time:.2f}s")
            return result
        
        # Check if function is async
        import asyncio
        if asyncio.iscoroutinefunction(func):
            return async_wrapper
        else:
            return sync_wrapper
    
    return decorator

# ============================================================================
# CORS Middleware Configuration
# ============================================================================

def get_cors_config():
    """Get CORS configuration for development/production"""
    return {
        "allow_origins": ["*"],  # For production, restrict to specific origins
        "allow_credentials": True,
        "allow_methods": ["*"],
        "allow_headers": ["*"],
    }
