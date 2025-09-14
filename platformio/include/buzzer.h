
#ifndef __SIMON_BUZZER_H__
#define __SIMON_BUZZER_H__

#include "types.h"
#include <Arduino.h>

namespace simon {
class Buzzer {
  private:
    int8_t _pin;

    /**
     * @brief Plays a tone on the buzzer.
     * @param note The frequency of the note to play.
     * @param duration The duration of the note in milliseconds.
     * This function uses the Arduino tone() function to generate a sound on the buzzer.
     */
    void _tone(uint16_t note, uint16_t duration);

  public:
    Buzzer(int8_t pin) : _pin(pin) {}

    ~Buzzer() {
        // Destructor
    }

    /**
     * @brief Initializes the buzzer.
     * This function sets the pin mode for the buzzer to OUTPUT.
     * It should be called once in the setup phase of the program.
     */
    void setup();

    /**
     * @brief Starts playing a tone on the buzzer.
     * @param note The frequency of the note to play.
     * @param duration The duration of the note in milliseconds (default is 0, meaning
     * the note will play indefinitely until stopped).
     */
    void toneStart(simon::note_t note, unsigned long duration = 0);

    void singleTone(uint16_t note, uint16_t duration);

    void stop();

    void playInitialSound();

    void playCountdownSound();

    // error sound
    void playErrorSound();

    // round success sound
    void success();

    // record sound
    void playRoundWinSound();

    void playNewHighScoreSound();
};

} // namespace simon

#endif // __SIMON_BUZZER_H__