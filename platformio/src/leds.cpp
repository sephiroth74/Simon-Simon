#include "leds.h"
#include <Arduino.h>

using namespace simon;

void Leds::showColor(simon::color_t c, unsigned long wait) {
    int16_t firstPixel = -1;                   // Start at the first pixel
    uint32_t color     = simon::colorToRGB(c); // Convert color_t to RGB value

    switch (c) {
    case color_t::ColorRed:    firstPixel = 0; break;

    case color_t::ColorGreen:  firstPixel = 6; break;

    case color_t::ColorBlue:   firstPixel = 12; break;

    case color_t::ColorYellow: firstPixel = 18; break;

    case color_t::ColorNone:
        firstPixel = -1; // No color, no pixels to show
        break;
    }

    if (firstPixel >= 0) {
        if (wait > 0) {
            wipe(color, simon::wipe_direction_t::WipeFromCenter, wait, firstPixel, 6);
        } else {
            fill(color, firstPixel, 6);
        }
    }
}

void Leds::fill(uint32_t color, unsigned int firstPixel, unsigned int count) {
    if (firstPixel >= _strip.numPixels()) {
        Serial.println(F("Error: firstPixel exceeds strip length!"));
        return; // Exit if firstPixel is out of bounds
    }

    if (count == 0 || firstPixel + count > _strip.numPixels()) {
        count = _strip.numPixels() - firstPixel; // Fill to the end of the strip
    }

    _strip.fill(color, firstPixel, count); // Fill the strip with the specified color
    show();                                // Update strip to match
}

void Leds::clear() { _strip.clear(); }

void Leds::clearNow() {
    clear();
    show();
}

void Leds::show() {
    _strip.show(); // Update strip to match
}

void Leds::rainbow(unsigned long wait, uint8_t count) {
    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this loop:
    for (long firstPixelHue = 0; firstPixelHue < count * 65536; firstPixelHue += 256) {
        // strip.rainbow() can take a single argument (first pixel hue) or
        // optionally a few extras: number of rainbow repetitions (default 1),
        // saturation and value (brightness) (both 0-255, similar to the
        // ColorHSV() function, default 255), and a true/false flag for whether
        // to apply gamma correction to provide 'truer' colors (default true).
        _strip.rainbow(firstPixelHue);
        // Above line is equivalent to:
        // strip.rainbow(firstPixelHue, 1, 255, 255, true);
        _strip.show(); // Update strip with new contents
        delay(wait);   // Pause for a moment
    }
}

void Leds::wipe(uint32_t color,
                simon::wipe_direction_t direction,
                unsigned long wait,
                unsigned int firstPixel,
                unsigned int count) {
    switch (direction) {
    case simon::wipe_direction_t::WipeFromStart:
        wipeFromStart(color, wait, firstPixel, count);
        break;

    case simon::wipe_direction_t::WipeFromCenter:
        wipeFromCenter(color, wait, firstPixel, count);
        break;

    case simon::wipe_direction_t::WipeFromEdges:
        wipeFromEdged(color, wait, firstPixel, count);
        break;
    }
}

void Leds::wipeFromStart(uint32_t color,
                         unsigned long wait,
                         unsigned int firstPixel,
                         unsigned int count) {
    if (firstPixel >= _strip.numPixels()) {
        Serial.println(F("Error: firstPixel exceeds strip length!"));
        return; // Exit if firstPixel is out of bounds
    }

    if (firstPixel + count > _strip.numPixels()) {
        Serial.println(F("Error: firstPixel + count exceeds strip length!"));
        return; // Exit if count exceeds strip length
    }

    for (int i = firstPixel; i < firstPixel + count; i++) { // For each pixel in strip...
        _strip.setPixelColor(i, color);                     //  Set pixel's color (in RAM)

        if (wait > 0) {
            show();      //  Update strip to match
            delay(wait); //  Pause for a moment
        }
    }

    if (wait <= 0) {
        show(); // If no wait, update strip once at the end
    }
}

void Leds::wipeFromCenter(uint32_t color,
                          unsigned long wait,
                          unsigned int firstPixel,
                          unsigned int count) {
    if (firstPixel >= _strip.numPixels()) {
        Serial.println(F("Error: firstPixel exceeds strip length!"));
        return; // Exit if firstPixel is out of bounds
    }

    if (firstPixel + count > _strip.numPixels()) {
        Serial.println(F("Error: firstPixel + count exceeds strip length!"));
        return; // Exit if count exceeds strip length
    }

    auto centerPixel = firstPixel + count / 2 - 1; // Center pixel index
    // start from the center and move outwards
    for (int i = 0; i < count / 2; i++) { // For each pixel in strip...
        if (i == 0) {
            _strip.setPixelColor(centerPixel, color); // Set center pixel's color (in RAM)
        } else {
            _strip.setPixelColor(centerPixel - i, color); // Set pixel's color (in RAM)
            _strip.setPixelColor(centerPixel + i, color); // Set the opposite pixel
        }

        if (wait > 0) {
            show();      //  Update strip to match
            delay(wait); //  Pause for a moment
        }
    }

    if (wait <= 0) {
        show(); // If no wait, update strip once at the end
    }
}

void Leds::wipeFromEdged(uint32_t color,
                         unsigned long wait,
                         unsigned int firstPixel,
                         unsigned int count) {
    if (firstPixel >= _strip.numPixels()) {
        Serial.println(F("Error: firstPixel exceeds strip length!"));
        return; // Exit if firstPixel is out of bounds
    }

    if (firstPixel + count > _strip.numPixels()) {
        Serial.println(F("Error: firstPixel + count exceeds strip length!"));
        return; // Exit if count exceeds strip length
    }

    for (int i = 0; i < count; i++) {                            // For each pixel in strip...
        _strip.setPixelColor(firstPixel + i, color);             //  Set pixel's color (in RAM)
        _strip.setPixelColor(firstPixel + count - 1 - i, color); // Set the opposite pixel

        if (wait > 0) {
            show();      //  Update strip to match
            delay(wait); //  Pause for a moment
        }
    }

    if (wait <= 0) {
        show(); // If no wait, update strip once at the end
    }
}

void Leds::setup() {
    _strip.begin();           // Initialize the NeoPixel strip
    _strip.show();            // Initialize all pixels to 'off'
    _strip.setBrightness(50); // Set brightness (0-255)
}