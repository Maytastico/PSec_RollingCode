#include "rolling_code_radio_helper.hpp"
#include <crypto_helper.hpp>
#include <Arduino.h>
#include <string.h>

namespace {
  // Entspricht Flipper Zeros "AM270"-Preset (FuriHalSubGhzPresetOok270Async)
  const float AM270_BIT_RATE_KBPS = 3.79372f;
  const float AM270_RX_BANDWIDTH_KHZ = 270.0f;

  const uint8_t REPLAY_REPEATS = 4;
  const unsigned long REPLAY_GAP_MS = 100;
}

void sendRollingCodeReplayBurst(CC1101& radio, uint32_t counterValue) {
  uint8_t txBuffer[16];
  memset(txBuffer, 0, sizeof(txBuffer));
  createEncryptedRadioPayload(counterValue, txBuffer);

  radio.setOOK(true);
  radio.setEncoding(RADIOLIB_ENCODING_NRZ);
  radio.setBitRate(AM270_BIT_RATE_KBPS);
  radio.setRxBandwidth(AM270_RX_BANDWIDTH_KHZ);
  radio.fixedPacketLengthMode(sizeof(txBuffer));

  for (uint8_t i = 0; i < REPLAY_REPEATS; i++) {
    int state = radio.transmit(txBuffer, sizeof(txBuffer));
    if (state == RADIOLIB_ERR_NONE) {
      log_i("AM270/NRZ Rolling-Code Replay %u/%u gesendet (Counter %u)", i + 1, REPLAY_REPEATS, counterValue);
    } else {
      log_e("AM270/NRZ Rolling-Code Sendefehler, RadioLib-Code: %d", state);
    }
    delay(REPLAY_GAP_MS);
  }
}
