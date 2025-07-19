#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Preferences.h>

#include "config.h"
#include "game.h"
#include "fsm.h"
#include "tones.h"
#include <vector>

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

Game::Game()
    : _leds(simon::Leds(strip)) // Initialize the LED controller with the NeoPixel strip
    , _buttons(BUTTONS_PIN) // Initialize the button controller with the defined pin
    , _buzzer(BUZZER_PIN) // Initialize the buzzer controller with the defined pin
    , _display(Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)) // Initialize the display controller
    , _board(Board()) // Initialize the board controller
{
    // Constructor implementation
}

void Game::displayWelcomeMessage()
{
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0); // Start at top-left corner
    _display.setTextColor(SSD1306_WHITE);
    _display.display();

    // draw the text centered horizontally on the display

    int16_t x1, y1;
    uint16_t w, h;

    _display.getTextBounds(F("Simon"), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 0); // Center text
    _display.println(F("Simon"));

    _display.getTextBounds(F("&"), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 16); // Center text
    _display.println(F("&"));

    _display.getTextBounds(F("Simon"), 0, 0, &x1, &y1, &w, &h);
    _display.setCursor((SCREEN_WIDTH - w) / 2, 32); // Center text
    _display.println(F("Simon"));
    _display.display();
}

bool Game::setup()
{
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
        return false;
    }

    Serial.println(F("done."));

    _display.clearDisplay();
    _display.setTextSize(1); // Set text size to 1
    _display.setTextColor(SSD1306_WHITE); // Set text color to white
    _display.setCursor(0, 0); // Set cursor to top-left corner
    _display.display();
    delay(100);

    _display.println(F("Init Preferences.."));
    _display.display();
    if (!_preferences.begin("simon", false)) {
        _display.println(F("error!"));
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

    _leds.wipe(strip.Color(255, 0, 0), simon::WipeFromStart, 50, 0, 6); // Red
    _leds.wipe(strip.Color(0, 255, 0), simon::WipeFromStart, 50, 6, 6); // Green
    _leds.wipe(strip.Color(0, 0, 255), simon::WipeFromStart, 50, 12, 6); // Blue
    _leds.wipe(strip.Color(255, 255, 0), simon::WipeFromStart, 50, 18, 6); // Yellow

    delay(1000); // Show the welcome message for 2 seconds

    _leds.clearNow();
    _display.clearDisplay();
    _display.display();

    // Adding callbacks for button events
    _buttons.setPressedCallback([this](Button& btn) {
        this->onButtonPressed(btn);
    });
    _buttons.setReleasedCallback([this](Button& btn) {
        this->onButtonReleased(btn);
    });

    // Set the initial state of the FSM
    simon::Fsm::setEnterCallback([this](Fsm::StateType const& type) {
        this->onStateEnter(type);
    });
    simon::Fsm::setExitCallback([this](Fsm::StateType const& type) {
        this->onStateExit(type);
    });

    fsm_handle::reset();
    fsm_handle::start();

    return true;
}

void Game::onStateEnter(Fsm::StateType const& type)
{
    Serial.print(F("==> Entering State: '"));
    Serial.print(Fsm::stateTypeToString(type));
    Serial.println(F("'"));

    state_start_time = millis(); // Record the time when the state is entered

    switch (type) {
    case Fsm::StateType::INITIAL_STATE:
        onEnterInitialState();
        break;

    case Fsm::StateType::GAME_START_STATE:
        onEnterGameStartState();
        break;

    case Fsm::StateType::PLAYING_SEQUENCE_STATE:
        onEnterPlayingSequenceState();
        break;

    case Fsm::StateType::PLAYING_USER_STATE:
        onEnterPlayingUserState();
        break;

    case Fsm::StateType::PLAYING_WIN_STATE:
        onEnterPlayingWinState();
        break;

    case Fsm::StateType::PLAYING_LOSE_STATE:
        onEnterPlayingLoseState();
        break;

    default:
        Serial.println(F("Unknown state entered."));
        break;
    }
}

void Game::onStateExit(Fsm::StateType const& type)
{
    Serial.print(F("<== Exiting State: '"));
    Serial.print(Fsm::stateTypeToString(type));
    Serial.println(F("'"));
}

void Game::onButtonPressed(Button& btn)
{
    auto currentState = fsm_handle::currentState();
    
    Serial.print(Fsm::stateTypeToString(currentState.getType()));
    Serial.print(F(" | Button pressed: "));
    Serial.println(btn.getName());

    // just play the sound and display the color of the pressed button
    simon::color_t pressedColor = btn.getType();

    if( currentState.getType() == Fsm::StateType::PLAYING_USER_STATE ) {
        // If not in PLAYING_USER_STATE, just play the sound and show the color
        _buzzer.toneStart(colorToNote(pressedColor), 0); // Play the corresponding note
        _leds.showColor(pressedColor, 0); // Show the color of the pressed button
        return;
    } else if( currentState.getType() == Fsm::StateType::INITIAL_STATE) {
        // If in INITIAL_STATE, just play the sound and show the color
        _buzzer.toneStart(colorToNote(pressedColor), 0); // Play the corresponding note
        _leds.showColor(pressedColor, 0); // Show the color of the pressed button
        return;
    }

    // simon::color_t pressedColor = btn.getType();
    // Serial.print(F("Pressed color: "));
    // Serial.println(colorToString(pressedColor));

    // _buzzer.toneStart(colorToNote(pressedColor), 0); // Play the corresponding note
    // _leds.showColor(pressedColor, 0); // Show the color of the pressed button
}

void Game::onButtonReleased(Button& btn)
{
    auto currentState = fsm_handle::currentState();
    Serial.print(Fsm::stateTypeToString(currentState.getType()));
    Serial.print(F(" | Button released: "));
    Serial.println(btn.getName());

    simon::color_t releasedColor = btn.getType();


    // If we're in the INITIAL state, transition to the GAME_START state
    if (currentState.getType() == Fsm::StateType::INITIAL_STATE) {
        _buzzer.stop(); // Stop the buzzer sound
        _leds.clearNow(); // Clear the LEDs
        delay(500);
        fsm_handle::dispatch(Fsm::EventType::GAME_START_EVENT);

    } else if (currentState.getType() == Fsm::StateType::PLAYING_USER_STATE) {
        button_timer = millis(); // Reset the button timer when a button is pressed
        _buzzer.stop(); // Stop the buzzer sound when the button is released
        _leds.clearNow(); // Clear the LEDs when the button is released

        Serial.print(F("Button released: "));
        Serial.println(btn.getName());

        Serial.print(F("Button index: "));
        Serial.println(button_index);

        Serial.print(F("Sequence size: "));
        Serial.println(sequence.size());

        // Check if the correct button was released
        if (releasedColor == sequence[button_index]) {
            if(button_index == sequence.size() - 1) {
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

void Game::loop()
{
    _buttons.loop();

    auto currentState = fsm_handle::currentState();
    onStateLoop(currentState.getType());
}

void Game::onStateLoop(Fsm::StateType const& type)
{
    switch (type) {
    case Fsm::StateType::INITIAL_STATE:
        onLoopInitialState();
        break;

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

    default:
        break;
    }
}

void Game::onLoopInitialState()
{
    unsigned long elapsedTime = (millis() - state_start_time) / 1000;
    int switchTime = 5;

    // switch text every 3 seconds
    if (elapsedTime % switchTime == 0) {
        if (elapsedTime % (switchTime * 2) == 0) {
            _display.clearDisplay();
            _display.setCursor(0, 0);
            _display.setTextSize(2);
            _display.println();

            _display.println(F("Premi un"));
            _display.println(F("tasto per"));
            _display.println(F("iniziare!"));
        } else {
            _display.clearDisplay();
            _display.setCursor(0, 0);
            _display.setTextSize(2);
            _display.println(F("Record"));
            _display.println(F("attuale:"));
            _display.println(_high_score);
        }
        _display.display();
    }
}

void Game::onEnterInitialState()
{
    sequence.clear();
    button_index = 0;
}

void Game::onEnterGameStartState()
{
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.setTextColor(SSD1306_WHITE);
    _display.display();
    _buzzer.playCountdownSound();
    delay(1000); // Short delay before starting the game

    _buzzer.playCountdownSound();
    _display.println(F("Pronti"));
    _display.display();
    delay(1000); // Show the message for 1 second

    _buzzer.playCountdownSound();
    _display.println(F("Partenza"));
    _display.display();
    delay(1000); // Show the message for 1 second

    _buzzer.toneStart(NOTE_C6, 500);
    _display.println(F("Via!"));
    _display.display();
    delay(1000); // Show the message for 1 second

    // Reset the game state
    button_index = 0; // Reset the button index
    sequence.clear(); // Clear the sequence

    // Transition to the PLAYING state
    fsm_handle::dispatch(Fsm::EventType::PLAYING_SEQUENCE_EVENT);
}

void Game::onEnterPlayingSequenceState()
{
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
        _leds.showColor(c, 0); // Show each color for 500 ms
        _display.clearDisplay();
        _display.setCursor(0, 0);
        _display.setTextSize(2);
        _display.println(colorToString(c));
        _display.display();

        delay(700); // Wait for 500 ms before showing the next color

        _leds.clearNow(); // Clear the LEDs after showing each color
        delay(100); // Short delay before the next color
    }

    // After showing the sequence, transition to the PLAYING_USER_STATE
    fsm_handle::dispatch(Fsm::EventType::PLAYING_USER_EVENT);
}

void Game::onEnterPlayingUserState()
{
    _leds.clearNow();
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(F("Premi il"));
    _display.println(F("tasto giusto!"));
    _display.display();

    button_timer = millis(); // Start the timer for button press duration
}

void Game::onLoopPlayingUserState()
{
    unsigned long elapsed_time = (millis() - button_timer);

    if(elapsed_time > IN_SEQUENCE_TIMEOUT) {
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

void Game::onEnterPlayingWinState()
{
    delay(500);

    _leds.clearNow();

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(F("Bravo!"));
    _display.display();
    
    _buzzer.playRoundWinSound();

    _leds.rainbow(2, 1);
    _leds.clearNow();

    delay(1000);

    _display.print(F("Round: "));
    _display.println(sequence.size() + 1);
    _display.display();
    delay(1000);

    fsm_handle::dispatch(Fsm::EventType::PLAYING_SEQUENCE_EVENT);

}

void Game::onEnterPlayingLoseState()
{
    _leds.clearNow();

    _buzzer.playErrorSound();
    _leds.fill_all(color_t::ColorRed); // Fill LEDs with red color
    delay(ERROR_TONE_DURATION);

    _leds.clearNow();
    

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setCursor(0, 0);
    _display.println(F("Hai perso!"));
    _display.display();
    delay(2000);

    // Check if the current score is higher than the high score
    if (sequence.size() + 1 > _high_score) {
        _high_score = sequence.size() + 1;
        _preferences.putUInt("high_score", _high_score); // Save the new high score
        _display.clearDisplay();
        _display.setTextSize(2);
        _display.setCursor(0, 0);
        _display.println(F("Nuovo"));
        _display.println(F("Record!"));
        _display.println(_high_score);
        _display.display();

        _buzzer.playNewHighScoreSound();
        delay(4000);
    }

    fsm_handle::dispatch(Fsm::EventType::INITIAL_STATE_EVENT); // Transition back to the initial state
}


} // namespace simon
