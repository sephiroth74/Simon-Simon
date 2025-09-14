
#include "buttons.h"
#include "config.h"

using namespace simon;

// -------------------------------------------
// Button
// -------------------------------------------

void Button::setPressed(bool value) {
    if (_is_pressed == value)
        return;

    _is_pressed = value;

    if (_is_pressed) {
        _is_tapped = false;
    }
}

void Button::setTapped(bool value) {
    if (_is_tapped == value) {
        return;
    }
    _is_tapped = value;
}

bool Button::process(int value) {
    if (_range.contains(value)) {
        if (_is_pressed) {
            return false;
        }

        _is_pressed = true;
        return true;
    } else {
        if (_is_pressed) {
            _is_pressed = false;
            return true;
        }
        return false;
    }
}

// -------------------------------------------
// Buttons
// -------------------------------------------

unsigned long lastDebounceTime = 0; // Last time the button state was changed
uint32_t readingIndex          = 0;

void Buttons::setup() {
    pinMode(_pin, INPUT_PULLDOWN);
    _pressed_button = nullptr;
    _tapped_button  = nullptr;
    _blue_button.reset();
    _yellow_button.reset();
    _green_button.reset();
    _red_button.reset();
    lastDebounceTime = millis(); // Initialize debounce time
}

void Buttons::loop() {
    if (!_paused)
        process_internal();
}

void Buttons::pause() {
    _paused         = true;
    _pressed_button = nullptr;
    _tapped_button  = nullptr;
}

void Buttons::resume() { _paused = false; }

uint16_t buttonState;

Button* Buttons::check_pressed_button() {
    buttonState = analogRead(_pin);

    if (buttonState > 1000) {
        // Serial.print(F("Button state: "));
        // Serial.println(buttonState);
    } else {
        // Serial.println("Button state is too low, ignoring.");
        return nullptr; // Ignore low values
    }

    if (_blue_button.inRange(buttonState)) {
        // Serial.println("Blue button pressed with value: " + String(buttonState));
        return &_blue_button;
    } else if (_yellow_button.inRange(buttonState)) {
        // Serial.println("Yellow button pressed with value: " + String(buttonState));
        return &_yellow_button;
    } else if (_green_button.inRange(buttonState)) {
        // Serial.println("Green button pressed with value: " + String(buttonState));
        return &_green_button;
    } else if (_red_button.inRange(buttonState)) {
        // Serial.println("Red button pressed with value: " + String(buttonState));
        return &_red_button;
    }
    return nullptr;
}

void Buttons::process_internal() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime < BUTTONS_DEBOUNCE_DELAY) {
        return; // Ignore if within debounce delay
    }

    Button* next_pressed_button = check_pressed_button();
    if (next_pressed_button != nullptr) {
        readingIndex++;
        if (readingIndex >= BUTTONS_MIN_READINGS_COUNT) {
            readingIndex = 0; // Reset index after reaching the minimum count
        } else {
            // If we haven't reached the minimum count, just return
            return;
        }
    } else {
        readingIndex = 0; // Reset index if no button is pressed
    }

    if (next_pressed_button != nullptr) {
        if (_pressed_button == nullptr) {
            // Serial.print(F("Button pressed: "));
            // Serial.print(next_pressed_button->getName());
            // Serial.print(F(" | Value: "));
            // Serial.println(buttonState);

            _pressed_button = next_pressed_button;
            _pressed_button->setPressed(true);
            _pressed_button->setTapped(false);

            if (pressed_cb) {
                pressed_cb(*_pressed_button);
            }

            lastDebounceTime = millis(); // Update debounce time
        }

    } else if (next_pressed_button == nullptr &&
               buttonState < lowestButtonState()) { // next_pressed_button is nullptr
        if (_pressed_button != nullptr) {
            // Serial.print(F("Button released: "));
            // Serial.print(_pressed_button->getName());
            // Serial.print(F(" | Value: "));
            // Serial.println(buttonState);

            _pressed_button->setPressed(false);
            _pressed_button->setTapped(true);
            _tapped_button  = _pressed_button;
            _pressed_button = nullptr;

            if (released_cb) {
                released_cb(*_tapped_button);
            }

            lastDebounceTime = millis(); // Update debounce time
        } else {
            // No button pressed, reset tapped button
            _tapped_button = nullptr;
        }
    }
}

bool Buttons::isTapped() { return _tapped_button != nullptr && _tapped_button->isTapped(); }

bool Buttons::isPressed() { return _pressed_button != nullptr && _pressed_button->isPressed(); }
