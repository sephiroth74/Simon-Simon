#ifndef __SIMON__TYPES_H__
#define __SIMON__TYPES_H__

#include <Arduino.h>

namespace simon {

typedef uint16_t note_t;

typedef enum Color {
    ColorYellow,
    ColorBlue,
    ColorGreen,
    ColorRed,
    ColorNone,
} color_t;

#define COLORS_COUNT 4

String colorToString(simon::color_t color);

uint32_t colorToRGB(simon::color_t color);

simon::note_t colorToNote(simon::color_t color);

color_t next_color();

} // namespace simon

#endif // __SIMON__TYPES_H__