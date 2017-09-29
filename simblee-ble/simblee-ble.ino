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

#define TEMP_PIN 3

OneWire ds(TEMP_PIN);

float temp = 20.0;

void setup() {
  Serial.begin(9600);
  printf("setup()\n");

  Wire.begin();
  
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  SimbleeBLE.advertisementData = "brewce-ih-cooker-ble";

  // start the BLE stack
  SimbleeBLE.begin();
  SimbleeBLE.end();

  SimbleeBLE_device_name = "brewce_cooker";
  SimbleeBLE_begin();
}

void loop() {
  // sample once per second
  Simblee_ULPDelay( SECONDS(1) );

  // get a cpu temperature sample
  // degrees c (-198.00 to +260.00)
  // degrees f (-128.00 to +127.00)
  /*
  if(Serial.available() > 0)
  {
    float tmp = Serial.parseFloat();
    if(tmp != 0.0)
      temp = tmp;
  }
  */

  // send the sample to the iPhone
  //SimbleeBLE.sendFloat(Simblee_temperature(CELSIUS));
  float temp = getTemp();
  if(temp != -1000.0f)
    SimbleeBLE.sendFloat(getTemp());
}

void SimbleeBLE_onConnect()
{
  printf("SimbleeBLE_onConnect()\n");
}

void SimbleeBLE_onDisconnect()
{
  printf("SimbleeBLE_onDisconnect()\n");
}

void SimbleeBLE_onReceive(char *data, int len) {
  printf("SimbleeBLE_onReceive(");
  for(int i=0; i<len; i++)
    printf(" %02x", data[i]);
  printf(", %i)\n", len);

  if(len != 2)
    return;
  
  uint8_t state = data[0];
  uint8_t power = data[1];

  Wire.beginTransmission(69);

  // ON - Manual
  if(state == 1)
  {
    Wire.write(1);Wire.write(255-power);
  }
  // ON - Auto
  else if(state == 2)
  {
    if(power < 36)
    {
      Wire.write(0);Wire.write(255);
    }
    else
    {
      Wire.write(1);Wire.write(255 - map(power, 36, 255, 0, 255));
    }
  }
  // OFF
  else
  {
    Wire.write(0);Wire.write(255);
  }
  
  Wire.endTransmission();
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
      Serial.println("CRC is not valid!");
      return -1000;
  }
 
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
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
