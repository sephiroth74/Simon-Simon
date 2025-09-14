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
// Reset button pin
#define RESET_BUTTON_PIN D7

// Individual button pins for Arduino Nano ESP32
#define RED_BUTTON_PIN    D3
#define GREEN_BUTTON_PIN  D4
#define BLUE_BUTTON_PIN   D5
#define YELLOW_BUTTON_PIN A7

#elif defined(XIAO_ESP32_C6)
#define LED_PIN           D10
#define LED_COUNT         24
#define BUZZER_PIN        D8
// Reset button pin
#define RESET_BUTTON_PIN  D7

// Individual button pins for Seeed Xiao ESP32-C6
#define RED_BUTTON_PIN    D0
#define GREEN_BUTTON_PIN  D1
#define BLUE_BUTTON_PIN   D2
#define YELLOW_BUTTON_PIN D3

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
// Button calibration mode is no longer needed with digital buttons
// #define BUTTON_CALIBRATION_MODE

#endif // __SIMON_CONFIG_H__