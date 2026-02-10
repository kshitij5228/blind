# Vosk Speech Recognition Models

This directory contains offline speech recognition models for the Vosk engine.

## Required Models

Download the following models for multilingual support:

### 1. English Model
- **Model**: `vosk-model-small-en-us-0.15`
- **Size**: ~40 MB
- **Download**: https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip
- **Extract to**: `backend/models/vosk_models/vosk-model-small-en-us-0.15/`

### 2. Hindi Model
- **Model**: `vosk-model-small-hi-0.22`
- **Size**: ~42 MB
- **Download**: https://alphacephei.com/vosk/models/vosk-model-small-hi-0.22.zip
- **Extract to**: `backend/models/vosk_models/vosk-model-small-hi-0.22/`

### 3. Marathi Model
- **Model**: `vosk-model-small-mr-0.1`
- **Size**: ~38 MB
- **Download**: https://alphacephei.com/vosk/models/vosk-model-small-mr-0.1.zip (if available)
- **Extract to**: `backend/models/vosk_models/vosk-model-small-mr-0.1/`
- **Note**: If Marathi model is not available, the system will fallback to Hindi for Devanagari script

## Installation Steps

```bash
# Create models directory
mkdir -p backend/models/vosk_models
cd backend/models/vosk_models

# Download English model
curl -L -O https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip
unzip vosk-model-small-en-us-0.15.zip
rm vosk-model-small-en-us-0.15.zip

# Download Hindi model
curl -L -O https://alphacephei.com/vosk/models/vosk-model-small-hi-0.22.zip
unzip vosk-model-small-hi-0.22.zip
rm vosk-model-small-hi-0.22.zip

# Download Marathi model (if available)
# Check https://alphacephei.com/vosk/models for availability
```

## Windows PowerShell Installation

```powershell
# Create models directory
New-Item -ItemType Directory -Force -Path backend\models\vosk_models
cd backend\models\vosk_models

# Download English model
Invoke-WebRequest -Uri "https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip" -OutFile "vosk-model-small-en-us-0.15.zip"
Expand-Archive -Path "vosk-model-small-en-us-0.15.zip" -DestinationPath "."
Remove-Item "vosk-model-small-en-us-0.15.zip"

# Download Hindi model
Invoke-WebRequest -Uri "https://alphacephei.com/vosk/models/vosk-model-small-hi-0.22.zip" -OutFile "vosk-model-small-hi-0.22.zip"
Expand-Archive -Path "vosk-model-small-hi-0.22.zip" -DestinationPath "."
Remove-Item "vosk-model-small-hi-0.22.zip"
```

## Directory Structure

After downloading and extracting, your directory should look like:

```
backend/models/vosk_models/
├── README.md (this file)
├── vosk-model-small-en-us-0.15/
│   ├── am/
│   ├── conf/
│   ├── graph/
│   └── ivector/
├── vosk-model-small-hi-0.22/
│   ├── am/
│   ├── conf/
│   ├── graph/
│   └── ivector/
└── vosk-model-small-mr-0.1/
    ├── am/
    ├── conf/
    ├── graph/
    └── ivector/
```

## Environment Configuration

Update your `.env` file with the model paths:

```env
VOSK_MODEL_PATH_EN=backend/models/vosk_models/vosk-model-small-en-us-0.15
VOSK_MODEL_PATH_HI=backend/models/vosk_models/vosk-model-small-hi-0.22
VOSK_MODEL_PATH_MR=backend/models/vosk_models/vosk-model-small-mr-0.1
```

## Model Information

- **Total Size**: ~120 MB (all 3 models)
- **License**: Apache 2.0
- **Source**: Alpha Cephei Inc.
- **Website**: https://alphacephei.com/vosk/

## Usage

The STT service will automatically load these models at startup. If a model is missing, a warning will be printed, but the service will still work with available models and the online SpeechRecognition fallback.

## Performance

- **Offline Recognition**: Works without internet connection
- **Low Latency**: <1 second for typical queries
- **Accuracy**: ~85-90% for clear audio in supported languages
- **No API Costs**: Completely free, unlimited usage

## Troubleshooting

1. **Model not found error**: Ensure the model is extracted to the exact path specified in `.env`
2. **Memory issues**: Small models require ~200MB RAM each when loaded
3. **Slow recognition**: Check that audio is 16kHz mono WAV format
4. **Poor accuracy**: Ensure clear audio with minimal background noise
