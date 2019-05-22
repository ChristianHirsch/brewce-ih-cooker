// brewce UUID: b5e9d1f2-cdb3-4758-a1b0-1d6ddd22dd0d

#include <SimbleeBLE.h>
#include <Wire.h>
#include <OneWire.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define TEMP_PIN 0
/*
#define POWER_I_PIN 3
#define POWER_O_PIN 16
#define IO_I_PIN 11
#define IO_O_PIN 9
*/

#define LED_PIN     19

#define POWER_I_PIN 3
#define POWER_O_PIN 16

#define IO_I_PIN    11
#define IO_O_PIN    9

#define MAX_SO_PIN  10
#define MAX_CS_PIN  13
#define MAX_SCK_PIN 14

//Create a MAX31855 reference and tell it what pin does what
Adafruit_MAX31855 kTC(MAX_SCK_PIN, MAX_CS_PIN, MAX_SO_PIN);

float temp = 20.0;

unsigned long ble_sent = 0;

boolean ble_on = false;
uint8_t ble_value = 255;

struct
{
  float    tempInternal;
  float    tempExternal;
  uint16_t power;
  uint8_t  status;
} ble_data;

void setup() {
  //Serial.begin(9600);
  
  /* --- BLE ------------------------ */
  SimbleeBLE.advertisementData = "brewce-ih-cooker-ble";
  // do iBeacon advertising
  SimbleeBLE.iBeacon = true;
  
  // override the default iBeacon settings
  uint8_t uuid[16] = {0xb5, 0xe9, 0xd1, 0xf2, 0xcd, 0xb3, 0x47, 0x58, 0xa1, 0xb0, 0x1d, 0x6d, 0xdd, 0x22, 0xdd, 0x0d};
  memcpy(SimbleeBLE.iBeaconUUID, uuid, sizeof(SimbleeBLE.iBeaconUUID));
  SimbleeBLE.iBeaconMajor = 1;
  SimbleeBLE.iBeaconMinor = 1;
  SimbleeBLE.iBeaconMeasuredPower = 0xC6;
  
  //SimbleeBLE.txPowerLevel = 4;
  //SimbleeBLE.advertisementInterval = 900;
  
  SimbleeBLE.begin(); // start the BLE stack

  //SimbleeBLE_device_name = "brewce_cooker";
  //SimbleeBLE_begin();

  pinMode(POWER_I_PIN, INPUT);
  pinMode(POWER_O_PIN, OUTPUT);
  pinMode(IO_I_PIN, INPUT);
  pinMode(IO_O_PIN, OUTPUT);
  
  pinMode(LED_PIN, OUTPUT);

  //analogWrite(POWER_O_PIN, 255);
  analogWrite(POWER_O_PIN, 127);
  digitalWrite(IO_O_PIN, LOW);
}

void loop() {
  // sample once per second
  
  unsigned long now = millis();

  boolean power_on = digitalRead(IO_I_PIN);
  
  if(power_on == true)
    ble_data.status = 1;
  else
    ble_data.status = 0;

  ble_data.power = analogRead(POWER_I_PIN);
  
  uint8_t power_value = min(map(ble_data.power, 5, 1000, 0, 255), 255);
  
  //SimbleeBLE.send((char *)&ble_data, sizeof(ble_data));
  
  if(power_on == true)
  {
    digitalWrite(LED_PIN,  HIGH);
    digitalWrite(IO_O_PIN, HIGH);
    analogWrite(POWER_O_PIN, power_value);
  }
  else if(ble_on == true)
  {
    digitalWrite(LED_PIN,  HIGH);
    digitalWrite(IO_O_PIN, HIGH);
    analogWrite(POWER_O_PIN, ble_value);
  }
  else
  {
    digitalWrite(LED_PIN,  LOW);
    digitalWrite(IO_O_PIN, LOW);
    analogWrite(POWER_O_PIN, 0);
  }

  if(now > ble_sent)
  {
    ble_sent = now + 1000;

    ble_data.tempInternal = kTC.readInternal();
    ble_data.tempExternal = kTC.readCelsius();
    if(ble_data.tempExternal == ble_data.tempExternal)
    {
      SimbleeBLE.sendFloat(ble_data.tempExternal);
      //Serial.println(ble_data.tempExternal);
    }
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
  uint8_t power = 255 - data[1];

  // ON - Manual
  if(state == 1)
  {
    ble_on = true;
    ble_value = power;
  }
  // ON - Auto
  else if(state == 2)
  {
    if(power < 36)
    {
      ble_on = false;
      ble_value = 0;
    }
    else
    {
      ble_on = true;
      ble_value = map(power, 36, 255, 0, 255);
    }
  }
  // OFF
  else
  {
    ble_on = false;
    ble_value = 0;
  }
}
