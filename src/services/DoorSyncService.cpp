#include "DoorSyncService.h"

#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <set>

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

    Serial.print(
      "Erro JSON: "
    );

    Serial.println(
      erro.c_str()
    );

    return;
  }

  JsonObject portas =
    doc.as<JsonObject>();

  std::set<String> idsFirebase;

  for (
    JsonPair kv : portas
  ) {

    idsFirebase.insert(
      kv.key().c_str()
    );
  }

  std::vector<String> portasRemovidas;

  for (
    auto& item : locks
  ) {

    if (
      !idsFirebase.count(
        item.first
      )
    ) {

      portasRemovidas.push_back(
        item.first
      );
    }
  }

  for (
    const String& portaId :
    portasRemovidas
  ) {

    Serial.print(
      "Removendo porta: "
    );

    Serial.println(
      portaId
    );

    removeLock(
      portaId
    );
  }

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

    bool abertaNoBanco =
      porta["isOpen"] |
      false;

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

    else if (
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

void updatePhysicalState(
  const String& id,
  bool physicalOpen
) {

  String path =
    "/doors/";

  path += id;

  path +=
    "/physicalOpen";

  Firebase.RTDB.setBool(
    &fbdo,
    path.c_str(),
    physicalOpen
  );
}