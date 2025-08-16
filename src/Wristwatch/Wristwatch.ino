#include "preferences.h"

#define DISP_REFRESH_TIME 100

const char SEG_Weekday[7][2] = {
  ///* ENG
  { 'm', 'o' },
  { 't', 'u' },
  { 'w', 'e' },
  { 't', 'h' },
  { 'f', 'r' },
  { 's', 'a' },
  { 's', 'u' }
  //*/

  /* GER
  { 'm', 'o' },
  { 'd', 'i' },
  { 'm', 'i' },
  { 'd', 'o' },
  { 'f', 'r' },
  { 's', 'a' },
  { 's', 'o' }
  //*/
};

void checkBrightness(bool ignore = false);
void refreshPeripherals();
void refreshMode();
void changeMode();
void showBatteryPercent();
void checkBattery(bool ignore = false);


uint32_t _brightnessTimestamp;
void checkBrightness(bool ignore)
{
  // update brightness of the display every 1s
  if (isTimestamp(_brightnessTimestamp, 1000) || ignore)
    disp.setBrightness(getBrightness(0, 7), false);
}
void checkBlueLED()
{
  // divide by 2 to lower brightness & save energy
  analogWrite(BLUE_LED_PIN, getBrightness() / 2);
}

#define SLEEP_TIME 10000
uint32_t _interactionTimestamp;
void startDeepSleep()
{
  // configure wakeup pins
  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[SET_BTN], NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[MODE_BTN], NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
  delay(500);
  // go to deep sleep
  sd_power_system_off();
  NRF_POWER->SYSTEMOFF = 1;
}
void checkSleepmode()
{
  if (isTimestamp(_interactionTimestamp, SLEEP_TIME))
  {
    // show battery percent if below or equal to 20%
    if (battery.GetBatteryPercent() <= 20)
    {
      playAnimation(new uint8_t[4] { 'l', 'o', 'w', 10 });
      delay(500);
      playAnimation(new uint8_t[4] { 'b', 'a', 't', 10 });
      delay(500);
      showBatteryPercent();
      delay(500);
    }

    playAnimation(new uint8_t[4] { 10, 10, 10, 10 });

    disp.enableDisplay(false);
    setColonLEDs(false);
    setBlueLED(false);

    startDeepSleep();
  }
}


#define BAT_CHECK_TIME 1000
uint32_t _batCheckTimestamp;
void showBatteryPercent()
{
  uint8_t p = battery.GetBatteryPercent();
  if (p == 100)
    playAnimation(new uint8_t[4] { 10, 1, 0, 0 });
  else if (p >= 10)
    playAnimation(new uint8_t[4] { 10, 10, p / 10, p % 10 });
  else
    playAnimation(new uint8_t[4] { 10, 10, 10, p });
}
void checkBattery(bool ignore)
{
  if (isTimestamp(_batCheckTimestamp, BAT_CHECK_TIME) || ignore)
  {
    // check if the battery is charging
    if (battery.IsChargingBattery()) 
    {
      showBatteryPercent();

      setBlueLED(false);
      setColonLEDs(false);
      disp.setBrightness(7, false);

      uint8_t animIndex = 0;
      const uint8_t animSegmentIndex[4] = { 0, 1, 6, 5 };
      while (battery.IsChargingBattery())
      {
        // show only even percentage (reduce flickering)
        uint8_t p = battery.GetBatteryPercent();
        if (p % 2 != 0)
          p -= 1;
        disp.setNumber(p);

        disp.setDigitSegment(0, animSegmentIndex[animIndex], true);
        disp.show();
        animIndex = (animIndex + 1) % 4;

        delay(75);
      }

      NVIC_SystemReset();
    }
    // check if the battery is low
    if (battery.GetBatteryPercent() <= 10)
    {
      disp.setBrightness(0, false);

      playAnimation(new uint8_t[4] { 'l', 'o', 'w', 10 });
      delay(500);
      playAnimation(new uint8_t[4] { 'b', 'a', 't', 10 });
      delay(500);
      showBatteryPercent();
      delay(500);
      playAnimation(new uint8_t[4] { 10, 10, 10, 10 });
      
      disp.enableDisplay(false);
      setColonLEDs(false);
      setBlueLED(false);

      startDeepSleep();
    }
  }
}


#include "Modes.h"
const int _modesCount = 3;
uint8_t _currentModeIndex = 0;
WristwatchMode * _modes[] = { new TimeDisplay(), new DateDisplay(), new TimerDisplay() };
WristwatchMode * _currentMode;

uint32_t refreshTimestamp;

void setup() 
{
  //Serial.begin(115200);

  pinMode(SET_BTN, INPUT);
  pinMode(MODE_BTN, INPUT);
  pinMode(LDR_PIN, INPUT);

  pinMode(COLON_LEDS_PIN, OUTPUT);
  setColonLEDs(false);
  pinMode(BLUE_LED_PIN, OUTPUT);
  setBlueLED(false);

  analogReadResolution(12); // Can be 8, 10, 12 or 14 bit

  // INITIALIZE RTC
  rtc.init();
  if (rtc.isHalted())
  {
    Ds1302::DateTime dt = {
      .year = 33,
      .month = Ds1302::MONTH_JAN,
      .day = 1,
      .hour = 12,
      .minute = 0,
      .second = 0,
      .dow = Ds1302::DOW_SAT
    };
    rtc.setDateTime(&dt);
  }

  checkBattery(true);
  checkBrightness(true);
  changeMode();
}
void loop() 
{
  // MODE BUTTON - switch between modes
  if (isMode())
  {
    bool both = false;
    while (isMode())
    {
      refreshMode();
      if (!both && isSet())
        both = true;
      delay(2);
    } 

    if (!both) 
    {
       _currentModeIndex = (_currentModeIndex + 1) % _modesCount;
      changeMode();
    }
    else
      _currentMode->onBothButtons();
   
    _interactionTimestamp = millis();
  }

  // SET BUTTON
  if (isSet())
  {
    while (isSet())
    {
      refreshMode();
      delay(2);
    }

    _currentMode->onSetButton();
    _interactionTimestamp = millis();
  }

  refreshMode();
}


void refreshPeripherals()
{
  checkBrightness();
  checkBlueLED();
  checkBattery();
}
void refreshMode()
{
  if (isTimestamp(refreshTimestamp, DISP_REFRESH_TIME))
  {
    _currentMode->onUpdate();
    checkSleepmode();
  }
  refreshPeripherals();
}
void changeMode()
{
  _currentMode = _modes[_currentModeIndex];
  _currentMode->onStart();
  setColonLEDs(false);
}