#include "nrz_helper.hpp"
#include <Arduino.h>
#include <string.h>

namespace {
  const uint8_t PREAMBLE_LENGTH = 32;
  const uint8_t PAYLOAD_DATA[] = {0x01, 0x02, 0x03};
  const uint8_t TOTAL_LENGTH = PREAMBLE_LENGTH + sizeof(PAYLOAD_DATA);
}

void sendNrzPreambleDemo(CC1101& radio) {
  uint8_t frame[TOTAL_LENGTH];
  memset(frame, 0x01, PREAMBLE_LENGTH);
  memcpy(frame + PREAMBLE_LENGTH, PAYLOAD_DATA, sizeof(PAYLOAD_DATA));

  radio.setOOK(true);
  radio.setEncoding(RADIOLIB_ENCODING_NRZ);
  radio.disableSyncWordFiltering();
  radio.fixedPacketLengthMode(TOTAL_LENGTH);

  int state = radio.transmit(frame, TOTAL_LENGTH);
  if (state == RADIOLIB_ERR_NONE) {
    log_i("NRZ-Telegramm gesendet: 32x 0x01 Praeambel + Daten 01 02 03");
  } else {
    log_e("NRZ-Telegramm Sendefehler, RadioLib-Code: %d", state);
  }
}
