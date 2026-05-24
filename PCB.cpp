#include "PCB.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

PCB::PCB() {}

void PCB::init() {
  String labels[3] = {"X", "Y", "Z"};
  double* values[3] = {&rotSpeed.x, &rotSpeed.y, &rotSpeed.z};
  lcd = LCD(labels, values, 3);
  lcd.begin();

  if(!mpu.begin()) {
    Serial.print("MPU failed");
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void PCB::update() {
  // event variables initialization and assignment
  sensors_event_t a, g, temp; 
  mpu.getEvent(&a, &g, &temp);
  acceleration.x = a.acceleration.x; 
  acceleration.y = a.acceleration.y; 
  acceleration.z = a.acceleration.z;
  rotSpeed.x = g.gyro.x;
  rotSpeed.y = g.gyro.y;
  rotSpeed.z = g.gyro.z;
  lcd.update();
}

Vect PCB::get_acc() {
  return acceleration;
}

Vect PCB::get_rot() {
  return rotSpeed;
}