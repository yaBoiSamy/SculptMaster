#include "PCB.h"
#include "BLEGlove.h"

static PCB pcb = PCB();
static BLEGlove bleGlove = BLEGlove();
#include "Arduino.h"

void setup() {
  Serial.begin(115200);
  delay(500);
  bleGlove.setup();
  pcb.init();
}

void loop() {
  pcb.update();
  //Serial.println(pcb.get_acc().str());
  bleGlove.update(pcb);
}
