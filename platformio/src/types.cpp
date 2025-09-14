#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "tones.h"
#include "types.h"

namespace simon {

color_t next_color() { return static_cast<color_t>(random(ColorYellow, ColorNone)); }

simon::note_t colorToNote(simon::color_t color) {
    switch (color) {
    case ColorRed:    return NOTE_A4;
    case ColorGreen:  return NOTE_B4;
    case ColorBlue:   return NOTE_C5;
    case ColorYellow: return NOTE_D5;
    default:          return 0;
    }
}

String colorToString(simon::color_t color) {
    switch (color) {
    case ColorYellow: return F("Yellow");
    case ColorGreen:  return F("Green");
    case ColorRed:    return F("Red");
    case ColorBlue:   return F("Blue");
    default:          return F("None");
    }
}

uint32_t colorToRGB(simon::color_t color) {
    switch (color) {
    case ColorYellow: return Adafruit_NeoPixel::Color(255, 255, 0); // Yellow
    case ColorGreen:  return Adafruit_NeoPixel::Color(0, 255, 0);   // Green
    case ColorRed:    return Adafruit_NeoPixel::Color(255, 0, 0);
    case ColorBlue:   return Adafruit_NeoPixel::Color(0, 0, 255); // Blue
    default:          return 0;                                            // Return 0 for unsupported colors
    }
}

} // namespace simon