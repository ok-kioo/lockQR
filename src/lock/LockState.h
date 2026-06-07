#pragma once

#include <ESP32Servo.h>

struct LockState {

  uint8_t servoPin;

  uint8_t sensorPin;

  Servo* servo;

  bool abertaFisicamente;

  unsigned long instanteAbertura;
};