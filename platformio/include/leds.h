#ifndef __SIMON_LEDS_H__
#define __SIMON_LEDS_H__

#include "types.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

namespace simon {

typedef enum WipeDirection {
    WipeFromStart,  // Wipe from the start of the strip
    WipeFromCenter, // Wipe from the center of the strip
    WipeFromEdges,  // Wipe from the edges of the strip
} wipe_direction_t;

class Leds {
  private:
    Adafruit_NeoPixel& _strip;

    void
    wipeFromCenter(uint32_t color, unsigned long wait, unsigned int firstPixel, unsigned int count);

    void
    wipeFromEdged(uint32_t color, unsigned long wait, unsigned int firstPixel, unsigned int count);

    void
    wipeFromStart(uint32_t color, unsigned long wait, unsigned int firstPixel, unsigned int count);

  public:
    Leds(Adafruit_NeoPixel& strip) : _strip(strip) {}

    void setup();

    void showColor(simon::color_t c, unsigned long wait = 0);

    void fill(uint32_t color, unsigned int firstPixel = 0, unsigned int count = 0);

    void fill_all(color_t color) { fill(colorToRGB(color), 0, _strip.numPixels()); }

    void clearNow();

    void clear();

    void show();

    // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
    void rainbow(unsigned long wait = 2, uint8_t count = 2);

    void wipe(uint32_t color,
              simon::wipe_direction_t direction = simon::WipeFromStart,
              unsigned long wait                = 0,
              unsigned int firstPixel           = 0,
              unsigned int count                = 0);
};
} // namespace simon

#endif // __SIMON_LEDS_H__