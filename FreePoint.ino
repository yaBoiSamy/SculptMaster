#include "PCB.h"
#include "BLEGlove.h"
#include "PinchDetector.h"
#include "Arduino.h"

static const int INDEX_GPIO = 19;
static const int MIDDLE_GPIO = 18;
static const int RING_GPIO = 5;
static const int PINKY_GPIO = 17;

static PinchDetector pinch_detector = PinchDetector(INDEX_GPIO, MIDDLE_GPIO, RING_GPIO, PINKY_GPIO);
// static PCB pcb = PCB();
// static BLEGlove bleGlove = BLEGlove();

void setup() {
  Serial.begin(115200);
  delay(500);
  // bleGlove.setup();
  // pcb.init();
}

void loop() {
  Serial.println(pinch_detector.ReadPinch(PinchDetector::Finger::INDEX));
  // pcb.update();
  //Serial.println(pcb.get_acc().str());
  // bleGlove.update(pcb);
}
