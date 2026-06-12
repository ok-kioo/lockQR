#include "LockManager.h"
#include "../services/DoorSyncService.h"

std::map<String, LockState> locks;

const int ANGULO_FECHADO = 90;
const int ANGULO_ABERTO = 135;

const unsigned long TEMPO_ABERTO_MS =
  10000;

const uint8_t SERVO_PINS[] = {
  18,
  19,
  21,
  22,
  23
};

const uint8_t SENSOR_PINS[] = {
  25,
  26,
  27,
  32,
  33
};

const int TOTAL_PORTAS = 5;

std::vector<int> freeSlots;

static void initializePins() {

  if (!freeSlots.empty()) {
    return;
  }

  for (
    int i = 0;
    i < TOTAL_PORTAS;
    i++
  ) {

    freeSlots.push_back(i);
  }
}

bool registerLock(
  const String& id
) {

  initializePins();

  if (
    locks.find(id)
    != locks.end()
  ) {

    return true;
  }

  if (
    freeSlots.empty()
  ) {

    Serial.println(
      "Sem portas livres"
    );

    return false;
  }

  int slot =
    freeSlots.front();

  freeSlots.erase(
    freeSlots.begin()
  );

  uint8_t servoPin =
    SERVO_PINS[slot];

  uint8_t sensorPin =
    SENSOR_PINS[slot];

  Servo* servo =
    new Servo();

  servo->setPeriodHertz(50);

  servo->attach(
    servoPin,
    500,
    2400
  );

  servo->write(
    ANGULO_FECHADO
  );

  pinMode(
    sensorPin,
    INPUT_PULLUP
  );

  locks[id] = {
    servoPin,
    sensorPin,
    servo,
    false,
    0
  };

  Serial.print(
    "Porta registrada: "
  );

  Serial.print(id);

  Serial.print(
    " Servo="
  );

  Serial.print(
    servoPin
  );

  Serial.print(
    " Sensor="
  );

  Serial.println(
    sensorPin
  );

  return true;
}

void removeLock(
  const String& id
) {

  auto it =
    locks.find(id);

  if (
    it ==
    locks.end()
  ) {

    return;
  }

  uint8_t servoPin =
    it->second.servoPin;

  int slot = -1;

  for (
    int i = 0;
    i < TOTAL_PORTAS;
    i++
  ) {

    if (
      SERVO_PINS[i] ==
      servoPin
    ) {

      slot = i;
      break;
    }
  }

  if (
    it->second.servo
  ) {

    it->second.servo->detach();

    delete it->second.servo;
  }

  locks.erase(it);

  if (
    slot >= 0
  ) {

    freeSlots.push_back(
      slot
    );
  }

  Serial.print(
    "Porta removida: "
  );

  Serial.println(id);
}

void openLock(
  const String& id
) {

  auto it =
    locks.find(id);

  if (
    it == locks.end()
  ) {
    return;
  }

  auto& lock =
    it->second;

  lock.servo->write(
    ANGULO_ABERTO
  );

  lock.instanteAbertura =
    millis();

  Serial.print(
    "Abrindo servo: "
  );

  Serial.println(id);
}

void closeLock(
  const String& id
) {

  auto it =
    locks.find(id);

  if (
    it == locks.end()
  ) {
    return;
  }

  auto& lock =
    it->second;

  lock.servo->write(
    ANGULO_FECHADO
  );

  Serial.print(
    "Fechando servo: "
  );

  Serial.println(id);
}

bool readPhysicalState(
  const String& id
) {

  auto it =
    locks.find(id);

  if (
    it ==
    locks.end()
  ) {

    return false;
  }

  return
    digitalRead(
      it->second.sensorPin
    ) == LOW;
}

void monitorSensors() {

  for (
    auto& item : locks
  ) {

    String id =
      item.first;

    auto& lock =
      item.second;

    bool estadoAtual =
      digitalRead(
        lock.sensorPin
      ) == LOW;

    if (
      estadoAtual !=
      lock.abertaFisicamente
    ) {

      lock.abertaFisicamente =
        estadoAtual;

      updatePhysicalState(
        id,
        estadoAtual
      );

      Serial.print(
        "Sensor alterado: "
      );

      Serial.print(id);

      Serial.print(
        " -> "
      );

      Serial.println(
        estadoAtual
      );
    }
  }
}

void processLocks() {

  unsigned long now =
    millis();

  for (
    auto& item : locks
  ) {

    auto& lock =
      item.second;

    if (
      lock.abertaFisicamente &&
      now -
      lock.instanteAbertura >=
      TEMPO_ABERTO_MS
    ) {

      Serial.print(
        "Fechando automaticamente: "
      );

      Serial.println(
        item.first
      );

      closeLock(
        item.first
      );

      updateDoorState(
        item.first,
        false
      );
    }
  }
}