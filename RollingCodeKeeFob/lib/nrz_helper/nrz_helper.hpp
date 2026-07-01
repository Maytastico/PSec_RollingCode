#ifndef NRZ_HELPER_HPP
#define NRZ_HELPER_HPP

#include <RadioLib.h>

/**
 * @brief Sendet ein manuell gerahmtes NRZ-Telegramm: 32 Praeambel-Bytes (0x01)
 *        gefolgt von den 3 Nutzdaten-Bytes 0x01 0x02 0x03.
 *
 * Die Praeambel wird bewusst als Teil der Nutzlast erzeugt (nicht ueber die
 * Hardware-Sync-Word-Erkennung des CC1101), damit im Workshop sichtbar wird,
 * wie eine Bitfolge ganz ohne Leitungscodierung (NRZ = jedes Bit direkt als
 * Traeger an/aus) auf der Luftschnittstelle aussieht.
 *
 * @param radio Initialisierte CC1101-Instanz.
 */
void sendNrzPreambleDemo(CC1101& radio);

#endif // NRZ_HELPER_HPP
