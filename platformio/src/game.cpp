#include <Arduino.h>
#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif

#include "config.h"
#include "fsm.h"
#include "game.h"
#include "tones.h"
#include <vector>

// PROGMEM strings for display
const char PROGMEM STR_SIMON[]          = "Simon";
const char PROGMEM STR_AMPERSAND[]      = "&";
const char PROGMEM STR_PRESS_BUTTON[]   = "Premi un";
const char PROGMEM STR_BUTTON_TO[]      = "tasto per";
const char PROGMEM STR_START[]          = "iniziare!";
const char PROGMEM STR_RECORD[]         = "Record";
const char PROGMEM STR_CURRENT[]        = "attuale:";
const char PROGMEM STR_READY[]          = "Pronti";
const char PROGMEM STR_START_GAME[]     = "Partenza";
const char PROGMEM STR_GO[]             = "Via!";
const char PROGMEM STR_PRESS_THE[]      = "Premi il";
const char PROGMEM STR_RIGHT_BUTTON[]   = "tasto giusto!";
const char PROGMEM STR_GREAT[]          = "Bravo!";
const char PROGMEM STR_ROUND[]          = "Round: ";
const char PROGMEM STR_YOU_LOST[]       = "Hai perso!";
const char PROGMEM STR_NEW[]            = "Nuovo";
const char PROGMEM STR_RECORD_EXCL[]    = "Record!";
const char PROGMEM STR_TESTING[]        = "Testing";
const char PROGMEM STR_CELEBRATION[]    = "Celebration";
const char PROGMEM STR_EFFECTS[]        = "Effects!";
const char PROGMEM STR_RECORD_SHORT[]   = "RECORD!";
const char PROGMEM STR_NEW_RECORD[]     = "NUOVO RECORD!";
const char PROGMEM STR_TOTAL_VICTORY[]  = "Vittoria";
const char PROGMEM STR_TOTAL[]          = "Totale!";
const char PROGMEM STR_SEQUENCE[]       = "Sequenza";
const char PROGMEM STR_MAXIMUM[]        = "Massima!";
const char PROGMEM STR_RESET_RECORD[]   = "Resetto";
const char PROGMEM STR_RECORD_RESET[]   = "Record...";
const char PROGMEM STR_RECORD_CLEARED[] = "Record";
const char PROGMEM STR_CLEARED[]        = "Cancellato!";

FSM_INITIAL_STATE(simon::Fsm::Switch, simon::Fsm::InitialState)

namespace simon {

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

Preferences _preferences; // Preferences object for storing game state

using fsm_handle = simon::Fsm::Switch;

// Global variables for game state
unsigned long state_start_time = 0;

// Sequence of colors for the game
std::vector<color_t> sequence;
// Current index in the sequence
size_t button_index = 0;
// Timer for button press duration
unsigned long button_timer = 0;

Game::Game() :
    _leds(simon::Leds(strip)) // Initialize the LED controller with the NeoPixel strip
    ,
    _buttons() // Initialize the button controller with individual pins
    ,
    _buzzer(BUZZER_PIN) // Initialize the buzzer controller with the defined pin
    ,
    _display(Adafruit_SSD1306(SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              &Wire,
                              OLED_RESET)) // Initialize the display controller
    ,
    _board(Board()) // Initialize the board controller
{
    // Constructor implementation
}

void Game::displayWelcomeMessage() {
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0); // Start at top-left corner
    _display.setTextColor(SSD1306_WHITE);
    _display.display();

    // draw the text centered horizontally on the display

    int16_t x1, y1;
    uint16_t w, h;

    _display.getTextBounds(FPSTR(STR_SIMON), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 0); // Center text
    _display.println(FPSTR(STR_SIMON));

    _display.getTextBounds(FPSTR(STR_AMPERSAND), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 16); // Center text
    _display.println(FPSTR(STR_AMPERSAND));

    _display.getTextBounds(FPSTR(STR_SIMON), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 32); // Center text
    _display.println(FPSTR(STR_SIMON));
    _display.display();
}

bool Game::setup() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Serial.println(F("Init board.."));
    _board.setup();
    Serial.println(F("done."));

    _board.turn_off_builtin_led();
    _board.turn_off_rgb_leds(); // Turn off RGB LEDs

    Serial.print(F("Init SSD1306 display..."));
    if (!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("failed!"));
        _board.set_rgb_led_color(true, false, false);

        // Try to continue without display but indicate error
        Serial.println(F("Warning: Continuing without display functionality"));
        delay(2000);
        _board.turn_off_rgb_leds();
    } else {
        Serial.println(F("done."));
    }

    _display.clearDisplay();
    _display.setTextSize(1);              // Set text size to 1
    _display.setTextColor(SSD1306_WHITE); // Set text color to white
    _display.setCursor(0, 0);             // Set cursor to top-left corner
    _display.display();
    delay(100);

    _display.println(F("Init Preferences.."));
    _display.display();
    if (!_preferences.begin("simon", false)) {
        _display.println(F("error!"));
        Serial.println(
            F("Warning: Preferences initialization failed, high score will not persist"));
        _high_score = 0; // Use default value
        delay(1000);
    } else {
        _high_score = _preferences.getUInt("high_score", 0);
        _display.println(F("ok"));
    }
    _display.display();

    delay(500);

    _display.println(F("Init leds.."));
    _display.display();
    _leds.setup();

    _display.println(F("ok"));
    _display.display();

    delay(500);

    _display.println(F("Init buttons.."));
    _display.display();

    _buttons.setup();

    _display.println(F("ok"));
    _display.display();
    delay(500);

    _display.println(F("Init buzzer.."));
    _display.display();

    _buzzer.setup();

    _display.println(F("ok"));
    _display.display();
    delay(500);

    // Display welcome message
    displayWelcomeMessage();

    _buzzer.playInitialSound();

    _leds.rainbow();
    _leds.clearNow();

    _leds.wipe(strip.Color(255, 0, 0), simon::WipeFromStart, 50, 0, 6);    // Red
    _leds.wipe(strip.Color(0, 255, 0), simon::WipeFromStart, 50, 6, 6);    // Green
    _leds.wipe(strip.Color(0, 0, 255), simon::WipeFromStart, 50, 12, 6);   // Blue
    _leds.wipe(strip.Color(255, 255, 0), simon::WipeFromStart, 50, 18, 6); // Yellow

    delay(1000); // Show the welcome message for 2 seconds

    _leds.clearNow();
    _display.clearDisplay();
    _display.display();

    // Adding callbacks for button events
    _buttons.setPressedCallback([this](Button& btn) { this->onButtonPressed(btn); });
    _buttons.setReleasedCallback([this](Button& btn) { this->onButtonReleased(btn); });

    // Set the initial state of the FSM
    simon::Fsm::setEnterCallback([this](Fsm::StateType const& type) { this->onStateEnter(type); });
    simon::Fsm::setExitCallback([this](Fsm::StateType const& type) { this->onStateExit(type); });

    fsm_handle::reset();
    fsm_handle::start();

    return true;
}

void Game::onStateEnter(Fsm::StateType const& type) {
    Serial.print(F("==> Entering State: '"));
    Serial.print(Fsm::stateTypeToString(type));
    Serial.println(F("'"));

    state_start_time = millis(); // Record the time when the state is entered

    switch (type) {
    case Fsm::StateType::INITIAL_STATE:          onEnterInitialState(); break;

    case Fsm::StateType::GAME_START_STATE:       onEnterGameStartState(); break;

    case Fsm::StateType::PLAYING_SEQUENCE_STATE: onEnterPlayingSequenceState(); break;

    case Fsm::StateType::PLAYING_USER_STATE:     onEnterPlayingUserState(); break;

    case Fsm::StateType::PLAYING_WIN_STATE:      onEnterPlayingWinState(); break;

    case Fsm::StateType::PLAYING_LOSE_STATE:     onEnterPlayingLoseState(); break;

    default:                                     Serial.println(F("Unknown state entered.")); break;
    }
}

void Game::onStateExit(Fsm::StateType const& type) {
    Serial.print(F("<== Exiting State: '"));
    Serial.print(Fsm::stateTypeToString(type));
    Serial.println(F("'"));
}

void Game::onButtonPressed(Button& btn) {
    auto currentState = fsm_handle::currentState();

    Serial.print(Fsm::stateTypeToString(currentState.getType()));
    Serial.print(F(" | Button pressed: "));
    Serial.println(btn.getName());

    // Play sound and display color for feedback in all states
    simon::color_t pressedColor = btn.getType();
    _buzzer.toneStart(colorToNote(pressedColor), 0); // Play the corresponding note
    _leds.showColor(pressedColor, 0);                // Show the color of the pressed button

    // Only process game logic in PLAYING_USER_STATE
    if (currentState.getType() != Fsm::StateType::PLAYING_USER_STATE) {
        return; // For other states, just provide feedback and return
    }
}

void Game::onButtonReleased(Button& btn) {
    auto currentState = fsm_handle::currentState();
    Serial.print(Fsm::stateTypeToString(currentState.getType()));
    Serial.print(F(" | Button released: "));
    Serial.println(btn.getName());

    simon::color_t releasedColor = btn.getType();

    // If we're in the INITIAL state, transition to the GAME_START state
    if (currentState.getType() == Fsm::StateType::INITIAL_STATE) {
        _buzzer.stop();   // Stop the buzzer sound
        _leds.clearNow(); // Clear the LEDs
        delay(500);
        fsm_handle::dispatch(Fsm::EventType::GAME_START_EVENT);

    } else if (currentState.getType() == Fsm::StateType::PLAYING_USER_STATE) {
        button_timer = millis(); // Reset the button timer when a button is pressed
        _buzzer.stop();          // Stop the buzzer sound when the button is released
        _leds.clearNow();        // Clear the LEDs when the button is released

        Serial.print(F("Button released: "));
        Serial.println(btn.getName());

        Serial.print(F("Button index: "));
        Serial.println(button_index);

        Serial.print(F("Sequence size: "));
        Serial.println(sequence.size());

        // Check if the correct button was released
        if (releasedColor == sequence[button_index]) {
            if (button_index == sequence.size() - 1) {
                fsm_handle::dispatch(Fsm::EventType::PLAYING_WIN_EVENT);
                return;
            }
            button_index++; // Move to the next button in the sequence
            delay(16);
        } else {
            fsm_handle::dispatch(Fsm::EventType::PLAYING_LOSE_EVENT);
        }
    }
}

void Game::loop() {
    _buttons.loop();

    auto currentState = fsm_handle::currentState();
    onStateLoop(currentState.getType());
}

void Game::onStateLoop(Fsm::StateType const& type) {
    switch (type) {
    case Fsm::StateType::INITIAL_STATE: onLoopInitialState(); break;

    case Fsm::StateType::GAME_START_STATE:
        // Handle logic for the GAME_START state if needed
        break;

    case Fsm::StateType::PLAYING_SEQUENCE_STATE:
        // Handle logic for the PLAYING state
        break;

    case Fsm::StateType::PLAYING_USER_STATE:
        // Handle logic for the PLAYING_USER state
        onLoopPlayingUserState();
        break;

    default: break;
    }
}

void Game::onLoopInitialState() {
    unsigned long elapsedTime = (millis() - state_start_time) / 1000;
    int switchTime            = 5;

    // switch text every 5 seconds
    if (elapsedTime % switchTime == 0) {
        if (elapsedTime % (switchTime * 2) == 0) {
            _display.clearDisplay();
            _display.setCursor(0, 0);
            _display.setTextSize(2);
            _display.println();

            _display.println(FPSTR(STR_PRESS_BUTTON));
            _display.println(FPSTR(STR_BUTTON_TO));
            _display.println(FPSTR(STR_START));
        } else {
            _display.clearDisplay();
            _display.setCursor(0, 0);
            _display.setTextSize(2);
            _display.println(FPSTR(STR_RECORD));
            _display.println(FPSTR(STR_CURRENT));
            _display.println(_high_score);
        }
        _display.display();
    }

    // Show rainbow effect every 15 seconds to indicate system is active
    static unsigned long lastRainbowTime = 0;
    const unsigned long rainbowInterval  = 15000; // 15 seconds

    if (millis() - lastRainbowTime > rainbowInterval) {
        _leds.rainbow(2, 2); // Quick rainbow with 3ms delay, 1 cycle
        _leds.clearNow();
        lastRainbowTime = millis();
    }
}

void Game::onEnterInitialState() {
    sequence.clear();
    button_index = 0;
}

void Game::onEnterGameStartState() {
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.setTextColor(SSD1306_WHITE);
    _display.display();
    _buzzer.playCountdownSound();
    delay(1000); // Short delay before starting the game

    _buzzer.playCountdownSound();
    _display.println(FPSTR(STR_READY));
    _display.display();
    delay(1000); // Show the message for 1 second

    _buzzer.playCountdownSound();
    _display.println(FPSTR(STR_START_GAME));
    _display.display();
    delay(1000); // Show the message for 1 second

    _buzzer.toneStart(NOTE_C6, 500);
    _display.println(FPSTR(STR_GO));
    _display.display();
    delay(1000); // Show the message for 1 second

    // Reset the game state
    button_index = 0; // Reset the button index
    sequence.clear(); // Clear the sequence

    // Transition to the PLAYING state
    fsm_handle::dispatch(Fsm::EventType::PLAYING_SEQUENCE_EVENT);
}

void Game::onEnterPlayingSequenceState() {
    // Check if we've reached the maximum sequence length
    if (sequence.size() >= MAX_SEQUENCE_LENGTH) {
        // Player has won by reaching the maximum sequence length!
        _display.clearDisplay();
        _display.setTextSize(2);
        _display.setCursor(0, 0);
        _display.println(FPSTR(STR_TOTAL_VICTORY));
        _display.println(FPSTR(STR_TOTAL));
        _display.println(FPSTR(STR_SEQUENCE));
        _display.println(FPSTR(STR_MAXIMUM));
        _display.display();
        delay(3000);

        // Set new high score and return to initial state
        _high_score = MAX_SEQUENCE_LENGTH;
        _preferences.putUInt("high_score", _high_score);
        fsm_handle::dispatch(Fsm::EventType::INITIAL_STATE_EVENT);
        return;
    }

    // Add the next color to the sequence
    button_index = 0; // Reset the button index for the new sequence
    sequence.push_back(next_color());

    _leds.clearNow();
    _display.clearDisplay();
    _display.display();

    delay(500);

    // now play the sequence
    for (const auto& c : sequence) {
        _buzzer.toneStart(colorToNote(c), 500); // Play the corresponding note
        _leds.showColor(c, 0);                  // Show each color for 500 ms
        _display.clearDisplay();
        _display.setCursor(0, 0);
        _display.setTextSize(2);
        _display.println(colorToString(c));
        _display.display();

        delay(700); // Wait for 500 ms before showing the next color

        _leds.clearNow(); // Clear the LEDs after showing each color
        delay(100);       // Short delay before the next color
    }

    // After showing the sequence, transition to the PLAYING_USER_STATE
    fsm_handle::dispatch(Fsm::EventType::PLAYING_USER_EVENT);
}

void Game::onEnterPlayingUserState() {
    _leds.clearNow();
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_PRESS_THE));
    _display.println(FPSTR(STR_RIGHT_BUTTON));
    _display.display();

    button_timer = millis(); // Start the timer for button press duration
}

void Game::onLoopPlayingUserState() {
    unsigned long elapsed_time = (millis() - button_timer);

    if (elapsed_time > IN_SEQUENCE_TIMEOUT) {
        fsm_handle::dispatch(Fsm::EventType::PLAYING_LOSE_EVENT);
    }

    // if (elapsed_time > 5) {
    //     // If no button is pressed for 5 seconds, consider it a timeout
    //     _buzzer.playErrorSound();
    //     _leds.fill_all(color_t::ColorRed); // Fill LEDs with red color
    //     delay(500); // Show the error color for 500 ms

    //     _leds.clearNow();
    //     _display.clearDisplay();
    //     _display.setCursor(0, 0);
    //     _display.setTextSize(2);
    //     _display.println(F("Tempo scaduto!"));
    //     _display.display();
    //     delay(2000); // Show the message for 2 seconds
    // }
}

void Game::onEnterPlayingWinState() {
    delay(500);

    _leds.clearNow();

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_GREAT));
    _display.display();

    _buzzer.playRoundWinSound();

    _leds.rainbow(2, 1);
    _leds.clearNow();

    delay(500);

    _display.print(FPSTR(STR_ROUND));
    _display.println(sequence.size());
    _display.display();
    delay(500);

    fsm_handle::dispatch(Fsm::EventType::PLAYING_SEQUENCE_EVENT);
}

void Game::onEnterPlayingLoseState() {
    _leds.clearNow();

    _buzzer.playErrorSound();
    _leds.fill_all(color_t::ColorRed); // Fill LEDs with red color
    delay(ERROR_TONE_DURATION);

    _leds.clearNow();

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_YOU_LOST));
    _display.display();
    delay(2000);

    // Check if the current score is higher than the high score
    if (sequence.size() > _high_score) {
        _high_score = sequence.size();
        _preferences.putUInt("high_score", _high_score); // Save the new high score
        _display.clearDisplay();
        _display.setTextSize(2);
        _display.setCursor(0, 0);
        _display.println(FPSTR(STR_NEW));
        _display.println(FPSTR(STR_RECORD_EXCL));
        _display.println(_high_score);
        _display.display();

        // Epic synchronized celebration with sound, lights, and fireworks!
        synchronizedCelebration();
    }

    fsm_handle::dispatch(
        Fsm::EventType::INITIAL_STATE_EVENT); // Transition back to the initial state
}

void Game::testCelebrationEffects() {
    Serial.println(F("🎉 Testing celebration effects!"));

    // Display test message
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_TESTING));
    _display.println(FPSTR(STR_CELEBRATION));
    _display.println(FPSTR(STR_EFFECTS));
    _display.display();

    // Synchronized celebration - lights and sound together!
    synchronizedCelebration();

    Serial.println(F("✨ Celebration test complete!"));

    // Return to normal display after a moment
    delay(1000);
    _display.clearDisplay();
    _display.display();
}

void Game::synchronizedCelebration() {
    // Start the smooth Pacman melody in the background
    _buzzer.playNewHighScoreSound();

    // Create visual celebration with lights and display fireworks!
    const int celebrationSteps = 30;
    const int stepDuration     = 150; // 150ms per step = ~4.5 seconds total

    for (int step = 0; step < celebrationSteps; step++) {
        // Synchronized light effects
        switch (step % 6) {
        case 0:
        case 1:
            // Rainbow burst
            _leds.rainbow(1, 1);
            break;
        case 2:
            // Red flash
            _leds.fill_all(color_t::ColorRed);
            break;
        case 3:
            // Blue flash
            _leds.fill_all(color_t::ColorBlue);
            break;
        case 4:
            // Green flash
            _leds.fill_all(color_t::ColorGreen);
            break;
        case 5:
            // Yellow flash
            _leds.fill_all(color_t::ColorYellow);
            break;
        }

        // Synchronized display fireworks
        drawFireworks(step);

        delay(stepDuration);

        // Clear LEDs between some steps for sparkle effect
        if (step % 2 == 1) {
            _leds.clearNow();
            delay(30);
        }
    }

    // Final fireworks burst on display
    drawFinalFireworks();
    _leds.clearNow();
}

void Game::drawFireworks(int step) {
    _display.clearDisplay();

    // Draw multiple fireworks at different stages
    int firework1_stage = step % 8;
    int firework2_stage = (step + 4) % 8;

    // Firework 1 (center-left)
    drawSingleFirework(32, 32, firework1_stage);

    // Firework 2 (center-right)
    drawSingleFirework(96, 32, firework2_stage);

    // Add some random sparkles
    for (int i = 0; i < 8; i++) {
        int x = random(0, SCREEN_WIDTH);
        int y = random(0, SCREEN_HEIGHT);
        _display.drawPixel(x, y, SSD1306_WHITE);
    }

    // Add celebration text
    _display.setTextSize(1);
    _display.setCursor(40, 0);
    _display.print(FPSTR(STR_RECORD_SHORT));

    _display.display();
}

void Game::drawSingleFirework(int centerX, int centerY, int stage) {
    switch (stage) {
    case 0:
    case 1:
        // Launch phase - vertical line moving up
        for (int i = 0; i < stage * 10; i++) {
            _display.drawPixel(centerX, centerY + 20 - i, SSD1306_WHITE);
        }
        break;

    case 2:
    case 3: {
        // Small explosion
        int radius1 = (stage - 1) * 3;
        for (int angle = 0; angle < 360; angle += 45) {
            int x = centerX + (radius1 * cos(angle * PI / 180));
            int y = centerY + (radius1 * sin(angle * PI / 180));
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                _display.drawPixel(x, y, SSD1306_WHITE);
            }
        }
        break;
    }

    case 4:
    case 5:
    case 6: {
        // Large explosion with spokes
        int radius2 = (stage - 2) * 4;
        for (int angle = 0; angle < 360; angle += 30) {
            int x1 = centerX + (radius2 * cos(angle * PI / 180));
            int y1 = centerY + (radius2 * sin(angle * PI / 180));
            if (x1 >= 0 && x1 < SCREEN_WIDTH && y1 >= 0 && y1 < SCREEN_HEIGHT) {
                _display.drawLine(centerX, centerY, x1, y1, SSD1306_WHITE);
            }
        }
        break;
    }

    case 7: {
        // Fading sparkles
        for (int i = 0; i < 6; i++) {
            int x = centerX + random(-15, 15);
            int y = centerY + random(-15, 15);
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                _display.drawPixel(x, y, SSD1306_WHITE);
            }
        }
        break;
    }
    }
}

void Game::drawFinalFireworks() {
    _display.clearDisplay();

    // Multiple large fireworks across the screen
    for (int fw = 0; fw < 4; fw++) {
        int centerX = 20 + fw * 25;
        int centerY = 20 + random(-10, 20);

        // Large starburst
        for (int angle = 0; angle < 360; angle += 15) {
            int x = centerX + (15 * cos(angle * PI / 180));
            int y = centerY + (15 * sin(angle * PI / 180));
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                _display.drawLine(centerX, centerY, x, y, SSD1306_WHITE);
            }
        }
    }

    // Victory text
    _display.setTextSize(1);
    _display.setCursor(30, 50);
    _display.print(FPSTR(STR_NEW_RECORD));

    _display.display();
    delay(1000);
}

void Game::resetHighScore() {
    Serial.println(F("🔄 Resetting high score!"));

    // Reset the high score
    _high_score = 0;
    _preferences.putUInt("high_score", _high_score);

    // Show reset notification
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_RESET_RECORD));
    _display.println(FPSTR(STR_RECORD_RESET));
    _display.display();
    delay(1500);

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(FPSTR(STR_RECORD_CLEARED));
    _display.println(FPSTR(STR_CLEARED));
    _display.display();

    // Visual feedback with LEDs
    for (int i = 0; i < 3; i++) {
        _leds.fill_all(color_t::ColorRed);
        delay(200);
        _leds.clearNow();
        delay(200);
    }

    delay(2000);

    // Clear display and return to normal state
    _display.clearDisplay();
    _display.display();

    Serial.println(F("✅ High score reset complete!"));
}

Fsm::StateType Game::getCurrentState() {
    auto currentState = fsm_handle::currentState();
    return currentState.getType();
}

} // namespace simon
