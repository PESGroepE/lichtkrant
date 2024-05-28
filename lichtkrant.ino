#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

//Pins defineren
#define MAX_DEVICES 4 //4 "units" aan schermen van de LED "Lichtkrant"
#define DATA_PIN D7
#define CS_PIN D8
#define CLK_PIN D5
#define TRIL_PIN D0
#define DRUK_PIN A0

//initieren van de display 
MD_Parola lichtkrant = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const String host = "10.0.10.1:8080"; //pi adres

/*!
 @brief Setup.
 
 Initialisatie van seriële communicatie, lichtkrant display, intensiteit van display.
 Verbinding maken met WiFi-netwerk.
 */

void setup() {
  //Start seriele monitor voor debug output.
  Serial.begin(9600);

  //Initialiseer lichtkrant.
  lichtkrant.begin();
  lichtkrant.setIntensity(10);
  lichtkrant.displayClear();
  lichtkrant.setTextAlignment(PA_LEFT);

  //Initialiseer gpio pins voor trilmotor en druksensor.
  pinMode(TRIL_PIN, OUTPUT);
  pinMode(DRUK_PIN, INPUT);

  //Initialiseer wifi verbinding.
  WiFi.begin("pigroep5", "pigroep5"); // probeer te verbinden met de wifi
  Serial.println("Connecting to pigroep5 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

/*!
 @brief Loop.
 
Alignment van de display wordt naar links gezet.
Er wordt verbinding gemaakt met HTTP Server (Raspberry Pi), Wemos is de Client.
Een string wordt opgevraagd van de Server; de payload dat verkregen wordt, wordt geprint op lichtkrant.
 */

void loop() {
  getLichtkrant();
  delay(100);

  getTrilmotor();
  delay(100);

  pushDruksensor(analogRead(A0));
  delay(800);
}

void getLichtkrant() {
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");

  String url = "http://" +host+ "/matrix/data";
  if (http.begin(client, url)) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if (httpCode < 0) {
      Serial.println("[HTTP] GET... failed, error: " + http.errorToString(httpCode));
      return;
    }

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      lichtkrant.print(payload);
      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }

  }
}

void getTrilmotor() {
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  
  String url = "http://" +host+ "/trilmotor/status";
  if (http.begin(client, url)) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if (httpCode < 0) {
      Serial.println("[HTTP] GET... failed, error: " + http.errorToString(httpCode));
      return;
    }

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      if (payload=="1") {
        digitalWrite(TRIL_PIN, HIGH);
      } else {
        digitalWrite(TRIL_PIN, LOW);
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }

  }
}

void pushDruksensor(int druk) {
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  
  String status = "false";
  if (druk >= 200) {
    status = "true";
  }
  String url = "http://" +host+ "/druksensor/status";
  if (http.begin(client, url)) {  // HTTP
    Serial.print("[HTTP] POST...\n");
    int httpCode = http.POST(status);

    if (httpCode < 0) {
      Serial.println("[HTTP] POST... failed");
      return;
    }

    if (httpCode == HTTP_CODE_OK) {
      Serial.println("[HTTP] POST success");
    } else {
      Serial.println("[HTTP] Unable to connect: " + String(httpCode));
    }

  }
}
