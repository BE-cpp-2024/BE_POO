#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "maisonc.h"

#ifndef STASSID
#define STASSID "OnePlus 12R"
#define STAPSK "aymen123"
#endif


const char* ssid = STASSID;
const char* password = STAPSK;

// Page HTML pour entrer le code
const char* htmlForm = R"(
  <!DOCTYPE html>
  <html lang="fr">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Entrer un code</title>
    <style>
      body {
        font-family: 'Arial', sans-serif;
        background-color: #f4f7fc;
        margin: 0;
        padding: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
      }
      .container {
        background-color: white;
        padding: 30px;
        border-radius: 10px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        width: 100%;
        max-width: 400px;
      }
      h1 {
        color: #4CAF50;
        text-align: center;
        margin-bottom: 20px;
      }
      label {
        font-size: 16px;
        color: #333;
        margin-bottom: 8px;
        display: block;
      }
      input[type="text"] {
        width: 100%;
        padding: 10px;
        font-size: 16px;
        margin-bottom: 20px;
        border: 2px solid #ccc;
        border-radius: 5px;
        outline: none;
        box-sizing: border-box;
      }
      input[type="text"]:focus {
        border-color: #4CAF50;
      }
      input[type="submit"] {
        background-color: #4CAF50;
        color: white;
        padding: 12px 20px;
        font-size: 16px;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        width: 100%;
      }
      input[type="submit"]:hover {
        background-color: #45a049;
      }
      .footer {
        text-align: center;
        margin-top: 20px;
        font-size: 14px;
        color: #777;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Entrez votre code</h1>
      <form action="/submit" method="POST">
        <label for="code">Code :</label>
        <input type="text" id="code" name="code" required>
        <input type="submit" value="Envoyer">
      </form>
    </div>
  </body>
  </html>
  )";

// Page HTML pour contrôler le serveur avec des boutons
const char* htmlForm2 = R"(
  <!DOCTYPE html>
  <html lang="fr">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Contrôler le serveur</title>
    <style>
      body {
        font-family: 'Arial', sans-serif;
        background-color: #f0f4f8;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
        margin: 0;
        color: #333;
      }
      h1 {
        text-align: center;
        font-size: 2.5em;
        color: #4a90e2;
        margin-bottom: 30px;
      }
      .container {
        text-align: center;
        background-color: #fff;
        padding: 40px;
        border-radius: 8px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        width: 300px;
      }
      button {
        font-size: 1.2em;
        background-color: #4a90e2;
        color: #fff;
        border: none;
        border-radius: 5px;
        padding: 15px 30px;
        margin: 10px 0;
        width: 100%;
        cursor: pointer;
        transition: background-color 0.3s ease;
      }
      button:hover {
        background-color: #357ab7;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Choisissez une action</h1>
      <button onclick="window.location.href='/button1'">Bouton 1</button>
      <button onclick="window.location.href='/button2'">Bouton 2</button>
      <button onclick="window.location.href='/button3'">Bouton 3</button>
    </div>
  </body>
  </html>
  )";


class ServeurMaison {
private:
  static ESP8266WebServer server;

public:
  // Constructeur de la classe
  ServeurMaison() {
    server.on("/", handleRoot);

    server.on("/submit", HTTP_POST, handleCodeSubmission);

    // Modifier pour chaque bouton
    server.on("/button1", handleButton1);
    server.on("/button2", handleButton2);
    server.on("/button3", handleButton3);

    server.begin();
    Serial.println("HTTP server started");
  }

  static void handleRoot() {
    server.send(200, "text/html", htmlForm);
  }

  // Fonction de gestion de la soumission du code
  static void handleCodeSubmission() {
    String code = server.arg("code");

    Serial.print("Code reçu: ");
    Serial.println(code);

    if (code == "baba") {
      server.send(200, "text/html", htmlForm2);
    } else {
      server.send(200, "text/html", htmlForm);
    }
  }

  // Gestion des boutons
  static void handleButton1() {
    server.send(200, "text/html", htmlForm2);
    Serial.println("Bouton 1 cliqué");
  }

  static void handleButton2() {
    server.send(200, "text/html", htmlForm2);
    Serial.println("Bouton 2 cliqué");
  }

  static void handleButton3() {
    server.send(200, "text/html", htmlForm2);
    Serial.println("Bouton 3 cliqué");
  }

  void HClient() {
    server.handleClient();
  }
};

ESP8266WebServer ServeurMaison::server(80);

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
}

ServeurMaison test;

StationMeteo m;

void loop() {
  test.HClient();
  m.getMeteo();
  m.TemperatureMoyenne();
  m.displayMeteo();
}
