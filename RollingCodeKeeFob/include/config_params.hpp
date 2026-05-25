#include <string.h>

#define CC_CS   10
#define CC_GDO0  4
#define CC_RST  RADIOLIB_NC  // RESET-Pin wird beim CC1101 meist nicht genutzt
#define CC_GDO2  3           // Optional

#define SPI_SCK  12
#define SPI_MISO 13
#define SPI_MOSI 11

const uint16_t SERIAL_NUMBER_SIZE = 12;

const uint8_t BTN_PIN_OPEN = 37;
const uint8_t BTN_PIN_CLOSE = 38;

// Eindeutige Seriennummer für deinen KeyFob (12 Bytes)
const unsigned char SERIAL_NUMBER[SERIAL_NUMBER_SIZE] = {
  0x4A, 0x9F, 0x2C, 0x8E, 0x11, 0xD3, 0x77, 0xA6,
  0xB9, 0xE0, 0x55, 0x22
};

// Kryptographisch zufälliger AES-128 Schlüssel (16 Bytes / 128 Bit)
const unsigned char AES_KEY[16] = {
  0xE2, 0x4C, 0xA8, 0x1F, 0x73, 0xB5, 0x92, 0x6D,
  0x0A, 0xDF, 0xC4, 0x88, 0x9B, 0xE1, 0x53, 0x76
};
