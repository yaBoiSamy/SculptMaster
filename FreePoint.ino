#include "PCB.h"
<<<<<<< HEAD
#include "BLEGlove.h"

static PCB pcb = PCB();
static BLEGlove bleGlove = BLEGlove();
=======
#include "Arduino.h"

static PCB pcb;
>>>>>>> 450c90bca6a2f8af3ad0661b4d9616f06187245e

void setup() {
  Serial.begin(115200);
  bleGlove.setup();
  delay(500);
  pcb.init();
}

void loop() {
  pcb.update();
<<<<<<< HEAD
  Serial.println(pcb.get_acc().str());
  bleGlove.update(pcb);
=======
  Serial.println(pcb.get_rot().str());
>>>>>>> 450c90bca6a2f8af3ad0661b4d9616f06187245e
}
