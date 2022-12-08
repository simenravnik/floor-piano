#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

SoftwareSerial mySerial(rxPin, txPin);   // RX, TX

void setup() 
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("System started");

  // UART
  mySerial.begin(9600);
}

void loop() 
{

  while (mySerial.available() > 0) {
    char inByte = mySerial.read();
    Serial.println(inByte);
  }

}