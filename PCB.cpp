#include "PCB.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

PCB::PCB() {}

void PCB::init() {
  String labels[3] = {"X", "Y", "Z"};
  double* values[3] = {&rotSpeed.x, &rotSpeed.y, &rotSpeed.z};

  if(!mpu.begin()) {
    Serial.print("MPU failed");
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void PCB::update() {
    sensors_event_t a, g, temp; 
    mpu.getEvent(&a, &g, &temp);
    
    rotSpeed = Vect(g.gyro.x, g.gyro.y, g.gyro.z);
    acceleration = Vect(a.acceleration.x, a.acceleration.y, a.acceleration.z);
}

Vect PCB::get_acc() {
  return acceleration;
}

Vect PCB::get_rot() {
  return rotSpeed;
}