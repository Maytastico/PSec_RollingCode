#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <Arduino.h>

class UserInterface {
private:
    // Variablen für RGB-Effekte ohne blockierendes delay()
    bool _isBlinking;
    unsigned long _lastToggleTime;
    unsigned long _blinkInterval;
    int _blinkCount;
    
    // Aktive Blink-Farbe speichern (R, G, B)
    uint8_t _blinkR;
    uint8_t _blinkG;
    uint8_t _blinkB;
    bool _ledState;

public:
    UserInterface();
    void begin();
    void update();

    /**
     * @brief Setzt die RGB-LED auf eine feste Farbe.
     * @param r Rot-Anteil (0-255)
     * @param g Grün-Anteil (0-255)
     * @param b Blau-Anteil (0-255)
     */
    void setLedColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Schaltet die RGB-LED komplett aus.
     */
    void ledOff();

    /**
     * @brief Lässt die RGB-LED in einer bestimmten Farbe asynchron blinken.
     */
    void flashLedColor(int times, uint8_t r, uint8_t g, uint8_t b, unsigned long interval = 200);
};

#endif // USER_INTERFACE_HPP