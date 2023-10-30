#include <Servo.h>
#include "HUSKYLENS.h"
Servo panServo;
Servo tiltServo;

int panPin = 8;
int tiltPin = 9;

int prevX;
int prevY;

int inputX;
int inputY;

int minPanAngle = 0;   // Minimum angle for the pan servo
int minTiltAngle = 0;  // Minimum angle for the tilt servo

int consecutiveReadingsRequired = 1;
int consecutiveReadings = 0;

void readObjectCoordinates(HUSKYLENSResult result);
HUSKYLENS huskylens;
void printResult(HUSKYLENSResult result);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  while (!huskylens.begin(Wire)) {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  // Initialize PID control
  tiltServo.write(0);
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
  Pos();      // run all tracking and computations here
  delay(1000);
  } else {
    Serial.println("no object");
    delay(1000);
  }  // Add a small delay
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
void Pos()
{
  if(prevX != inputX || prevY != inputY)
  {
    int servoX = map(inputX, 360, 0, 0, 179);
    int servoY = map(inputY, 240, 0, 90, 0);

    servoX = min(servoX, 179);
    servoX = max(servoX, 0);
    servoY = min(servoY, 90);
    servoY = max(servoY, 0);
    
    panServo.write(servoX);
    tiltServo.write(servoY);
    Serial.println(servoX);
    Serial.println(servoY);
  }
}
