#include <Wire.h>
#include <SimbleeForMobile.h>

// UI elements
uint8_t sl_power;
uint8_t sw_io;

// values
uint8_t power_lvl = 255;
uint8_t io = 0;

void setup()
{
  Wire.begin();
  
  SimbleeForMobile.deviceName = "HobC";
  SimbleeForMobile.advertisementData = "Hob Control";
  SimbleeForMobile.begin();
}

void loop()
{
  SimbleeForMobile.process();
}

void ui()
{
  SimbleeForMobile.beginScreen(rgb(85,85,85));

  const int y_inc = 100;
  
  int y_offset = 10;

  char buf[10];
  sprintf(buf, "Power:");
  SimbleeForMobile.drawText(10, y_offset, buf, BLACK, 25);
  sl_power = SimbleeForMobile.drawSlider(0, y_offset + 50, SimbleeForMobile.screenWidth, 0, 255);

  y_offset += y_inc;
  int x_offset = 25;
  
  SimbleeForMobile.drawText(x_offset, y_offset, "I/O:", BLACK, 25);
  sw_io = SimbleeForMobile.drawSwitch(x_offset + 90, y_offset);

  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event)
{
  if(event.id == sl_power)
  {
    power_lvl = ((uint8_t) event.value);
  }

  if(event.id == sw_io)
  {
    io = ((uint8_t) event.value);
  }

  Wire.beginTransmission(69);
  Wire.write(io);Wire.write(power_lvl);
  Wire.endTransmission();
}
