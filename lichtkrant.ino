#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

//Pins defineren
#define MAX_DEVICES 4 //4 "units" aan schermen van de LED "Lichtkrant"
#define DATA_PIN D7   //DATA
#define CS_PIN D8   //CS 
#define CLK_PIN D5 //CLCK

//initieren van de display 
MD_Parola lichtkrant = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const String host = "10.0.10.2:8080"; //pi adres

void setup() {
  Serial.begin(9600);
  lichtkrant.begin(); //
  lichtkrant.setIntensity(10);
  lichtkrant.displayClear();

  WiFi.begin("pigroep5", "pigroep5"); // probeer te verbinden met de wifi
  Serial.println("Connecting to pigroep5 WiFi");
  while (WiFi.status() != WL_CONNECTED) // wacht totdat hij is verbonden
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  lichtkrant.setTextAlignment(PA_LEFT); //display alignment aan de linkerkant

  WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    String url = "http://" +host+ "/matrix/data";
    if (http.begin(client, url)) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode < 0) {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return;
      }

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          lichtkrant.print(payload);
        }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }

  delay(1000);

}
