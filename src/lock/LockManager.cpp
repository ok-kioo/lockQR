#include "LockManager.h"
#include "Pins.h"

std::map<String, LockState> locks;

const int ANGULO_FECHADO = 0;
const int ANGULO_ABERTO = 90;

const unsigned long TEMPO_ABERTO_MS =
  10000;

int nextPin = 0;

bool registerLock(
  const String& id
) {

  if (
    locks.find(id)
    != locks.end()
  ) {
    return true;
  }

  if (
    nextPin >= TOTAL_PINOS
  ) {
    return false;
  }

  uint8_t pin =
    PINOS_DISPONIVEIS[
      nextPin++
    ];

  Servo* servo =
    new Servo();

  servo->setPeriodHertz(50);

  servo->attach(
    pin,
    500,
    2400
  );

  servo->write(
    ANGULO_FECHADO
  );

  locks[id] = {
    pin,
    servo,
    false,
    0
  };

  return true;
}

void openLock(
  const String& id
) {

  auto& lock =
    locks[id];

  lock.servo->write(
    ANGULO_ABERTO
  );

  lock.abertaFisicamente =
    true;

  lock.instanteAbertura =
    millis();
}

void closeLock(
  const String& id
) {

  auto& lock =
    locks[id];

  lock.servo->write(
    ANGULO_FECHADO
  );

  lock.abertaFisicamente =
    false;
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