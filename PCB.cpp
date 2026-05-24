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
    sensors_event_t a, g, temp; 
    mpu.getEvent(&a, &g, &temp);
    
    // 1. Appliquer le lissage + Power Curve sur la rotation (Vitesse)
    rotSpeed.x = computeSmoothing(g.gyro.x, smoothRot.x, POWER_EXPONENT);
    rotSpeed.y = computeSmoothing(g.gyro.y, smoothRot.y, POWER_EXPONENT);
    rotSpeed.z = computeSmoothing(g.gyro.z, smoothRot.z, POWER_EXPONENT);
    
    // Sauvegarde pour la prochaine frame
    smoothRot = rotSpeed;

    // Pas de power curve sur l'accélération pure, juste du lissage (exposant de 1.0)
    acceleration.x = computeSmoothing(a.acceleration.x, smoothAcc.x, 1.0);
    acceleration.y = computeSmoothing(a.acceleration.y, smoothAcc.y, 1.0);
    acceleration.z = computeSmoothing(a.acceleration.z, smoothAcc.z, 1.0);
    
    // Sauvegarde pour la prochaine frame
    smoothAcc = acceleration;
    
    lcd.update();
}

float PCB::applyPowerCurve(float inputVal) {
    float sign = (inputVal > 0) ? 1.0 : -1.0;
    float absoluteVal = abs(inputVal);
    float outputVal = sign * pow(absoluteVal, POWER_EXPONENT);
    
    return outputVal;
}

float PCB::computeSmoothing(float currentRaw, float previousSmoothed, float exponent) {
    float smoothed = (currentRaw * SMOOTHING_FACTOR) + (previousSmoothed * (1 - SMOOTHING_FACTOR));
    float sign = (smoothed > 0) ? 1.0 : -1.0;
    float finalOutput = sign * pow(abs(smoothed), exponent);
    return finalOutput;
}

Vect PCB::get_acc {
  return acceleration;
}

Vect PCB::get_rot() {
  return rotSpeed;
}