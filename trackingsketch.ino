#include <Servo.h>
#include "HUSKYLENS.h"
Servo panServo;
Servo tiltServo;
int width = 360, height = 240;  // total resolution of the video
int panPin = 5;
int tiltPin = 6;
const int angle = 2;  // degree of increment or decrement
int prevX;
int prevY;
int xpos = 0, ypos = 0;
int inputX;
int inputY;
int consecutiveReadingsRequired = 2;
int consecutiveReadings = 0;
HUSKYLENS huskylens;
void printResult(HUSKYLENSResult result);
void setup() {
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  Wire.begin();
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  tiltServo.write(0);
  while (!huskylens.begin(Wire)) {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  digitalWrite(7, HIGH);
}

void loop() {
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if (!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  if (!huskylens.available()) {
    Serial.println(F("No block or arrow appears on the screen!"));
    consecutiveReadings = 0;
  } else {
    Serial.println(F("###########"));
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
  }
  if (consecutiveReadings >= consecutiveReadingsRequired) {
    Serial.println("tracking");
    if (inputX > width / 2 + 30)
      xpos -= angle;
    if (inputX < width / 2 - 30)
      xpos += angle;
    if (inputY < height / 2 + 30)
      ypos += angle;
    if (inputY > height / 2 - 30)
      ypos -= angle;  // run all tracking and computations here
    if (xpos >= 180)
      xpos = 180;
    else if (xpos <= 0)
      xpos = 0;
    if (ypos >= 45)
      ypos = 45;
    else if (ypos <= 0)
      ypos = 0;

    panServo.write(xpos);
    tiltServo.write(ypos);
    Serial.println(xpos);
    Serial.println(ypos);
    if (consecutiveReadings >= 25) {
      digitalWrite(7, LOW);
    }
  } else {
    Serial.println("no object");
    digitalWrite(7, HIGH);
  }  // Add a small
}


void printResult(HUSKYLENSResult result) {
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
    consecutiveReadings++;
    inputX = result.xCenter;
    inputY = result.yCenter;
  } else {
    consecutiveReadings = 0;
  }
}
