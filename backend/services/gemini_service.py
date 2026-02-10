"""
Google Gemini AI Service Module

Handles multimodal AI interaction using Google Gemini 1.5 Flash.
Supports image analysis with conversational context and multilingual responses.
"""

import os
import base64
from typing import List, Dict, Optional
import io

try:
    import google.generativeai as genai
    GEMINI_AVAILABLE = True
except ImportError:
    GEMINI_AVAILABLE = False
    print("Google Generative AI library not available")

# ============================================================================
# Gemini Service Class
# ============================================================================

class GeminiService:
    """Google Gemini 1.5 Flash service for multimodal AI"""
    
    def __init__(self, api_key: str = None):
        """
        Initialize Gemini service
        
        Args:
            api_key: Google AI API key
        """
        self.api_key = api_key or os.getenv("GEMINI_API_KEY")
        self.model = None
        self.sessions = {}  # Store chat sessions per user session
        
        if not self.api_key:
            print("Warning: No Gemini API key provided!")
            return
        
        if not GEMINI_AVAILABLE:
            print("Warning: Gemini library not available!")
            return
        
        try:
            genai.configure(api_key=self.api_key)
            self.model = genai.GenerativeModel('gemini-1.5-flash')
            print("Gemini 1.5 Flash initialized successfully")
        except Exception as e:
            print(f"Error initializing Gemini: {e}")
    
    def get_system_instruction(self, language: str = "en") -> str:
        """
        Get the system instruction (base prompt) for the AI
        
        Args:
            language: Target language code (en, hi)
        
        Returns:
            System instruction string
        """
        language_instructions = {
            "en": "You are the real-time visual guide for a blind user. Your goal is to be their eyes.\n\n"
                  "Identity: The user is blind. Always prioritize safety and navigation.\n\n"
                  "Descriptive Style: Instead of saying 'There is a chair,' say 'There is a wooden chair about 3 feet directly in front of you.'\n\n"
                  "Spatial Awareness: Use clock-face directions (e.g., 'Your water bottle is at 2 o'clock') or Left/Right.\n\n"
                  "Conciseness: Be detailed but brief. Do not use flowery language.\n\n"
                  "Context: Use the provided chat history to understand if the user is asking a follow-up question about an object you already described.\n\n"
                  "IMPORTANT: Respond ONLY in English.",
            
            "hi": "आप एक दृष्टिहीन उपयोगकर्ता के लिए वास्तविक समय की दृश्य मार्गदर्शिका हैं। आपका लक्ष्य उनकी आँखें बनना है।\n\n"
                  "पहचान: उपयोगकर्ता दृष्टिहीन है। हमेशा सुरक्षा और नेविगेशन को प्राथमिकता दें।\n\n"
                  "वर्णनात्मक शैली: 'एक कुर्सी है' कहने के बजाय, कहें 'आपके सामने लगभग 3 फीट की दूरी पर एक लकड़ी की कुर्सी है।'\n\n"
                  "स्थानिक जागरूकता: घड़ी के चेहरे की दिशाओं का उपयोग करें (जैसे, 'आपकी पानी की बोतल 2 बजे है') या बाएं/दाएं।\n\n"
                  "संक्षिप्तता: विस्तृत लेकिन संक्षिप्त रहें। फूलदार भाषा का प्रयोग न करें।\n\n"
                  "संदर्भ: प्रदान किए गए चैट इतिहास का उपयोग यह समझने के लिए करें कि क्या उपयोगकर्ता किसी ऐसी वस्तु के बारे में अनुवर्ती प्रश्न पूछ रहा है जिसका आपने पहले ही वर्णन किया है।\n\n"
                  "महत्वपूर्ण: केवल हिंदी में उत्तर दें।"
        }
        
        return language_instructions.get(language, language_instructions["en"])
    
    def analyze_image(self, image_data: bytes, user_query: str, 
                     chat_history: List[Dict], language: str = "en",
                     session_id: str = None) -> str:
        """
        Analyze image with user query and chat history
        
        Args:
            image_data: Image data as bytes (JPEG format)
            user_query: User's question or empty for default description
            chat_history: Previous interactions in this session
            language: Target language code (en, hi)
            session_id: Session ID for maintaining conversation
        
        Returns:
            AI-generated response text
        """
        if not self.model:
            return "Sorry, AI service is not available."
        
        try:
            # Get or create chat session
            if session_id and session_id in self.sessions:
                chat = self.sessions[session_id]
            else:
                # Create new chat with system instruction
                system_instruction = self.get_system_instruction(language)
                # Pass system instruction when creating chat
                chat = self.model.start_chat(
                    history=[],
                    system_instruction=system_instruction
                )
                if session_id:
                    self.sessions[session_id] = chat
            
            # Prepare the prompt
            if user_query:
                prompt = user_query
            else:
                # Default prompt for snapshot mode
                prompts = {
                    "en": "Describe what you see in detail, focusing on obstacles and important objects.",
                    "hi": "विस्तार से बताएं कि आप क्या देखते हैं, बाधाओं और महत्वपूर्ण वस्तुओं पर ध्यान केंद्रित करें।"
                }
                prompt = prompts.get(language, prompts["en"])
            
            # Add context from chat history
            if chat_history and len(chat_history) > 0:
                context_prompts = {
                    "en": "\n\nPrevious context:",
                    "hi": "\n\nपिछला संदर्भ:"
                }
                prompt += context_prompts.get(language, context_prompts["en"])
                
                # Include last 3 interactions for context
                for interaction in chat_history[-3:]:
                    user_q = interaction.get("user_query", "")
                    ai_resp = interaction.get("ai_response", "")
                    if user_q:
                        prompt += f"\nUser: {user_q}"
                    if ai_resp:
                        prompt += f"\nAssistant: {ai_resp}"
            
            # Convert image to PIL Image for Gemini
            from PIL import Image
            image = Image.open(io.BytesIO(image_data))
            
            # Send message with image
            response = chat.send_message([prompt, image])
            
            response_text = response.text.strip()
            
            print(f"Gemini response (language: {language}): {response_text[:100]}...")
            return response_text
            
        except Exception as e:
            print(f"Gemini analysis error: {e}")
            error_messages = {
                "en": "Sorry, I couldn't analyze the image. Please try again.",
                "hi": "क्षमा करें, मैं छवि का विश्लेषण नहीं कर सका। कृपया पुनः प्रयास करें।"
            }
            return error_messages.get(language, error_messages["en"])
    
    def clear_session(self, session_id: str):
        """Clear chat session for a user"""
        if session_id in self.sessions:
            del self.sessions[session_id]
            print(f"Cleared Gemini session: {session_id}")

# ============================================================================
# Global Gemini Service Instance
# ============================================================================

gemini_service = None

def init_gemini_service(api_key: str = None) -> GeminiService:
    """Initialize the global Gemini service"""
    global gemini_service
    gemini_service = GeminiService(api_key)
    return gemini_service

def get_gemini_service() -> GeminiService:
    """Get the global Gemini service instance"""
    global gemini_service
    if gemini_service is None:
        gemini_service = GeminiService()
    return gemini_service
