#include "board.h"

namespace simon {

void Board::setup() {
#ifdef ARDUINO_NANO_ESP32
    Serial.println(F("Setting up board ARDUINO_NANO_ESP32..."));
    pinMode(LED_RED, OUTPUT);     // Set the red LED pin as output
    pinMode(LED_GREEN, OUTPUT);   // Set the green LED pin as output
    pinMode(LED_BLUE, OUTPUT);    // Set the blue LED pin as output
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as output
#endif
}

void Board::turn_off_builtin_led() {
#ifdef ARDUINO_NANO_ESP32
    Serial.println(F("Turning off built-in LED..."));
    digitalWrite(LED_BUILTIN, LOW); // Turn off the built-in LED
#endif
}

void Board::set_rgb_led_color(bool red, bool green, bool blue) {
#ifdef ARDUINO_NANO_ESP32
    digitalWrite(LED_RED, red ? LOW : HIGH);
    digitalWrite(LED_GREEN, green ? LOW : HIGH);
    digitalWrite(LED_BLUE, blue ? LOW : HIGH);
#endif
}

void Board::turn_off_rgb_leds() {
#ifdef ARDUINO_NANO_ESP32
    Serial.println(F("Turning off RGB LEDs..."));
    digitalWrite(LED_RED, HIGH);   // Turn off the red LED
    digitalWrite(LED_GREEN, HIGH); // Turn off the green LED
    digitalWrite(LED_BLUE, HIGH);  // Turn off the blue LED
#endif
}

} // namespace simon