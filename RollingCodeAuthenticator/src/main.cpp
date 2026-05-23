#include <RadioLib.h>
#include <Arduino.h>
#include <SPI.h>
#include <Preferences.h>
#include <preferences_helper.hpp>
#include <crypto_helper.hpp>

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
Module* mod = new Module(CC_CS, CC_GDO0, CC_RST, CC_GDO2, customSPI, SPISettings(2000000, MSBFIRST, SPI_MODE0));
CC1101 radio = mod;

// Globale Variablen für die Krypto-Verarbeitung
RadioPayload decryptedData;
unsigned char rxBuffer[16];

void setup() {
  Serial.begin(115200);

  // NVS Flash-Speicher für Counter initialisieren
  initialize_storage();
  
  // SPI-Bus mit den ESP32-S3 Pins starten
  customSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CC_CS);

  // CC1101 initialisieren
  log_i("[CC1101] Initializing... ");
  int state = radio.begin(433.92, 4.8, 5.0, 58.0, 10);

  if (state == RADIOLIB_ERR_NONE) {
    log_i("Initializing Radio Complete!");
  } else {
    log_e("Initializing Failed!: %u", state);
    while (true); // Stoppt den ESP32 bei Hardware-Fehlern
  }

  // Wichtig für Rolling-Code-Hacking: OOK/ASK Modulation aktivieren
  radio.setOOK(true);
}

void loop() {
  log_i("[CC1101] Waiting for incoming transmission ... ");

  // WICHTIG: Puffer vor jedem Empfang komplett nullen,
  // damit keine Reste von alten Funksprüchen das Krypto-Ergebnis verfälschen!
  memset(rxBuffer, 0, sizeof(rxBuffer));

  // Blockierendes Einlesen von exakt 16 Bytes über die Antenne
  int state = radio.receive(rxBuffer, 16);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));

    // Signalstärke protokollieren
    Serial.print(F("[CC1101] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    Serial.print(F("[CC1101] LQI:\t\t"));
    Serial.println(radio.getLQI());

    // 16 Bytes entschlüsseln und in die Struktur parsen
    decryptEncryptedRadioPayload((unsigned char*)rxBuffer, &decryptedData);   

    // Sicherheitsprüfung des Rolling Codes
    if (update_counter(decryptedData.counter)) {
        log_i("🔑 ACCESS GRANTED: Valid rolling code counter received!");
    } else {
        log_w("🚨 ACCESS DENIED: Counter check failed (Replay Attack detected or old out-of-sync counter).");
    }

  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    log_e("CRC error! Bad packet received.");
  } else {
    // Wenn RadioLib im Hintergrund aussteigt oder ein Timeout wirft
    log_e("failed, code: %d", state); 
  }
  
  // Kurze Pause zur Stabilisierung der CPU
  delay(100);
}