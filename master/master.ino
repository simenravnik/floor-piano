#include "src/VS/VS10XX.h"
#include <Wire.h>
#include "src/espsoftwareserial/src/SoftwareSerial.h"

/**
    Wiring (master):

    1. VS1003
    ------------------------------------
    | VS1003  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   SCLK  |   D18   |     18       |
    |   MISO  |   D19   |     19       |
    |   MOSI  |   D23   |     23       |
    |   XRST  |   D5    |     5        |
    |   CS    |   D4    |     4        |
    |   XDCS  |   RX2   |     16       |
    |   DREQ  |   TX2   |     17       |
    |   5V    |   3.3V  |     3.3V     |
    |   GND   |   GND   |     GND      |
    ------------------------------------

    2. Octaves (UART)

    Connect RX pins from slave controllers to TX pins on master controller,
    and TX pins from slave controllers to master TX pins on master controller.

    Octave 1:
    ------------------------------------
    | UART 1  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   RX    |   D13   |     13       |
    |   TX    |   D12   |     12       |
    ------------------------------------

    Octave 2:
    ------------------------------------
    | UART 2  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   RX    |   D14   |     14       |
    |   TX    |   D27   |     27       |
    ------------------------------------

    Octave 3:
    ------------------------------------
    | UART 3  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   RX    |   D26   |     26       |
    |   TX    |   D25   |     25       |
    ------------------------------------

    Octave 4:
    ------------------------------------
    | UART 4  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   RX    |   D33   |     33       |
    |   TX    |   D32   |     32       |
    ------------------------------------

    1. Rotary Encoder
    ------------------------------------
    | KY-040  |  ESP32  | ESP32 (GPIO) |
    ------------------------------------
    |   CLK   |   D34   |     34       |
    |   DT    |   D35   |     35       |
    |   SW    |   VN    |     39       |
    ------------------------------------
**/

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

// UART communication
#define numOctaves 4

uint8_t RxTx[numOctaves][2] = {
    {13, 12}, // RX, TX
    {14, 27},
    {26, 25},
    {33, 32}};

SoftwareSerial *connections[numOctaves] = {
    new SoftwareSerial(),
    new SoftwareSerial(),
    new SoftwareSerial(),
    new SoftwareSerial()};

uint8_t message[numOctaves][1];

// Rotary Encoder
#define CLK 34
#define DT 35
#define SW 39

bool rotaryEncoderActive; // True or False
unsigned long lastButtonPress = 0;
int8_t channel;
int8_t prevChannel;

int currentStateCLK;
int lastStateCLK;

void setup()
{

  Serial.begin(115200);
  Serial.println("System started");

  // Initialize all 4 UART connections
  for (int i = 0; i < numOctaves; i++)
  {
    connections[i]->begin(38400, SWSERIAL_8N1, RxTx[i][0], RxTx[i][1], false);
    if (!connections[i])
    { // If the object did not initialize, then its configuration is invalid
      Serial.println("Invalid SoftwareSerial pin configuration, check config");
      while (1)
      { // Don't continue with invalid configuration
        delay(1000);
      }
    }
    message[i][0] = -1;
  }

  Serial.println("Initialising VS10xx");
  // put your setup code here, to run once:
  vs10xx.initialiseVS10xx();
  delay(1000);

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT);

  rotaryEncoderActive = false;
  channel = 0;
  prevChannel = 0;

  lastStateCLK = digitalRead(CLK);
}

#define OCTAVE_1 48
#define OCTAVE_2 60

void loop()
{

  // ROTARY ENCODER
  checkButtonPressed();

  if (rotaryEncoderActive)
  {
    updateEncoder();
  }

  // VS10xx
  for (uint8_t i = 0; i < numOctaves; i++)
  {
    if (connections[i]->available())
    {
      connections[i]->readBytes(message[i], sizeof(message[i]));

      if (message[i][0] >= 0 && message[i][0] < 12)
      {
        // Serial.println(message[i][0]);
        uint8_t note = message[i][0] + (36 + (12 * i));
        vs10xx.noteOn(channel, note, 127);
      }
      else if (message[i][0] >= 12)
      {
        // Serial.println(message[i][0]);
        uint8_t note = message[i][0] - 12 + (36 + (12 * i));
        vs10xx.noteOff(channel, note, 127);
      }

      message[i][0] = -1;
    }
  }
}

void checkButtonPressed()
{
  // Update rotary encoder state when the button was pressed
  if (digitalRead(SW) == LOW)
  {
    // If the interval between last and current press is larger than 50ms
    if (millis() - lastButtonPress > 50)
    {
      rotaryEncoderActive = !rotaryEncoderActive;
    }
    lastButtonPress = millis();
  }
}

void updateEncoder()
{
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
  {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK)
    {
      channel = (channel - 1) % 16;
      if (channel < 0)
      {
        channel = 0;
      }
    }
    else
    {
      // Encoder is rotating CW so increment
      channel = (channel + 1) % 16;
    }

    // Turn off currently playing notes
    for (int j = 0; j < 12; j++)
    {
      uint8_t noteOctave1 = j + 36;
      uint8_t noteOctave2 = j + 36 + (12 * 1);
      uint8_t noteOctave3 = j + 36 + (12 * 2);
      uint8_t noteOctave4 = j + 36 + (12 * 2);
      vs10xx.noteOff(prevChannel, noteOctave1, 127);
      vs10xx.noteOff(prevChannel, noteOctave2, 127);
      vs10xx.noteOff(prevChannel, noteOctave3, 127);
      vs10xx.noteOff(prevChannel, noteOctave4, 127);
    }
    prevChannel = channel;
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;
}