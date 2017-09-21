// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

uint8_t power_level = 255;
uint8_t io = 0;

#define IO_PIN 6
#define LVL_PIN 5

void setup()
{
  Wire.begin(69);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  pinMode(IO_PIN, OUTPUT);
  digitalWrite(IO_PIN, LOW);
  
  pinMode(LVL_PIN, OUTPUT);
  analogWrite(LVL_PIN, 255);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.println("setup()");
}

void loop()
{
  // ON
  if(io == 1)
  {
    digitalWrite(IO_PIN, HIGH);
    digitalWrite(13, HIGH);
    analogWrite(LVL_PIN, power_level);
  }
  // OFF
  else
  {
    digitalWrite(IO_PIN, LOW);
    digitalWrite(13, LOW);
    analogWrite(LVL_PIN, 255);
  }
  
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  uint8_t command = 0;
  uint8_t value = 0;

  command = Wire.read();
  value = Wire.read();

  while(Wire.available())
  {
    Wire.read();
  }

  io = (command != 0);
  power_level = value;
  Serial.print("Received: ");
  Serial.print(command);
  Serial.print(" ");
  Serial.println(value);
}
