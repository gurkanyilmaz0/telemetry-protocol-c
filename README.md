![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
# Telemetry Protocol Library (C99)

This repository contains a lightweight, custom telemetry protocol library written in C. It is designed for embedded systems, UAVs (Unmanned Aerial Vehicles), and ground control stations to communicate efficiently and reliably over serial, UDP, or other data links.xxssllaasaaakskaa
a
## Key Features
* **No Dynamic Memory Allocation:** Completely avoids `malloc`/`free`, making it suitable for safety-critical embedded systems and MISRA C compliance.
* **Struct Packing:** Uses `#pragma pack` to prevent memory alignment/padding issues across different architectures.
* **Data Integrity:** Implements a XOR Checksum to detect data corruption during transmission.
* **Unit Tested:** Includes comprehensive unit tests for encoding, decoding, and failure scenarios.

## Packet Structure
The protocol wraps the telemetry payload in a structured packet:
```text
[SYNC_BYTE] [PAYLOAD_LENGTH] [PAYLOAD (Struct)] [CHECKSUM]
```
* **SYNC_BYTE**: `0xAA` (1 byte) - Indicates the start of a packet.
* **PAYLOAD_LENGTH**: (1 byte) - The size of the telemetry data.
* **PAYLOAD**: (N bytes) - The actual `TelemetryData` struct (e.g., altitude, speed, flight status).
* **CHECKSUM**: (1 byte) - XOR calculation of the payload length and payload bytes.

## How to Compile & Run Tests
This project includes a standard `Makefile` for easy compilation of the unit tests. A GCC compiler is required.

To compile and run the unit tests:
```bash
make test
```

Expected output:
```text
--- Telemetri Protokolu Unit Testleri ---
Calistiriliyor: test_calculate_checksum... BASARILI (PASSED)
Calistiriliyor: test_encode_success... BASARILI (PASSED)
Calistiriliyor: test_decode_success... BASARILI (PASSED)
Calistiriliyor: test_decode_bad_sync... BASARILI (PASSED)
Calistiriliyor: test_decode_bad_checksum... BASARILI (PASSED)
--- Butun testler basariyla gecti! ---
```

## Usage Example

### Encoding (Sending Data)
```c
TelemetryData data = { .altitude = 15000, .speed = 25.5f, .status = STATUS_FLYING };
uint8_t buffer[64];

// Encodes data into buffer and returns the total packet size
int packet_size = encode_telemetry_packet(&data, buffer, sizeof(buffer));
```

### Decoding (Receiving Data)
```c
TelemetryData received_data;
// Decodes the buffer back into a struct. Validates Sync Byte and Checksum.
ProtocolStatus status = decode_telemetry_packet(buffer, received_buffer_length, &received_data);

if (status == PROTOCOL_OK) {
    // Successfully decoded!
}
```
