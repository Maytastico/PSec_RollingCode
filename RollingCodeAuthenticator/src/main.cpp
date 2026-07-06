#include <RadioLib.h>
#include <Arduino.h>
#include <SPI.h>
#include <preferences_helper.hpp>
#include <crypto_helper.hpp>
#include "simple_ui.hpp"

// Pins für den ESP32-S3
#define CC_CS 10
#define CC_GDO0 2 // <--- Dieser Pin fängt jetzt den Hardware-Interrupt ab!
#define CC_RST RADIOLIB_NC
#define CC_GDO2 4

#define SPI_SCK 12
#define SPI_MISO 13
#define SPI_MOSI 11

SPIClass customSPI(FSPI);
Module *mod = new Module(CC_CS, CC_GDO0, CC_RST, CC_GDO2, customSPI, SPISettings(2000000, MSBFIRST, SPI_MODE0));
CC1101 radio = mod;

UserInterface ui(37);

RadioPayload decryptedData;
unsigned char rxBuffer[16];
unsigned char sameCounter = 0;

// Flag für den Interrupt-Service
volatile bool receivedFlag = false;

// Diese ultraleichte Funktion wird sofort aufgerufen, wenn das CC1101 ein valides Paket sieht
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void setFlag(void)
{
	receivedFlag = true;
}

void setup()
{
	Serial.begin(115200);
	ui.begin();
	initialize_storage();

	customSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CC_CS);

	log_i("[CC1101] Initializing... ");
	int state = radio.begin(433.92, 4.8, 5.0, 58.0, 10);

	if (state == RADIOLIB_ERR_NONE)
	{
		log_i("Initializing Radio Complete!");
		radio.setOOK(true);
		radio.setEncoding(RADIOLIB_ENCODING_NRZ);
		radio.setSyncWord((uint8_t)0x2D, (uint8_t)0xD4);
		radio.setPreambleLength(32);

		// --- DIE INTERRUPT-RETTUNG ---
		// Verknüpft den GDO0-Pin mit unserer setFlag-Funktion bei steigender Flanke (RISING)
		radio.setPacketReceivedAction(setFlag);

		// Startet den asynchronen Empfangsmodus im Hintergrund. Der ESP läuft einfach weiter!
		state = radio.startReceive();
		if (state != RADIOLIB_ERR_NONE)
		{
			log_e("startReceive failed, code: %d", state);
		}
	}
	else
	{
		log_e("❌ Radio hardware initialization FAILED! Code: %d", state);
		while (true)
		{
			ui.flashLedColor(1, 100, 0, 0, 200);
			delay(400);
		}
	}
	reset_storage();
}

void loop()
{
	ui.update(); // Die LED bleibt flüssig animiert, da die loop() nie blockiert!

	// Prüfen, ob das CC1101 im Hintergrund "Daten da!" signalisiert hat
	if (receivedFlag)
	{
		receivedFlag = false; // Flag sofort zurücksetzen

		memset(rxBuffer, 0, sizeof(rxBuffer));

		// Liest die Daten aus dem FIFO-Speicher des CC1101
		int state = radio.readData(rxBuffer, 16);

		if (state == RADIOLIB_ERR_NONE)
		{
			Serial.println(F("\n📥 Genuine Packet Received via Interrupt!"));
			Serial.print(F("[CC1101] RSSI:\t\t"));
			Serial.print(radio.getRSSI());
			Serial.println(F(" dBm"));
			log_i("Rohdaten im rxBuffer (16 Bytes HEX):");
			log_buf_i(rxBuffer, 16);

			if (radio.getRSSI() < -100)
			{
				log_w("⚠️  Very weak signal detected! RSSI: %d dBm", radio.getRSSI());
			}

			if (rxBuffer[0] == 0xAE)
			{
				if (rxBuffer[1] == 0x01 && rxBuffer[2] == 0x02 && rxBuffer[3] == 0x03)
				{
					log_w("⚠️  Known test pattern detected! Packet may be a replay or test signal.");
					ui.flashLedColor(5, 0, 150, 0, 500);
				}
			}
			else
			{
				// 16 Bytes entschlüsseln
				decryptEncryptedRadioPayload((unsigned char *)rxBuffer, &decryptedData);

				// Sicherheitsprüfung des Rolling Codes
				if (decryptedData.counter > counter + 256)
				{
					log_w("⚠️  Received counter is suspiciously high! Possible desynchronization. Current: %u, Received: %u", counter, decryptedData.counter);
					ui.flashLedColor(4, 150, 150, 0, 100);
				}
				else
				{
					if (update_counter(decryptedData.counter))
					{
						log_i("🔑 ACCESS GRANTED: Valid rolling code counter received!");
						ui.flashLedColor(4, 0, 120, 0, 100);
					}
					else
					{
						if (counter == decryptedData.counter)
						{
							sameCounter++;
							log_w("⚠️  Same counter received %d times. Possible replay attack.", sameCounter);
							if (sameCounter >= 4)
							{
								log_e("🚨 SECURITY ALERT: Multiple identical counters received! Possible replay attack.");
								ui.flashLedColor(5, 155, 155, 0, 500);
							}
						}
						else
						{
							sameCounter = 1; // Reset counter if a different value is received
						}
					}
				}
			}
		}
		else if (state == RADIOLIB_ERR_CRC_MISMATCH)
		{
			// Background-Rauschen abfangen
			log_d("CRC error in background.");
		}
		else
		{
			log_e("Read error occurred, code: %d", state);
		}

		// WICHTIG: Nach dem Auslesen den Hintergrund-Empfänger wieder scharf schalten!
		radio.startReceive();
	}
}