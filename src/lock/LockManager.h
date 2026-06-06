#pragma once

#include <map>
#include <Arduino.h>
#include "LockState.h"

extern std::map<String, LockState> locks;

bool registerLock(
  const String& id
);

void openLock(
  const String& id
);

void closeLock(
  const String& id
);

void processLocks();

void updateDoorState(
  const String& id,
  bool isOpen
);