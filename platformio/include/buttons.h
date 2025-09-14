
#ifndef __SIMON_BUTTONS_H__
#define __SIMON_BUTTONS_H__

#include "config.h"
#include "types.h"
#include <Arduino.h>
#include <functional>

namespace simon {

// -------------------------------------------
// Button
// -------------------------------------------

class Button {
  private:
    const char* _name;
    color_t _type;
    int8_t _pin;
    bool _is_pressed = false;
    bool _is_tapped  = false;
    bool _last_state = false;
    unsigned long _last_debounce_time = 0;

  public:
    Button(const char* name, color_t type, int8_t pin) : _name(name), _type(type), _pin(pin) {}

    ~Button() {}

    const char* getName() { return _name; }

    color_t getType() { return _type; }

    int8_t getPin() { return _pin; }

    bool isPressed() { return _is_pressed; }

    bool isTapped() { return _is_tapped; }

    void setTapped(bool value);

    void setPressed(bool value);

    bool readDigitalPin();

    void updateState();

    void reset() {
        _is_pressed = false;
        _is_tapped  = false;
        _last_state = false;
        _last_debounce_time = 0;
    }
};

// -------------------------------------------
// Buttons
// -------------------------------------------

class Buttons {
  private:
    Button _red_button;
    Button _green_button;
    Button _blue_button;
    Button _yellow_button;
    bool _paused            = false;

    Button* _pressed_button = nullptr;
    Button* _tapped_button  = nullptr;

    typedef std::function<void(Button& btn)> CallbackFunction;

    CallbackFunction pressed_cb  = nullptr;
    CallbackFunction released_cb = nullptr;

    void process_internal();

  public:
    Buttons() :
        _red_button("red", color_t::ColorRed, RED_BUTTON_PIN),
        _green_button("green", color_t::ColorGreen, GREEN_BUTTON_PIN),
        _blue_button("blue", color_t::ColorBlue, BLUE_BUTTON_PIN),
        _yellow_button("yellow", color_t::ColorYellow, YELLOW_BUTTON_PIN) {}

    ~Buttons() {
        // Destructor
    }

    void setup();

    void loop();

    bool isPressed();

    bool isTapped();

    void setPressedCallback(CallbackFunction cb) { pressed_cb = cb; }

    void setReleasedCallback(CallbackFunction cb) { released_cb = cb; }

    color_t getPressedButtonType() {
        return _pressed_button ? _pressed_button->getType() : color_t::ColorNone;
    }

    color_t getTappedButtonType() {
        return _tapped_button ? _tapped_button->getType() : color_t::ColorNone;
    }

    Button* getPressedButton() { return _pressed_button; }

    Button* getTappedButton() { return _tapped_button; }

    void pause();

    void resume();

    bool isPaused() { return _paused; }
};

} // namespace simon

#endif // __SIMON_BUTTONS_H__