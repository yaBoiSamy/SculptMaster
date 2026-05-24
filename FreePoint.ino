#include "PCB.h"
#include "BLEGlove.h"

static PCB pcb = PCB();
static BLEGlove bleGlove = BLEGlove();

void setup() {
  Serial.begin(115200);
  bleGlove.setup();
  delay(500);
}

void loop() {
  pcb.update();
  Serial.println(pcb.get_acc().str());
  bleGlove.update(pcb);
}
