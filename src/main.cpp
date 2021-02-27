#include <keypadFunction.h>
#include <function.h>


void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  StartSound();
  pinMode(ledVert, OUTPUT);
  pinMode(ledRouge, OUTPUT);

  digitalWrite(ledVert, LOW);
  pinMode(pinSerrure, INPUT);
  digitalWrite(ledRouge, HIGH);
  
  setupFinger();
  delay(1000);
  
}


void loop() {

  getKeypad();
  checkSon();
  checkFinger();
  checkButtonSaveFinger();
  checkMotion();
}