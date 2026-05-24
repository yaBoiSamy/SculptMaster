#ifndef PinchDetector_h
#define PinchDetector_h

class PinchDetector {
public:
  enum class Finger {
    INDEX,
    MIDDLE,
    RING,
    PINKY
  };
  PinchDetector(int index_finger_gpio, int middle_finger_gpio, int ring_finger_gpio, int pinky_finger_gpio);
  bool ReadPinch(Finger finger);
private:
  Button index;
  Button middle;
  Button ring;
  Button pinky;
};

#endif