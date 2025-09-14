
#ifndef __SIMON_BUTTONS_H__
#define __SIMON_BUTTONS_H__

#include "config.h"
#include "types.h"
#include <Arduino.h>
#include <functional>

namespace simon {

// -------------------------------------------
// Range
// -------------------------------------------

class Range {
  private:
    uint16_t _min_value;
    uint16_t _max_value;

  public:
    Range(uint16_t min_value, uint16_t max_value) : _min_value(min_value), _max_value(max_value) {}

    ~Range() {}

    uint16_t getMin() { return _min_value; }
    uint16_t getMax() { return _max_value; }

    bool const contains(uint16_t value) const { return value >= _min_value && value <= _max_value; }
};

// -------------------------------------------
// Button
// -------------------------------------------

class Button {
  private:
    const char* _name;
    color_t _type;
    Range _range;
    bool _is_pressed = false;
    bool _is_tapped  = false;

  public:
    Button(const char* name, color_t type, Range range) : _name(name), _type(type), _range(range) {}

    ~Button() {}

    const char* getName() { return _name; }

    color_t getType() { return _type; }

    Range getRange() { return _range; }

    bool isPressed() { return _is_pressed; }

    bool isTapped() { return _is_tapped; }

    void setTapped(bool value);

    void setPressed(bool value);

    bool process(int value);

    bool inRange(int value) { return _range.contains(value); }

    void reset() {
        _is_pressed = false;
        _is_tapped  = false;
    }
};

// -------------------------------------------
// Buttons
// -------------------------------------------
// Range yellowRange(1950, 2100);
// Range blueRange(2250, 2400);
// Range greenRange(2600, 2700);
// Range redRange(3150, 3300);

class Buttons {
  private:
    int8_t _pin;
    Button _yellow_button;
    Button _blue_button;
    Button _green_button;
    Button _red_button;
    bool _paused            = false;

    Button* _pressed_button = nullptr;
    Button* _tapped_button  = nullptr;

    typedef std::function<void(Button& btn)> CallbackFunction;

    CallbackFunction pressed_cb  = nullptr;
    CallbackFunction released_cb = nullptr;

    void process_internal();
    Button* check_pressed_button();

  public:
    Buttons(int8_t pin) :
        _pin(pin),
        _yellow_button("yellow", color_t::ColorYellow, Range(YELLOW_BUTTON_MIN, YELLOW_BUTTON_MAX)),
        _blue_button("blue", color_t::ColorBlue, Range(BLUE_BUTTON_MIN, BLUE_BUTTON_MAX)),
        _green_button("green", color_t::ColorGreen, Range(GREEN_BUTTON_MIN, GREEN_BUTTON_MAX)),
        _red_button("red", color_t::ColorRed, Range(RED_BUTTON_MIN, RED_BUTTON_MAX)) {}

    ~Buttons() {
        // Destructor
    }

    void setup();

    void loop();

    bool isPressed();

    bool isTapped();

    uint16_t lowestButtonState() { return _yellow_button.getRange().getMin(); }

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