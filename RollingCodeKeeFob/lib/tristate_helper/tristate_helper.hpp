#ifndef TRISTATE_HELPER_HPP
#define TRISTATE_HELPER_HPP

#include <RadioLib.h>

/**
 * @brief Sendet ein zufaelliges Princeton/PT2262-Tristate-Telegramm per rohem OOK-Bitbanging.
 *
 * Erzeugt 12 zufaellige Tristate-Digits (0, 1, F), codiert sie im klassischen
 * PT2262-Timing (kurz:lang = 1:3) inklusive Sync-Gap (1:31) und sendet das
 * Telegramm mehrfach hintereinander, damit z.B. der Sub-GHz-"Princeton"-Decoder
 * eines Flipper Zero es direkt erkennen und aufzeichnen kann.
 *
 * @param radio   Initialisierte CC1101-Instanz (befindet sich danach wieder im
 *                normalen Paket-Modus).
 * @param gdo0Pin GPIO-Pin, der mit dem GDO0-Ausgang des CC1101 verbunden ist und
 *                zum direkten Keyen der OOK-Traegerwelle genutzt wird.
 */
void sendRandomTristateTelegram(CC1101& radio, uint8_t gdo0Pin);

#endif // TRISTATE_HELPER_HPP
