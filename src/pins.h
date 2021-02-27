#include <Arduino.h>

#define buzzer 2
#define ledVert A1
#define ledRouge A0
#define out A2
#define pinSerrure 10
#define saveFingerButton A7
#define motionSensor A6

const int tailleCode = 5;
const String codeOK = "00000"; //Code
String code = "";
bool w = 0;

bool wantFinger = 0;
char key;
String strKey;



