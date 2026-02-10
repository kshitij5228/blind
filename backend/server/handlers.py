"""
Request Handlers Module

Implements the main processing pipeline for the Interactive AI Vision Assistant.
Handles multipart requests, processes audio/image, and returns audio responses.
"""

import io
import os
import tempfile
from typing import Tuple, Optional
from fastapi import UploadFile

from services.stt_service import get_stt_service
from services.gemini_service import get_gemini_service
from services.tts_service import get_tts_service
from models.session import get_session_manager

# ============================================================================
# Constants
# ============================================================================

SUPPORTED_LANGUAGES = {'en', 'hi'}

# ============================================================================
# Helper Functions
# ============================================================================

def _validate_language(language: str) -> str:
    """
    Validate and sanitize language code to supported values.
    Logs warning if correction is needed.
    
    Args:
        language: Language code to validate
    
    Returns:
        Validated language code (en or hi)
    """
    if language not in SUPPORTED_LANGUAGES:
        print(f"Warning: Unsupported language '{language}' detected, falling back to 'en'")
        return 'en'
    return language

# ============================================================================
# Main Request Handler
# ============================================================================

async def handle_analyze_request(
    session_id: str,
    mode: str,
    image_data: bytes,
    audio_data: Optional[bytes] = None
) -> Tuple[bytes, str, str]:
    """
    Process analyze request and return audio response
    
    Args:
        session_id: User session ID
        mode: "snapshot" or "conversation"
        image_data: Image data as bytes (JPEG)
        audio_data: Audio data as bytes (WAV, optional)
    
    Returns:
        Tuple of (audio_data, response_text, detected_language)
    """
    # Get service instances
    stt = get_stt_service()
    gemini = get_gemini_service()
    tts = get_tts_service()
    session_mgr = get_session_manager()
    
    # Get or create session
    session = session_mgr.get_or_create_session(session_id)
    
    # Initialize variables
    user_query = ""
    detected_language = session.detected_language or "en"
    
    # Process audio if in conversation mode
    if mode == "conversation" and audio_data:
        # Transcribe audio
        transcribed_text, lang = stt.transcribe(audio_data, "wav")
        user_query = transcribed_text
        detected_language = lang
        
        print(f"Transcribed query: '{user_query}' (language: {detected_language})")
    else:
        # Snapshot mode - use default language or session language
        if not session.detected_language:
            detected_language = "en"
    
    # Save image temporarily for session tracking
    image_path = f"temp_{session_id}.jpg"
    
    # Analyze image with Gemini
    response_text = gemini.analyze_image(
        image_data=image_data,
        user_query=user_query,
        chat_history=session.chat_history,
        language=detected_language,
        session_id=session_id
    )
    
    print(f"Gemini response: {response_text[:100]}...")
    
    # Validate and sanitize language
    safe_language = _validate_language(detected_language)
    
    # Update session with interaction
    session.add_interaction(
        user_query=user_query if user_query else "[Snapshot Mode]",
        detected_language=safe_language,
        image_path=image_path,
        ai_response=response_text
    )
    session_mgr.update_session(session)
    
    # Convert response to speech (WAV format for I2S DAC compatibility)
    audio_response = tts.synthesize(
        text=response_text,
        language=safe_language,
        output_format="wav"
    )
    
    if not audio_response:
        # Fallback error message
        error_messages = {
            "en": "Sorry, I couldn't generate audio response.",
            "hi": "क्षमा करें, मैं ऑडियो प्रतिक्रिया उत्पन्न नहीं कर सका।"
        }
        error_text = error_messages.get(safe_language, error_messages["en"])
        audio_response = tts.synthesize(error_text, safe_language, "wav")
    
    return audio_response, response_text, safe_language

# ============================================================================
# Session Reset Handler
# ============================================================================

def handle_session_reset(session_id: str) -> bool:
    """
    Reset a user session
    
    Args:
        session_id: Session ID to reset
    
    Returns:
        True if successful
    """
    session_mgr = get_session_manager()
    gemini = get_gemini_service()
    
    # Clear session from session manager
    session_mgr.delete_session(session_id)
    
    # Clear Gemini chat history
    gemini.clear_session(session_id)
    
    print(f"Session reset: {session_id}")
    return True

# ============================================================================
# Session Retrieval Handler
# ============================================================================

def handle_get_session(session_id: str) -> Optional[dict]:
    """
    Retrieve session information
    
    Args:
        session_id: Session ID to retrieve
    
    Returns:
        Session data as dictionary or None
    """
    session_mgr = get_session_manager()
    session = session_mgr.get_session(session_id)
    
    if session:
        return session.to_dict()
    
    return None

# ============================================================================
# Error Handling Utilities
# ============================================================================

def generate_error_audio(error_message: str, language: str = "en") -> bytes:
    """
    Generate audio for error message
    
    Args:
        error_message: Error message text
        language: Language code (en, hi)
    
    Returns:
        Audio data as bytes (WAV format)
    """
    tts = get_tts_service()
    
    # Handle empty/None error messages
    if not error_message:
        error_message = "unknown error"
    
    # Validate language
    safe_language = _validate_language(language)
    
    error_messages = {
        "en": f"Sorry, an error occurred: {error_message}",
        "hi": f"क्षमा करें, एक त्रुटि हुई: {error_message}"
    }
    
    text = error_messages.get(safe_language, error_messages["en"])
    audio_data = tts.synthesize(text, safe_language, "wav")
    
    return audio_data if audio_data else b""
