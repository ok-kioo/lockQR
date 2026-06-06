#include "DoorSyncService.h"

#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

#include "../config/FirebaseManager.h"
#include "../lock/LockManager.h"

extern FirebaseData fbdo;

void loadDoors() {

  Serial.println(
    "Carregando portas..."
  );

  if (
    !Firebase.RTDB.getJSON(
      &fbdo,
      "/doors"
    )
  ) {

    Serial.print(
      "Erro Firebase: "
    );

    Serial.println(
      fbdo.errorReason()
    );

    return;
  }

  String jsonString;

  fbdo
    .jsonObject()
    .toString(jsonString);

  JsonDocument doc;

  DeserializationError erro =
    deserializeJson(
      doc,
      jsonString
    );

  if (erro) {

    Serial.println(
      erro.c_str()
    );

    return;
  }

  JsonObject portas =
    doc.as<JsonObject>();

  int total = 0;

  for (
    JsonPair kv : portas
  ) {

    String portaId =
      kv.key().c_str();

    if (
      registerLock(
        portaId
      )
    ) {

      total++;
    }
  }

  Serial.print(
    "Portas carregadas: "
  );

  Serial.println(
    total
  );
}

void syncDoors() {

  if (
    !Firebase.RTDB.getJSON(
      &fbdo,
      "/doors"
    )
  ) {

    Serial.print(
      "Erro Firebase: "
    );

    Serial.println(
      fbdo.errorReason()
    );

    return;
  }

  String jsonString;

  fbdo
    .jsonObject()
    .toString(jsonString);

  JsonDocument doc;

  DeserializationError erro =
    deserializeJson(
      doc,
      jsonString
    );

  if (erro) {
    return;
  }

  JsonObject portas =
    doc.as<JsonObject>();

  for (
    JsonPair kv : portas
  ) {

    String portaId =
      kv.key().c_str();

    JsonObject porta =
      kv.value().as<JsonObject>();

    registerLock(
      portaId
    );

    bool abertaNoBanco =
      porta["isOpen"] |
      false;

    auto it =
      locks.find(
        portaId
      );

    if (
      it ==
      locks.end()
    ) {
      continue;
    }

    LockState& estado =
      it->second;

    if (
      abertaNoBanco &&
      !estado.abertaFisicamente
    ) {

      Serial.print(
        "Abrindo via Firebase: "
      );

      Serial.println(
        portaId
      );

      openLock(
        portaId
      );
    }

    if (
      !abertaNoBanco &&
      estado.abertaFisicamente
    ) {

      Serial.print(
        "Fechando via Firebase: "
      );

      Serial.println(
        portaId
      );

      closeLock(
        portaId
      );
    }
  }
}

void updateDoorState(
  const String& id,
  bool isOpen
) {

  String path ="/doors/";
    path += id;
    path += "/isOpen";

  Firebase.RTDB.setBool(
    &fbdo,
    path.c_str(),
    isOpen
  );
}