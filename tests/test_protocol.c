#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/protocol.h"

// Unit Test Sonuclarini Yazdirmak Icin Basit Bir Makro
#define RUN_TEST(test_func) \
    do { \
        printf("Calistiriliyor: %s... ", #test_func); \
        test_func(); \
        printf("BASARILI (PASSED)\n"); \
    } while (0)

void test_calculate_checksum(void) {
    uint8_t data[] = {0x01, 0x02, 0x03}; 
    // 0x01 ^ 0x02 ^ 0x03 = 0x00
    assert(calculate_checksum(data, 3) == 0x00);
    
    uint8_t data2[] = {0xFF, 0xAA, 0x55}; 
    // 11111111 ^ 10101010 = 01010101 (0x55). 0x55 ^ 0x55 = 0x00
    assert(calculate_checksum(data2, 3) == 0x00);
}

void test_encode_success(void) {
    TelemetryData t_data = {
        .altitude = 15000,
        .speed = 25.5f,
        .status = STATUS_FLYING
    };

    uint8_t buffer[64] = {0};
    int bytes_written = encode_telemetry_packet(&t_data, buffer, sizeof(buffer));

    // encode isleminden donen deger sifirdan buyuk olmali
    assert(bytes_written > 0);
    
    // Paket baslangici SYNC byte olmali
    assert(buffer[0] == SYNC_BYTE);
    
    // Ikinci byte payload boyutu olmali
    assert(buffer[1] == sizeof(TelemetryData));
    
    // Paketin son byte'i dogru checksum olmali
    uint8_t expected_chk = calculate_checksum(&buffer[1], 1 + sizeof(TelemetryData));
    assert(buffer[bytes_written - 1] == expected_chk);
}

void test_decode_success(void) {
    // 1. Ornek veri olustur ve paketle
    TelemetryData original_data = {
        .altitude = 12345,
        .speed = 10.2f,
        .status = STATUS_ERROR
    };

    uint8_t buffer[64] = {0};
    int bytes_written = encode_telemetry_packet(&original_data, buffer, sizeof(buffer));
    assert(bytes_written > 0);

    // 2. Paketlenmis veriyi coz ve struct'a geri yaz
    TelemetryData decoded_data = {0};
    ProtocolStatus status = decode_telemetry_packet(buffer, bytes_written, &decoded_data);

    // 3. Basarili olmali ve sonuclar ilk veriyle eslesmeli
    assert(status == PROTOCOL_OK);
    assert(decoded_data.altitude == original_data.altitude);
    assert(decoded_data.speed == original_data.speed); 
    assert(decoded_data.status == original_data.status);
}

void test_decode_bad_sync(void) {
    uint8_t buffer[] = {0xBB, 0x09, 0x00, 0x00, 0x00}; // Yanlis sync byte (0xBB)
    TelemetryData data;
    ProtocolStatus status = decode_telemetry_packet(buffer, sizeof(buffer), &data);
    
    // Fonksiyon hata kodu donmeli ve cokmemeli
    assert(status == PROTOCOL_ERR_INVALID_SYNC);
}

void test_decode_bad_checksum(void) {
    TelemetryData original_data = { .altitude = 100, .speed = 5.0f, .status = STATUS_GROUND };
    uint8_t buffer[64] = {0};
    int bytes_written = encode_telemetry_packet(&original_data, buffer, sizeof(buffer));
    
    // Veriyi yolda bozulmus gibi simule edelim
    buffer[3] = 0xFF; 

    TelemetryData decoded_data;
    ProtocolStatus status = decode_telemetry_packet(buffer, bytes_written, &decoded_data);
    
    // Fonksiyon checksum hatasi donmeli
    assert(status == PROTOCOL_ERR_BAD_CHECKSUM);
}

int main(void) {
    printf("--- Telemetri Protokolu Unit Testleri ---\n");
    
    RUN_TEST(test_calculate_checksum);
    RUN_TEST(test_encode_success);
    RUN_TEST(test_decode_success);
    RUN_TEST(test_decode_bad_sync);
    RUN_TEST(test_decode_bad_checksum);

    printf("--- Butun testler basariyla gecti! ---\n");
    return 0;
}
