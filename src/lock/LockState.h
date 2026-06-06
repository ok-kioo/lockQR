#pragma once

#include <ESP32Servo.h>

struct LockState {
  uint8_t pin;
  Servo* servo;
  bool abertaFisicamente;
  unsigned long instanteAbertura;
};