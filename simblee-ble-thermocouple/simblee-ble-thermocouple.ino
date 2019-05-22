// brewce UUID: b5e9d1f2-cdb3-4758-a1b0-1d6ddd22dd0d

#include <SimbleeBLE.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define LED_PIN     19

#define MAX_SO_PIN  10
#define MAX_CS_PIN  13
#define MAX_SCK_PIN 14

//Create a MAX31855 reference and tell it what pin does what
Adafruit_MAX31855 kTC(MAX_SCK_PIN, MAX_CS_PIN, MAX_SO_PIN);

bool isConnected = false;

struct
{
  float    tempInternal;
  float    tempExternal;
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
  SimbleeBLE.iBeaconMajor = 2;
  SimbleeBLE.iBeaconMinor = 1;
  SimbleeBLE.iBeaconMeasuredPower = 0xC6;
  
  //SimbleeBLE.txPowerLevel = 4;
  //SimbleeBLE.advertisementInterval = 900;
  
  SimbleeBLE.begin(); // start the BLE stack

  //SimbleeBLE_device_name = "brewce_cooker";
  //SimbleeBLE_begin();
  
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN,  HIGH);
  Simblee_ULPDelay( 10 );
  digitalWrite(LED_PIN,  LOW);
}

void loop() {

  Simblee_ULPDelay( 1000 );
  
  if(!isConnected)
    return;
  
  ble_data.tempInternal = kTC.readInternal();
  ble_data.tempExternal = kTC.readCelsius();
  if(ble_data.tempExternal == ble_data.tempExternal)
  {
    SimbleeBLE.send((char *)&ble_data, sizeof(ble_data));
  }
}

void SimbleeBLE_onConnect()
{
  isConnected = true;
}

void SimbleeBLE_onDisconnect()
{
  isConnected = false;
}

void SimbleeBLE_onReceive(char *data, int len) {
}
