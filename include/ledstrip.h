#include <Arduino.h>
#include <FastLED.h>
#include "tasktimer.h"

// LIGHTS =====================
class LEDStrip {
public:
    static constexpr unsigned int LED_PIN = D4;
    static constexpr int NUM_LEDS = 7;
    static constexpr int MAX_LED_BRIGHTNESS = 135;
    static constexpr int MIN_LED_BRIGHTNESS = 5;
    static constexpr int LED_BRIGHTNESS_STEP = 15;
    static constexpr int NUM_LED_PLAYS = 1;
    static constexpr int LED_DURATION = 20;
 
    static constexpr int LED_IDLE_DELAY = 150;
    static constexpr int LED_MIN_DELAY = 20;
    static constexpr int LED_DELAY_STEP = 20;
    /*
    * Set up lights
    */
    void setup() {
        FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds_, NUM_LEDS);
        last_led_idx_ = NUM_LEDS - 1;
        //FastLED.setBrightness(MIN_LED_BRIGHTNESS);
        reset_level();
    }

    /*
    * Turn off all the lights.
    */
    void turn_off() {
        for (int i=0; i<NUM_LEDS; ++i) {
            leds_[i] = CRGB::Black;
        }
        FastLED.show();
    }

    /*
    * Play the lights
    */
    bool play() {
    if (led_timer_.expired(led_delay_ + LED_DURATION)) {
        led_timer_.reset();
        led_idx_ = (last_led_idx_ + 1) % NUM_LEDS;
        //FastLED.setBrightness(led_brightness);
        leds_[last_led_idx_] = CRGB::Black;
        leds_[led_idx_] = CRGB(led_brightness_, MIN_LED_BRIGHTNESS, MIN_LED_BRIGHTNESS);
        FastLED.show(); 
        last_led_idx_ = led_idx_;
        return true;
    }
    return false;
    }

    /*
     * Reset level.
     */
    void reset_level() {
        led_delay_ = LED_IDLE_DELAY;
        led_brightness_ = MIN_LED_BRIGHTNESS;
    }

    /*
     * Raise the LED cycle rate and brightness.
     * Return true while still raising and false when at max.
     */
    bool raise() {
        // Maxed out so do nothing
        if ((led_delay_ == LED_MIN_DELAY) && (led_brightness_ == MAX_LED_BRIGHTNESS)) return false;

        if (change_timer_.expired(led_delay_ + LED_DURATION)) {
            change_timer_.reset();

            if (play_count_ < NUM_LED_PLAYS) {
                play_count_++;
            } else {
                led_brightness_ = led_brightness_ <= (MAX_LED_BRIGHTNESS - LED_BRIGHTNESS_STEP) ? led_brightness_ + LED_BRIGHTNESS_STEP : MAX_LED_BRIGHTNESS;
                led_delay_ = led_delay_ >= (LED_MIN_DELAY + LED_DELAY_STEP) ? led_delay_ - LED_DELAY_STEP : LED_MIN_DELAY;
            }
        }
        return true;
    }

    /*
     * Lower the LED cycle rate and brightness.
     * Return true while still lowering and false when at min.
     */
    bool lower() {
        // Maxed out so do nothing
        if ((led_delay_ == LED_IDLE_DELAY) && (led_brightness_ == MIN_LED_BRIGHTNESS)) return false;

        if (change_timer_.expired(led_delay_ + LED_DURATION)) {
            change_timer_.reset();

            if (play_count_ < NUM_LED_PLAYS) {
                play_count_++;
            } else {
                led_brightness_ = led_brightness_ >= (MIN_LED_BRIGHTNESS + LED_BRIGHTNESS_STEP) ? led_brightness_ - LED_BRIGHTNESS_STEP : MIN_LED_BRIGHTNESS;
                led_delay_ = led_delay_ <= (LED_IDLE_DELAY - LED_DELAY_STEP) ? led_delay_ + LED_DELAY_STEP : LED_IDLE_DELAY;
            }
        }
        return true;
    }

private:
    int led_time_;
    int total_led_time_;
    int last_led_time_;
    int led_idx_ = 0;
    int last_led_idx_;
    int led_brightness_ = 5;
    int play_count_ = 0;
    int led_delay_;

    CRGB leds_[NUM_LEDS];
    TaskTimer led_timer_;
    TaskTimer change_timer_;
};