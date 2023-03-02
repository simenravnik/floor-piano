#include <Arduino.h>
#include "src/MPR/MPR121.h"
#include "src/espsoftwareserial/src/SoftwareSerial.h"

// MPR121
#define INTERUPT_PIN 33
#define numElectrodes 12

MPR121_type MPR121_1;

#define MYPORT_TX 12
#define MYPORT_RX 13

SoftwareSerial mySerial;

void setup()
{
  Serial.begin(115200);
  Serial.println("System started");

  mySerial.begin(38400, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  if (!mySerial)
  { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
    while (1)
    { // Don't continue with invalid configuration
      delay(1000);
    }
  }

  if (!MPR121.begin(0x5A))
  {

    Serial.println("error setting up MPR121");
    switch (MPR121.getError())
    {
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
  }

  MPR121.setInterruptPin(INTERUPT_PIN);
  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);
  MPR121.updateTouchData();
}

void loop()
{
  // Read MPR121 and play notes accordingly
  if (MPR121.touchStatusChanged())
  {
    MPR121.updateTouchData();
    for (int i = 0; i < numElectrodes; i++)
    {
      if (MPR121.isNewTouch(i))
      {
        Serial.println(i);
        mySerial.write(i);
      }
      else if (MPR121.isNewRelease(i))
      {
        Serial.println(i + 12);
        mySerial.write(i + 12);
      }
    }
  }
}