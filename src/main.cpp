#include <Arduino.h>
#include <TM1637Display.h>

// Reglage du contraste 
#define CONTRAST 4
//Delay pour 1 passage 
const int Delay1Passage = 1000;

//Delay entre 2 passage
const int Delay2Passage = 500;

#define CLK D3
#define DIO D4


TM1637Display display(CLK, DIO);

const int Sensor1 = D5;
const int Sensor2 = D6;
int compteur=0;
int affichage=0;

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

boolean MotionOn1=false;
boolean MotionOn2=false;

IRAM_ATTR void detectsMovement1() {
  MotionOn1=true;
  Serial.println("MOTION ON 1 !");
}
IRAM_ATTR void detectsMovement2() {
  MotionOn2=true;
  Serial.println("MOTION ON 2 !");
}



void setup() {
  display.setBrightness(CONTRAST);
  display.showNumberDec(compteur, false, 4, 0); 
  
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Sensor1), detectsMovement1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Sensor2), detectsMovement2, FALLING);

  Serial.begin(9600);
  Serial.println("Start!!!");
}

void loop() {
  //Fonction d'affichage
  if (compteur!=affichage){
    affichage=compteur;
    display.clear();
    display.showNumberDec(compteur, false, 4, 0); 
    Serial.println(compteur);

  }
  if (MotionOn1){
    Serial.println("MotionOn1 stage1");

    while(MotionOn2==false){
        delay(5);

    }
    Serial.println("MotionOn1 stage2");
    compteur+=1;
    delay(Delay2Passage);
    MotionOn2=false;
    MotionOn1=false;
  }  
  if (MotionOn2){
    Serial.println("MotionOn2 stage1");

    while(MotionOn1==false){
        delay(5);

    }
    Serial.println("MotionOn2 stage2");
    compteur-=1;
    delay(Delay2Passage);
    MotionOn2=false;
    MotionOn1=false;
  }  
}

