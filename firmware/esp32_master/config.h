#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ESP32 Master Configuration
// ============================================================================

// Button Configuration
#define BUTTON_PIN        4
#define BUTTON_DEBOUNCE_MS 50
#define LONG_PRESS_MS     500

// INMP441 I2S Microphone Configuration (I2S0)
#define MIC_I2S_NUM       I2S_NUM_0
#define MIC_SCK_PIN       26
#define MIC_WS_PIN        25
#define MIC_SD_PIN        33
#define MIC_SAMPLE_RATE   16000
#define MIC_BITS_PER_SAMPLE 16
#define MIC_CHANNELS      1  // Mono

// PCM5102A I2S DAC Configuration (I2S1)
#define DAC_I2S_NUM       I2S_NUM_1
#define DAC_BCK_PIN       14
#define DAC_WS_PIN        27
#define DAC_DIN_PIN       12
#define DAC_SAMPLE_RATE   44100
#define DAC_BITS_PER_SAMPLE 16
#define DAC_CHANNELS      2  // Stereo

// UART Communication with ESP32-CAM (UART2)
#define CAM_UART_NUM      2
#define CAM_RX_PIN        16
#define CAM_TX_PIN        17
#define CAM_BAUD_RATE     115200

// Status LED
#define STATUS_LED_PIN    2

// ============================================================================
// UART Protocol Constants
// ============================================================================

#define START_BYTE        0xAA
#define ACK_BYTE          0x06
#define NACK_BYTE         0x15

// Command IDs (Master -> CAM)
#define CMD_CAPTURE       0x01
#define CMD_SEND_IMAGE    0x02
#define CMD_GET_STATUS    0x03
#define CMD_DELETE_IMAGE  0x04

// Status Codes (CAM -> Master)
#define STATUS_SUCCESS    0x00
#define STATUS_CAM_ERROR  0x01
#define STATUS_SD_ERROR   0x02
#define STATUS_IMG_TOO_LARGE 0x03
#define STATUS_NO_IMAGE   0x04

// Transfer settings
#define CHUNK_SIZE        1024
#define MAX_IMAGE_SIZE    (512 * 1024)  // 512KB max
#define UART_TIMEOUT_MS   5000

// ============================================================================
// Audio Buffer Settings
// ============================================================================

#define AUDIO_RECORD_BUFFER_SIZE (64 * 1024)  // 64KB for recording
#define AUDIO_PLAY_BUFFER_SIZE   (8 * 1024)   // 8KB for playback
#define I2S_READ_LEN             1024
#define WAV_HEADER_SIZE          44

// ============================================================================
// Wi-Fi Settings
// ============================================================================

#define WIFI_CONNECT_TIMEOUT_MS  20000
#define WIFI_RECONNECT_DELAY_MS  5000
#define MAX_WIFI_RETRIES         5

// ============================================================================
// HTTP Client Settings
// ============================================================================

#define HTTP_TIMEOUT_MS          30000
#define MAX_HTTP_RETRIES         3
#define MULTIPART_BOUNDARY       "----WebKitFormBoundary7MA4YWxkTrZu0gW"

// ============================================================================
// State Machine States
// ============================================================================

enum SystemState {
  STATE_IDLE,
  STATE_RECORDING,
  STATE_CAPTURING,
  STATE_UPLOADING,
  STATE_PLAYING,
  STATE_ERROR
};

// ============================================================================
// Session Settings
// ============================================================================

#define SESSION_ID_LENGTH 36  // UUID length

// ============================================================================
// Memory Allocation
// ============================================================================

#define PSRAM_ENABLED     true

#endif // CONFIG_H
