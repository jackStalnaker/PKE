#include <Arduino.h>
#include "armservo.h"

bool raising = true;

ArmServo arm_servo;

void setup() {
    arm_servo.setup();
}

void loop() {
    arm_servo.move();
    if (raising) {
        raising = arm_servo.raise();
    } else {
        raising = !arm_servo.lower();
    }
    delay(15);
}