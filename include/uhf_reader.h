
#pragma once

#include <Arduino.h>

class UHFReader {
public:
    UHFReader(HardwareSerial& serial)
        : serial_(serial) {}

    void begin(int rxPin, int txPin) {
        serial_.begin(115200, SERIAL_8N1, rxPin, txPin);
    }

    void update() {
        processRFID();

        if (millis() - lastInventory_ >= 1000) {
            lastInventory_ = millis();
            serial_.write(inventoryCommand_,
                          sizeof(inventoryCommand_));
        }
    }

private:
    HardwareSerial& serial_;

    static constexpr uint8_t inventoryCommand_[] = {
        0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E
    };

    uint8_t rxBuffer_[128] = {};
    size_t rxPos_ = 0;
    unsigned long lastInventory_ = 0;

    void processRFID() {
        while (serial_.available()) {
            uint8_t b = serial_.read();

            if (rxPos_ == 0 && b != 0xBB)
                continue;

            rxBuffer_[rxPos_++] = b;

            if (b == 0x7E) {
                processFrame();
                rxPos_ = 0;
                continue;
            }

            if (rxPos_ >= sizeof(rxBuffer_))
                rxPos_ = 0;
        }
    }

    void processFrame() {
        Serial.print("FRAME: ");

        for (size_t i = 0; i < rxPos_; ++i) {
            Serial.printf("%02X ", rxBuffer_[i]);
        }

        Serial.println();
    }
};
