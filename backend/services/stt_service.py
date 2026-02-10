"""
Speech-to-Text Service Module

Handles audio transcription using FREE alternatives:
- SpeechRecognition library with Google Web Speech API (primary, free, no API key)
- Vosk (fallback, completely free, offline)

Supports multilingual transcription with automatic language detection.
"""

import os
import io
from typing import Tuple, Optional
import tempfile
import json
import wave

# Import SpeechRecognition
try:
    import speech_recognition as sr
    SPEECH_RECOGNITION_AVAILABLE = True
except ImportError:
    SPEECH_RECOGNITION_AVAILABLE = False
    print("SpeechRecognition library not available")

# Import Vosk for offline recognition
try:
    from vosk import Model, KaldiRecognizer
    VOSK_AVAILABLE = True
except ImportError:
    VOSK_AVAILABLE = False
    print("Vosk library not available")

# Import pydub for audio conversion
try:
    from pydub import AudioSegment
    PYDUB_AVAILABLE = True
except ImportError:
    PYDUB_AVAILABLE = False
    print("Pydub library not available")

# Optional: language detection
try:
    from langdetect import detect
    LANGDETECT_AVAILABLE = True
except ImportError:
    LANGDETECT_AVAILABLE = False

# ============================================================================
# STT Service Class
# ============================================================================

class STTService:
    """Speech-to-Text service with free backends"""
    
    def __init__(self):
        """
        Initialize STT service with free backends
        
        Primary: SpeechRecognition (Google Web Speech API - free, no API key)
        Fallback: Vosk (offline, completely free)
        """
        self.recognizer = None
        self.vosk_models = {}
        
        # Initialize SpeechRecognition
        if SPEECH_RECOGNITION_AVAILABLE:
            self.recognizer = sr.Recognizer()
            self.primary_backend = "speech_recognition"
            print("Using SpeechRecognition (Google Web Speech) as primary STT backend")
        else:
            self.primary_backend = None
            print("Warning: SpeechRecognition not available!")
        
        # Initialize Vosk models
        if VOSK_AVAILABLE:
            self._load_vosk_models()
            print("Vosk offline STT initialized as fallback")
        else:
            print("Warning: Vosk not available for offline fallback!")
    
    def _load_vosk_models(self):
        """Load Vosk models for all supported languages"""
        # Get model paths from environment or use defaults
        model_paths = {
            'en': os.getenv('VOSK_MODEL_PATH_EN', 'backend/models/vosk_models/vosk-model-small-en-us-0.15'),
            'hi': os.getenv('VOSK_MODEL_PATH_HI', 'backend/models/vosk_models/vosk-model-small-hi-0.22')
        }
        
        for lang, path in model_paths.items():
            if os.path.exists(path):
                try:
                    self.vosk_models[lang] = Model(path)
                    print(f"Loaded Vosk model for {lang}: {path}")
                except Exception as e:
                    print(f"Error loading Vosk model for {lang}: {e}")
            else:
                print(f"Vosk model not found for {lang}: {path}")
                print(f"Download from https://alphacephei.com/vosk/models")
    
    def transcribe(self, audio_data: bytes, audio_format: str = "wav") -> Tuple[str, str]:
        """
        Transcribe audio to text with language detection
        
        Args:
            audio_data: Audio data as bytes
            audio_format: Audio format (wav, mp3, etc.)
        
        Returns:
            Tuple of (transcribed_text, detected_language_code)
        """
        # Try primary backend first (SpeechRecognition) when available
        if SPEECH_RECOGNITION_AVAILABLE and self.recognizer:
            result = self._transcribe_speech_recognition(audio_data, audio_format)
            if result:
                text, language = result
                if language not in {"en", "hi"}:
                    language = self._detect_language_from_text(text)
                return text, language
        
        # Fallback to Vosk (offline) whenever available
        if VOSK_AVAILABLE and self.vosk_models:
            result = self._transcribe_vosk(audio_data, audio_format)
            if result:
                text, language = result
                if language not in {"en", "hi"}:
                    language = self._detect_language_from_text(text)
                return text, language
        
        # Return empty if all fail
        print("All STT backends failed")
        return "", "en"
    
    def _transcribe_speech_recognition(self, audio_data: bytes, audio_format: str) -> Optional[Tuple[str, str]]:
        """
        Transcribe using SpeechRecognition (Google Web Speech API)
        
        Args:
            audio_data: Audio data as bytes
            audio_format: Audio format
        
        Returns:
            Tuple of (transcribed_text, detected_language_code) or None on error
        """
        try:
            # Convert bytes to AudioData
            # SpeechRecognition expects raw audio data
            audio_io = io.BytesIO(audio_data)
            
            # Read WAV file
            with sr.AudioFile(audio_io) as source:
                audio = self.recognizer.record(source)
            
            # Try recognizing with different languages
            languages = [
                ("en-US", "en"),
                ("hi-IN", "hi")
            ]
            
            best_result = None
            best_length = 0
            
            for lang_code, short_code in languages:
                try:
                    # Use Google Web Speech API (free, no API key required)
                    text = self.recognizer.recognize_google(audio, language=lang_code)
                    
                    if text and len(text) > best_length:
                        best_length = len(text)
                        best_result = (text.strip(), short_code)
                    
                except sr.UnknownValueError:
                    # Speech was unintelligible in this language
                    continue
                except sr.RequestError as e:
                    # API request failed (might be offline)
                    print(f"SpeechRecognition request error for {lang_code}: {e}")
                    continue
            
            if best_result:
                text, language = best_result
                print(f"SpeechRecognition transcription: '{text}' (language: {language})")
                return text, language
            
            return None
            
        except Exception as e:
            print(f"SpeechRecognition transcription error: {e}")
            return None
    
    def _transcribe_vosk(self, audio_data: bytes, audio_format: str) -> Optional[Tuple[str, str]]:
        """
        Transcribe using Vosk (offline)
        
        Args:
            audio_data: Audio data as bytes
            audio_format: Audio format
        
        Returns:
            Tuple of (transcribed_text, detected_language_code) or None on error
        """
        try:
            # Save audio to temporary WAV file for processing
            with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as temp_file:
                temp_file.write(audio_data)
                temp_file_path = temp_file.name
            
            # Read WAV file properties
            wf = wave.open(temp_file_path, "rb")
            
            # Vosk requires 16kHz sample rate
            sample_rate = wf.getframerate()
            if sample_rate != 16000:
                print(f"Warning: Vosk expects 16kHz audio, got {sample_rate}Hz")
            
            # Try each language model
            language_order = ['en', 'hi']  # Try English first, then Hindi
            
            for lang in language_order:
                if lang not in self.vosk_models:
                    continue
                
                try:
                    # Reset file pointer
                    wf.rewind()
                    
                    # Create recognizer with model
                    rec = KaldiRecognizer(self.vosk_models[lang], sample_rate)
                    rec.SetWords(True)
                    
                    # Process audio in chunks
                    while True:
                        data = wf.readframes(4000)
                        if len(data) == 0:
                            break
                        rec.AcceptWaveform(data)
                    
                    # Get final result
                    result = json.loads(rec.FinalResult())
                    text = result.get('text', '').strip()
                    
                    if text:
                        # Found non-empty transcription
                        print(f"Vosk transcription: '{text}' (language: {lang})")
                        wf.close()
                        os.unlink(temp_file_path)
                        return text, lang
                
                except Exception as e:
                    print(f"Error with Vosk model for {lang}: {e}")
                    continue
            
            wf.close()
            os.unlink(temp_file_path)
            
            # No successful transcription
            return None
            
        except Exception as e:
            print(f"Vosk transcription error: {e}")
            try:
                os.unlink(temp_file_path)
            except:
                pass
            return None

    def _detect_language_from_text(self, text: str) -> str:
        """
        Detect language from transcribed text as fallback

        Args:
            text: Transcribed text

        Returns:
            Language code (en, hi)
        """
        if not text:
            return "en"

        if LANGDETECT_AVAILABLE:
            try:
                detected = detect(text)

                # Map langdetect codes to our codes
                lang_map = {
                    'en': 'en',
                    'hi': 'hi'
                }

                return lang_map.get(detected, 'en')
            except:
                pass

        # Fallback: simple heuristic based on character sets
        # Devanagari script (used for Hindi): U+0900 to U+097F
        devanagari_count = sum(1 for c in text if '\u0900' <= c <= '\u097F')

        if devanagari_count > len(text) * 0.3:  # More than 30% Devanagari
            return "hi"

        return "en"

# ============================================================================
# Global STT Service Instance
# ============================================================================

stt_service = None

def init_stt_service() -> STTService:
    """Initialize the global STT service with free backends"""
    global stt_service
    stt_service = STTService()
    return stt_service

def get_stt_service() -> STTService:
    """Get the global STT service instance"""
    global stt_service
    if stt_service is None:
        stt_service = STTService()
    return stt_service
