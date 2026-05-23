#include <RadioLib.h>
#include <SPI.h>
#include <Preferences.h>
#include <preferences_helper.hpp>

// 1. Pins für den ESP32-S3 definieren
#define CC_CS   10
#define CC_GDO0  4
#define CC_RST  RADIOLIB_NC  // RESET-Pin wird beim CC1101 meist nicht genutzt
#define CC_GDO2  3           // Optional

#define SPI_SCK  12
#define SPI_MISO 13
#define SPI_MOSI 11

// 2. Eigene SPI-Instanz anlegen
SPIClass customSPI(FSPI); 

// 3. RadioLib Modul-Instanz erstellen
// Argumente: CS, GDO0, RST, GDO2, SPI-Bus, SPI-Settings
Module* mod = new Module(CC_CS, CC_GDO0, CC_RST, CC_GDO2, customSPI, SPISettings(2000000, MSBFIRST, SPI_MODE0));
CC1101 radio = mod;

// 4 Counter Logic


void setup() {
  Serial.begin(115200);

  initialize_storage();

  // SPI-Bus mit den ESP32-S3 Pins starten
  customSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CC_CS);

  // CC1101 initialisieren
  Serial.print(F("[CC1101] Initialisiere... "));
  // Parameter: Frequenz (MHz), Bitrate (kbps), Frequenzhub (kHz), Rx-Bandbreite (kHz), Sendeleistung (dBm)
  int state = radio.begin(433.92, 4.8, 5.0, 58.0, 10);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Erfolgreich!"));
  } else {
    Serial.print(F("Fehlgeschlagen, Code: "));
    Serial.println(state);
    while (true);
  }

  // Für dein Vorhaben wichtig: OOK/ASK Modulation aktivieren
  // (Standardmäßig startet RadioLib im FSK-Modus)
  radio.setOOK(true);

  
}

void loop(){

}