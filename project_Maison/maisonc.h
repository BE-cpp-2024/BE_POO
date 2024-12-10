#ifndef __MAISONC_H__
#define __MAISONC_H__

#include <iostream>
#include "Ultrasonic.h"
#include <Servo.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "SHT31.h"
#include <Wire.h>
#include "rgb_lcd.h"
#include <Arduino.h>
#include <list>
#include <algorithm>
#include <numeric>

#include <vector>

#ifndef STASSID
#define STASSID "OnePlus 12R"
#define STAPSK "aymen123"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;



// Page HTML avec un formulaire pour entrer le code
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

// Page HTML avec un formulaire pour entrer le code
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
    <button onclick="window.location.href='/button1'">Ouverture portail</button>
    <button onclick="window.location.href='/button2'">Désactivation Sonnette</button>
    <button onclick="window.location.href='/button3'">Lumière</button>
  </div>
</body>
</html>
)";


// Classe de base "Maison"
class Maison {
protected : 
  String type;

public:
    Maison(String type): type(type){}
    virtual void afficherInfo() const = 0;  // Méthode virtuelle pure pour afficher les informations
    virtual ~Maison() {}  // Destructeur virtuel
};

class Portail : public Maison{

public:

  Portail(): Maison("Portail"){}

  void afficherInfo() const override {
      Serial.println(type);
  }
};

class Lumiere : public Maison {
private:
  bool alu;

public:
  Lumiere() : Maison("Sonnette"), alu(false){}

  void afficherInfo() const override {
      Serial.println(type);
  }

  void toggleLumiere() {
    Serial.println("test1");
    alu = !alu;  // Bascule l'état de la lumière
    if (alu) {
      Serial.println("test2");
      digitalWrite(D9, HIGH);  // Allume la LED
    } else {
      Serial.println("test3");
      digitalWrite(D9, LOW);   // Éteint la LED
    }
  }
};

class Sonnette : public Maison{

private :

  bool autorisation;

public :

  Sonnette():Maison("Sonnette"),autorisation(true){
    pinMode(D8, OUTPUT);
  }
  
  void afficherInfo() const override {
    Serial.println(type);
  }

  void toggleAutorisation(){
    if(autorisation==true){
      autorisation=false;
    }
    else{
      autorisation=true;
    }
  }

  void sonnerie(){
    if(autorisation){
      if (digitalRead(D6) == HIGH) {
      digitalWrite(D8, HIGH);
      }
      else {
        digitalWrite(D8, LOW);
      }
    }
  }
};

class DectectionPortail : public Portail {
protected:
  Ultrasonic ultrasonic;

public :
  DectectionPortail():Portail(),ultrasonic(D7){}
  long Distance(){
    
    return ultrasonic.MeasureInMillimeters();
  }
};


class CmdPortail : public DectectionPortail {

protected : 
  Servo myservo;
  long lastime;
  bool ouvert;
  int currentPos;
  long lastUpdate;
  bool autorisation;



public :
  CmdPortail():lastime(0),ouvert(false),currentPos(0),lastUpdate(0){
    myservo.attach(D5);
    InitMoteur();
  }

  void setAutorisation(bool autorisation){
    this->autorisation=autorisation;
  }

  void OuvrirPortail(){
    myservo.write(0);  // Déplacer le servo à la nouvelle position
  }

  void FermerPortail(){
    myservo.write(180); 
  }
  void InitMoteur(){
    myservo.write(180);
  }

  void automatic(){
      long D = Distance();  // Obtention de la distance
      auto now = millis();  // Obtenir l'heure actuelle en millisecondes

      if(autorisation){
        // Si la distance est correcte et le portail est fermé (ouvert == false)
        if (D < 100 && D > 0 && !ouvert) {
            lastime = now;  // Enregistrer le temps d'ouverture
            Serial.println("Ouverture du portail !!!");
            OuvrirPortail();  // Ouvrir le portail
            ouvert = true;  // Marquer le portail comme ouvert
        }

        // Si le portail est ouvert et que 5 secondes se sont écoulées depuis l'ouverture
        if (ouvert && (now - lastime > 5000)) {
            Serial.println("Fermeture du portail !!!");
            FermerPortail();  // Fermer le portail
            ouvert = false;  // Marquer le portail comme fermé
            setAutorisation(false);
        }
      }
  }
};

class StationMeteo : public Maison{

protected:
  SHT31 sht31 = SHT31();
  rgb_lcd lcd;
  std::list<float> Ltemp,Lhum;
  int nbLtemp;
  long lasttimes;
  float averageT;
  float averageH;

public:

  void afficherInfo() const override {
      Serial.println(type);
  }
  
  StationMeteo(): Maison("StationMeteo"){
    lcd.begin(16, 2);
    sht31.begin();
    lcd.setRGB(255, 0, 0);/*RVB*/
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print("Humidite:");
    lasttimes=0;
  }

  void getMeteo(){
    auto now = millis();
    if (now - lasttimes > 10000){
      lasttimes =now;
      nbLtemp++;
      Ltemp.push_front(sht31.getTemperature());
      Lhum.push_front(sht31.getHumidity());
      Serial.println(sht31.getHumidity());


      if (nbLtemp>100){
        Ltemp.pop_back();
        Lhum.pop_back();
      }
    }
  }

  void TemperatureMoyenne(){
    if(!Lhum.empty() and !Ltemp.empty()){
      // somme des elements
      averageT= std::accumulate(Ltemp.begin(), Ltemp.end(), 0.0f)/Ltemp.size();
      averageH= std::accumulate(Lhum.begin(), Lhum.end(), 0.0f)/Lhum.size();
      //Serial.println(averageH);
    }
  }

  void displayMeteo(){
    TemperatureMoyenne();
    lcd.setCursor(14, 0);
    lcd.print(averageT);
    lcd.setCursor(11, 1);
    lcd.print(averageH);
  }
};

CmdPortail cmd;
StationMeteo s1;
Sonnette son;
Lumiere lumi;

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
    Serial.println("Portail");
    cmd.setAutorisation(true);
  }

  static void handleButton2() {
    server.send(200, "text/html", htmlForm2);
    Serial.println("Son");
    son.toggleAutorisation();
  }

  static void handleButton3() {
    server.send(200, "text/html", htmlForm2);
    Serial.println("Lumière");
    lumi.toggleLumiere();
  }

  void HClient() {
    server.handleClient();
  }
};

ESP8266WebServer ServeurMaison::server(80);

#endif
