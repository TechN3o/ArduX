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
#include <Scheduler.h>
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
//classes
class functionInfoClass {
public:
  String name;
  int listNumber;
};
void classSetup() {  // class objects
  functionInfoClass sonicLCDClass;
  sonicLCDClass.name = "sonic measuring";
  functionInfoClass temperatureClass;
  temperatureClass.name = "temperature";
  functionInfoClass irClass;
  irClass.name = "IR controll";
  //class settings ♦️
}
String functionList[]{
  "IR trans", "sonic measure"
};

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
long encOldPos = -999;
long encNewPos;
Encoder myEnc(4, 5);
int encPosition;
int encoderReturn;
int encLimitTo = 1;

//debugInfo

//USER SETTINGS ☢️ ♦️
int refreshRate = 2;  //seconds
String debugInfoOnOff = "on";
void debugInfo();
void lcdPrint();
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
  debugInfo("SonicLcd", "work", "on", true);
  delay(300);
}
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
template<typename T, size_t N>
int getArraySize(T (&array)[N]) {
  int arrayLength = sizeof(array) / sizeof(array[0]);
  Serial.println(arrayLength);
  return arrayLength;
}
void menuSetup(int menuArrayLenght) {
  encLimitTo = menuArrayLenght;
}



// example of use: debugInfo("lcd motor", "calculate mass of sun", "idk" **depends on if I turned dbgInfOnOff to on or off - else "true" or "false"**, MyVar == 10)
bool debugInfo(String debugInfoName, String debugInfoDescription, String debugInfoSingleOnOff = "idk", bool debugInfoExpect = false) {
  if (debugInfoSingleOnOff == "idk" or debugInfoSingleOnOff == "on") {
    if (debugInfoSingleOnOff == "on" || debugInfoOnOff == "on") {
      Serial.print(debugInfoName);
      if (debugInfoExpect) {
        Serial.print(" did ");
        Serial.print(debugInfoDescription);
        Serial.print(", as expected");
        Serial.println();
        return true;
      } else {
        Serial.print(" didn't ");
        Serial.print(debugInfoDescription);
        Serial.print(" because it has failed/unspecified, expecting");
        Serial.println();
        return false;
      }
    }
  } else {
    return false;  // No info
  }
}

void lcdPrint(String lcdReq = "lcdPrint.no.task") {
  if (lcdReq == lastLcdReq || lcdReq == "0x0") {
    debugInfo("lcdPrint", "nothing (no requests)", "on", true);
  } else if (lcdReq != lastLcdReq) {
    lcd.clear();
    lcd.print(lcdReq);
    lastLcdReq = lcdReq;
    lcd.setCursor(lcdTopLeft);
    debugInfo("lcdPrint", "refresh for a new text", "off", lastLcdReq == lcdReq);
  }
  if (timer.read() >= refreshRate * 1000) {
    lcd.clear();
    lcd.print(lcdReq);
    timer.stop();
    timer.start();
  }
}



void encoderPosition() {  //before calling function MUST call function (thats bullshit :D)

  // READING from encoder
  if (encNewPos != encOldPos) {
    encOldPos = encNewPos;
    encPosition = encNewPos;  // returning "encPosition" that WILL be limited (down bellow) (encOldPos and encNewPos are just ONLY for making sure that something has to be done if encoder.read() CHANGE to smthg different )
    debugInfo("encoder", "change position", "idk", encOldPos == encNewPos);
  }
  debugInfo("encoder", String(myEnc.read()), "off", true);


  // LIMITING encoder position (encPosition) by variable "encLimitTo"
  if (encPosition < 0) {
    myEnc.write(0);
    encPosition = 0;
  }
  if (encPosition >= encLimitTo) {
    myEnc.write(encLimitTo);
    encPosition = encLimitTo;
  }
  debugInfo("EncoderPosition", String(encPosition), "on", true);
  encoderReturn = encPosition;
}



void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  //scheduler:
  // Scheduler.startLoop(encoderPosition);
  //class setup
  void classSetup();


  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.noCursor();
  debugInfo("Serial", "open with 9600 bauds", "idk", true);
  timer.start();
}

void loop() {
  // menuSetup(getArraySize(functionList));
   lcdPrint();
}