#include <Arduino.h>
#include "config_params.hpp"
#include "crypto_helper.hpp"
#include <mbedtls/aes.h>
#include <string.h>

void encryptAES(unsigned char* input, unsigned char* output) {
  mbedtls_aes_context aes;
  
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, AES_KEY, 128);
  
  // Process data in 16-byte blocks (AES block size)
  for (int i = 0; i < 2; i++) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input + (i * 16), output + (i * 16));
  }
  
  mbedtls_aes_free(&aes);
}

void decryptAES(unsigned char* input, unsigned char* output) {
  mbedtls_aes_context aes;
  
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, AES_KEY, 128);
  
  // Process data in 16-byte blocks (AES block size)
  for (int i = 0; i < 2; i++) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, input + (i * 16), output + (i * 16));
  }
  
  mbedtls_aes_free(&aes);
}

void createEncryptedRadioPayload(uint32_t counterToEncrypt, unsigned char* output) {
  RadioPayload payload;
  memset(&payload, 0, sizeof(RadioPayload));
  
  size_t bytesToCopy = (SERIAL_NUMBER_SIZE > 12) ? 12 : SERIAL_NUMBER_SIZE;
  memcpy(payload.serialnumber, SERIAL_NUMBER, bytesToCopy);
  payload.counter = counterToEncrypt;

  // 1. VOR DER VERSCHLÜSSELUNG (Klartext loggen)
  // Wir nutzen log_d (Debug), damit der Serial Monitor nicht überflutet wird,
  // wenn man das Debug-Level später herabsetzt.
  log_d("--- Preparing Telegram ---");
  log_d("Plaintext Counter: %u", payload.counter);
  
  // 2. VERSCHLÜSSELUNG AUSFÜHREN
  encryptAES((unsigned char*)&payload, output);

  // 3. NACH DER VERSCHLÜSSELUNG (Geheimtext als Hex-Werte loggen)
  // Das zeigt den Schülern, dass aus den lesbaren Daten "Müll" geworden ist.
  log_i("TX Payload Encrypted (16 Bytes): "
        "%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X",
        output[0], output[1], output[2], output[3],
        output[4], output[5], output[6], output[7],
        output[8], output[9], output[10], output[11],
        output[12], output[13], output[14], output[15]);
}