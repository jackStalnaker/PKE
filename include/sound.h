#include <Arduino.h>
#include "tasktimer.h"

// SOUND =======================
class Sound {
public:
    static constexpr int BUZZER_PIN = D5;

    static constexpr int IDLE_TONE_FREQ = 75;
    static constexpr int MAX_TONE_FREQ = 875;
    static constexpr int TONE_FREQ_STEP = 150;
                
    static constexpr int TONE_IDLE_DELAY = 150;
    static constexpr int TONE_MIN_DELAY = 20;
    static constexpr int TONE_DELAY_STEP = 20;

    static constexpr float PHASOR_FREQ = 50.0f;
    static constexpr float PHASOR_DT = 1.0f / 360.0f;
    static constexpr int TONE_DURATION = 20;
    static constexpr int TOTAL_CYCLES = 250;
    static constexpr int NUM_TONE_PLAYS = 1;

    /*
    * Set up sound.
    */
    void setup() {
        setToneChannel(4);
        reset_level();
    }

    /*
    * Play the sounds.
    */
    bool play() {
        if (tone_timer_.expired(tone_delay_ + TONE_DURATION)) { 
            phased_tone_ = tone_freq_ + phasor_;
            tone(BUZZER_PIN, phased_tone_, TONE_DURATION);
            tone_timer_.reset();
            return true;
        }
        return false;
    }

    /*
     * Reset level.
     */
    void reset_level() {
        tone_freq_ = IDLE_TONE_FREQ;
        tone_delay_ = TONE_IDLE_DELAY;
        cycle_count_ = 0;
        phasor_ = 0;
    }

    /*
     * Raise sound level
     * Return true while still raising and false when at max.
     */
    bool raise() {
        update_phasor();

        // Maxed out so do nothing
        if ((tone_freq_ == MAX_TONE_FREQ) && (tone_delay_ == TONE_MIN_DELAY)) return false;
    
        if (change_timer_.expired(tone_delay_ + TONE_DURATION)) {
            change_timer_.reset();

            if (play_count_ < NUM_TONE_PLAYS) {
                play_count_++;
            } else {
                tone_freq_ = tone_freq_ <= (MAX_TONE_FREQ - TONE_FREQ_STEP) ? tone_freq_ + TONE_FREQ_STEP : MAX_TONE_FREQ;
                tone_delay_ = tone_delay_ >= (TONE_MIN_DELAY + TONE_DELAY_STEP) ? tone_delay_ - TONE_DELAY_STEP : TONE_MIN_DELAY;
            }
        }
        return true;
    }

    /*
     * Lower sound level
     * Return true while still lowering and false when at min.
     */
    bool lower() {
        update_phasor();

        // Maxed out so do nothing
        if ((tone_freq_ == IDLE_TONE_FREQ) && (tone_delay_ == TONE_IDLE_DELAY)) return false;

        if (change_timer_.expired(tone_delay_ + TONE_DURATION)) {
            change_timer_.reset();

            if (play_count_ < NUM_TONE_PLAYS) {
                play_count_++;
            } else {
                tone_freq_ = tone_freq_ >= (IDLE_TONE_FREQ + TONE_FREQ_STEP) ? tone_freq_ - TONE_FREQ_STEP : IDLE_TONE_FREQ;
                tone_delay_ = tone_delay_ <= (TONE_IDLE_DELAY - TONE_DELAY_STEP) ? tone_delay_ + TONE_DELAY_STEP : TONE_IDLE_DELAY;
            }
        }
        return true;
    }

    void update_phasor() {
        // Apply phasor effect
        phasor_ = int(0.1 * float(tone_freq_) * sin(float(cycle_count_) * 2.0 * PI * PHASOR_FREQ * PHASOR_DT));
        cycle_count_ = cycle_count_ < TOTAL_CYCLES ? cycle_count_ + 1 : 0;
    }

private:
    int tone_freq_;
    int tone_delay_;
    int phased_tone_;
    int play_count_ = 0;
    int cycle_count_ = 0;
    int phasor_;

    TaskTimer tone_timer_;
    TaskTimer change_timer_;
};