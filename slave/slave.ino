#include <MPR121.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define numElectrodes 2
#define INTERUPT_PIN 2

SoftwareSerial mySerial(3, 1);   // RX, TX
char message[2];

void setup()
{
  Serial.begin(115200);
  Serial.println("System started");

  mySerial.begin(9600);

  Wire.begin();

  // 0x5C is the MPR121 I2C address on the Bare Touch Board
  if (!MPR121.begin(0x5A)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  
  MPR121.setInterruptPin(INTERUPT_PIN);
  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);
  MPR121.updateTouchData();
}

void loop()
{
  if (MPR121.touchStatusChanged()) {
    MPR121.updateTouchData();
    for (int i = 0; i < numElectrodes; i++) {
      if (MPR121.isNewTouch(i)) {
        if (i == 0) {
          Serial.println('1');
          message[0] = '1';
          mySerial.write(message, sizeof(message));
        }

      } else if (MPR121.isNewRelease(i)) {
        if (i == 1) {
          Serial.println('0');
          message[0] = '2';
          mySerial.write(message, sizeof(message));
        }
      }
    }
  }
}