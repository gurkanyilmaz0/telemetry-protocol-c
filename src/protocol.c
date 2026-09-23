#include "protocol.h"
#include <string.h>

uint8_t calculate_checksum(const uint8_t *data, uint16_t length) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) {
        checksum ^= data[i]; // XOR bitwise operator
    }
    return checksum;
}

int encode_telemetry_packet(const TelemetryData *data, uint8_t *buffer, uint16_t buffer_size) {
    // 1. Pointer (Bellek adresi) kontrolleri
    if (data == NULL || buffer == NULL) {
        return PROTOCOL_ERR_NULL_PTR;
    }

    uint8_t payload_len = sizeof(TelemetryData);
    
    // Toplam paket boyutu = SYNC(1) + LEN(1) + PAYLOAD(N) + CHK(1)
    uint16_t total_packet_size = 1 + 1 + payload_len + 1; 

    if (buffer_size < total_packet_size) {
        return PROTOCOL_ERR_BUFFER_TOO_SMALL;
    }

    uint16_t idx = 0;
    
    // 2. Sync Byte ekle
    buffer[idx++] = SYNC_BYTE;
    
    // 3. Length ekle
    buffer[idx++] = payload_len;
    
    // 4. Payload ekle
    // Basitlik adina memcpy kullaniyoruz. 
    // Not: Farkli endianness'a sahip mimariler arasinda iletisim kurarken, 
    // degiskenleri tek tek byte'lara ayirarak (ornek: bit kaydirma ile) paketlemek daha guvenlidir.
    memcpy(&buffer[idx], data, payload_len);
    idx += payload_len;
    
    // 5. Checksum Hesapla ve Ekle
    // Checksum hesaplamasi PAYLOAD_LENGTH'den (dahil) Payload'un sonuna kadar yapilir
    buffer[idx] = calculate_checksum(&buffer[1], 1 + payload_len);
    idx++;

    return (int)total_packet_size;
}

ProtocolStatus decode_telemetry_packet(const uint8_t *buffer, uint16_t buffer_length, TelemetryData *data) {
    // 1. Pointer (Bellek adresi) kontrolleri
    if (buffer == NULL || data == NULL) {
        return PROTOCOL_ERR_NULL_PTR;
    }

    // En kucuk paket boyutu: SYNC(1) + LEN(1) + CHK(1) = 3
    if (buffer_length < 3) {
        return PROTOCOL_ERR_BUFFER_TOO_SMALL;
    }

    // 2. Sync Byte Kontrolu
    if (buffer[0] != SYNC_BYTE) {
        return PROTOCOL_ERR_INVALID_SYNC;
    }

    // 3. Length Kontrolu
    uint8_t payload_len = buffer[1];
    uint16_t expected_total_len = 1 + 1 + payload_len + 1;

    if (buffer_length < expected_total_len) {
         // Verinin tamami henuz gelmemis (buffer kucuk)
         return PROTOCOL_ERR_BUFFER_TOO_SMALL;
    }

    // 4. Checksum Dogrulamasi
    uint8_t expected_checksum = buffer[expected_total_len - 1]; // Paketin son byte'i
    uint8_t calculated_checksum = calculate_checksum(&buffer[1], 1 + payload_len);

    if (expected_checksum != calculated_checksum) {
        return PROTOCOL_ERR_BAD_CHECKSUM; // Veri yolda bozulmus!
    }

    // 5. Payload'u Cikart ve Struct'a Kopyala
    memcpy(data, &buffer[2], payload_len);

    return PROTOCOL_OK;
}
