#include <Adafruit_TinyUSB.h> // for serial

#define SET_BTN 6
#define MODE_BTN 7
bool isSet() { return digitalRead(SET_BTN) == 0; }
bool isMode() { return digitalRead(MODE_BTN) == 0; }


#define LDR_PIN A0
#define LOW_LIGHT 600
#define BRIGHT_LIGHT 3000
// get value from lowerLimit (low light) to upperLimit (bright light)
uint16_t getBrightness(uint16_t lowerLimit, uint16_t upperLimit) 
{
  uint16_t ldr_value = analogRead(LDR_PIN); // read adc
  ldr_value = constrain(ldr_value, LOW_LIGHT, BRIGHT_LIGHT); // clamp values
  return (uint16_t)map(ldr_value, LOW_LIGHT, BRIGHT_LIGHT, lowerLimit, upperLimit); // map to new range
}
// get value from 0 (low light) to 255 (bright light)
uint8_t getBrightness()
{
  return getBrightness(0, 255);
}


#define COLON_LEDS_PIN 3
void setColonLEDs(bool value) { analogWrite(COLON_LEDS_PIN, value ? getBrightness(50, 255) : 0); }
#define BLUE_LED_PIN 4
void setBlueLED(bool value) { analogWrite(BLUE_LED_PIN, value ? getBrightness() : 0); }


// https://github.com/Treboada/Ds1302, warning can be ignored, works perfectly fine
#include <Ds1302.h>
#define PIN_ENA 10
#define PIN_DAT 9
#define PIN_CLK 8
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

// https://github.com/honvl/Seeed-Xiao-NRF52840-Battery
#include "xiaobattery.h"
Xiao battery;

// included, no library needed
#include "TM1637Driver.h"
#define PIN_SCK 1
#define PIN_DIO 2
TM1637 disp(PIN_DIO, PIN_SCK);


bool isInterval(uint16_t interval) { return millis() % interval > (interval / 2); }
bool isTimestamp(uint32_t& timestamp, uint32_t interval) 
{
  if (millis() - timestamp >= interval)
  {
    timestamp = millis();
    return true;
  }
  return false;
}