#include <Arduino.h>
#include <ESP32Servo.h>
#include "tasktimer.h"

// Servo ======================
class ArmServo {
public:
    static constexpr int ARM_SERVO_PIN = D0;
    static constexpr int MIN_ARM_SERVO_ANGLE = 0;
    static constexpr int MAX_ARM_SERVO_ANGLE = 65;
    static constexpr int ARM_SERVO_ANGLE_STEP = 1;
    static constexpr unsigned int ARM_CHANGE_DELAY = 30;

    /*
    * Set up arm servo
    */
    void setup() {
        arm_servo_.attach(ARM_SERVO_PIN);
        arm_servo_.write(MIN_ARM_SERVO_ANGLE);
        reset();
    }

    /*
     * Reset to min.
     */
    void reset() {
        arm_servo_angle_ = MIN_ARM_SERVO_ANGLE;
        last_arm_servo_angle_ = MIN_ARM_SERVO_ANGLE;
    }

    /*
    * Move the arm servo.
    */
    void move() {
        if (arm_servo_angle_ != last_arm_servo_angle_) {
            arm_servo_.write(arm_servo_angle_);
            last_arm_servo_angle_ = arm_servo_angle_;
        }
    }

    /*
     * Raise arm servo.
     * Return true while still raising and false when at max.
     */
    bool raise() {
        if (arm_servo_angle_ == MAX_ARM_SERVO_ANGLE) return false;

        if (change_timer_.expired(ARM_CHANGE_DELAY)) {
            change_timer_.reset();
            arm_servo_angle_ = arm_servo_angle_ <= (MAX_ARM_SERVO_ANGLE - ARM_SERVO_ANGLE_STEP) ? arm_servo_angle_ + ARM_SERVO_ANGLE_STEP : MAX_ARM_SERVO_ANGLE;
        }
        return true;
    }

    /*
     * Lower arm servo.
     * Return true while still lowering and false when at min.
     */
    bool lower() {
        if (arm_servo_angle_ == MIN_ARM_SERVO_ANGLE) return false;

        if (change_timer_.expired(ARM_CHANGE_DELAY)) {
            change_timer_.reset();
            arm_servo_angle_ = arm_servo_angle_ >= (MIN_ARM_SERVO_ANGLE + ARM_SERVO_ANGLE_STEP) ? arm_servo_angle_ - ARM_SERVO_ANGLE_STEP : MIN_ARM_SERVO_ANGLE;
        }
        return true;
    }

private:
    Servo arm_servo_;
    int last_arm_servo_angle_ = 0;
    int arm_servo_angle_ = 0;
    TaskTimer change_timer_;
};