#include <RadioLib.h>
#include <Arduino.h>
#include <SPI.h>
#include <preferences_helper.hpp>
#include <crypto_helper.hpp>
#include <simple_ui.hpp>
#include <config_params.hpp>
#include <tristate_helper.hpp>
#include <nrz_helper.hpp>
#include <rolling_code_radio_helper.hpp>

// 2. Eigene SPI-Instanz anlegen
SPIClass customSPI(FSPI); 

// 3. RadioLib Modul-Instanz erstellen
Module* mod = new Module(CC_CS, CC_GDO0, CC_RST, CC_GDO2, customSPI, SPISettings(2000000, MSBFIRST, SPI_MODE0));
CC1101 radio = mod;

// Globale Variablen für die Krypto-Verarbeitung
RadioPayload decryptedData;
unsigned char rxBuffer[16];

//ui
UserInterface ui(BTN_PIN_OPEN);
UserInterface uiTask1(BUTTON_PIN_TASK1);
UserInterface uiTask2(BUTTON_PIN_TASK2);
UserInterface uiTask3(BUTTON_PIN_TASK3);

void setup() {
  ui.begin();
  uiTask1.begin();
  uiTask2.begin();
  uiTask3.begin();

  Serial.begin(115200);

  // NVS Flash-Speicher für Counter initialisieren
  initialize_storage();
  
  // SPI-Bus mit den ESP32-S3 Pins starten
  customSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CC_CS);

  // CC1101 initialisieren
  log_i("[CC1101] Initializing... ");
  int state = radio.begin(433.92, 4.8, 5.0, 58.0, 10);
  if (state == RADIOLIB_ERR_NONE) {
    radio.setOOK(true);
    
    // Wir verpassen dem Sender das identische Sync Word wie dem Empfänger
    radio.setSyncWord((uint8_t)0x2D, (uint8_t)0xD4);
    
    radio.setPreambleLength(32);       // Sendet vor dem Sync Word eine stabile Präambel
  } else {
    log_e("Initializing Failed!: %u", state);
    ui.setLedColor(255,0,0);
    while (true); // Stoppt den ESP32 bei Hardware-Fehlern
  }
  // Kurzes violettes Aufblinken: Empfänger bereit!
}

unsigned char payload[sizeof(RadioPayload)];

void loop() {
  // 1. UI-Status direkt zu Beginn aktualisieren (für präzises Tasten-Entprellen)
  ui.update();
  uiTask1.update();
  uiTask2.update();
  uiTask3.update();

  // 2. Prüfen, ob der Button gedrückt wurde
  if (ui.isButtonPressed()) {
    // Rolling Code counter erhöhen, BEVradio.setSyncWord((uint8_t)0, (uint8_t)0);OR wir ihn verschlüsseln und senden
    counter++;

    // Einen sauberen 16-Byte Sendepuffer anlegen
    uint8_t txBuffer[16];
    memset(txBuffer, 0, sizeof(txBuffer));

    // RGB-LED blau blinken lassen als optisches Feedback für das Senden
    ui.flashLedColor(2, 0, 0, 100, 150);

    // Radio auf die Standardparameter dieses Sende-Modes zuruecksetzen, falls
    // zuvor Task1/2/3 abweichende Einstellungen (Bitrate, Bandbreite, Sync-Word,
    // Paketlaenge) gesetzt haben.
    radio.setOOK(true);
    radio.setBitRate(4.8);
    radio.setRxBandwidth(58.0);
    radio.setSyncWord((uint8_t)0x2D, (uint8_t)0xD4);
    radio.fixedPacketLengthMode(16);

    // Funktion aufrufen: Befüllt die Struktur im Hintergrund mit Counter & SN,
    // verschlüsselt sie mit AES-128 und schreibt das Ergebnis in den txBuffer.
    createEncryptedRadioPayload(counter, (unsigned char*)txBuffer);

    // Jetzt das fertig verschlüsselte 16-Byte Paket über den CC1101 jagen
    int state = radio.transmit(txBuffer, 16);

    // 3. Erfolgskontrolle im Log (didaktisch wertvoll für den Workshop)
    if (state == RADIOLIB_ERR_NONE) {
      log_i("🚀 Telegram successfully sent! Plaintext Counter: %u", counter);
      ui.setLedColor(0,255,0);
      delay(1000);
    } else {
      log_e("❌ Transmission failed, RadioLib code: %d", state);
      ui.setLedColor(255,0,0);
    }

    // Den neuen Counter-Stand sicher im NVS-Flash merken,
    // damit er nach einem Neustart/Batteriewechsel nicht verloren geht!
    store_counter(counter);
  }

  // TASK1: Zufaelliges Tristate/Princeton-Telegramm senden (Flipper-lesbar)
  if (uiTask1.isButtonPressed()) {
    uiTask1.flashLedColor(2, 150, 0, 150, 150);
    sendRandomTristateTelegram(radio, CC_GDO0);
    uiTask1.setLedColor(0, 255, 0);
    delay(500);
  }

  // TASK2: NRZ-Telegramm mit manueller 32-Byte-Praeambel + Daten 01 02 03
  if (uiTask2.isButtonPressed()) {
    uiTask2.flashLedColor(2, 0, 150, 150, 150);
    sendNrzPreambleDemo(radio);
    uiTask2.setLedColor(0, 255, 0);
    delay(500);
  }

  // TASK3: AES-128-Rolling-Code als AM270/NRZ Replay-Burst senden
  if (uiTask3.isButtonPressed()) {
    counter++;
    uiTask3.flashLedColor(2, 150, 150, 0, 150);
    sendRollingCodeReplayBurst(radio, counter);
    uiTask3.setLedColor(0, 255, 0);
    store_counter(counter);
    delay(500);
  }

  delay(100);
}