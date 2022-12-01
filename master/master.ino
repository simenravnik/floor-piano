#include <SoftwareSerial.h>
#include <SoftwareSerial.h>

#define LEDPIN 3

SoftwareSerial mySerial(0, 1);   // RX, TX

char message[2];

void setup() 
{
  Serial.begin(9600);
  Serial.println("System started");

  // UART
  mySerial.begin(9600);

  pinMode(LEDPIN, OUTPUT);
}

void loop() 
{
  mySerial.readBytes(message, sizeof(message));
  Serial.println(message);

  if (message[0] == '1') {
    digitalWrite(LEDPIN, HIGH); // switch LED On
  }
  else if (message[0] == '2') {
    digitalWrite(LEDPIN, LOW);  // switch LED Off
  }
}