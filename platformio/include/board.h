#ifndef __SIMON_BOARD_H__
#define __SIMON_BOARD_H__

#include <Arduino.h>
#include "color.h"

namespace simon {

class Board {

public:
    Board() = default;
    ~Board() = default;

    void setup();

    void turn_off_builtin_led();

    void set_rgb_led_color(bool red, bool green, bool blue);

    void turn_off_rgb_leds();
};

} // namespace simon

#endif // __SIMON_BOARD_H__