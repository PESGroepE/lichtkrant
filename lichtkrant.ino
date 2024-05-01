#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

//Pins defineren
#define MAX_DEVICES 4 //4 "units" aan schermen van de LED "Lichtkrant"
#define DATA_PIN D7   //DATA
#define CS_PIN D8   //CS 
#define CLK_PIN D5 //CLCK

//initieren van de display 
MD_Parola lichtkrant = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
  lichtkrant.begin(); //
  lichtkrant.setIntensity(10);
  lichtkrant.displayClear();
}

void loop() {
  lichtkrant.setTextAlignment(PA_LEFT); //display alignment aan de linkerkant
  char message[] = "User2";
  lichtkrant.print(message); //print "Username"
  delay(2000);
  lichtkrant.displayClear();

}
