#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>
#include <map>
#include <vector>
#include "LockState.h"

extern std::map<String, LockState> locks;

extern const int ANGULO_FECHADO;
extern const int ANGULO_ABERTO;

extern const unsigned long TEMPO_ABERTO_MS;

bool registerLock(
  const String& id
);

void removeLock(
  const String& id
);

void openLock(
  const String& id
);

void closeLock(
  const String& id
);

bool readPhysicalState(
  const String& id
);

void processLocks();

void monitorSensors();