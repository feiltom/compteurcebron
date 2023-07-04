#include <Arduino.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino


//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "ThingSpeak.h"
#include <arduino-timer.h>

// Reglage du contraste 
#define CONTRAST 4
//Delay pour 1 passage 
const int Delay1Passage = 2000;
//Delay entre 2 passage
const int Delay2Passage = 1000;

#define CLK D3
#define DIO D4
TM1637Display display(CLK, DIO);

const int Sensor1 = D5;
const int Sensor2 = D6;
String request = "";

int compteur=0;
int entree=0;
int sortie=0;
int affichage=0;

unsigned long lastTrigger = 0;

boolean MotionOn1=false;
boolean MotionOn2=false;

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

//INIT THINKSPEAK
WiFiClient  client;

unsigned long myChannelNumber = 2178608;
const char * myWriteAPIKey = "VCQY7FP9C1MR81C2";

auto timer = timer_create_default(); // create a timer with default settings


void handleRoot();              // function prototypes for HTTP handlers

IRAM_ATTR void detectsMovement1() {
  MotionOn1=true;
}
IRAM_ATTR void detectsMovement2() {
  MotionOn2=true;
}
bool push(void *) {

  ThingSpeak.setField(1, entree);
  ThingSpeak.setField(2, sortie);
  ThingSpeak.setField(3, compteur);

  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  return true; // repeat? true
}
void push_aff(){
  ThingSpeak.setField(1, entree);
  ThingSpeak.setField(2, sortie);
  ThingSpeak.setField(3, compteur);

  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

}

void aff(){
    if (compteur!=affichage){
    affichage=compteur;
    display.clear();
    display.showNumberDec(compteur, false, 4, 0); 
    Serial.println("compteur");
    Serial.println(compteur);
    Serial.println("entree");
    Serial.println(entree);
    Serial.println("sortie");
    Serial.println(sortie);
    //push_aff();
  }

}




void setup() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  display.setBrightness(CONTRAST);
  display.showNumberDec(compteur, false, 4, 0); 
  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.begin();                           // Actually start the server

  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Sensor1), detectsMovement1, RISING);
  attachInterrupt(digitalPinToInterrupt(Sensor2), detectsMovement2, RISING);

  Serial.begin(9600);
  Serial.println("Start!!!");
  Serial.println("connected!");
  Serial.println(WiFi.localIP());


  ThingSpeak.begin(client);  // Initialize ThingSpeak
  timer.every(300000, push);

}

void loop() {
    server.handleClient();   
                     // Listen for HTTP requests from clients
  //Fonction d'affichage
  aff();

  // Passage sens +1
  if (MotionOn1){
    Serial.println("MotionOn1 stage1");
    lastTrigger = millis();
    while((MotionOn2==false) and ((millis()-lastTrigger)< Delay1Passage)){
        delay(1);
    }
    Serial.println("MotionOn1 stage2");
    if (MotionOn2){
      compteur+=1;
      entree+=1;
      aff();
      delay(Delay2Passage);
    }
    MotionOn2=false;
    MotionOn1=false;
  }  

  // Passage sens -1
  if (MotionOn2){
    Serial.println("MotionOn2 stage1");
    lastTrigger = millis();
    while((MotionOn1==false) and ((millis()-lastTrigger)< Delay1Passage)){
        delay(1);
    }
    Serial.println("MotionOn2 stage2");
    if (MotionOn1){
      compteur-=1;
      sortie+=1;
      aff();
      delay(Delay2Passage);
    }
    MotionOn2=false;
    MotionOn1=false;
  }  

  timer.tick(); // tick the timer


}
void handleRoot() {
  server.send(200, "text/plain", "Compteur : "+String(compteur));   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
