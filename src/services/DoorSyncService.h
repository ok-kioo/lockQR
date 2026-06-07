#pragma once
#include <Arduino.h>

void loadDoors();
void syncDoors();
void updateDoorState(
  const String& id,
  bool isOpen
);
void updatePhysicalState(
  const String& id,
  bool physicalOpen
);