#include "tristate_helper.hpp"
#include <Arduino.h>
#include <esp_system.h>

namespace {
  const unsigned int PULSE_UNIT_US = 350;   // Princeton-Kurzpuls (~350us, im Flipper-Decodebereich)
  const uint8_t TRISTATE_DIGIT_COUNT = 12;  // 12 Trits = 24 Bit, Standard-Codewortlaenge bei PT2262
  const uint8_t TELEGRAM_REPEATS = 6;       // Mehrfachsendung wie bei einem echten Handsender

  void transmitPulse(uint8_t gdo0Pin, uint8_t highUnits, uint8_t lowUnits) {
    digitalWrite(gdo0Pin, HIGH);
    delayMicroseconds(highUnits * PULSE_UNIT_US);
    digitalWrite(gdo0Pin, LOW);
    delayMicroseconds(lowUnits * PULSE_UNIT_US);
  }

  // Jedes Tristate-Digit wird als Paar von Puls-Verhaeltnissen gesendet:
  // '0' -> (1:3)(1:3), '1' -> (3:1)(3:1), 'F' (floating) -> (1:3)(3:1)
  void transmitTristateDigit(uint8_t gdo0Pin, char digit) {
    switch (digit) {
      case '0':
        transmitPulse(gdo0Pin, 1, 3);
        transmitPulse(gdo0Pin, 1, 3);
        break;
      case '1':
        transmitPulse(gdo0Pin, 3, 1);
        transmitPulse(gdo0Pin, 3, 1);
        break;
      default: // 'F'
        transmitPulse(gdo0Pin, 1, 3);
        transmitPulse(gdo0Pin, 3, 1);
        break;
    }
  }

  char randomTristateDigit() {
    switch (esp_random() % 3) {
      case 0: return '0';
      case 1: return '1';
      default: return 'F';
    }
  }
}

void sendRandomTristateTelegram(CC1101& radio, uint8_t gdo0Pin) {
  char code[TRISTATE_DIGIT_COUNT + 1];
  for (uint8_t i = 0; i < TRISTATE_DIGIT_COUNT; i++) {
    code[i] = randomTristateDigit();
  }
  code[TRISTATE_DIGIT_COUNT] = '\0';
  log_i("Tristate-Telegramm (Flipper-lesbar): %s", code);

  radio.setOOK(true);
  radio.disableSyncWordFiltering();
  pinMode(gdo0Pin, OUTPUT);
  radio.transmitDirectAsync();

  for (uint8_t repeat = 0; repeat < TELEGRAM_REPEATS; repeat++) {
    for (uint8_t i = 0; i < TRISTATE_DIGIT_COUNT; i++) {
      transmitTristateDigit(gdo0Pin, code[i]);
    }
    transmitPulse(gdo0Pin, 1, 31); // Sync-Gap
  }

  digitalWrite(gdo0Pin, LOW);
  radio.finishTransmit();
}
