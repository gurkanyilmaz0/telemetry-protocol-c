#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

/* 
 * Paket Yapısı (Packet Structure):
 * [SYNC_BYTE] [PAYLOAD_LENGTH] [PAYLOAD...] [CHECKSUM]
 * 
 * SYNC_BYTE: Paketin baslangicini belirten sabit byte. Ornegin 0xAA. (1 byte)
 * PAYLOAD_LENGTH: Gonderilecek gercek verinin (payload) boyutu. (1 byte)
 * PAYLOAD: Asil veri yapisi (TelemetryData struct). (N byte)
 * CHECKSUM: PAYLOAD_LENGTH'ten PAYLOAD'un sonuna kadar olan bytelarin XOR'u. (1 byte)
 */

#define SYNC_BYTE 0xAA

typedef enum {
    STATUS_GROUND = 0,
    STATUS_FLYING = 1,
    STATUS_ERROR = 2
} FlightStatus;

/* 
 * Struct padding (hizalama) olmamasi icin pragma pack kullaniyoruz.
 * Eger bunu kullanmazsak, derleyici 'speed' veya 'status' oncesine bos bytelar ekleyebilir.
 * Bu da veriyi byte byte karsi tarafa iletirken yapinin bozulmasina sebep olur.
 */
#pragma pack(push, 1)
typedef struct {
    uint32_t altitude; // Irtifa (cm cinsinden)
    float speed;       // Hiz (m/s cinsinden)
    uint8_t status;    // Ucus durumu (FlightStatus)
} TelemetryData;
#pragma pack(pop)

// Hata Kodlari
typedef enum {
    PROTOCOL_OK = 0,
    PROTOCOL_ERR_NULL_PTR = -1,
    PROTOCOL_ERR_INVALID_SYNC = -2,
    PROTOCOL_ERR_BAD_CHECKSUM = -3,
    PROTOCOL_ERR_BUFFER_TOO_SMALL = -4
} ProtocolStatus;

// XOR Checksum (Dogrulama) hesaplar
uint8_t calculate_checksum(const uint8_t *data, uint16_t length);

// TelemetryData yapisini bir byte dizisine (buffer) donusturur (paketler)
// Basarili olursa yazilan byte sayisini, hata olursa ProtocolStatus doner.
int encode_telemetry_packet(const TelemetryData *data, uint8_t *buffer, uint16_t buffer_size);

// Byte dizisini (buffer) cozumleyerek TelemetryData yapisina donusturur (ayristirir)
// Basarili olursa PROTOCOL_OK doner.
ProtocolStatus decode_telemetry_packet(const uint8_t *buffer, uint16_t buffer_length, TelemetryData *data);

#endif // PROTOCOL_H
