#include <Arduino.h>
#include "src/MPR/MPR121.h"
#include "src/espsoftwareserial/src/SoftwareSerial.h"

/**
    Wiring (slave):

    1. MPR121

    ------------------------------------
    | MPR121  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   IRQ   |   D4    |     4        |
    |   SCL   |   D22   |     22       |
    |   SDA   |   D21   |     21       |
    |   ADD   |   -     |     -        |
    |   3.3V  |   3.3V  |     3.3V     |
    |   GND   |   GND   |     GND      |
    ------------------------------------

    2. UART

    ------------------------------------
    |  UART   |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   RX    |   RX2   |     16       |
    |   TX    |   TX2   |     17       |
    ------------------------------------

    3. LEDs

    Electrodes from MPR121 that are connected to
    GPIO pins which serves as outputs for LEDs.

    --------------------------------------
    | Electrode |  ESP32  | ESP32 (GPIO) |
    --------------------------------------
    |     0     |   D13   |     13       |
    |     1     |    -    |      -       |
    |     2     |   D12   |     12       |
    |     3     |    -    |      -       |
    |     4     |   D14   |     14       |
    |     5     |   D27   |     27       |
    |     6     |    -    |      -       |
    |     7     |   D26   |     26       |
    |     8     |    -    |      -       |
    |     9     |   D25   |     25       |
    |     10    |    -    |      -       |
    |     11    |   D33   |     33       |
    --------------------------------------

**/

// MPR121
#define INTERUPT_PIN 4
#define numElectrodes 12

MPR121_type MPR121_1;

#define MYPORT_TX 17
#define MYPORT_RX 16

SoftwareSerial mySerial;

uint8_t LEDS[numElectrodes] = {13, 0, 12, 0, 14, 27, 0, 26, 0, 25, 0, 33};

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

  for (uint8_t i = 0; i < numElectrodes; i++)
  {
    if (LEDS[i] != 0)
    {
      pinMode(LEDS[i], OUTPUT); // set ESP32 pin to output mode
    }
  }
}

void loop()
{
  // Read MPR121 and play notes accordingly
  if (MPR121.touchStatusChanged())
  {
    MPR121.updateTouchData();
    for (uint8_t i = 0; i < numElectrodes; i++)
    {
      if (MPR121.isNewTouch(i))
      {
        // Serial.println(i);
        mySerial.write(i);
        ledOn(i);
      }
      else if (MPR121.isNewRelease(i))
      {
        // Serial.println(i + 12);
        mySerial.write(i + 12);
        ledOff(i);
      }
    }
  }
}

void ledOn(uint8_t pin)
{
  if (LEDS[pin] != 0)
  {
    digitalWrite(LEDS[pin], HIGH);
  }
}

void ledOff(uint8_t pin)
{
  if (LEDS[pin] != 0)
  {
    digitalWrite(LEDS[pin], LOW);
  }
}