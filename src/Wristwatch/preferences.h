//#include <Adafruit_TinyUSB.h> // for serial

#define SET_BTN 6
#define MODE_BTN 7
bool isSet() { return digitalRead(SET_BTN) == 0; }
bool isMode() { return digitalRead(MODE_BTN) == 0; }


#define LDR_PIN A0
#define LOW_LIGHT 500
#define BRIGHT_LIGHT 3250
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

// https://github.com/honvl/Seeed-Xiao-NRF52840-Battery, already included (changed a bit)
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



// --- ANIMATION SECTION ---
// only number or lower case characters, two animation types to select from
// animation range: 0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z

/* ALL DIGITS AT ONCE
#define ANIMATION_STEPS 5
void playAnimation(uint8_t digits[4])
{
  uint8_t currentNumber[4];
  bool isOff[4] = { false, false, false, false };
  for (uint8_t i = 0; i < 4; i++)
  {
    // character
    if (digits[i] >= 97)
      digits[i] -= 87; // 10 = a, 11 = b, ...
    // off
    else if (digits[i] >= 10) 
    {
      digits[i] = ANIMATION_STEPS;
      isOff[i] = true;
    }

    currentNumber[i] = digits[i] - ANIMATION_STEPS;
    if (currentNumber[i] > 35) // unsigned byte -> no negative numbers
      currentNumber[i] += 36;
  }
  
  for (uint8_t i = 0; i < ANIMATION_STEPS + 1; i++)
  {
    for (uint8_t d = 0; d < 4; d++)
    {
      if (isOff[d] && currentNumber[d] == digits[d])
          disp.setDigit(d, 0);
      else if (currentNumber[d] < 10)
        disp.setDigit(d, disp.getNumber(currentNumber[d]));
      else
        disp.setDigit(d, disp.getCharacter(currentNumber[d] + 87));
      
      currentNumber[d] = (currentNumber[d] + 1) % 36;
    }
    
    disp.show();
    delay(25);
  }
}
//*/

///* DIGITS ONCE AT A TIME
#define ANIMATION_STEPS 3
void playAnimation(uint8_t digits[4])
{
  uint8_t currentNumber[4];
  bool isOff[4] = { false, false, false, false };
  for (uint8_t i = 0; i < 4; i++)
  {
    // character
    if (digits[i] >= 97)
      digits[i] -= 87; // 10 = a, 11 = b, ...
    // off
    else if (digits[i] >= 10) 
    {
      digits[i] = ANIMATION_STEPS;
      isOff[i] = true;
    }

    currentNumber[i] = digits[i] - ANIMATION_STEPS;
    if (currentNumber[i] > 35) // unsigned byte -> no negative numbers
      currentNumber[i] += 36;
  }
  
  for (uint8_t d = 0; d < 4; d++)
  {
    for (uint8_t i = 0; i < ANIMATION_STEPS + 1; i++)
    {
      if (isOff[d] && currentNumber[d] == digits[d])
          disp.setDigit(d, 0);
      else if (currentNumber[d] < 10)
        disp.setDigit(d, disp.getNumber(currentNumber[d]));
      else
        disp.setDigit(d, disp.getCharacter(currentNumber[d] + 87));
      
      currentNumber[d] = (currentNumber[d] + 1) % 36;
      
      disp.show();
      delay(10);
    }
  }
}
//*/