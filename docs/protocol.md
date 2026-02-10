# UART Communication Protocol

## ESP32 Master ↔ ESP32-CAM Protocol Specification

This document defines the binary communication protocol used between the ESP32 Master and ESP32-CAM modules.

---

## Overview

The UART protocol enables reliable image transfer and command/control communication between the two ESP32 modules. It uses a packet-based structure with CRC-8 checksum validation.

### Protocol Characteristics
- **Baud Rate:** 115200
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Flow Control:** Software (ACK/NACK)
- **Max Packet Size:** 1024 bytes (data)
- **Checksum:** CRC-8

---

## Command Structure (Master → CAM)

Commands sent from ESP32 Master to ESP32-CAM follow this structure:

```
[START_BYTE][COMMAND_ID][PAYLOAD_LENGTH][PAYLOAD...][CHECKSUM]
```

### Field Descriptions

| Field | Size | Value | Description |
|-------|------|-------|-------------|
| START_BYTE | 1 byte | 0xAA | Packet start delimiter |
| COMMAND_ID | 1 byte | 0x01-0x04 | Command identifier |
| PAYLOAD_LENGTH | 1 byte | 0-255 | Length of payload in bytes |
| PAYLOAD | 0-255 bytes | Variable | Command-specific data |
| CHECKSUM | 1 byte | CRC-8 | CRC-8 checksum of all preceding bytes |

### Example Command Packet

**Capture Image Command (no payload):**
```
0xAA 0x01 0x00 0x5C
```
- START_BYTE: 0xAA
- COMMAND_ID: 0x01 (capture)
- PAYLOAD_LENGTH: 0x00 (no payload)
- CHECKSUM: 0x5C (calculated CRC-8)

---

## Command Definitions

### 0x01: CAPTURE_IMAGE

**Purpose:** Instruct ESP32-CAM to capture an image and save to SD card.

**Request:**
```
[0xAA][0x01][0x00][CHECKSUM]
```

**No payload required.**

**Expected Response:**
- Status: 0x00 (SUCCESS) if image captured
- Status: 0x01 (CAM_ERROR) if camera failed
- Status: 0x02 (SD_ERROR) if SD card failed

---

### 0x02: SEND_IMAGE

**Purpose:** Request ESP32-CAM to send the last captured image via UART.

**Request:**
```
[0xAA][0x02][0x00][CHECKSUM]
```

**No payload required.**

**Expected Response:**
- Status: 0x00 (SUCCESS) followed by image chunks
- Status: 0x04 (NO_IMAGE) if no image available
- Status: 0x02 (SD_ERROR) if read failed

---

### 0x03: GET_STATUS

**Purpose:** Query ESP32-CAM status.

**Request:**
```
[0xAA][0x03][0x00][CHECKSUM]
```

**Response Payload:**
```
[SD_CARD_STATUS][IMAGE_COUNT]
```
- SD_CARD_STATUS: 0x01 if initialized, 0x00 if not
- IMAGE_COUNT: Number of images captured (0-255)

---

### 0x04: DELETE_IMAGE

**Purpose:** Delete the last captured image from SD card.

**Request:**
```
[0xAA][0x04][0x00][CHECKSUM]
```

**Expected Response:**
- Status: 0x00 (SUCCESS) if deleted
- Status: 0x04 (NO_IMAGE) if no image to delete
- Status: 0x02 (SD_ERROR) if deletion failed

---

## Response Structure (CAM → Master)

Responses sent from ESP32-CAM to ESP32 Master:

```
[START_BYTE][STATUS_CODE][DATA_LENGTH][DATA...][CHECKSUM]
```

### Field Descriptions

| Field | Size | Value | Description |
|-------|------|-------|-------------|
| START_BYTE | 1 byte | 0xAA | Packet start delimiter |
| STATUS_CODE | 1 byte | 0x00-0x04 | Status/error code |
| DATA_LENGTH | 1 byte | 0-255 | Length of data in bytes |
| DATA | 0-255 bytes | Variable | Response data (if any) |
| CHECKSUM | 1 byte | CRC-8 | CRC-8 checksum of all preceding bytes |

---

## Status Codes

| Code | Name | Description |
|------|------|-------------|
| 0x00 | SUCCESS | Command executed successfully |
| 0x01 | CAM_ERROR | Camera initialization or capture failed |
| 0x02 | SD_ERROR | SD card read/write error |
| 0x03 | IMG_TOO_LARGE | Image exceeds maximum size |
| 0x04 | NO_IMAGE | No image available to send/delete |

---

## Image Transfer Protocol

Image transfer uses a chunked protocol to avoid buffer overflow.

### Chunk Structure (CAM → Master)

```
[START_BYTE][CHUNK_IDX_H][CHUNK_IDX_L][TOTAL_H][TOTAL_L][SIZE_H][SIZE_L][DATA...][CHECKSUM]
```

### Field Descriptions

| Field | Size | Description |
|-------|------|-------------|
| START_BYTE | 1 byte | 0xAA |
| CHUNK_IDX_H | 1 byte | Chunk index (high byte) |
| CHUNK_IDX_L | 1 byte | Chunk index (low byte) |
| TOTAL_H | 1 byte | Total chunks (high byte) |
| TOTAL_L | 1 byte | Total chunks (low byte) |
| SIZE_H | 1 byte | Chunk size (high byte) |
| SIZE_L | 1 byte | Chunk size (low byte) |
| DATA | 1-1024 bytes | Image data chunk |
| CHECKSUM | 1 byte | CRC-8 of entire packet |

### Image Transfer Flow

1. **Master sends SEND_IMAGE command**
   ```
   Master → CAM: [0xAA][0x02][0x00][CHECKSUM]
   ```

2. **CAM sends SUCCESS response**
   ```
   CAM → Master: [0xAA][0x00][0x00][CHECKSUM]
   ```

3. **CAM sends first chunk**
   ```
   CAM → Master: [0xAA][0x00][0x00][0x00][0x14][0x04][0x00][DATA...][CHECKSUM]
   ```
   - Chunk 0 of 20 (0x0014)
   - Chunk size: 1024 bytes (0x0400)

4. **Master sends ACK**
   ```
   Master → CAM: [0x06]
   ```

5. **Repeat steps 3-4 for each chunk**

6. **Transfer complete when all chunks received**

### Flow Control

- **ACK (0x06):** Chunk received successfully, send next
- **NACK (0x15):** Chunk failed checksum, resend

### Retry Mechanism

- If no ACK received within 1000ms, resend chunk
- Maximum 3 retries per chunk
- If all retries fail, abort transfer

---

## CRC-8 Checksum Algorithm

### Polynomial: 0x07

```c
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
```

### Checksum Calculation

The checksum is calculated over all bytes **before** the checksum field.

**Example:**
```
Packet: [0xAA][0x01][0x00]
CRC = CRC8([0xAA, 0x01, 0x00])
Final: [0xAA][0x01][0x00][CRC]
```

---

## Timing Specifications

| Parameter | Value | Description |
|-----------|-------|-------------|
| Command Timeout | 5000 ms | Max time to wait for response |
| ACK Timeout | 1000 ms | Max time to wait for ACK |
| Chunk Interval | ~10 ms | Minimum time between chunks |
| Retry Delay | 100 ms | Delay before retry |
| Max Retries | 3 | Maximum retry attempts |

---

## Buffer Sizes

| Buffer | Size | Purpose |
|--------|------|---------|
| RX Buffer (Master) | 1536 bytes | Receive image chunks |
| TX Buffer (CAM) | 1536 bytes | Send image chunks |
| Command Buffer | 256 bytes | Command/response packets |

---

## Error Handling

### Lost Packet Detection
- Timeout waiting for response
- Invalid START_BYTE
- Checksum mismatch

### Recovery Procedures
1. **Timeout:** Resend command (up to 3 times)
2. **Invalid START_BYTE:** Flush buffer, wait for valid start
3. **Checksum Error:** Send NACK, request retransmission

### Critical Errors
- Hardware failure (camera, SD card)
- Communication timeout after max retries
- Image too large for buffer

In case of critical errors, the system should:
1. Log error to Serial
2. Return error status to Master
3. Reset state machine to IDLE

---

## State Machines

### ESP32-CAM State Machine

```
IDLE → (Receive CAPTURE) → CAPTURING → SAVING → IDLE
IDLE → (Receive SEND_IMAGE) → READING → SENDING → IDLE
IDLE → (Receive GET_STATUS) → IDLE
IDLE → (Receive DELETE_IMAGE) → DELETING → IDLE
```

### ESP32 Master State Machine

```
IDLE → (Button Press) → REQUEST_CAPTURE → WAIT_RESPONSE → REQUEST_IMAGE → 
RECEIVE_CHUNKS → UPLOAD_TO_BACKEND → IDLE
```

---

## Example Communication Sequences

### Sequence 1: Simple Image Capture

```
1. Master → CAM: CAPTURE command
   [0xAA][0x01][0x00][0x5C]

2. CAM → Master: SUCCESS response
   [0xAA][0x00][0x00][0xA7]

3. Master → CAM: SEND_IMAGE command
   [0xAA][0x02][0x00][0x4E]

4. CAM → Master: SUCCESS response
   [0xAA][0x00][0x00][0xA7]

5. CAM → Master: Chunk 0 of 50
   [0xAA][0x00][0x00][0x00][0x32][0x04][0x00][DATA...][CHECKSUM]

6. Master → CAM: ACK
   [0x06]

7-9. Repeat steps 5-6 for chunks 1-49

10. Transfer complete
```

### Sequence 2: Error Recovery

```
1. Master → CAM: CAPTURE command
   [0xAA][0x01][0x00][0x5C]

2. CAM → Master: CAM_ERROR
   [0xAA][0x01][0x00][0x48]

3. Master waits 1 second

4. Master → CAM: CAPTURE command (retry)
   [0xAA][0x01][0x00][0x5C]

5. CAM → Master: SUCCESS
   [0xAA][0x00][0x00][0xA7]
```

---

## Protocol Versions

### Version 1.0 (Current)
- Basic command set
- Chunked image transfer
- CRC-8 checksums
- ACK/NACK flow control

### Future Enhancements
- Compression support
- Multiple image buffer
- Video streaming capability
- Extended status information

---

## Testing and Validation

### Unit Tests
- CRC-8 calculation validation
- Packet parsing
- Chunk assembly

### Integration Tests
- End-to-end image transfer
- Error recovery scenarios
- Performance benchmarks

### Performance Targets
- Image transfer: <3 seconds for 100KB image
- Command response: <100ms
- Reliability: >99% success rate

---

## Implementation Notes

### Best Practices
1. Always validate START_BYTE before processing
2. Verify checksum before acting on commands
3. Implement proper timeout handling
4. Use state machines for clarity
5. Log all errors for debugging

### Common Pitfalls
- Forgetting to flush RX buffer between packets
- Not accounting for partial reads
- Missing timeout on blocking operations
- Buffer overflow on large images

---

## Debugging Tips

### Enable Verbose Logging
Add debug prints for:
- Received bytes (hex dump)
- Checksum calculations
- State transitions
- Error conditions

### Tools
- Logic analyzer for signal inspection
- Serial monitor for packet inspection
- Oscilloscope for timing verification

---

## References

- ESP32 UART Documentation
- CRC-8 Algorithm (polynomial 0x07)
- Binary Protocol Design Best Practices
