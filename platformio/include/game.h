#ifndef __SIMON_GAME_H__
#define __SIMON_GAME_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include "leds.h"
#include "fsm.h"
#include "buttons.h"
#include "buzzer.h"
#include "board.h"

namespace simon {

class Game {
private:
    Leds _leds; // Reference to the LED controller
    Buttons _buttons; // Reference to the button controller
    Buzzer _buzzer; // Reference to the buzzer controller
    Adafruit_SSD1306 _display; // Reference to the OLED display controller
    Board _board; // Reference to the board controller

    uint32_t _high_score = 0; // High score

    void onStateEnter(Fsm::StateType const& type);

    void onStateLoop(Fsm::StateType const& type);

    void onStateExit(Fsm::StateType const& type);

    void onButtonPressed(Button& btn);

    void onButtonReleased(Button& btn);

    void displayWelcomeMessage();

    void onEnterInitialState();
    
    void onEnterGameStartState();

    void onEnterPlayingSequenceState();

    void onEnterPlayingUserState();

    void onEnterPlayingWinState();

    void onEnterPlayingLoseState();

    void onLoopInitialState();

    void onLoopPlayingUserState();

    void synchronizedCelebration();
    void drawFireworks(int step);
    void drawSingleFirework(int centerX, int centerY, int stage);
    void drawFinalFireworks();

public:
    Game();

    ~Game() = default;

    bool setup(); // Setup the game
    void loop(); // Main game loop
    void testCelebrationEffects(); // Test celebration effects (for debugging)
    void resetHighScore(); // Reset high score with visual notification
    Fsm::StateType getCurrentState(); // Get current FSM state
};

} // namespace simon

#endif // __SIMON_GAME_H__