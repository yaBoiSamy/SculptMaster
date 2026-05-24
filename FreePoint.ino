#include "PCB.h"

static PCB pcb = PCB();

void setup() {
  Serial.begin(115200);
  delay(500);
}

void loop() {
  pcb.update();
  Serial.println(pcb.get_acc().str());
}
