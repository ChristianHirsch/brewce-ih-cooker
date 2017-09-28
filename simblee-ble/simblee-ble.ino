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
#include <Wire.h>
#include <SimbleeBLE.h>

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
  SimbleeBLE.sendFloat(Simblee_temperature(CELSIUS));
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

  // OFF
  if(state == 0)
  {
    Wire.write(0);Wire.write(255);
  }
  // ON - Manual
  else if(state == 1)
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
  
  Wire.endTransmission();
}
