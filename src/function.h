#include <Arduino.h>
#include <pins.h>
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>

#define EEPROM_Address 5

SoftwareSerial SerialFinger(12, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&SerialFinger);


void OkSound()
{
    noTone(buzzer);
    digitalWrite(ledVert, HIGH);
    tone(buzzer, 523, 50);
    delay(50);
    tone(buzzer, 783, 50);
    delay(50);
    tone(buzzer, 1046, 50);
    delay(50);
    tone(buzzer, 1568, 50);
    delay(50);
    tone(buzzer, 2093, 70);
    delay(250);
    digitalWrite(ledVert, LOW);
}

void StartSound()
{
    noTone(buzzer);
    digitalWrite(ledVert, HIGH);
    tone(buzzer, 523, 50);
    delay(50);
    tone(buzzer, 783, 50);
    delay(50);
    tone(buzzer, 1046, 50);
    delay(50);
    tone(buzzer, 1568, 50);
    delay(50);
    tone(buzzer, 2093, 70);
    delay(50);
    tone(buzzer, 2593, 70);
    delay(50);
    tone(buzzer, 3093, 70);
    delay(250);
    digitalWrite(ledVert, LOW);
}

void checkSon()
{
    bool c = digitalRead(pinSerrure);
    if (c == 1 && w == 0)
    {
        OkSound();
        w = 1;
    }
    else if (c == 0)
    {
        w = 0;
    }
}

void openDoor()
{
    wantFinger = 0;
    pinMode(out, OUTPUT);
    digitalWrite(out, HIGH);
    delay(200);
    digitalWrite(out, LOW);
}

void errorCode()
{
    noTone(buzzer);
    digitalWrite(ledRouge, LOW);
    tone(buzzer, 370, 50);
    delay(100);
    tone(buzzer, 370, 300);
    digitalWrite(ledRouge, HIGH);
}

void checkCode()
{
    if (code == codeOK)
    {
        Serial.println("Code OK");
        code = "";
        openDoor();
    }
    else
    {
        errorCode();
        code = "";
    }
}

void getKeypad()
{
    key = keypad.getKey();
    if (key)
    {
        strKey = "";
        strKey += key;
        code += strKey;
        if (strKey == "*")
        {
            tone(buzzer, 750, 400);
            delay(500);
            tone(buzzer, 750, 70);
            code = "";
        }
        else
        {
            tone(buzzer, 1568, 50);
        }
        if (code.length() == tailleCode)
        {
            checkCode();
        }
    }
}


void setupFinger()
{
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword())
    {
        //OK
    }
    else
    {
        //ERROR Finger
        while (1)
        {
            delay(1);
        }
    }
    finger.getParameters();
    finger.getTemplateCount();
}

uint8_t getFingerID()
{
    uint8_t p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
        break;
    case FINGERPRINT_NOFINGER:
        return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
        return 0;
    case FINGERPRINT_IMAGEFAIL:
        return 0;
    default:
        return 0;
    }
    p = finger.image2Tz();
    switch (p)
    {
    case FINGERPRINT_OK:
        break;
    case FINGERPRINT_IMAGEMESS:
        return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
        return 0;
    case FINGERPRINT_FEATUREFAIL:
        return 0;
    case FINGERPRINT_INVALIDIMAGE:
        return 0;
    default:
        return 0;
    }

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        wantFinger = 0;
        return finger.fingerID + 2;
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        return 0;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        return 1;
    }
    else
    {
        return 0;
    }

    return finger.fingerID + 2;
}

uint8_t getFingerprintEnroll()
{
    int p = -1;
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            errorCode();
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            errorCode();
            break;
        default:
            Serial.println("Unknown error");
            errorCode();
            break;
        }
    }

    p = finger.image2Tz(1);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        errorCode();
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        errorCode();
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        errorCode();
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        errorCode();
        return p;
    default:
        Serial.println("Unknown error");
        errorCode();
        return p;
    }

    Serial.println("Remove finger");
    tone(buzzer, 800, 200);
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    p = -1;
    Serial.println("Place same finger again");
    tone(buzzer, 1000, 200);
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            errorCode();
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            errorCode();
            break;
        default:
            Serial.println("Unknown error");
            errorCode();
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        errorCode();
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        errorCode();
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        errorCode();
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        errorCode();
        return p;
    default:
        Serial.println("Unknown error");
        errorCode();
        return p;
    }

    // OK converted!

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Prints matched!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        errorCode();
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Fingerprints did not match");
        errorCode();
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        errorCode();
        return p;
    }
    int id = EEPROM.read(EEPROM_Address);
    p = finger.storeModel(id+1);
    if (p == FINGERPRINT_OK)
    {
        OkSound();
        Serial.println("Stored!");
        EEPROM.write(EEPROM_Address, id+1);
        Serial.println(EEPROM.read(EEPROM_Address));
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        errorCode();
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        Serial.println("Could not store in that location");
        errorCode();
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        Serial.println("Error writing to flash");
        errorCode();
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        errorCode();
        return p;
    }

    return true;
}

void checkFinger()
{
    if (wantFinger)
    {
        int finger = getFingerID();
        if (finger > 1)
        {
            openDoor();
            wantFinger = 0;
            //Finger ok
        }
        else if (finger == 1)
        {
            errorCode();
            //Finger unkown
        }
        else
        {
            //nothing
        }
    }
}

void checkButtonSaveFinger(){
    if(analogRead(saveFingerButton) > 1000){
        while (!  getFingerprintEnroll() );
    }
}

void checkMotion(){
    if(analogRead(motionSensor) > 600){
        wantFinger = true;
    }else{
        wantFinger = false;
    }
}