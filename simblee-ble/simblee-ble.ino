/*
The sketch demonstrates how to do accept a Bluetooth Low Energy 4
Advertisement connection with the Simblee, then send CPU temperature
updates once a second.

It is suppose to be used with the simbleeTemperature iPhone application.
*/

/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */
#include <SimbleeBLE.h>
#include <Wire.h>
#include <OneWire.h>

#define TEMP_PIN 0
#define POWER_I_PIN 6
#define POWER_O_PIN 2
#define IO_I_PIN 5
#define IO_O_PIN 3

OneWire ds(TEMP_PIN);

float temp = 20.0;

unsigned long ble_sent = 0;

boolean ble_on = false;
uint8_t ble_value = 255;

void setup() {
  SimbleeBLE.advertisementData = "brewce-ih-cooker-ble";

  // start the BLE stack
  SimbleeBLE.begin();
  SimbleeBLE.end();

  SimbleeBLE_device_name = "brewce_cooker";
  SimbleeBLE_begin();

  pinMode(POWER_I_PIN, INPUT);
  pinMode(POWER_O_PIN, OUTPUT);
  pinMode(IO_I_PIN, INPUT);
  pinMode(IO_O_PIN, OUTPUT);

  analogWrite(POWER_O_PIN, 255);
  digitalWrite(IO_O_PIN, LOW);
}

void loop() {
  // sample once per second
  unsigned long now = millis();

  analogWrite(POWER_O_PIN, 0);
  digitalWrite(IO_O_PIN, HIGH);
/*
  if(digitalRead(IO_I_PIN) == HIGH)
  {
    digitalWrite(IO_O_PIN, HIGH);
    analogWrite(POWER_O_PIN, map(analogRead(POWER_I_PIN), 1, 410, 0, 255));
  }
  else if(ble_on == true)
  {
    digitalWrite(IO_O_PIN, HIGH);
    analogWrite(POWER_O_PIN, ble_value);
  }
  else
  {
    digitalWrite(IO_O_PIN, LOW);
    analogWrite(POWER_O_PIN, 255);
  }
*/
  if((now - ble_sent) > 1000)
  {
    ble_sent = now;
    float temp = getTemp();
    if(digitalRead(IO_I_PIN) == LOW)
      SimbleeBLE.sendFloat(0.0f);
    else
      SimbleeBLE.sendFloat(1.0f);
    //if(temp != -1000.0f)
    //  SimbleeBLE.sendFloat(temp);
  }
  
  Simblee_ULPDelay( 100 );
}

void SimbleeBLE_onConnect()
{
}

void SimbleeBLE_onDisconnect()
{
}

void SimbleeBLE_onReceive(char *data, int len) {
  if(len != 2)
    return;
  
  uint8_t state = data[0];
  uint8_t power = data[1];

  // ON - Manual
  if(state == 1)
  {
    ble_on = true;
    ble_value = 255 - power;
  }
  // ON - Auto
  else if(state == 2)
  {
    if(power < 36)
    {
      ble_on = false;
      ble_value = 255;
    }
    else
    {
      ble_on = true;
      ble_value = 255 - map(power, 36, 255, 0, 255);
    }
  }
  // OFF
  else
  {
    ble_on = false;
    ble_value = 255;
  }
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
 
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      return -1000;
  }
 
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      return -1000;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
 
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
 
  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];
 
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
