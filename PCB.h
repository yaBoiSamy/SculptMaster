#ifndef PCB_H
#define PCB_H
#include <ConvenienceLib.h>
#include <Adafruit_MPU6050.h>


class PCB {
public:
  PCB();
  void init();
  void update();
  Vect get_acc();
  Vect get_rot();
private:
  Vect rotSpeed;
  Vect acceleration;
  const String labels[3] = {"X", "Y", "Z"};
  LCD lcd;
  Adafruit_MPU6050 mpu;
};

#endif