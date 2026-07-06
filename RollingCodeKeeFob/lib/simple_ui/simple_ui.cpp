#include "simple_ui.hpp"

UserInterface::UserInterface() {
    _isBlinking = false;
    _lastToggleTime = 0;
    _blinkInterval = 0;
    _blinkCount = 0;
    _ledState = false;
}

void UserInterface::begin() {
    
    // RGB-LED beim Start einmal ausschalten
    ledOff(); 
}

void UserInterface::update() {
    unsigned long currentMillis = millis();

    // --- 2. ASYNCHRONES RGB BLINKEN ---
    if (_isBlinking && (currentMillis - _lastToggleTime >= _blinkInterval)) {
        _lastToggleTime = currentMillis;
        _ledState = !_ledState;

        if (_ledState) {
            neopixelWrite(RGB_BUILTIN, _blinkR, _blinkG, _blinkB);
        } else {
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
        }

        if (_blinkCount > 0) {
            _blinkCount--;
            if (_blinkCount == 0) {
                _isBlinking = false;
                ledOff();
            }
        }
    }
}



void UserInterface::setLedColor(uint8_t r, uint8_t g, uint8_t b) {
    _isBlinking = false; // Laufende Blink-Muster stoppen
    neopixelWrite(RGB_BUILTIN, r, g, b);
}

void UserInterface::ledOff() {
    _isBlinking = false;
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
}

void UserInterface::flashLedColor(int times, uint8_t r, uint8_t g, uint8_t b, unsigned long interval) {
    _blinkR = r;
    _blinkG = g;
    _blinkB = b;
    _blinkInterval = interval;
    _lastToggleTime = millis();
    _isBlinking = true;
    _ledState = true;
    
    _blinkCount = (times > 0) ? (times * 2) : -1; 
    
    // Sofort mit der Farbe starten
    neopixelWrite(RGB_BUILTIN, _blinkR, _blinkG, _blinkB);
}