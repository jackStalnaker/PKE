#include <Arduino.h>
#include "touchsensor.h"

TouchSensor touch_sensor;

 void setup() {
    Serial.begin(115200);
    delay(1000);
    touch_sensor.setup();
}

void loop() {
    unsigned int touch_state = touch_sensor.read();
    // if (touch_state == HIGH) {
    //     Serial.println("Touch DETECTED!");
    // }
    Serial.println(touch_state);
    delay(200);
}