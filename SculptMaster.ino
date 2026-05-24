#include "PCB.h"
#include "BLEGlove.h"

static PCB pcb = PCB();
static BLEGlove bleGlove = BLEGlove();
#include "Arduino.h"

void setup() {
  Serial.begin(115200);
  bleGlove.setup();
  delay(500);
  pcb.init();
}

void loop() {
  pcb.update();
  bleGlove.update(pcb);
}
