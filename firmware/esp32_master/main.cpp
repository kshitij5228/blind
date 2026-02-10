/*
 * ESP32 Master Firmware for Interactive AI Vision Assistant
 * 
 * This firmware handles:
 * - Button input (short/long press detection)
 * - Audio recording via INMP441 I2S microphone
 * - Audio playback via PCM5102A I2S DAC
 * - Wi-Fi connectivity and management
 * - UART communication with ESP32-CAM
 * - HTTP communication with backend server
 * - State machine for user interaction flow
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>
#include "config.h"
#include "secrets.h"

// ============================================================================
// Global Variables
// ============================================================================

// State management
SystemState currentState = STATE_IDLE;
unsigned long buttonPressTime = 0;
bool buttonPressed = false;
bool longPressDetected = false;

// Audio buffers
uint8_t *recordBuffer = NULL;
size_t recordBufferIndex = 0;
uint8_t *imageBuffer = NULL;
size_t imageBufferSize = 0;

// Session ID
char sessionId[SESSION_ID_LENGTH + 1];

// Wi-Fi status
bool wifiConnected = false;

// ============================================================================
// Function Prototypes
// ============================================================================

void IRAM_ATTR buttonISR();
void setupButton();
void setupI2S();
void setupWiFi();
void setupUART();
void generateSessionId();
bool captureImageFromCAM();
bool recordAudio();
void stopRecording();
bool uploadToBackend(bool includeAudio);
bool playAudioResponse(WiFiClient &client);
void changeState(SystemState newState);
void blinkStatusLED(int times);
uint8_t calculateCRC8(uint8_t *data, size_t length);
bool sendUARTCommand(uint8_t cmdId, uint8_t *payload, size_t payloadLen);
bool receiveUARTResponse(uint8_t *statusCode, uint8_t *data, size_t *dataLen);
bool receiveImageChunks();
void createWAVHeader(uint8_t *header, uint32_t dataSize, uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample);

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Master Starting...");
  
  // Configure status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  blinkStatusLED(3);
  
  // Setup button
  setupButton();
  
  // Setup I2S for audio
  setupI2S();
  
  // Setup UART for ESP32-CAM communication
  setupUART();
  
  // Allocate buffers (use PSRAM if available)
  if (psramFound()) {
    Serial.println("PSRAM found, using it for buffers");
    recordBuffer = (uint8_t*)ps_malloc(AUDIO_RECORD_BUFFER_SIZE);
    imageBuffer = (uint8_t*)ps_malloc(MAX_IMAGE_SIZE);
  } else {
    Serial.println("PSRAM not found, using heap");
    recordBuffer = (uint8_t*)malloc(AUDIO_RECORD_BUFFER_SIZE);
    imageBuffer = (uint8_t*)malloc(MAX_IMAGE_SIZE);
  }
  
  if (!recordBuffer || !imageBuffer) {
    Serial.println("Failed to allocate buffers!");
    while (1) {
      blinkStatusLED(10);
      delay(1000);
    }
  }
  
  // Setup Wi-Fi
  setupWiFi();
  
  // Generate session ID
  generateSessionId();
  
  Serial.println("System ready!");
  blinkStatusLED(2);
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED && currentState == STATE_IDLE) {
    Serial.println("Wi-Fi disconnected, reconnecting...");
    setupWiFi();
  }
  
  // State machine
  switch (currentState) {
    case STATE_IDLE:
      // Check for button events (handled by interrupt)
      break;
      
    case STATE_RECORDING:
      // Recording is handled in button release
      break;
      
    case STATE_CAPTURING:
      // Capture image from ESP32-CAM
      if (captureImageFromCAM()) {
        Serial.println("Image captured successfully");
        changeState(STATE_UPLOADING);
      } else {
        Serial.println("Image capture failed");
        changeState(STATE_ERROR);
      }
      break;
      
    case STATE_UPLOADING:
      // Upload to backend (with or without audio)
      if (uploadToBackend(longPressDetected)) {
        Serial.println("Upload successful, playing response");
        // Response is played during upload
        changeState(STATE_IDLE);
      } else {
        Serial.println("Upload failed");
        changeState(STATE_ERROR);
      }
      longPressDetected = false;
      break;
      
    case STATE_PLAYING:
      // Handled in upload function
      changeState(STATE_IDLE);
      break;
      
    case STATE_ERROR:
      // Blink error pattern and return to idle
      blinkStatusLED(5);
      delay(1000);
      changeState(STATE_IDLE);
      break;
  }
  
  delay(10);
}

// ============================================================================
// Button Interrupt Service Routine
// ============================================================================

void IRAM_ATTR buttonISR() {
  unsigned long currentTime = millis();
  
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Button pressed
    if (!buttonPressed && (currentTime - buttonPressTime > BUTTON_DEBOUNCE_MS)) {
      buttonPressed = true;
      buttonPressTime = currentTime;
      // Start audio recording immediately for potential long press
      recordAudio();
    }
  } else {
    // Button released
    if (buttonPressed && (currentTime - buttonPressTime > BUTTON_DEBOUNCE_MS)) {
      unsigned long pressDuration = currentTime - buttonPressTime;
      
      if (pressDuration >= LONG_PRESS_MS) {
        // Long press - stop recording and process
        longPressDetected = true;
        stopRecording();
        changeState(STATE_CAPTURING);
      } else {
        // Short press - snapshot mode
        longPressDetected = false;
        changeState(STATE_CAPTURING);
      }
      
      buttonPressed = false;
    }
  }
}

// ============================================================================
// Button Setup
// ============================================================================

void setupButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);
}

// ============================================================================
// I2S Setup
// ============================================================================

void setupI2S() {
  // Configure I2S for microphone (I2S0)
  i2s_config_t mic_i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = MIC_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)MIC_BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t mic_pin_config = {
    .bck_io_num = MIC_SCK_PIN,
    .ws_io_num = MIC_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD_PIN
  };
  
  i2s_driver_install(MIC_I2S_NUM, &mic_i2s_config, 0, NULL);
  i2s_set_pin(MIC_I2S_NUM, &mic_pin_config);
  i2s_zero_dma_buffer(MIC_I2S_NUM);
  
  // Configure I2S for speaker (I2S1)
  i2s_config_t dac_i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = DAC_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)DAC_BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t dac_pin_config = {
    .bck_io_num = DAC_BCK_PIN,
    .ws_io_num = DAC_WS_PIN,
    .data_out_num = DAC_DIN_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  i2s_driver_install(DAC_I2S_NUM, &dac_i2s_config, 0, NULL);
  i2s_set_pin(DAC_I2S_NUM, &dac_pin_config);
  i2s_zero_dma_buffer(DAC_I2S_NUM);
  
  Serial.println("I2S initialized");
}

// ============================================================================
// Wi-Fi Setup
// ============================================================================

void setupWiFi() {
  Serial.printf("Connecting to Wi-Fi: %s\n", WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
    blinkStatusLED(1);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  } else {
    Serial.println("\nWi-Fi connection failed!");
    wifiConnected = false;
  }
}

// ============================================================================
// UART Setup
// ============================================================================

void setupUART() {
  Serial2.begin(CAM_BAUD_RATE, SERIAL_8N1, CAM_RX_PIN, CAM_TX_PIN);
  Serial.println("UART initialized");
}

// ============================================================================
// Generate Session ID
// ============================================================================

void generateSessionId() {
  // Generate a simple session ID using MAC address and timestamp
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(sessionId, SESSION_ID_LENGTH + 1, 
           "%02x%02x%02x%02x-%04lx-%04lx-%04lx-%04lx",
           mac[0], mac[1], mac[2], mac[3],
           (unsigned long)(millis() & 0xFFFF),
           (unsigned long)((millis() >> 16) & 0xFFFF),
           (unsigned long)(random(65536)),
           (unsigned long)(random(65536)));
  Serial.printf("Session ID: %s\n", sessionId);
}

// ============================================================================
// State Change Handler
// ============================================================================

void changeState(SystemState newState) {
  currentState = newState;
  Serial.printf("State changed to: %d\n", newState);
}

// ============================================================================
// Status LED Control
// ============================================================================

void blinkStatusLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100);
  }
}

// ============================================================================
// Audio Recording
// ============================================================================

bool recordAudio() {
  Serial.println("Recording audio...");
  recordBufferIndex = 0;
  
  // Skip WAV header space
  recordBufferIndex = WAV_HEADER_SIZE;
  
  changeState(STATE_RECORDING);
  
  while (buttonPressed && recordBufferIndex < AUDIO_RECORD_BUFFER_SIZE) {
    size_t bytesRead = 0;
    uint8_t i2sBuffer[I2S_READ_LEN];
    
    esp_err_t result = i2s_read(MIC_I2S_NUM, i2sBuffer, I2S_READ_LEN, &bytesRead, portMAX_DELAY);
    
    if (result == ESP_OK && bytesRead > 0) {
      size_t bytesToCopy = min(bytesRead, AUDIO_RECORD_BUFFER_SIZE - recordBufferIndex);
      memcpy(&recordBuffer[recordBufferIndex], i2sBuffer, bytesToCopy);
      recordBufferIndex += bytesToCopy;
    }
    
    delay(1);
  }
  
  return true;
}

void stopRecording() {
  if (currentState == STATE_RECORDING) {
    Serial.printf("Recording stopped. Recorded %d bytes\n", recordBufferIndex);
    
    // Add WAV header
    uint32_t dataSize = recordBufferIndex - WAV_HEADER_SIZE;
    createWAVHeader(recordBuffer, dataSize, MIC_SAMPLE_RATE, MIC_CHANNELS, MIC_BITS_PER_SAMPLE);
  }
}

// ============================================================================
// Create WAV Header
// ============================================================================

void createWAVHeader(uint8_t *header, uint32_t dataSize, uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample) {
  uint32_t byteRate = sampleRate * channels * (bitsPerSample / 8);
  uint16_t blockAlign = channels * (bitsPerSample / 8);
  uint32_t chunkSize = dataSize + 36;
  
  memcpy(&header[0], "RIFF", 4);
  memcpy(&header[4], &chunkSize, 4);
  memcpy(&header[8], "WAVE", 4);
  memcpy(&header[12], "fmt ", 4);
  
  uint32_t subchunk1Size = 16;
  memcpy(&header[16], &subchunk1Size, 4);
  
  uint16_t audioFormat = 1; // PCM
  memcpy(&header[20], &audioFormat, 2);
  memcpy(&header[22], &channels, 2);
  memcpy(&header[24], &sampleRate, 4);
  memcpy(&header[28], &byteRate, 4);
  memcpy(&header[32], &blockAlign, 2);
  memcpy(&header[34], &bitsPerSample, 2);
  
  memcpy(&header[36], "data", 4);
  memcpy(&header[40], &dataSize, 4);
}

// ============================================================================
// Capture Image from ESP32-CAM
// ============================================================================

bool captureImageFromCAM() {
  Serial.println("Requesting image capture from ESP32-CAM...");
  
  // Send capture command
  if (!sendUARTCommand(CMD_CAPTURE, NULL, 0)) {
    Serial.println("Failed to send capture command");
    return false;
  }
  
  // Wait for response
  uint8_t statusCode;
  if (!receiveUARTResponse(&statusCode, NULL, NULL)) {
    Serial.println("No response from ESP32-CAM");
    return false;
  }
  
  if (statusCode != STATUS_SUCCESS) {
    Serial.printf("Capture failed with status: 0x%02X\n", statusCode);
    return false;
  }
  
  Serial.println("Capture successful, requesting image transfer...");
  
  // Send image transfer command
  if (!sendUARTCommand(CMD_SEND_IMAGE, NULL, 0)) {
    Serial.println("Failed to send image transfer command");
    return false;
  }
  
  // Receive image chunks
  if (!receiveImageChunks()) {
    Serial.println("Failed to receive image");
    return false;
  }
  
  Serial.printf("Image received: %d bytes\n", imageBufferSize);
  return true;
}

// ============================================================================
// Send UART Command
// ============================================================================

bool sendUARTCommand(uint8_t cmdId, uint8_t *payload, size_t payloadLen) {
  uint8_t packet[256];
  packet[0] = START_BYTE;
  packet[1] = cmdId;
  packet[2] = payloadLen & 0xFF;
  
  if (payload != NULL && payloadLen > 0) {
    memcpy(&packet[3], payload, payloadLen);
  }
  
  uint8_t checksum = calculateCRC8(packet, 3 + payloadLen);
  packet[3 + payloadLen] = checksum;
  
  Serial2.write(packet, 4 + payloadLen);
  Serial2.flush();
  
  return true;
}

// ============================================================================
// Receive UART Response
// ============================================================================

bool receiveUARTResponse(uint8_t *statusCode, uint8_t *data, size_t *dataLen) {
  unsigned long startTime = millis();
  uint8_t rxBuffer[256];
  int rxIndex = 0;
  
  while (millis() - startTime < UART_TIMEOUT_MS) {
    if (Serial2.available() > 0) {
      uint8_t inByte = Serial2.read();
      
      if (rxIndex == 0 && inByte != START_BYTE) {
        continue;
      }
      
      rxBuffer[rxIndex++] = inByte;
      
      if (rxIndex >= 4) {
        uint8_t status = rxBuffer[1];
        uint8_t length = rxBuffer[2];
        
        if (rxIndex >= 4 + length) {
          uint8_t receivedChecksum = rxBuffer[3 + length];
          uint8_t calculatedChecksum = calculateCRC8(rxBuffer, 3 + length);
          
          if (receivedChecksum == calculatedChecksum) {
            *statusCode = status;
            
            if (data != NULL && dataLen != NULL && length > 0) {
              memcpy(data, &rxBuffer[3], length);
              *dataLen = length;
            }
            
            return true;
          } else {
            Serial.println("Checksum error in response");
            return false;
          }
        }
      }
    }
    delay(1);
  }
  
  return false;
}

// ============================================================================
// Receive Image Chunks
// ============================================================================

bool receiveImageChunks() {
  imageBufferSize = 0;
  uint16_t expectedChunkIndex = 0;
  uint16_t totalChunks = 0;
  
  while (true) {
    unsigned long startTime = millis();
    uint8_t header[7];
    int headerIndex = 0;
    
    // Read header
    while (headerIndex < 7 && millis() - startTime < UART_TIMEOUT_MS) {
      if (Serial2.available() > 0) {
        uint8_t inByte = Serial2.read();
        
        if (headerIndex == 0 && inByte != START_BYTE) {
          continue;
        }
        
        header[headerIndex++] = inByte;
      }
      delay(1);
    }
    
    if (headerIndex < 7) {
      Serial.println("Timeout reading chunk header");
      Serial2.write(NACK_BYTE);
      continue;
    }
    
    uint16_t chunkIndex = (header[1] << 8) | header[2];
    totalChunks = (header[3] << 8) | header[4];
    uint16_t chunkSize = (header[5] << 8) | header[6];
    
    // Read chunk data
    uint8_t chunkData[CHUNK_SIZE];
    size_t bytesRead = 0;
    startTime = millis();
    
    while (bytesRead < chunkSize && millis() - startTime < UART_TIMEOUT_MS) {
      if (Serial2.available() > 0) {
        chunkData[bytesRead++] = Serial2.read();
      }
      delay(1);
    }
    
    if (bytesRead < chunkSize) {
      Serial.println("Timeout reading chunk data");
      Serial2.write(NACK_BYTE);
      continue;
    }
    
    // Read checksum
    startTime = millis();
    uint8_t receivedChecksum = 0;
    while (Serial2.available() == 0 && millis() - startTime < 1000) {
      delay(1);
    }
    
    if (Serial2.available() > 0) {
      receivedChecksum = Serial2.read();
    } else {
      Serial.println("Timeout reading checksum");
      Serial2.write(NACK_BYTE);
      continue;
    }
    
    // Validate checksum
    uint8_t calculatedChecksum = calculateCRC8(header, 7);
    for (size_t i = 0; i < chunkSize; i++) {
      calculatedChecksum ^= chunkData[i];
    }
    
    if (calculatedChecksum != receivedChecksum) {
      Serial.println("Checksum error in chunk");
      Serial2.write(NACK_BYTE);
      continue;
    }
    
    // Check chunk index
    if (chunkIndex != expectedChunkIndex) {
      Serial.printf("Unexpected chunk index: %d (expected %d)\n", chunkIndex, expectedChunkIndex);
      Serial2.write(NACK_BYTE);
      continue;
    }
    
    // Copy chunk to buffer
    if (imageBufferSize + chunkSize <= MAX_IMAGE_SIZE) {
      memcpy(&imageBuffer[imageBufferSize], chunkData, chunkSize);
      imageBufferSize += chunkSize;
    } else {
      Serial.println("Image too large for buffer");
      return false;
    }
    
    // Send ACK
    Serial2.write(ACK_BYTE);
    
    expectedChunkIndex++;
    
    // Check if this was the last chunk
    if (chunkIndex + 1 >= totalChunks) {
      Serial.printf("Received all %d chunks\n", totalChunks);
      break;
    }
  }
  
  return true;
}

// ============================================================================
// Upload to Backend
// ============================================================================

bool uploadToBackend(bool includeAudio) {
  if (!wifiConnected) {
    Serial.println("Cannot upload - no Wi-Fi connection");
    return false;
  }
  
  Serial.println("Uploading to backend...");
  
  HTTPClient http;
  String url = String(SERVER_URL) + String(API_ENDPOINT);
  
  http.begin(url);
  http.setTimeout(HTTP_TIMEOUT_MS);
  
  // Create multipart request
  String boundary = String(MULTIPART_BOUNDARY);
  String contentType = "multipart/form-data; boundary=" + boundary;
  http.addHeader("Content-Type", contentType);
  
  // Build request body
  String requestBody = "";
  
  // Add session ID
  requestBody += "--" + boundary + "\r\n";
  requestBody += "Content-Disposition: form-data; name=\"session_id\"\r\n\r\n";
  requestBody += String(sessionId) + "\r\n";
  
  // Add mode
  requestBody += "--" + boundary + "\r\n";
  requestBody += "Content-Disposition: form-data; name=\"mode\"\r\n\r\n";
  requestBody += includeAudio ? "conversation" : "snapshot";
  requestBody += "\r\n";
  
  // Add image
  requestBody += "--" + boundary + "\r\n";
  requestBody += "Content-Disposition: form-data; name=\"image\"; filename=\"image.jpg\"\r\n";
  requestBody += "Content-Type: image/jpeg\r\n\r\n";
  
  String requestTail = "\r\n";
  
  // Add audio if in conversation mode
  if (includeAudio && recordBufferIndex > 0) {
    requestTail += "--" + boundary + "\r\n";
    requestTail += "Content-Disposition: form-data; name=\"audio\"; filename=\"audio.wav\"\r\n";
    requestTail += "Content-Type: audio/wav\r\n\r\n";
  }
  
  requestTail += "--" + boundary + "--\r\n";
  
  // Calculate total content length
  size_t contentLength = requestBody.length() + imageBufferSize + requestTail.length();
  if (includeAudio && recordBufferIndex > 0) {
    contentLength += recordBufferIndex;
  }
  
  http.addHeader("Content-Length", String(contentLength));
  
  // Send request
  WiFiClient *stream = http.getStreamPtr();
  
  // Send header part
  stream->print(requestBody);
  
  // Send image data
  stream->write(imageBuffer, imageBufferSize);
  
  // Send audio if applicable
  if (includeAudio && recordBufferIndex > 0) {
    stream->print("\r\n--" + boundary + "\r\n");
    stream->print("Content-Disposition: form-data; name=\"audio\"; filename=\"audio.wav\"\r\n");
    stream->print("Content-Type: audio/wav\r\n\r\n");
    stream->write(recordBuffer, recordBufferIndex);
  }
  
  // Send tail
  stream->print("\r\n--" + boundary + "--\r\n");
  
  // Wait for response
  int httpCode = http.POST();  // Changed from GET to POST to match backend endpoint
  
  if (httpCode == 200) {
    Serial.println("Upload successful, receiving audio response...");
    
    // Play audio response
    if (playAudioResponse(*stream)) {
      http.end();
      return true;
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode);
  }
  
  http.end();
  return false;
}

// ============================================================================
// Play Audio Response
// ============================================================================

bool playAudioResponse(WiFiClient &client) {
  Serial.println("Playing audio response...");
  changeState(STATE_PLAYING);
  
  // Backend now returns WAV format (16kHz mono PCM)
  // Skip WAV header (44 bytes) and play PCM data directly to I2S DAC
  
  uint8_t audioBuffer[AUDIO_PLAY_BUFFER_SIZE];
  size_t bytesWritten;
  bool headerSkipped = false;
  size_t bytesProcessed = 0;
  
  while (client.connected() || client.available()) {
    if (client.available()) {
      size_t bytesRead = client.read(audioBuffer, AUDIO_PLAY_BUFFER_SIZE);
      
      if (bytesRead > 0) {
        bytesProcessed += bytesRead;
        
        // Skip first 44 bytes (WAV header)
        if (!headerSkipped) {
          if (bytesProcessed >= 44) {
            // Calculate offset within this buffer
            size_t headerBytesInBuffer = 44 - (bytesProcessed - bytesRead);
            if (headerBytesInBuffer < bytesRead) {
              // Write PCM data after header
              i2s_write(DAC_I2S_NUM, &audioBuffer[headerBytesInBuffer], 
                       bytesRead - headerBytesInBuffer, &bytesWritten, portMAX_DELAY);
            }
            headerSkipped = true;
          }
          // If header not complete yet, continue reading
        } else {
          // Write PCM data directly to I2S DAC
          i2s_write(DAC_I2S_NUM, audioBuffer, bytesRead, &bytesWritten, portMAX_DELAY);
        }
      }
    }
    delay(1);
  }
  
  Serial.println("Audio playback complete");
  return true;
}

// ============================================================================
// CRC-8 Checksum Calculation
// ============================================================================

uint8_t calculateCRC8(uint8_t *data, size_t length) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}
