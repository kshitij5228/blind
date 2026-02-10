/*
 * ESP32-CAM Firmware for Interactive AI Vision Assistant
 * 
 * This firmware handles:
 * - Camera initialization and image capture
 * - SD card management with circular buffer
 * - UART communication with ESP32 Master
 * - Chunked image transfer protocol
 */

#include <Arduino.h>
#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"
#include "config.h"

// ============================================================================
// Global Variables
// ============================================================================

camera_fb_t *fb = NULL;
String latestImagePath = "";
int imageCounter = 0;
bool sdCardInitialized = false;

// UART buffer
uint8_t rxBuffer[UART_RX_BUFFER_SIZE];
int rxIndex = 0;

// ============================================================================
// Function Prototypes
// ============================================================================

bool initCamera();
bool initSDCard();
void blinkLED(int times, int delayMs);
String captureAndSaveImage();
bool sendImageViaUART(String imagePath);
void processUARTCommand();
uint8_t calculateCRC8(uint8_t *data, size_t length);
void sendUARTResponse(uint8_t statusCode, uint8_t *data, size_t dataLength);
void cleanupOldImages();
void sendChunk(uint8_t *chunkData, size_t chunkSize, uint16_t chunkIndex, uint16_t totalChunks);
bool waitForACK();

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
  // Initialize Serial for debugging
  Serial.begin(UART_BAUD_RATE);
  
  // Configure LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Blink to indicate startup
  blinkLED(3, 200);
  
  // Initialize camera
  if (!initCamera()) {
    Serial.println("Camera init failed!");
    while (1) {
      blinkLED(5, 100);
      delay(1000);
    }
  }
  Serial.println("Camera initialized");
  
  // Initialize SD card
  if (!initSDCard()) {
    Serial.println("SD Card init failed!");
    sdCardInitialized = false;
    // Continue anyway - we'll retry on first capture
  } else {
    Serial.println("SD Card initialized");
    sdCardInitialized = true;
    
    // Create images directory if it doesn't exist
    if (!SD_MMC.exists(IMAGE_DIR)) {
      SD_MMC.mkdir(IMAGE_DIR);
    }
  }
  
  blinkLED(2, 100);
  Serial.println("ESP32-CAM Ready");
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
  // Check for incoming UART commands
  if (Serial.available() > 0) {
    processUARTCommand();
  }
  
  delay(10);
}

// ============================================================================
// Camera Initialization
// ============================================================================

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = CAMERA_FRAME_SIZE;
    config.jpeg_quality = JPEG_QUALITY;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Adjust sensor settings for better quality
  sensor_t *s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect)
    s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled
    s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
    s->set_aec2(s, 0);           // 0 = disable , 1 = enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0); // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable , 1 = enable
    s->set_wpc(s, 1);            // 0 = disable , 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
    s->set_lenc(s, 1);           // 0 = disable , 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    s->set_dcw(s, 1);            // 0 = disable , 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  }
  
  return true;
}

// ============================================================================
// SD Card Initialization
// ============================================================================

bool initSDCard() {
  if (!SD_MMC.begin("/sdcard", true)) { // 1-bit mode for compatibility
    Serial.println("SD Card Mount Failed");
    return false;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return false;
  }
  
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  
  return true;
}

// ============================================================================
// LED Control
// ============================================================================

void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}

// ============================================================================
// Image Capture and Storage
// ============================================================================

String captureAndSaveImage() {
  // Retry SD card init if it failed previously
  if (!sdCardInitialized) {
    sdCardInitialized = initSDCard();
    if (!sdCardInitialized) {
      Serial.println("SD Card still not available");
      return "";
    }
  }
  
  // Turn on LED during capture
  digitalWrite(LED_PIN, HIGH);
  
  // Capture image
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    digitalWrite(LED_PIN, LOW);
    return "";
  }
  
  delay(CAPTURE_DELAY_MS);
  
  // Cleanup old images (keep circular buffer)
  cleanupOldImages();
  
  // Generate filename with timestamp
  String filename = String(IMAGE_DIR) + "/" + String(IMAGE_PREFIX) + 
                    String(millis()) + String(IMAGE_EXTENSION);
  
  // Save to SD card
  File file = SD_MMC.open(filename.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    esp_camera_fb_return(fb);
    digitalWrite(LED_PIN, LOW);
    return "";
  }
  
  file.write(fb->buf, fb->len);
  file.close();
  
  Serial.printf("Image saved: %s (%d bytes)\n", filename.c_str(), fb->len);
  
  // Store the latest image path
  latestImagePath = filename;
  imageCounter++;
  
  // Return frame buffer
  esp_camera_fb_return(fb);
  digitalWrite(LED_PIN, LOW);
  
  return filename;
}

// ============================================================================
// Cleanup Old Images (Circular Buffer)
// ============================================================================

void cleanupOldImages() {
  File root = SD_MMC.open(IMAGE_DIR);
  if (!root || !root.isDirectory()) {
    return;
  }
  
  // Count files
  int fileCount = 0;
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      fileCount++;
    }
    file = root.openNextFile();
  }
  
  // If we have too many, delete the oldest
  if (fileCount >= MAX_IMAGES) {
    root = SD_MMC.open(IMAGE_DIR);
    file = root.openNextFile();
    if (file && !file.isDirectory()) {
      String oldestFile = String(IMAGE_DIR) + "/" + String(file.name());
      SD_MMC.remove(oldestFile.c_str());
      Serial.printf("Deleted old image: %s\n", oldestFile.c_str());
    }
  }
}

// ============================================================================
// UART Command Processing
// ============================================================================

void processUARTCommand() {
  while (Serial.available() > 0) {
    uint8_t inByte = Serial.read();
    
    // Look for start byte
    if (rxIndex == 0 && inByte != START_BYTE) {
      continue;
    }
    
    rxBuffer[rxIndex++] = inByte;
    
    // Check if we have at least the header
    if (rxIndex >= 4) {
      uint8_t cmdId = rxBuffer[1];
      uint8_t payloadLength = rxBuffer[2];
      
      // Check if we have the complete packet
      if (rxIndex >= 4 + payloadLength) {
        uint8_t receivedChecksum = rxBuffer[3 + payloadLength];
        uint8_t calculatedChecksum = calculateCRC8(rxBuffer, 3 + payloadLength);
        
        // Validate checksum
        if (receivedChecksum == calculatedChecksum) {
          // Process command
          switch (cmdId) {
            case CMD_CAPTURE: {
              String imagePath = captureAndSaveImage();
              if (imagePath.length() > 0) {
                sendUARTResponse(STATUS_SUCCESS, NULL, 0);
              } else {
                sendUARTResponse(STATUS_CAM_ERROR, NULL, 0);
              }
              break;
            }
            
            case CMD_SEND_IMAGE: {
              if (latestImagePath.length() > 0) {
                if (sendImageViaUART(latestImagePath)) {
                  sendUARTResponse(STATUS_SUCCESS, NULL, 0);
                } else {
                  sendUARTResponse(STATUS_SD_ERROR, NULL, 0);
                }
              } else {
                sendUARTResponse(STATUS_NO_IMAGE, NULL, 0);
              }
              break;
            }
            
            case CMD_GET_STATUS: {
              uint8_t statusData[2];
              statusData[0] = sdCardInitialized ? 1 : 0;
              statusData[1] = imageCounter & 0xFF;
              sendUARTResponse(STATUS_SUCCESS, statusData, 2);
              break;
            }
            
            case CMD_DELETE_IMAGE: {
              if (latestImagePath.length() > 0) {
                SD_MMC.remove(latestImagePath.c_str());
                latestImagePath = "";
                sendUARTResponse(STATUS_SUCCESS, NULL, 0);
              } else {
                sendUARTResponse(STATUS_NO_IMAGE, NULL, 0);
              }
              break;
            }
            
            default:
              Serial.println("Unknown command");
              break;
          }
        } else {
          Serial.println("Checksum error");
        }
        
        // Reset buffer
        rxIndex = 0;
      }
    }
    
    // Prevent buffer overflow
    if (rxIndex >= UART_RX_BUFFER_SIZE) {
      rxIndex = 0;
    }
  }
}

// ============================================================================
// Send UART Response
// ============================================================================

void sendUARTResponse(uint8_t statusCode, uint8_t *data, size_t dataLength) {
  uint8_t response[256];
  response[0] = START_BYTE;
  response[1] = statusCode;
  response[2] = dataLength & 0xFF;
  
  if (data != NULL && dataLength > 0) {
    memcpy(&response[3], data, dataLength);
  }
  
  uint8_t checksum = calculateCRC8(response, 3 + dataLength);
  response[3 + dataLength] = checksum;
  
  Serial.write(response, 4 + dataLength);
  Serial.flush();
}

// ============================================================================
// Send Image via UART (Chunked Transfer)
// ============================================================================

bool sendImageViaUART(String imagePath) {
  File file = SD_MMC.open(imagePath.c_str(), FILE_READ);
  if (!file) {
    Serial.println("Failed to open image file");
    return false;
  }
  
  size_t fileSize = file.size();
  uint16_t totalChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
  
  Serial.printf("Sending image: %d bytes in %d chunks\n", fileSize, totalChunks);
  
  uint8_t chunkBuffer[CHUNK_SIZE];
  uint16_t chunkIndex = 0;
  
  while (file.available()) {
    size_t bytesRead = file.read(chunkBuffer, CHUNK_SIZE);
    
    // Send chunk
    sendChunk(chunkBuffer, bytesRead, chunkIndex, totalChunks);
    
    // Wait for ACK
    if (!waitForACK()) {
      Serial.println("ACK timeout - retrying chunk");
      file.seek(chunkIndex * CHUNK_SIZE);
      continue;
    }
    
    chunkIndex++;
    
    // Visual feedback
    if (chunkIndex % 10 == 0) {
      blinkLED(1, 20);
    }
  }
  
  file.close();
  Serial.println("Image transfer complete");
  return true;
}

// ============================================================================
// Send Chunk
// ============================================================================

void sendChunk(uint8_t *chunkData, size_t chunkSize, uint16_t chunkIndex, uint16_t totalChunks) {
  // Packet structure: [START][CHUNK_IDX_H][CHUNK_IDX_L][TOTAL_H][TOTAL_L][SIZE_H][SIZE_L][DATA...][CRC]
  uint8_t header[7];
  header[0] = START_BYTE;
  header[1] = (chunkIndex >> 8) & 0xFF;
  header[2] = chunkIndex & 0xFF;
  header[3] = (totalChunks >> 8) & 0xFF;
  header[4] = totalChunks & 0xFF;
  header[5] = (chunkSize >> 8) & 0xFF;
  header[6] = chunkSize & 0xFF;
  
  // Send header
  Serial.write(header, 7);
  
  // Send data
  Serial.write(chunkData, chunkSize);
  
  // Calculate and send checksum (over header + data)
  uint8_t checksum = calculateCRC8(header, 7);
  for (size_t i = 0; i < chunkSize; i++) {
    checksum ^= chunkData[i];
  }
  Serial.write(checksum);
  Serial.flush();
}

// ============================================================================
// Wait for ACK
// ============================================================================

bool waitForACK() {
  unsigned long startTime = millis();
  while (millis() - startTime < ACK_TIMEOUT_MS) {
    if (Serial.available() > 0) {
      uint8_t ack = Serial.read();
      if (ack == 0x06) { // ACK byte
        return true;
      }
    }
    delay(1);
  }
  return false;
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
