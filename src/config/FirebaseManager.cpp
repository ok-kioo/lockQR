#include "FirebaseManager.h"
#include "../secrets.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setupFirebase() {

  config.host =
    FIREBASE_DATABASE_URL;

  config.signer.tokens.legacy_token =
    FIREBASE_API_KEY;

  Firebase.begin(
    &config,
    &auth
  );

  Firebase.reconnectWiFi(true);
}