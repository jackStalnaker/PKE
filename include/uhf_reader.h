#pragma once

#include <Arduino.h>

class UHFReader {
public:
    static constexpr int SERIALID = 1;
    static constexpr int RXPIN = D6;
    static constexpr int TXPIN = D7;
    static constexpr size_t EPC_LENGTH = 12; 
    static constexpr unsigned long RX_TIMEOUT = 100;

    UHFReader()
        : serial_(UHFReader::SERIALID) {}

    void setup() {
        serial_.begin(115200, SERIAL_8N1, UHFReader::RXPIN, UHFReader::TXPIN);
    }

    void update() {
        process_RFID();

        if (millis() - lastInventory_ >= 1000) {
            lastInventory_ = millis();
            serial_.write(inventoryCommand_, sizeof(inventoryCommand_));
        }
    }

    bool get_epc(uint8_t* epc) {
        if (!epc_available_)
            return false;

        memcpy(epc, pending_epc_, EPC_LENGTH);
        epc_available_ = false;

        return true;
    }

private:
    HardwareSerial serial_;

    static constexpr uint8_t inventoryCommand_[] = {
        0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E
    };

    uint8_t rx_buffer_[128] = {};
    size_t rx_pos_ = 0;
    unsigned long lastInventory_ = 0;
    uint8_t pending_epc_[UHFReader::EPC_LENGTH] = {};
    bool epc_available_ = false;
    unsigned long last_rx_byte_ = 0;

    void process_RFID() {
        // Discard an incomplete frame if reception has stalled
        if (rx_pos_ > 0 && millis() - last_rx_byte_ >= RX_TIMEOUT) {
            rx_pos_ = 0;
        }

        while (serial_.available()) {
            uint8_t b = serial_.read();
            last_rx_byte_ = millis();

            // Ignore everything until a BB start byte
            if (rx_pos_ == 0 && b != 0xBB)
                continue;

            rx_buffer_[rx_pos_++] = b;
    
            if (rx_pos_ >= 5) {
                uint16_t payload_len = (uint16_t(rx_buffer_[3]) << 8) | rx_buffer_[4];
                size_t expected_len = payload_len + 7;

                if (expected_len > sizeof(rx_buffer_) || expected_len < 7) {
                    // Malformed frame received
                    rx_pos_ = 0;
                    continue;
                }

                if (rx_pos_ == expected_len) {
                    // Complete frame received.
                    process_frame();
                    rx_pos_ = 0;
                }
            }

            // Safety reset on overflow
            if (rx_pos_ >= sizeof(rx_buffer_)) {
                rx_pos_ = 0;
            }
        }
    }
    void process_frame() {
        // Validate frame length and type
        if (rx_pos_ != 24 ||
            rx_buffer_[0] != 0xBB ||
            rx_buffer_[1] != 0x02 ||
            rx_buffer_[2] != 0x22 ||
            rx_buffer_[rx_pos_ - 1] != 0x7E)
            return;

        // Verify checksum
        uint8_t checksum = 0;

        for (size_t i = 1; i < rx_pos_ - 2; ++i)
            checksum += rx_buffer_[i];

        if (checksum != rx_buffer_[rx_pos_ - 2])
            return;

        // Extract the 12-byte EPC
        memcpy(pending_epc_, &rx_buffer_[8], UHFReader::EPC_LENGTH);
        epc_available_ = true;
    }
};
