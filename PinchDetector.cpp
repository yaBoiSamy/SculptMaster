#include "ConvenienceLib.h"
#include "PinchDetector.h"

PinchDetector::PinchDetector(int index_finger_gpio, int middle_finger_gpio, int ring_finger_gpio, int pinky_finger_gpio) 
  : index(index_finger_gpio), middle(middle_finger_gpio), ring(ring_finger_gpio), pinky(pinky_finger_gpio) {}

bool PinchDetector::ReadPinch(PinchDetector::Finger finger) {
  switch (finger) {
    case PinchDetector::Finger::INDEX: return index.read();
    case PinchDetector::Finger::MIDDLE: return middle.read();
    case PinchDetector::Finger::RING: return ring.read();
    case PinchDetector::Finger::PINKY: return pinky.read();
    default: Serial.println("Invalid pinch finger query"); return false;
  }
}
