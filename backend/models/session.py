"""
Session Management Module

Handles session creation, storage, and retrieval with language awareness.
Supports Redis for production and in-memory storage for development.
"""

import uuid
import time
from typing import Dict, List, Optional
from datetime import datetime, timedelta
import json

# Try to import Redis, fall back to in-memory storage
try:
    import redis
    REDIS_AVAILABLE = True
except ImportError:
    REDIS_AVAILABLE = False
    print("Redis not available, using in-memory storage")

# ============================================================================
# Session Data Structure
# ============================================================================

class SessionData:
    """Represents a user session with chat history and language preference"""
    
    def __init__(self, session_id: str = None):
        self.session_id = session_id or str(uuid.uuid4())
        self.created_at = datetime.utcnow().isoformat()
        self.last_activity = datetime.utcnow().isoformat()
        self.detected_language = None
        self.chat_history = []
    
    def to_dict(self) -> Dict:
        """Convert session to dictionary"""
        return {
            "session_id": self.session_id,
            "created_at": self.created_at,
            "last_activity": self.last_activity,
            "detected_language": self.detected_language,
            "chat_history": self.chat_history
        }
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'SessionData':
        """Create session from dictionary"""
        session = cls(data.get("session_id"))
        session.created_at = data.get("created_at", datetime.utcnow().isoformat())
        session.last_activity = data.get("last_activity", datetime.utcnow().isoformat())
        session.detected_language = data.get("detected_language")
        session.chat_history = data.get("chat_history", [])
        return session
    
    def add_interaction(self, user_query: str, detected_language: str, 
                       image_path: str, ai_response: str):
        """Add an interaction to chat history"""
        interaction = {
            "timestamp": datetime.utcnow().isoformat(),
            "user_query": user_query,
            "detected_language": detected_language,
            "image_path": image_path,
            "ai_response": ai_response
        }
        self.chat_history.append(interaction)
        
        # Keep only last 10 interactions
        if len(self.chat_history) > 10:
            self.chat_history = self.chat_history[-10:]
        
        # Update language if not set
        if not self.detected_language:
            self.detected_language = detected_language
        
        self.last_activity = datetime.utcnow().isoformat()

# ============================================================================
# Session Manager
# ============================================================================

class SessionManager:
    """Manages user sessions with Redis or in-memory storage"""
    
    def __init__(self, redis_url: str = None, session_ttl: int = 1800):
        """
        Initialize session manager
        
        Args:
            redis_url: Redis connection URL (e.g., redis://localhost:6379)
            session_ttl: Session time-to-live in seconds (default: 30 minutes)
        """
        self.session_ttl = session_ttl
        self.redis_client = None
        self.memory_storage = {}
        
        if REDIS_AVAILABLE and redis_url:
            try:
                self.redis_client = redis.from_url(redis_url, decode_responses=True)
                self.redis_client.ping()
                print(f"Connected to Redis at {redis_url}")
            except Exception as e:
                print(f"Failed to connect to Redis: {e}")
                print("Falling back to in-memory storage")
                self.redis_client = None
        else:
            print("Using in-memory storage for sessions")
    
    def create_session(self, session_id: str = None) -> SessionData:
        """Create a new session"""
        session = SessionData(session_id)
        self._save_session(session)
        return session
    
    def get_session(self, session_id: str) -> Optional[SessionData]:
        """Retrieve a session by ID"""
        if self.redis_client:
            try:
                data = self.redis_client.get(f"session:{session_id}")
                if data:
                    return SessionData.from_dict(json.loads(data))
            except Exception as e:
                print(f"Error retrieving session from Redis: {e}")
        else:
            if session_id in self.memory_storage:
                return self.memory_storage[session_id]
        
        return None
    
    def update_session(self, session: SessionData):
        """Update an existing session"""
        session.last_activity = datetime.utcnow().isoformat()
        self._save_session(session)
    
    def delete_session(self, session_id: str):
        """Delete a session"""
        if self.redis_client:
            try:
                self.redis_client.delete(f"session:{session_id}")
            except Exception as e:
                print(f"Error deleting session from Redis: {e}")
        else:
            if session_id in self.memory_storage:
                del self.memory_storage[session_id]
    
    def _save_session(self, session: SessionData):
        """Save session to storage backend"""
        if self.redis_client:
            try:
                key = f"session:{session.session_id}"
                value = json.dumps(session.to_dict())
                self.redis_client.setex(key, self.session_ttl, value)
            except Exception as e:
                print(f"Error saving session to Redis: {e}")
        else:
            self.memory_storage[session.session_id] = session
    
    def cleanup_expired_sessions(self):
        """Clean up expired sessions (for in-memory storage only)"""
        if not self.redis_client:
            now = datetime.utcnow()
            expired_sessions = []
            
            for session_id, session in self.memory_storage.items():
                last_activity = datetime.fromisoformat(session.last_activity)
                if now - last_activity > timedelta(seconds=self.session_ttl):
                    expired_sessions.append(session_id)
            
            for session_id in expired_sessions:
                del self.memory_storage[session_id]
            
            if expired_sessions:
                print(f"Cleaned up {len(expired_sessions)} expired sessions")
    
    def get_or_create_session(self, session_id: str = None) -> SessionData:
        """Get existing session or create new one"""
        if session_id:
            session = self.get_session(session_id)
            if session:
                return session
        
        return self.create_session(session_id)

# ============================================================================
# Global Session Manager Instance
# ============================================================================

# This will be initialized in the main app
session_manager = None

def init_session_manager(redis_url: str = None, session_ttl: int = 1800):
    """Initialize the global session manager"""
    global session_manager
    session_manager = SessionManager(redis_url, session_ttl)
    return session_manager

def get_session_manager() -> SessionManager:
    """Get the global session manager instance"""
    global session_manager
    if session_manager is None:
        session_manager = SessionManager()
    return session_manager
