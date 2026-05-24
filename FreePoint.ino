#include "PCB.h"
#include "Arduino.h"

static PCB pcb;

void setup() {
  Serial.begin(115200);
  delay(500);
  pcb.init();
}

void loop() {
  pcb.update();
  Serial.println(pcb.get_rot().str());
}
