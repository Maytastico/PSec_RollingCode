#ifndef ROLLING_CODE_RADIO_HELPER_HPP
#define ROLLING_CODE_RADIO_HELPER_HPP

#include <RadioLib.h>
#include <stdint.h>

/**
 * @brief Verschluesselt den uebergebenen Rolling-Code-Zaehler mit AES-128-ECB
 *        (ueber crypto_helper) und sendet das entstehende 16-Byte-Telegramm
 *        mehrfach hintereinander als "Replay-Burst".
 *
 * Die Funkparameter entsprechen dabei dem "AM270"-Preset von Flipper Zero
 * (OOK, ~270 kHz RX-Bandbreite, ~3.79 kBaud) mit NRZ-Leitungscodierung, damit
 * das Telegramm mit einem Flipper aufgezeichnet und spaeter erneut abgespielt
 * werden kann - der Empfaenger muss den wiederholten, unveraenderten Zaehler
 * dank Rolling-Code-Pruefung als Replay erkennen und ablehnen.
 *
 * @param radio        Initialisierte CC1101-Instanz.
 * @param counterValue Der aktuelle Rolling-Code-Zaehler, der eingebettet und
 *                      verschluesselt werden soll.
 */
void sendRollingCodeReplayBurst(CC1101& radio, uint32_t counterValue);

#endif // ROLLING_CODE_RADIO_HELPER_HPP
