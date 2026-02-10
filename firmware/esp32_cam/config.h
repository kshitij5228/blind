#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ESP32-CAM Configuration
// ============================================================================

// Camera pins (AI-Thinker ESP32-CAM default layout)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// SD Card SPI pins
#define SD_CS             13
#define SD_MOSI           15
#define SD_MISO            2
#define SD_SCK            14

// UART Communication pins
#define UART_TX_PIN        1  // TX to ESP32 Master RX
#define UART_RX_PIN        3  // RX from ESP32 Master TX
#define UART_BAUD_RATE  115200

// Status LED
#define LED_PIN            4  // Built-in flash LED

// ============================================================================
// Camera Settings
// ============================================================================

// Image quality settings (1-63, lower is higher quality)
#define JPEG_QUALITY      12

// Frame size options:
// FRAMESIZE_QQVGA (160x120)
// FRAMESIZE_QCIF (176x144)
// FRAMESIZE_HQVGA (240x176)
// FRAMESIZE_QVGA (320x240)
// FRAMESIZE_CIF (400x296)
// FRAMESIZE_VGA (640x480)
// FRAMESIZE_SVGA (800x600)
// FRAMESIZE_XGA (1024x768)
// FRAMESIZE_SXGA (1280x1024)
// FRAMESIZE_UXGA (1600x1200)
#define CAMERA_FRAME_SIZE FRAMESIZE_SVGA

// ============================================================================
// SD Card Settings
// ============================================================================

#define IMAGE_DIR "/images"
#define MAX_IMAGES 10  // Keep only last 10 images (circular buffer)
#define IMAGE_PREFIX "IMG_"
#define IMAGE_EXTENSION ".jpg"

// ============================================================================
// UART Protocol Constants
// ============================================================================

#define START_BYTE        0xAA
#define END_BYTE          0x55

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
#define CHUNK_SIZE        1024  // 1KB chunks for image transfer
#define MAX_RETRIES       3
#define ACK_TIMEOUT_MS    1000

// ============================================================================
// Memory Settings
// ============================================================================

#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 1024

// ============================================================================
// Timing Constants
// ============================================================================

#define CAPTURE_DELAY_MS  100   // Delay after capture before saving
#define LED_BLINK_MS      50    // LED blink duration for status indication

#endif // CONFIG_H
