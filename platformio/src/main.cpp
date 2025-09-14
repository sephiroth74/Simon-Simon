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

simon::Game game; // Create an instance of the Simon game

#ifdef BUTTON_CALIBRATION_MODE
void buttonCalibrationTest()
{
    static unsigned long lastPrint = 0;
    static uint16_t lastValue = 0;
    static uint16_t minValue = 4095;
    static uint16_t maxValue = 0;

    const unsigned long printInterval = 100; // Print every 100ms

    if (millis() - lastPrint > printInterval) {
        uint16_t currentValue = analogRead(BUTTONS_PIN);

        // Track min/max values
        if (currentValue > 1000) { // Only track significant readings
            minValue = min(minValue, currentValue);
            maxValue = max(maxValue, currentValue);
        }

        // Only print if value changed significantly or periodically
        if (abs(currentValue - lastValue) > 10 || (millis() - lastPrint) > 1000) {
            Serial.print(F("Analog: "));
            Serial.print(currentValue);
            Serial.print(F(" | Min: "));
            Serial.print(minValue);
            Serial.print(F(" | Max: "));
            Serial.print(maxValue);

            // Show which button range this falls into
            if (currentValue >= YELLOW_BUTTON_MIN && currentValue <= YELLOW_BUTTON_MAX) {
                Serial.print(F(" | YELLOW"));
            } else if (currentValue >= BLUE_BUTTON_MIN && currentValue <= BLUE_BUTTON_MAX) {
                Serial.print(F(" | BLUE"));
            } else if (currentValue >= GREEN_BUTTON_MIN && currentValue <= GREEN_BUTTON_MAX) {
                Serial.print(F(" | GREEN"));
            } else if (currentValue >= RED_BUTTON_MIN && currentValue <= RED_BUTTON_MAX) {
                Serial.print(F(" | RED"));
            } else if (currentValue > 1000) {
                Serial.print(F(" | UNKNOWN"));
            } else {
                Serial.print(F(" | NO_PRESS"));
            }

            Serial.println();
            lastValue = currentValue;
        }

        lastPrint = millis();
    }

    // Reset min/max every 30 seconds
    static unsigned long lastReset = 0;
    if (millis() - lastReset > 30000) {
        minValue = 4095;
        maxValue = 0;
        lastReset = millis();
        Serial.println(F("--- Min/Max values reset ---"));
    }
}
#endif

void setup()
{
    // Initialize serial communication for debugging
    Serial.begin(115200);
    delay(1000);

    // Setup reset button pin
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

#ifdef BUTTON_CALIBRATION_MODE
    Serial.println(F("========================================"));
    Serial.println(F("BUTTON CALIBRATION MODE ENABLED"));
    Serial.println(F("========================================"));
    Serial.println(F("Press each button and observe the values:"));
    Serial.println(F("- Note the min/max values for each button"));
    Serial.println(F("- Update config.h with appropriate ranges"));
    Serial.println(F("- Comment out BUTTON_CALIBRATION_MODE when done"));
    Serial.println(F("========================================"));

    // Only initialize button pin for calibration
    pinMode(BUTTONS_PIN, INPUT_PULLDOWN);
    delay(1000);
#else
    // Normal game setup
    if (!game.setup()) {
        Serial.println(F("Game setup failed!"));
        return;
    }
#endif
}

void checkResetButton()
{
    static bool lastButtonState = HIGH;
    static unsigned long lastButtonTime = 0;
    static bool buttonPressed = false;
    static unsigned long buttonPressStartTime = 0;
    const unsigned long debounceDelay = 50;
    const unsigned long longPressDelay = 7000; // 7 seconds for high score reset

    bool currentButtonState = digitalRead(RESET_BUTTON_PIN);

    // Check if button state changed and debounce
    if (currentButtonState != lastButtonState) {
        lastButtonTime = millis();
        lastButtonState = currentButtonState;
    }

    // If button has been stable for debounce time
    if ((millis() - lastButtonTime) > debounceDelay) {
        // Detect button press (transition from HIGH to LOW)
        if (currentButtonState == LOW && !buttonPressed) {
            buttonPressed = true;
            buttonPressStartTime = millis();
            Serial.println(F("Reset button pressed..."));
        }
        // Check for long press while button is held down
        else if (currentButtonState == LOW && buttonPressed) {
            unsigned long holdDuration = millis() - buttonPressStartTime;

            // Check if we're in initial state and long press detected
            if (holdDuration > longPressDelay) {
                auto currentState = game.getCurrentState();
                if (currentState == simon::Fsm::StateType::INITIAL_STATE) {
                    Serial.println(F("Long press detected - resetting high score!"));
                    game.resetHighScore();
                    buttonPressed = false; // Reset to avoid repeated triggers
                }
            }
        }
        // Reset press flag when button is released
        else if (currentButtonState == HIGH && buttonPressed) {
            unsigned long holdDuration = millis() - buttonPressStartTime;

            // Short press - restart system (only if not long press)
            if (holdDuration < longPressDelay) {
                Serial.println(F("Short press - restarting system..."));
                delay(100);
                ESP.restart();
            }

            buttonPressed = false;
        }
    }
}

void loop()
{
    // Check for reset button press
    checkResetButton();

#ifdef BUTTON_CALIBRATION_MODE
    // Run button calibration test instead of game
    buttonCalibrationTest();
#else
    // Call the game loop to handle button presses and game logic
    game.loop();
#endif
}

// Some functions of our own for creating animated effects -----------------
