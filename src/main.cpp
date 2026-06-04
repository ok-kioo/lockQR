#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../secrets.h"
#include <Firebase_ESP_Client.h>

#define LOCK_PIN 2

WebServer server(80);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const unsigned long TEMPO_ABERTO_MS = 3000;

void conectarWiFi();
void configurarFirebase();
void configurarServidorWeb();

void rotaLogin();
bool validarAcesso(String porta);

void abrirFechadura(String caminho);
void fecharFechadura(String caminho);

void setup() {
  Serial.begin(115200);

  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW);

  conectarWiFi();
  configurarFirebase();
  configurarServidorWeb();
}

void loop() {
  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
}

void conectarWiFi() {
  Serial.println("Conectando ao WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
}

void configurarFirebase() {
  config.host = FIREBASE_DATABASE_URL;
  config.signer.tokens.legacy_token = FIREBASE_API_KEY;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase iniciado");
}

void configurarServidorWeb() {
  server.on("/login", HTTP_POST, rotaLogin);

  server.begin();

  Serial.println("Servidor web iniciado");
  Serial.print("Acesse pelo navegador ou QR Code: http://");
  Serial.println(WiFi.localIP());
}

void rotaLogin() {

  String body = server.arg("plain");

  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, body);

  if (erro) {
    server.send(400, "text/plain", "JSON invalido");
    return;
  }

  String porta = doc["porta"].as<String>();

  Serial.println(porta);

  String caminho = "/doors/";
  caminho += porta;
  caminho += "/isOpen";

  if (validarAcesso(caminho)) {
    abrirFechadura(caminho);
    delay(TEMPO_ABERTO_MS);
    fecharFechadura(caminho);
    server.send(200, "text/plain", "Acesso liberado");
  } else {
    server.send(401, "text/plain", "Erro interno");
  }
}

bool validarAcesso(String caminho) {
  if (!Firebase.ready()) {
    Serial.println("Firebase ainda nao esta pronto");
    return false;
  }

  Serial.print("Consultando Firebase em: ");
  Serial.println(caminho);

  if (Firebase.RTDB.getBool(&fbdo, caminho.c_str())) {
    bool doorIsOpen = fbdo.boolData();

    if (doorIsOpen == true) {
      server.send(401, "text/plain", "Porta já está aberta");
      return false;
    }

    Serial.println("Porta fechada");
    return true;
  }

  Serial.print("Erro ao consultar Firebase: ");
  Serial.println(fbdo.errorReason());

  return false;
}

void abrirFechadura(String caminho) {
  if (Firebase.RTDB.setBool(&fbdo, caminho.c_str(), true)) {
    Serial.println("Porta marcada como aberta no Firebase");
  } else {
    Serial.println(fbdo.errorReason());
  }
  Serial.println("Abrindo fechadura");

  digitalWrite(LOCK_PIN, HIGH);
}

void fecharFechadura(String caminho) {
  if (Firebase.RTDB.setBool(&fbdo, caminho.c_str(), false)) {
    Serial.println("Porta marcada como fechada no Firebase");
  } else {
    Serial.println(fbdo.errorReason());
  }

  digitalWrite(LOCK_PIN, LOW);

  Serial.println("Fechadura fechada novamente");
}