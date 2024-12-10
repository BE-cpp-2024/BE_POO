#include "maisonc.h"

void setup(void) {
  Serial.begin(74880);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Connexion au Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(D9, OUTPUT);
}

ServeurMaison M;

void loop(void) {
  M.HClient();
  s1.getMeteo();
  s1.displayMeteo();
  cmd.automatic();
  son.sonnerie();
}
