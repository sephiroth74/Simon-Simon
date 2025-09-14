
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

bool Button::readDigitalPin() {
    return digitalRead(_pin) == LOW; // Buttons pull to ground when pressed
}

void Button::updateState() {
    bool current_reading = readDigitalPin();

    // If the reading changed, reset debounce timer
    if (current_reading != _last_state) {
        _last_debounce_time = millis();
        _last_state = current_reading;
    }

    // If stable for debounce delay, update button state
    if ((millis() - _last_debounce_time) > BUTTONS_DEBOUNCE_DELAY) {
        if (current_reading && !_is_pressed) {
            // Button just pressed
            _is_pressed = true;
            _is_tapped = false;
        } else if (!current_reading && _is_pressed) {
            // Button just released
            _is_pressed = false;
            _is_tapped = true;
        }
    }
}

// -------------------------------------------
// Buttons
// -------------------------------------------

void Buttons::setup() {
    // Configure all button pins as input with pullup resistors
    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(YELLOW_BUTTON_PIN, INPUT_PULLUP);

    _pressed_button = nullptr;
    _tapped_button  = nullptr;

    // Reset all button states
    _red_button.reset();
    _green_button.reset();
    _blue_button.reset();
    _yellow_button.reset();
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

void Buttons::process_internal() {
    // Update all button states
    _red_button.updateState();
    _green_button.updateState();
    _blue_button.updateState();
    _yellow_button.updateState();

    // Check for newly pressed buttons
    Button* buttons[] = {&_red_button, &_green_button, &_blue_button, &_yellow_button};

    for (Button* button : buttons) {
        // Check for button press events
        if (button->isPressed() && _pressed_button != button) {
            // New button pressed
            if (_pressed_button != nullptr) {
                // Release previous button if any
                _pressed_button->setPressed(false);
            }

            _pressed_button = button;
            _tapped_button = nullptr; // Clear tapped state

            if (pressed_cb) {
                pressed_cb(*_pressed_button);
            }
        }

        // Check for button release events
        if (button->isTapped() && _pressed_button == button) {
            // Button was released
            _pressed_button = nullptr;
            _tapped_button = button;

            if (released_cb) {
                released_cb(*_tapped_button);
            }

            // Clear tapped state after handling
            button->setTapped(false);
        }
    }
}

bool Buttons::isTapped() { return _tapped_button != nullptr && _tapped_button->isTapped(); }

bool Buttons::isPressed() { return _pressed_button != nullptr && _pressed_button->isPressed(); }
