
#include <Arduino.h>
#include "buzzer.h"
#include "tones.h"
#include "config.h"
#include "melodies/pacman.h"


namespace simon {

void Buzzer::_tone(uint16_t note, uint16_t duration)
{
    tone(_pin, note, duration);
} // _tone

void Buzzer::setup()
{
    pinMode(_pin, OUTPUT);
} // setup

void Buzzer::toneStart(simon::note_t note, unsigned long duration)
{
    _tone(note, duration);
} // toneStart

void Buzzer::stop()
{
    noTone(_pin);
} // stop

void Buzzer::playErrorSound()
{
    _tone(NOTE_A2, ERROR_TONE_DURATION);
} // error

void Buzzer::success()
{
    singleTone(NOTE_E5, SUCCESS_TONE_DURATION);
    singleTone(NOTE_G5, SUCCESS_TONE_DURATION);
    singleTone(NOTE_E6, SUCCESS_TONE_DURATION);
    singleTone(NOTE_D6, SUCCESS_TONE_DURATION);
    singleTone(NOTE_G6, SUCCESS_TONE_DURATION);
} // success

void Buzzer::singleTone(uint16_t note, uint16_t duration)
{
    toneStart(note);
    delay(duration);
    stop();
} // singleTone

void Buzzer::playRoundWinSound()
{
    uint16_t pause = 50;
    toneStart(NOTE_C5, pause);
    toneStart(NOTE_E5, pause);
    toneStart(NOTE_G5, pause);
    toneStart(NOTE_C5, pause);
    toneStart(NOTE_E5, pause);
    toneStart(NOTE_G5, pause);
    toneStart(NOTE_C6, pause);

    stop();
} // win

void Buzzer::playInitialSound()
{
    uint16_t pause = 100;

    toneStart(NOTE_C5, pause);
    toneStart(NOTE_E5, pause);
    toneStart(NOTE_G5, pause);
    toneStart(NOTE_C5, pause);
    toneStart(NOTE_E5, pause);
    toneStart(NOTE_G5, pause);
    toneStart(NOTE_C6, pause);
} // playInitialSound

void Buzzer::playNewHighScoreSound()
{
    play_melody(_pin); // Play the Pacman melody
} // playNewHighScoreSound

void Buzzer::playCountdownSound()
{
    toneStart(NOTE_C5, 50);
} // playCountdownSound

} // namespace buzzer