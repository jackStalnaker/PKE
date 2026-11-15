#include <Arduino.h>
#include "ghost.h"
#include "ghosts/bigpumpkin.h"
#include "uhf_reader.h"

const Ghost* ghosts[] = {
  &bigpumpkin
};

UHFReader uhf_reader;
uint8_t epc[UHFReader::EPC_LENGTH];

const Ghost* find_ghost(const uint8_t* epc) {
    for (const Ghost* ghost : ghosts) {
        for (size_t j = 0; j < ghost->epc_count; j++) {
            if (memcmp(epc, ghost->tag_epcs[j], EPC_LENGTH) == 0) {
                return ghost;
            }
        }
    }

    return nullptr;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  uhf_reader.setup();
}

void loop() {
    uhf_reader.update();
    if (uhf_reader.get_epc(epc)) {
        for (size_t i = 0; i < uhf_reader.EPC_LENGTH; ++i) {
            Serial.printf("0x%02X, ", epc[i]);
        }
        Serial.println();

        const Ghost* ghost = find_ghost(epc);
        if (ghost) {
            Serial.printf("Ghost: %s\n", ghost->name);
        }
    }
}