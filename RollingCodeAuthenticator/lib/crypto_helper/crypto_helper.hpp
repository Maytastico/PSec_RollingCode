#ifndef CRYPTO_HELPER_HPP
#define CRYPTO_HELPER_HPP
#include <stdint.h>

/**
 * @brief Represents a fixed-size, packed network transmission frame for rolling codes.
 * 
 * This structure unifies the device identification and the cryptographic rolling 
 * counter into a tightly packed memory layout. Forcing byte alignment ensures that 
 * the total structure size is precisely 16 bytes, which perfectly satisfies the block 
 * size constraint of the AES-128 standard without wasting bytes on compiler padding.
 * 
 * @attention The total size of this structure MUST be exactly 16 bytes.
 */
struct __attribute__((packed)) RadioPayload {
    /** @brief Unique 12-byte identification sequence of the transmitting hardware device. */
    unsigned char serialnumber[12]; 
    
    /** @brief Monotonically increasing 32-bit counter value used to prevent replay attacks. */
    uint32_t counter;               
};                                // Total: exactly 16 Bytes (Perfect AES block size!)

/**
 * @brief Encrypts a single 16-byte block of data using AES-128 ECB.
 * 
 * @param input  Pointer to the 16-byte plaintext buffer.
 * @param output Pointer to the 16-byte buffer where the ciphertext will be stored.
 */
void encryptAES(unsigned char* input, unsigned char* output);

/**
 * @brief Decrypts a single 16-byte block of data using AES-128 ECB.
 * 
 * @param input  Pointer to the 16-byte ciphertext buffer.
 * @param output Pointer to the 16-byte buffer where the decrypted plaintext will be stored.
 */
void decryptAES(unsigned char* input, unsigned char* output);

/**
 * @brief Constructs a plaintext RadioPayload, secures it with hardware parameters, 
 *        and encrypts the result using AES-128.
 * 
 * This high-level helper acts as a packet factory and encryption pipeline combined. 
 * It zeroes out an internal 16-byte packed structure, injects the hardcoded unique 
 * device serial number, appends the fresh rolling counter, and then encrypts the 
 * entire unified block. It also handles debugging output to visualize the state change 
 * from plaintext to ciphertext.
 * 
 * @note This function processes exactly one 16-byte cryptographic block.
 * @warning The global array `SERIAL_NUMBER` must be valid and visible within this context.
 * 
 * @param counter The current rolling code counter value to embed in the payload.
 * @param output  Pointer to a pre-allocated 16-byte destination buffer where the 
 *                resulting ciphertext will be stored.
 */
void createEncryptedRadioPayload(uint32_t counter, unsigned char* output);

#endif // CRYPTO_HELPER_HPP