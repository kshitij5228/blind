"""
Text-to-Speech Service Module

Handles text-to-speech conversion using gTTS and pyttsx3.
Supports multilingual audio generation (English, Hindi).
"""

import os
import io
from typing import Optional
import tempfile

# Try to import gTTS
try:
    from gtts import gTTS
    GTTS_AVAILABLE = True
except ImportError:
    GTTS_AVAILABLE = False
    print("gTTS library not available")

# Try to import pyttsx3 for offline fallback
try:
    import pyttsx3
    PYTTSX3_AVAILABLE = True
except ImportError:
    PYTTSX3_AVAILABLE = False
    print("pyttsx3 library not available")

# ============================================================================
# TTS Service Class
# ============================================================================

class TTSService:
    """Text-to-Speech service with multiple backends"""
    
    def __init__(self):
        """Initialize TTS service"""
        if GTTS_AVAILABLE:
            self.primary_backend = "gtts"
            print("Using gTTS as primary TTS backend")
        elif PYTTSX3_AVAILABLE:
            self.primary_backend = "pyttsx3"
            print("Using pyttsx3 as primary TTS backend")
        else:
            self.primary_backend = None
            print("Warning: No TTS backend available!")
        
        # Initialize pyttsx3 engine if available
        self.pyttsx3_engine = None
        if PYTTSX3_AVAILABLE:
            try:
                self.pyttsx3_engine = pyttsx3.init()
                self._configure_pyttsx3()
            except Exception as e:
                print(f"Error initializing pyttsx3: {e}")
                self.pyttsx3_engine = None
    
    def _configure_pyttsx3(self):
        """Configure pyttsx3 engine settings"""
        if not self.pyttsx3_engine:
            return
        
        # Set rate (speed of speech)
        self.pyttsx3_engine.setProperty('rate', 150)  # Default is 200
        
        # Set volume (0.0 to 1.0)
        self.pyttsx3_engine.setProperty('volume', 0.9)
        
        # Try to find appropriate voices for different languages
        voices = self.pyttsx3_engine.getProperty('voices')
        self.voice_map = {
            'en': None,
            'hi': None
        }
        
        for voice in voices:
            voice_lower = voice.name.lower()
            if 'english' in voice_lower or 'en' in voice_lower:
                self.voice_map['en'] = voice.id
            elif 'hindi' in voice_lower or 'hi' in voice_lower:
                self.voice_map['hi'] = voice.id
    
    def synthesize(self, text: str, language: str = "en", 
                   output_format: str = "wav") -> Optional[bytes]:
        """
        Convert text to speech
        
        Args:
            text: Text to convert to speech
            language: Language code (en, hi)
            output_format: Output audio format (wav is default for I2S DAC compatibility)
        
        Returns:
            Audio data as bytes (WAV format, 16kHz mono) or None on error
        """
        if not text:
            return None
        
        # Try primary backend first
        if self.primary_backend == "gtts":
            audio_data = self._synthesize_gtts(text, language)
            if audio_data:
                return audio_data
        
        # Fallback to pyttsx3
        if PYTTSX3_AVAILABLE and self.pyttsx3_engine:
            audio_data = self._synthesize_pyttsx3(text, language)
            if audio_data:
                return audio_data
        
        # If all fail, return None
        print("All TTS backends failed")
        return None
    
    def _synthesize_gtts(self, text: str, language: str) -> Optional[bytes]:
        """
        Synthesize speech using gTTS and convert to WAV for I2S DAC playback
        
        Args:
            text: Text to convert
            language: Language code
        
        Returns:
            Audio data as bytes (WAV format) or None on error
        """
        try:
            # Map language codes to gTTS language codes
            lang_map = {
                'en': 'en',
                'hi': 'hi'
            }
            
            gtts_lang = lang_map.get(language, 'en')
            
            # Create gTTS object
            tts = gTTS(text=text, lang=gtts_lang, slow=False)
            
            # Save to temporary MP3 file
            with tempfile.NamedTemporaryFile(suffix='.mp3', delete=False) as mp3_file:
                mp3_path = mp3_file.name
            
            tts.save(mp3_path)
            
            # Convert MP3 to WAV using pydub
            try:
                from pydub import AudioSegment
                
                # Load MP3
                audio = AudioSegment.from_mp3(mp3_path)
                
                # Convert to 16kHz mono for I2S DAC compatibility
                audio = audio.set_frame_rate(16000)
                audio = audio.set_channels(1)
                audio = audio.set_sample_width(2)  # 16-bit
                
                # Export to WAV
                wav_buffer = io.BytesIO()
                audio.export(wav_buffer, format='wav')
                wav_buffer.seek(0)
                audio_data = wav_buffer.read()
                
                # Clean up MP3 file
                os.unlink(mp3_path)
                
                print(f"gTTS synthesis successful (language: {gtts_lang}, WAV size: {len(audio_data)} bytes)")
                return audio_data
                
            except ImportError:
                # Fallback: return MP3 if pydub not available
                print("Warning: pydub not available, returning MP3 (will not play correctly on I2S DAC)")
                with open(mp3_path, 'rb') as f:
                    audio_data = f.read()
                os.unlink(mp3_path)
                return audio_data
            
        except Exception as e:
            print(f"gTTS synthesis error: {e}")
            return None
    
    def _synthesize_pyttsx3(self, text: str, language: str) -> Optional[bytes]:
        """
        Synthesize speech using pyttsx3 (offline)
        
        Args:
            text: Text to convert
            language: Language code
        
        Returns:
            Audio data as bytes (WAV format) or None on error
        """
        try:
            if not self.pyttsx3_engine:
                return None
            
            # Set voice based on language
            voice_id = self.voice_map.get(language)
            if voice_id:
                self.pyttsx3_engine.setProperty('voice', voice_id)
            
            # Create temporary file for output
            with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as temp_file:
                temp_file_path = temp_file.name
            
            # Save to file
            self.pyttsx3_engine.save_to_file(text, temp_file_path)
            self.pyttsx3_engine.runAndWait()
            
            # Read the file
            with open(temp_file_path, 'rb') as audio_file:
                audio_data = audio_file.read()
            
            # Clean up
            os.unlink(temp_file_path)
            
            print(f"pyttsx3 synthesis successful (language: {language}, size: {len(audio_data)} bytes)")
            return audio_data
            
        except Exception as e:
            print(f"pyttsx3 synthesis error: {e}")
            return None
    
    def add_silence_padding(self, audio_data: bytes, padding_ms: int = 100) -> bytes:
        """
        Add silence padding to audio for smooth playback
        
        Args:
            audio_data: Original audio data
            padding_ms: Padding duration in milliseconds
        
        Returns:
            Audio data with padding
        """
        # For simplicity, return original audio
        # Proper implementation would require audio processing library
        return audio_data

# ============================================================================
# Global TTS Service Instance
# ============================================================================

tts_service = None

def init_tts_service() -> TTSService:
    """Initialize the global TTS service"""
    global tts_service
    tts_service = TTSService()
    return tts_service

def get_tts_service() -> TTSService:
    """Get the global TTS service instance"""
    global tts_service
    if tts_service is None:
        tts_service = TTSService()
    return tts_service
