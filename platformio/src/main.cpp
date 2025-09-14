#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "config.h" // Configuration file for pin definitions and other constants
#include "fsm.h"
#include "game.h"

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

simon::Game game; // Create an instance of the Simon game


void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    delay(1000);

    // Setup reset button pin
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

    // Normal game setup
    if (!game.setup()) {
        Serial.println(F("Game setup failed!"));
        return;
    }
}

void checkResetButton() {
    static bool lastButtonState               = HIGH;
    static unsigned long lastButtonTime       = 0;
    static bool buttonPressed                 = false;
    static unsigned long buttonPressStartTime = 0;
    const unsigned long debounceDelay         = 50;
    const unsigned long longPressDelay        = 7000; // 7 seconds for high score reset

    bool currentButtonState                   = digitalRead(RESET_BUTTON_PIN);

    // Check if button state changed and debounce
    if (currentButtonState != lastButtonState) {
        lastButtonTime  = millis();
        lastButtonState = currentButtonState;
    }

    // If button has been stable for debounce time
    if ((millis() - lastButtonTime) > debounceDelay) {
        // Detect button press (transition from HIGH to LOW)
        if (currentButtonState == LOW && !buttonPressed) {
            buttonPressed        = true;
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

void loop() {
    // Check for reset button press
    checkResetButton();

    // Call the game loop to handle button presses and game logic
    game.loop();
}

// Some functions of our own for creating animated effects -----------------
