#include "src/VS/VS10XX.h"
#include <Wire.h>
#include "src/espsoftwareserial/src/SoftwareSerial.h"

// VS1003 pin definitions
#define VS_XCS 4   // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS 16 // Data Chip Select / BSYNC Pin
#define VS_DREQ 17 // Data Request Pin: Player asks for more data
#define VS_RESET 5 // Reset is active low

// VS10xx SPI pin connections
// Provided here for info only - not used in the sketch as the SPI library handles this
#define VS_MOSI 23
#define VS_MISO 19
#define VS_SCK 18
#define VS_SS 5

VS10XX vs10xx(VS_XCS, VS_XDCS, VS_DREQ, VS_RESET, VS_SS);

#define MYPORT_TX 12
#define MYPORT_RX 13

SoftwareSerial mySerial;

uint8_t message[1];

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

  Serial.println("Initialising VS10xx");
  // put your setup code here, to run once:
  vs10xx.initialiseVS10xx();
  delay(1000);
}

#define OCTAVE_1 48
#define OCTAVE_2 60

void loop()
{
  mySerial.readBytes(message, sizeof(message));

  if (message[0] >= 0 && message[0] < 12)
  {
    Serial.println(message[0]);
    uint8_t note = message[0] + OCTAVE_1;
    vs10xx.noteOn(0, note, 127);
  }
  else if (message[0] >= 12)
  {
    Serial.println(message[0]);
    uint8_t note = message[0] - 12 + OCTAVE_1;
    vs10xx.noteOff(0, note, 127);
  }

  message[0] = -1;
}
