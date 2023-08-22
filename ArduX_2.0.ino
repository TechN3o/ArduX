/*

MIT License
Copyright © 2023 Štěpán Otruba

*/

//    LIBRARIES:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Thread.h>
#include <Timer.h>
//definitions
#define lcdTopLeft 0, 0
#define lcdTopRight 15, 0
#define lcdBottomLeft 0, 1
#define lcdBottomRight 15, 1

/*/circuit diagram on arduino:
  Digital:
      0 - nothing
      1 - nothing
      2 - Enc1
      3 - Enc2
      4 - nothing
      5 - buttonOK
      6 - Sonothing: Echo
      7 - Sonothing: Trigger
      8 - nothing
      9 - nothing
      10 - nothing
      11 - nothing
      12 - nothing
      13 - nothing
  Analog:
      0 - pinEncSW
      1 - nothing
      2 - nothing
      3 - nothing
      4 - LCD: SDA
      5 - LCD: SCL    /*/


//global variables:
String debugInfoName;
String debugInfoDescription;
bool debugInfoExpect;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD 16 chars, 2 lines
const int pingPin = 7;
const int echoPin = 6;
String lastLcdReq;
String lcdReq;
Timer timer;
int lcdRow;



//debugInfo

//USER SETTINGS ☢️
int refreshRate = 2;  //seconds
bool debugInfoOnOff = true;



void sonicLCD() {  //not my own code, source is unknown
  long duration, inches, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  lcdPrint(String(cm));
  delay(300);
}
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}


// example of use: debugInfo("lcd motor", "calculate mass of sun", "idk" (depends on if I turned dbgInfOnOff on or off - else "true" of "false"), MyVar == 10)
bool debugInfo(String debugInfoName, String debugInfoDescription, String debugInfoSingleOnOff = "idk", bool debugInfoExpect = false) {
  if (debugInfoSingleOnOff == "idk" or "on") {
    if (debugInfoOnOff or debugInfoSingleOnOff == "true") {
      Serial.print(debugInfoName);
      if (debugInfoExpect) {
        Serial.print(" did ");
        Serial.print(debugInfoDescription);
        Serial.print(", as expected  ");
        Serial.println();
        return true;
      } else {
        Serial.print(" didnt ");
        Serial.print(debugInfoDescription);
        Serial.print(" beacuse has failed/unspecified expecting  ");
        Serial.println();
        return false;
      }
    }
  } else {
    return false;  //no info
  }
}
int lcdPrint(String lcdReq) {
  if (lcdReq != lastLcdReq) {
    lcd.clear();
    lcd.print(lcdReq);
    lastLcdReq = lcdReq;
    lcd.setCursor(lcdTopLeft);
    debugInfo("lcdPrint", "refresh for a new text", "idk", lastLcdReq == lcdReq);
  }
  if (timer.read() >= refreshRate * 1000) {
    lcd.clear();
    lcd.print(lcdReq);
    timer.stop();
    timer.start();
  }
  if (lcdReq == lastLcdReq) {
    debugInfo("lcdPrint", "nothing (no requests)", "idk");
  }
}
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.noCursor();
  debugInfo("Serial", "open with 9600 bauds", "idk", true);
  timer.start();
}

void loop() {
  sonicLCD();
  debugInfo("Serial test", "work", "on", Serial.available());
}