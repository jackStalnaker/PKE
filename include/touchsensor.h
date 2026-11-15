#include <Arduino.h>
#include "tasktimer.h"

// Touch sensor ===============
class TouchSensor {
public:
    static constexpr unsigned int TOUCH_PIN = D3;
    static constexpr unsigned int TOUCH_READ_INTERVAL = 50;

    /*
    * Prepare touch sensor for reading
    */
    void setup() {
        pinMode(TOUCH_PIN, INPUT);
        last_touch_state_ = digitalRead(TOUCH_PIN);
        touch_timer_.reset();
    }

    /*
    * Read the touch sensor, debouncing a little.
    */
    bool read() {
        if (!touch_timer_.expired(TOUCH_READ_INTERVAL)) return false;
        touch_timer_.reset();

        bool touch_state = digitalRead(TOUCH_PIN);
        if (touch_state == last_touch_state_) { return false; }
        else {
            last_touch_state_ = touch_state; 
            return touch_state;
        }
    }

private:
    unsigned int last_touch_state_ = LOW;
    TaskTimer touch_timer_;
};