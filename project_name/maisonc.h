#ifndef __MAISONC_H__
#define __MAISONC_H__

#include <iostream>
#include "Ultrasonic.h"
#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>
#include "SHT31.h"
#include "rgb_lcd.h"
#include <list>
#include <algorithm>
#include <numeric>

#include <vector>


// Classe de base "Maison"
class Maison {
protected:
  String type;

public:
  Maison(String type)
    : type(type) {}
  virtual void afficherInfo() const = 0;  // Méthode virtuelle pure pour afficher les informations
  virtual ~Maison() {}                    // Destructeur virtuel
};

class Portail : public Maison {

public:

  Portail()
    : Maison("Portail") {}

  void afficherInfo() const override {
    Serial.println(type);
  }
};

class Sonnette : public Maison {

public:

  Sonnette()
    : Maison("Sonnette") {}

  void afficherInfo() const override {
    Serial.println(type);
  }

  void sonnerie() {
    if (digitalRead(D6) == HIGH) {
      digitalWrite(D5, HIGH);
    } else {
      digitalWrite(D5, LOW);
    }
  }
};

class DectectionPortail : public Portail {
protected:
  Ultrasonic ultrasonic;

public:
  DectectionPortail()
    : Portail(), ultrasonic(D7) {}
  long Distance() {

    return ultrasonic.MeasureInMillimeters();
  }
};


class CmdPortail : public DectectionPortail {

protected:
  Servo myservo;
  long lastime;

public:
  CmdPortail():lastime(0) {
    myservo.attach(D5);
  }

  void OuvrirPortail(int vitesse) {
    for (int pos = 0; pos <= 180; pos += vitesse) {  // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);  // tell servo to go to position in variable 'pos'
      delay(15);           // waits 15ms for the servo to reach the position
    }
  }
  void FermerPortail(int vitesse) {
    for (int pos = 180; pos >= 0; pos -= vitesse) {  // goes from 180 degrees to 0 degrees
      myservo.write(pos);                            // tell servo to go to position in variable 'pos'
      delay(15);                                     // waits 15ms for the servo to reach the position
    }
  }
  void InitMoteur() {
    myservo.write(0);
  }

  void automatic() {
    long D = Distance();
    if (D < 100 and D > 0) {
      Serial.println("ttttt");
      OuvrirPortail(1);
      auto now = millis();
      if (now - lastime > 5000) {
        FermerPortail(1);
      }
    }
  }
};

class StationMeteo : public Maison {

protected:
  SHT31 sht31 = SHT31();
  rgb_lcd lcd;
  std::list<float> Ltemp, Lhum;
  int nbLtemp;
  long lasttimes;
  float averageT;
  float averageH;

public:

  void afficherInfo() const override {
    Serial.println(type);
  }

  StationMeteo()
    : Maison("StationMeteo") {
    lcd.begin(16, 2);
    sht31.begin();
    lcd.setRGB(255, 0, 0); /*RVB*/
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print("Humidite:");

    lasttimes = 0;
  }

  void getMeteo() {
    auto now = millis();
    if (now - lasttimes > 10000) {
      lasttimes = now;
      nbLtemp++;
      Ltemp.push_front(sht31.getTemperature());
      Lhum.push_front(sht31.getHumidity());
      Serial.println(sht31.getHumidity());


      if (nbLtemp > 100) {
        Ltemp.pop_back();
        Lhum.pop_back();
      }
    }
  }

  void TemperatureMoyenne() {
    if (!Lhum.empty() and !Ltemp.empty()) {
      // somme des elements
      averageT = std::accumulate(Ltemp.begin(), Ltemp.end(), 0.0f) / Ltemp.size();
      averageH = std::accumulate(Lhum.begin(), Lhum.end(), 0.0f) / Lhum.size();
      //Serial.println(averageH);
    }
  }

  void displayMeteo() {
    TemperatureMoyenne();
    lcd.setCursor(14, 0);
    lcd.print(averageT);
    lcd.setCursor(11, 1);
    lcd.print(averageH);
  }
};

#endif
