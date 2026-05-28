#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "../secrets.h"
#include "site/web_content.h"
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

void rotaPrincipal();
void rotaCss();
void rotaJs();
void rotaLogin();

String limparUsuario(String usuario);
bool validarUsuarioNoFirebase(String usuario, String senhaDigitada);
void abrirFechadura();

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
  config.api_key = FIREBASE_API_KEY;
  config.database_url = FIREBASE_DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase iniciado");
}

void configurarServidorWeb() {
  server.on("/", HTTP_GET, rotaPrincipal);
  server.on("/style.css", HTTP_GET, rotaCss);
  server.on("/script.js", HTTP_GET, rotaJs);
  server.on("/login", HTTP_POST, rotaLogin);

  server.begin();

  Serial.println("Servidor web iniciado");
  Serial.print("Acesse pelo navegador ou QR Code: http://");
  Serial.println(WiFi.localIP());
}

void rotaPrincipal() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void rotaCss() {
  server.send_P(200, "text/css", STYLE_CSS);
}

void rotaJs() {
  server.send_P(200, "application/javascript", SCRIPT_JS);
}

void rotaLogin() {
  String usuario = server.arg("usuario");
  String senha = server.arg("senha");

  usuario.trim();
  senha.trim();

  Serial.println("Tentativa de login");
  Serial.print("Usuario recebido: ");
  Serial.println(usuario);

  bool acessoPermitido = validarUsuarioNoFirebase(usuario, senha);

  if (acessoPermitido) {
    abrirFechadura();
    server.send(200, "text/plain", "Acesso liberado");
  } else {
    server.send(401, "text/plain", "Usuario ou senha incorretos");
  }
}

String limparUsuario(String usuario) {
  usuario.trim();
  usuario.toLowerCase();

  usuario.replace(".", "");
  usuario.replace("#", "");
  usuario.replace("$", "");
  usuario.replace("[", "");
  usuario.replace("]", "");
  usuario.replace("/", "");

  return usuario;
}

bool validarUsuarioNoFirebase(String usuario, String senhaDigitada) {
  if (!Firebase.ready()) {
    Serial.println("Firebase ainda nao esta pronto");
    return false;
  }

  usuario = limparUsuario(usuario);
  senhaDigitada.trim();

  if (usuario.length() == 0 || senhaDigitada.length() == 0) {
    Serial.println("Usuario ou senha vazios");
    return false;
  }

  String caminho = "/usuarios/";
  caminho += usuario;
  caminho += "/senha";

  Serial.print("Consultando Firebase em: ");
  Serial.println(caminho);

  if (Firebase.RTDB.getString(&fbdo, caminho.c_str())) {
    String senhaFirebase = fbdo.stringData();
    senhaFirebase.trim();

    if (senhaFirebase == senhaDigitada) {
      Serial.println("Senha correta");
      return true;
    }

    Serial.println("Senha incorreta");
    return false;
  }

  Serial.print("Erro ao consultar Firebase: ");
  Serial.println(fbdo.errorReason());

  return false;
}

void abrirFechadura() {
  Serial.println("Abrindo fechadura");

  digitalWrite(LOCK_PIN, HIGH);
  delay(TEMPO_ABERTO_MS);
  digitalWrite(LOCK_PIN, LOW);

  Serial.println("Fechadura travada novamente");
}