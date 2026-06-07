#include <Arduino.h>

#include "config/WifiManager.h"
#include "config/FirebaseManager.h"

#include "services/DoorSyncService.h"

#include "lock/LockManager.h"

unsigned long lastSync = 0;

const unsigned long
SYNC_INTERVAL = 2000;

void setup() {

  Serial.begin(115200);

  connectWifi();

  setupFirebase();

  loadDoors();
}

void loop() {

  processLocks();

  monitorSensors();

  if (
    millis() - lastSync >=
    SYNC_INTERVAL
  ) {

    lastSync =
      millis();

    syncDoors();
  }
}