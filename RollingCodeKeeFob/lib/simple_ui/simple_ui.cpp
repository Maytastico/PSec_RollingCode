#include "simple_ui.hpp"

UserInterface::UserInterface(uint8_t buttonPin) {
    _btnPin = buttonPin;
    _lastButtonState = HIGH;
    _buttonPressedTriggered = false;
    _lastDebounceTime = 0;
    
    _isBlinking = false;
    _lastToggleTime = 0;
    _blinkInterval = 0;
    _blinkCount = 0;
    _ledState = false;
}

void UserInterface::begin() {
    pinMode(_btnPin, INPUT);
    
    // RGB-LED beim Start einmal ausschalten
    ledOff(); 
}

void UserInterface::update() {
    unsigned long currentMillis = millis();

    // --- 1. BUTTON DEBOUNCE & ACTIVE-HIGH LOGIC ---
    bool reading = digitalRead(_btnPin);

    // Wenn sich der physikalische Zustand geändert hat, Entprell-Timer zurücksetzen
    if (reading != _lastButtonState) {
        _lastDebounceTime = currentMillis;
    }

    if ((currentMillis - _lastDebounceTime) > _debounceDelay) {
        // FIX: Für Active-High prüfen wir auf "HIGH" beim Tastendruck!
        if (reading == HIGH && !_buttonPressedTriggered) { 
            // Taste wurde gerade frisch gedrückt (Flankenwechsel LOW -> HIGH)
            _buttonPressedTriggered = true; 
        }
    }
    _lastButtonState = reading;

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

bool UserInterface::isButtonPressed() {
    if (_buttonPressedTriggered) {
        _buttonPressedTriggered = false;
        return true;
    }
    return false;
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