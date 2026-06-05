#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../secrets.h"
#include <Firebase_ESP_Client.h>
#include <map>
#include <set>

WebServer server(80);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const unsigned long TEMPO_ABERTO_MS = 10000;

struct LockState {
  uint8_t pin;
  bool aberta;
  unsigned long instanteAbertura;
};

std::map<String, LockState> fechaduras;

const uint8_t PINOS_DISPONIVEIS[] = {
  2,
  4,
  5,
  18,
  19,
  21,
  22,
  23,
  25,
  26,
  27,
  32,
  33
};

const int TOTAL_PINOS =
  sizeof(PINOS_DISPONIVEIS) /
  sizeof(PINOS_DISPONIVEIS[0]);

int proximoPino = 0;

void conectarWiFi();
void configurarFirebase();
void configurarServidorWeb();
void adicionarCors();

void carregarPortasFirebase();

void rotaAbrir();
void rotaNovaPorta();

bool validarAcesso(const String& caminho);

void abrirFechadura(
  const String& portaId,
  const String& caminho
);

void fecharFechadura(
  const String& portaId,
  const String& caminho
);

bool registrarPorta(
  const String& portaId
);

void setup() {

  Serial.begin(115200);

  conectarWiFi();

  configurarFirebase();

  carregarPortasFirebase();

  configurarServidorWeb();
}

void loop() {

  server.handleClient();

  unsigned long agora = millis();

  for (auto& item : fechaduras) {

    String portaId = item.first;
    LockState& estado = item.second;

    if (
      estado.aberta &&
      agora - estado.instanteAbertura >= TEMPO_ABERTO_MS
    ) {

        String caminho = "/doors/";
        caminho += portaId;
        caminho += "/isOpen";

      fecharFechadura(
        portaId,
        caminho
      );
    }
  }

  if (
    WiFi.status() != WL_CONNECTED
  ) {
    conectarWiFi();
  }
}

void conectarWiFi() {

  Serial.println(
    "Conectando ao WiFi..."
  );

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "WiFi conectado"
  );

  Serial.println(
    WiFi.localIP()
  );
}

void configurarFirebase() {

  config.host =
    FIREBASE_DATABASE_URL;

  config.signer.tokens
      .legacy_token =
      FIREBASE_API_KEY;

  Firebase.begin(
    &config,
    &auth
  );

  Firebase.reconnectWiFi(true);

  Serial.println(
    "Firebase iniciado"
  );
}

void adicionarCors() {

  server.sendHeader(
    "Access-Control-Allow-Origin",
    "*"
  );

  server.sendHeader(
    "Access-Control-Allow-Methods",
    "GET, POST, PUT, DELETE, OPTIONS"
  );

  server.sendHeader(
    "Access-Control-Allow-Headers",
    "*"
  );
}

bool registrarPorta(
  const String& portaId
) {

  if (
    fechaduras.find(portaId)
    != fechaduras.end()
  ) {

    return true;
  }

  if (
    proximoPino >= TOTAL_PINOS
  ) {

    Serial.println(
      "Sem GPIOs livres"
    );

    return false;
  }

  uint8_t pino =
    PINOS_DISPONIVEIS[
      proximoPino++
    ];

  pinMode(
    pino,
    OUTPUT
  );

  digitalWrite(
    pino,
    LOW
  );

  fechaduras[portaId] = {
    pino,
    false,
    0
  };

  Serial.print(
    "Porta registrada: "
  );

  Serial.print(portaId);

  Serial.print(
    " -> GPIO "
  );

  Serial.println(pino);

  return true;
}

void carregarPortasFirebase() {

  Serial.println("Carregando portas do Firebase...");

  if (!Firebase.RTDB.getJSON(
          &fbdo,
          "/doors")) {

    Serial.print("Erro Firebase: ");
    Serial.println(fbdo.errorReason());

    return;
  }

  String jsonString;

  fbdo.jsonObject().toString(
      jsonString,
      true
  );

  JsonDocument doc;

  DeserializationError erro =
      deserializeJson(
          doc,
          jsonString
      );

  if (erro) {

    Serial.print(
        "Erro ao parsear JSON: "
    );

    Serial.println(
        erro.c_str()
    );

    return;
  }

  JsonObject portas =
      doc.as<JsonObject>();

  int total = 0;

  for (JsonPair kv : portas) {

    String portaId =
        kv.key().c_str();

    if (
        registrarPorta(
            portaId
        )) {

      total++;
    }
  }

  Serial.print(
      "Total de portas carregadas: "
  );

  Serial.println(total);
}

void configurarServidorWeb() {

  server.on("/open", HTTP_OPTIONS, []() {

    adicionarCors();

    Serial.println("OPTIONS /open");

    server.send(204);
  });

  server.on("/new-lock", HTTP_OPTIONS, []() {

    adicionarCors();

    Serial.println("OPTIONS /new-lock");

    server.send(204);
  });

  server.on(
    "/open",
    HTTP_POST,
    rotaAbrir
  );

  server.on(
    "/new-lock",
    HTTP_POST,
    rotaNovaPorta
  );

  server.onNotFound([]() {

    adicionarCors();

    Serial.print("Rota nao encontrada: ");
    Serial.println(server.uri());

    server.send(
      404,
      "text/plain",
      "Rota nao encontrada"
    );
  });

  server.begin();

  Serial.println("Servidor iniciado");
}

void rotaNovaPorta() {

  adicionarCors();

  String body =
    server.arg("plain");

  JsonDocument doc;

  if (
    deserializeJson(
      doc,
      body
    )
  ) {

    server.send(
      400,
      "text/plain",
      "JSON invalido"
    );

    return;
  }

  String portaId =
    doc["id"] | "";

  if (
    portaId.isEmpty()
  ) {

    server.send(
      400,
      "text/plain",
      "ID invalido"
    );

    return;
  }

  if (
    !registrarPorta(portaId)
  ) {

    server.send(
      500,
      "text/plain",
      "Sem GPIO disponivel"
    );

    return;
  }

  server.send(
    200,
    "text/plain",
    "Porta registrada"
  );
}

void rotaAbrir() {

  adicionarCors();

  String body =
    server.arg("plain");

  JsonDocument doc;

  if (
    deserializeJson(
      doc,
      body
    )
  ) {

    server.send(
      400,
      "text/plain",
      "JSON invalido"
    );

    return;
  }

  String porta =
    doc["porta"] | "";

  if (
    !fechaduras.count(porta)
  ) {

    server.send(
      404,
      "text/plain",
      "Porta nao encontrada"
    );

    return;
  }

  String caminho = "/doors/";
  caminho += porta;
  caminho += "/isOpen";

  if (
    !validarAcesso(caminho)
  ) {

    server.send(
      409,
      "text/plain",
      "Porta ja aberta"
    );

    return;
  }

  abrirFechadura(
    porta,
    caminho
  );

  server.send(
    200,
    "text/plain",
    "Acesso liberado"
  );
}

bool validarAcesso(
  const String& caminho
) {

  if (
    !Firebase.RTDB.getBool(
      &fbdo,
      caminho.c_str()
    )
  ) {

    return false;
  }

  return !fbdo.boolData();
}

void abrirFechadura(
  const String& portaId,
  const String& caminho
) {

  LockState& estado =
    fechaduras[portaId];

  digitalWrite(
    estado.pin,
    HIGH
  );

  estado.aberta = true;

  estado.instanteAbertura =
    millis();

  Firebase.RTDB.setBool(
    &fbdo,
    caminho.c_str(),
    true
  );

  Serial.print(
    "Abrindo "
  );

  Serial.println(portaId);
}

void fecharFechadura(
  const String& portaId,
  const String& caminho
) {

  LockState& estado =
    fechaduras[portaId];

  digitalWrite(
    estado.pin,
    LOW
  );

  estado.aberta = false;

  Firebase.RTDB.setBool(
    &fbdo,
    caminho.c_str(),
    false
  );

  Serial.print(
    "Fechando "
  );

  Serial.println(portaId);
}