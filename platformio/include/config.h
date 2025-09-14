#ifndef __SIMON_CONFIG_H__
#define __SIMON_CONFIG_H__

//
// Simon Game Configuration
//

// Pin Definitions
// ------------------------------------------------------

#ifdef ARDUINO_NANO_ESP32
// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN 6
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 24
// Buzzer pin
#define BUZZER_PIN D2
// Buttons pin
#define BUTTONS_PIN A7
// Reset button pin
#define RESET_BUTTON_PIN D7

// Button analog ranges for Arduino Nano ESP32
#define YELLOW_BUTTON_MIN 2000
#define YELLOW_BUTTON_MAX 2250
#define BLUE_BUTTON_MIN   2330
#define BLUE_BUTTON_MAX   2530
#define GREEN_BUTTON_MIN  2730
#define GREEN_BUTTON_MAX  2930
#define RED_BUTTON_MIN    3000
#define RED_BUTTON_MAX    3500

#elif defined(XIAO_ESP32_C6)
#define LED_PIN           D10
#define LED_COUNT         24
#define BUZZER_PIN        D8
#define BUTTONS_PIN       A1
// Reset button pin
#define RESET_BUTTON_PIN  D7

// Button analog ranges for Seeed Xiao ESP32-C6 (calibrated values)
#define YELLOW_BUTTON_MIN 1630
#define YELLOW_BUTTON_MAX 1790
#define BLUE_BUTTON_MIN   1870
#define BLUE_BUTTON_MAX   2030
#define GREEN_BUTTON_MIN  2170
#define GREEN_BUTTON_MAX  2330
#define RED_BUTTON_MIN    2590
#define RED_BUTTON_MAX    2750

#endif // ARDUINO_NANO_ESP32

// OLED Display Configuration
// ------------------------------------------------------

#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  64   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Buttons Configuration
// ------------------------------------------------------
#define BUTTONS_TAP_DURATION   50
#define BUTTONS_DEBOUNCE_DELAY 50 // Debounce delay in milliseconds
#define BUTTONS_MIN_READINGS_COUNT                                                                 \
    (uint8_t)5                   // Minimum readings count for button state stabilization
#define IN_SEQUENCE_TIMEOUT 5000 // Timeout for user input in milliseconds

// Buzzer Configuration
// ------------------------------------------------------
#define BUZZER_DEBUG          1
#define SUCCESS_TONE_DURATION 64   // Duration of success tone in milliseconds
#define ERROR_TONE_DURATION   1500 // Duration of error tone in milliseconds

// Game Configuration
// ------------------------------------------------------
#define MAX_SEQUENCE_LENGTH 100 // Maximum sequence length to prevent memory overflow

// Debug Configuration
// ------------------------------------------------------
// Uncomment the line below to enable button calibration mode
// #define BUTTON_CALIBRATION_MODE

#endif // __SIMON_CONFIG_H__