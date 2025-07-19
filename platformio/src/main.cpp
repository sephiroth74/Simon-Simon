#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h" // Configuration file for pin definitions and other constants
#include "game.h"
#include "fsm.h"

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// simon::Leds leds(strip); // Initialize the LED with the NeoPixel strip and index 0
// simon::Buttons buttons(BUTTONS_PIN); // Initialize the buttons with the specified pin
// simon::Buzzer buzzer(BUZZER_PIN); // Initialize the buzzer with the specified pin

simon::Game game; // Create an instance of the Simon game

// FSM_INITIAL_STATE(simon::Fsm::Switch, simon::Fsm::InitialState)

// void onStateEnter(simon::Fsm::StateType const& type)
// {
//     Serial.print(F("Entering State: "));
//     Serial.println(simon::Fsm::stateTypeToString(type));
// }

// void onStateExit(simon::Fsm::StateType const& type)
// {
//     Serial.print(F("Exiting State: "));
//     Serial.println(simon::Fsm::stateTypeToString(type));
// }


// void onButtonPressed(simon::Button& btn)
// {
//     Serial.print(F("Button pressed: "));
//     Serial.println(btn.getName());
//     leds.clear();
//     leds.showColor(btn.getType(), 40); // Set strip color based on pressed button

//     buzzer.toneStart(simon::colorToNote(btn.getType())); // Play sound for the pressed button
//     display.clearDisplay();
//     display.setTextSize(1);
//     display.setCursor(0, 16);
//     display.print(F("Pressed: "));
//     display.println(btn.getName());
//     display.display();
//     fsm_handle::dispatch(simon::Fsm::Start());
// }

// void onButtonReleased(simon::Button& btn)
// {
//     Serial.print(F("Button released: "));
//     Serial.println(btn.getName());

//     leds.clear(); // Clear the strip if button is released
//     leds.show(); // Update the strip to show the cleared state

//     buzzer.stop(); // Stop the buzzer sound
//     display.clearDisplay();
//     display.display();
// }

void setup()
{
    Serial.begin(115200); // Initialize serial communication for debugging
    delay(1000);

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.

    if(!game.setup()) {
        Serial.println(F("Game setup failed!"));
        return;
    }

}



void loop()
{
    game.loop(); // Call the game loop to handle button presses and game logic
    
    // Fill along the length of the strip in various colors...
    // display.clearDisplay();
    // buttons.loop(); // Process button events

    // auto pressed_button = buttons.get_pressed_button();
    // auto tapped_button = buttons.get_tapped_button();

    // buzzer.stop();

    // if (pressed_button) {
    //     Serial.print(F("Button pressed: "));
    //     Serial.println(pressed_button->get_name());
    //     display.setTextSize(1);
    //     display.setCursor(0, 16);
    //     display.print(F("Pressed: "));
    //     display.println(pressed_button->get_name());
    //     display.display();

    //     strip_color(pressed_button->get_type()); // Set strip color based on pressed button
    //     buzzer.play_color(pressed_button->get_type()); // Play sound for the pressed button
    // } else if (tapped_button) {
    //     Serial.print(F("Button tapped: "));
    //     Serial.println(tapped_button->get_name());
    //     display.setTextSize(1);
    //     display.setCursor(0, 24);
    //     display.print(F("Tapped: "));
    //     display.println(tapped_button->get_name());
    //     display.display();
    //     delay(500); // Short delay to avoid flooding the serial output

    //     display.clearDisplay();
    //     display.display();
    // } else {
    //   strip.clear(); // Clear the strip if no button is pressed or tapped
    //   strip.show();
    // }

    // if(buttons.is_tapped()) {
    //     Serial.println(F("Button tapped!"));
    //     auto tapped_button = buttons.get_tapped_button();

    //     Serial.print(F("Tapped button: "));
    //     if (tapped_button) {
    //         Serial.println(tapped_button->get_name());
    //     } else {
    //         Serial.println(F("No button tapped"));
    //     }
    // }

    // if (buttons.is_pressed()) {
    //     Serial.println(F("Button pressed!"));
    //     auto pressed_button = buttons.get_pressed_button();

    //     Serial.print(F("Pressed button: "));
    //     if (pressed_button) {
    //         Serial.println(pressed_button->get_name());
    //     } else {
    //         Serial.println(F("No button pressed"));
    //     }
    // }

    // delay(16); // Short delay to avoid flooding the serial output
    // return;

    // display.clearDisplay();
    // display.setTextSize(2);
    // display.setCursor(0, 0);
    // display.print(F("Loop #"));
    // display.println(count++);
    // display.setTextSize(1);

    // display.println(F("Rainbow Cycle"));
    // display.display();
    // leds.rainbow(2); // Flowing rainbow cycle along the whole strip

    // display.println(F("Color Wipe"));
    // display.display();

    // leds.wipe(strip.Color(255, 255, 255), simon::wipe_direction_t::WipeFromStart, 50, 0, strip.numPixels());
    // leds.wipe(strip.Color(255, 0, 0), simon::wipe_direction_t::WipeFromStart, 50, 0, 6); // Red
    // leds.wipe(strip.Color(0, 255, 0), simon::wipe_direction_t::WipeFromStart, 50, 6, 6); // Green
    // leds.wipe(strip.Color(0, 0, 255), simon::wipe_direction_t::WipeFromStart, 50, 12, 6); // Blue
    // leds.wipe(strip.Color(255, 255, 0), simon::wipe_direction_t::WipeFromStart, 50, 18, 6); // Yellow

    // delay(1000);

    // strip.clear(); // Clear the strip
    // strip.show(); // Update strip to match
    // delay(200); // Wait a moment before next effect

    // leds.wipe(strip.Color(255, 0, 0), simon::wipe_direction_t::WipeFromStart, 0, 0, 6);
    // leds.wipe(strip.Color(0, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 6, 6);
    // leds.wipe(strip.Color(0, 0, 255), simon::wipe_direction_t::WipeFromStart, 0, 12, 6);
    // leds.wipe(strip.Color(255, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 18, 6);
    // delay(200);

    // strip.clear(); // Clear the strip
    // strip.show(); // Update strip to match
    // delay(200); // Wait a moment before next effect

    // leds.wipe(strip.Color(255, 0, 0), simon::wipe_direction_t::WipeFromStart, 0, 0, 6);
    // leds.wipe(strip.Color(0, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 6, 6);
    // leds.wipe(strip.Color(0, 0, 255), simon::wipe_direction_t::WipeFromStart, 0, 12, 6);
    // leds.wipe(strip.Color(255, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 18, 6);
    // delay(200); // Wait a moment before next effect

    // strip.clear(); // Clear the strip
    // strip.show(); // Update strip to match
    // delay(200); // Wait a moment before next effect

    // leds.wipe(strip.Color(255, 0, 0), simon::wipe_direction_t::WipeFromStart, 0, 0, 6);
    // leds.wipe(strip.Color(0, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 6, 6);
    // leds.wipe(strip.Color(0, 0, 255), simon::wipe_direction_t::WipeFromStart, 0, 12, 6);
    // leds.wipe(strip.Color(255, 255, 0), simon::wipe_direction_t::WipeFromStart, 0, 18, 6);
    // delay(200); // Wait a moment before next effect

    // delay(1000); // Wait a second before starting again

    // strip.clear();
    // delay(1000); // Clear the strip and wait a second before repeating
}

// Some functions of our own for creating animated effects -----------------


