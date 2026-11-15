/*
 * PKE Meter with large TFT display. A 2026 reimagining of what Egon might've built with current tools.
 * A followup to the 2025 version that used a tiny mono OLED display and a fare card reader that 
 * required the user to tap the "ghost" with the meter to detect it. 
 * 
 * This version has a full-color display and a UHF reader with several feet of sensing distance. 
 *
 *
 *  D0 - servo         5V
 *  D1 - TFT CS        GND
 *  D2 - TFT DC        3V3 
 *  D3 - Touch sensor  D10 - TFT MOSI (SDA)
 *  D4 - LED strip     D9  - NC (because it messes with boots)
 *  D5 - Buzzer        D8  - TFT SCLK (SCK)
 *  D6 - UHF RX        D7  - UHF TX
 *
 * Jack Stalnaker, 2026
*/
#include <Arduino.h>
#include "display.h"
#include "customcolors.h"
#include "tasktimer.h"
#include "splashpage.h"
#include "readypage.h"
#include "scannerpage.h"
#include "detectionpage.h"
#include "analysispage.h"
#include "ledstrip.h"
#include "touchsensor.h"
#include "armservo.h"
#include "sound.h"
#include "uhf_reader.h"
#include "ghost.h"
#include "ghosts/ghosts.h"

LGFX display;
LEDStrip led_strip;
TouchSensor touch_sensor;
ArmServo arm_servo;
Sound sound;
UHFReader uhf_reader;

SplashPage splash_page(display);
ReadyPage ready_page(display);
DetectionPage detection_page(display);
BasePage base_page(display);
AnalysisPage analysis_page(display);
ScannerPage scanner_page(display);

// Ghost detection status
enum class GhostDetectStatus {
  NEW,    // New ghost detected
  OLD,    // Same old ghost detected
  NONE    // No ghost detected
};

// Alert change direction
enum class AlertChangeDirection {
  RAISE,  // Raise alert level
  LOWER,  // Lower alert level
  NONE    // No change to alert level
};

// Scanning
constexpr unsigned long DETECT_TIMEOUT = 2500;  // Min time between ghost detections
const Ghost* ghost = nullptr;
const Ghost* last_ghost = nullptr;
GhostDetectStatus ghost_status = GhostDetectStatus::NONE;
bool is_scanning = false;
bool is_detecting = false;
bool is_alert_reset = true;
bool are_arms_reset = true;
bool is_meter_running = false;
int level_count = 0;
constexpr int NUM_PER_LEVEL = 1;

// Timers for time slicing
TaskTimer detect_timer;

// Function prototypes
void run_PKE_meter(void);
void idle_PKE_meter(void);
void detect_ghost(void);
const Ghost* find_ghost(const uint8_t* epc);
bool handle_ghost(const GhostDetectStatus ghost_status);
void start_detecting(void);
void stop_detecting(void);
void reset_detection(void);

void setup() {
  Serial.begin(115200);
  Serial.println("PKE 2.0");
  display.init();
  display.setRotation(2);
  display.fillScreen(TFT_BLACK);
  led_strip.setup();
  sound.setup();
  arm_servo.setup();
  uhf_reader.setup();
  touch_sensor.setup();
  
  splash_page.draw();
  ready_page.draw();

}

void loop() {
  // Serial.println(touch_sensor.read());
  if (touch_sensor.read()) { is_meter_running = !is_meter_running; }
  if (is_meter_running) { run_PKE_meter(); }
  else { idle_PKE_meter(); }
}

/*
 * Top-level function for running the PKE meter
 */
 void run_PKE_meter(void) {
  ready_page.reset();
  led_strip.play();
  sound.play();
  arm_servo.move();
  
  // Look for ghosts
  if (is_detecting) { detect_ghost(); }

  // Found a ghost, so handle it, and keep handling it until handling is done
  if (ghost_status != GhostDetectStatus::NONE) {
    scanner_page.reset();
    bool sound_done = !sound.raise();
    bool led_done = !led_strip.raise();
    bool arm_done = !arm_servo.raise();
    bool handle_done = !handle_ghost(ghost_status);

    if (sound_done && led_done && arm_done && handle_done) {
      start_detecting();
    } else {
      stop_detecting();
    }
  } else {
    // Do these things once without detecting, because detecting slows things down
    bool sound_done = !sound.lower();
    bool led_done = !led_strip.lower();
    bool arm_done = !arm_servo.lower();
    if (sound_done && led_done && arm_done) {
      start_detecting();
    } else {
      stop_detecting();
    }
    detection_page.reset();
    analysis_page.reset();
    scanner_page.draw();
  }
 }

/*
 * Idle the PKE meter.
 */
void idle_PKE_meter() {
  stop_detecting();
  reset_detection();
  ready_page.draw();
  scanner_page.reset();
  detection_page.reset();
  analysis_page.reset();

  bool sound_done = !sound.lower();
  bool led_done = !led_strip.lower();
  bool arm_done = !arm_servo.lower();

  if (sound_done && led_done && arm_done) led_strip.turn_off();
  if (!sound_done) sound.play();
  if (!led_done) led_strip.play(); 
  if (!arm_done) arm_servo.move();
}

/*
 * Set meter to detection mode. This is separate from scanning, because we don't always want the scanning animation to play
 * (e.g. when looking for a repeated detection).
 */
 void start_detecting(void) {
  if (!is_detecting) { 
    detect_timer.reset();
    is_detecting = true;
  }
 }

 /*
  * End detection mode.
  */
void stop_detecting(void) {
  if (is_detecting) { is_detecting = false; }
}

/*
 * Check RFID reader for a ghost name.
 */
void detect_ghost(void) {
  // Check if UHF reader has a valid detection
  // Get EPC from detection (ignore RSSI for now)

  // Serial.println("in detect_ghost");
  uint8_t epc[UHFReader::EPC_LENGTH];

  uhf_reader.update();

  if (uhf_reader.get_epc(epc)) {
      const Ghost* ghost_candidate = find_ghost(epc);

      if (ghost_candidate) {
        if (ghost_candidate == last_ghost) {
          // Same ghost: keep it alive and mark subsequent reads OLD
          detect_timer.reset();
          ghost_status = GhostDetectStatus::OLD;
        } else if (last_ghost == nullptr) {
          ghost = ghost_candidate;
          last_ghost = ghost_candidate;
          ghost_status = GhostDetectStatus::NEW;
          detect_timer.reset();
        }
        // Ignore different ghosts while one is active
    }
  }

  // Only clear detection after a period without a valid EPC
  if (last_ghost && detect_timer.expired(DETECT_TIMEOUT)) {
    ghost_status = GhostDetectStatus::NONE;
    last_ghost = nullptr;
    ghost = nullptr;
  }
}

/*
 * Reset detection
 */
void reset_detection() {
    ghost_status = GhostDetectStatus::NONE;
    ghost = nullptr;
    last_ghost = nullptr;
    detect_timer.reset();
}

/*
 * Handle a detected ghost
 * @returns True if still running
 */
bool handle_ghost(const GhostDetectStatus ghost_status) {
  if (!detection_page.is_done()) {
    if (detection_page.draw(ghost, 1500)) return true;
  }
  if (!analysis_page.is_done()) {
    if (analysis_page.draw(ghost, 3000)) return true;
  }
  return false;
}

/*
 * Find the ghost associated with a UHF tag EPC
 */
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