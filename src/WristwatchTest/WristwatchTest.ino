#include "preferences.h"

const uint8_t SEG_Weekday[7][4] = {
  // LSB: A, MSB: DP, 0 off, 1 on    DP G F E D C B A
  { 0b01010100, 0b01010100, 0b01011100, 0b00000000 },
  { 0b01111000, 0b00011100, 0b00000000, 0b00000000 },
  { 0b00011100, 0b00011100, 0b01111001, 0b00000000 },
  { 0b01111000, 0b01110100, 0b00000000, 0b00000000 },
  { 0b01110001, 0b01010000, 0b00000000, 0b00000000 },
  { 0b01101101, 0b01110111, 0b00000000, 0b00000000 },
  { 0b01101101, 0b00011100, 0b00000000, 0b00000000 }
};

void setup() 
{
  Serial.begin(115200);

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

  // CHECK BLUE LED
  for (int i = 0; i < 255; i++)
  {
    analogWrite(BLUE_LED_PIN, i);
    delay(5);
  }
  for (int i = 255; i >= 0; i--)
  {
    analogWrite(BLUE_LED_PIN, i);
    delay(5);
  }

  // CHECK COLON LEDS
  for (int i = 0; i < 255; i++)
  {
    analogWrite(COLON_LEDS_PIN, i);
    delay(5);
  }
  for (int i = 255; i >= 0; i--)
  {
    analogWrite(COLON_LEDS_PIN, i);
    delay(5);
  }

  // SHOW WEEKDAYS
  disp.setBrightness(7);
  for (int i = 0; i < 7; i++)
  {
    disp.setSegments(SEG_Weekday[i]);
    disp.show();
    delay(500);
  }

  // CHECK BRIGHTNESS
  for (int i = 7; i >= 0; i--)
  {
    disp.setBrightness(i);
    disp.show();
    delay(250);
  }

  // CHECK NUMBER DISPLAY
  disp.setBrightness(7);
  disp.setNumber(0); disp.show(); // ___0
  delay(500);
  disp.setNumber(0, true); disp.show(); // 0000
  delay(500);
  disp.setNumber(12); disp.show(); // __12
  delay(500);
  disp.setNumber(12, true); disp.show(); // 0012
  delay(500);
  disp.setNumber(123); disp.show(); // _123
  delay(500);
  disp.setNumber(123, true); disp.show(); // 0123
  delay(500);
  disp.setNumber(1234); disp.show(); // 1234
  delay(500);
  disp.setNumber(1234, true); disp.show(); // 1234
  delay(500);
  disp.setNumber(-1); disp.show(); // __-1
  delay(500);
  disp.setNumber(-1, true); disp.show(); // -001
  delay(500);
  disp.setNumber(-12); disp.show(); // _-12
  delay(500);
  disp.setNumber(-12, true); disp.show(); // -012
  delay(500);
  disp.setNumber(-123); disp.show(); // -123
  delay(500);
  disp.setNumber(-123, true); disp.show(); //-123
  delay(500);

  // CHECK ALPHABET
  disp.clear();
  for (char i = 97; i < 123; i++) // see ascii table for reference
  {
    disp.setDigit(0, disp.getCharacter(i));
    disp.show();
    delay(250);
  }
}

uint8_t last_second = 0;
uint8_t selected_mode = 0;
uint32_t lastTimestamp;
void loop() 
{
  Ds1302::DateTime now;
  rtc.getDateTime(&now);

  // MODE BUTTON
  if (isMode())
  {
    while (isMode()) delay(2);

    selected_mode = (selected_mode + 1) % 7;
    setColonLEDs(false);
  }

  // SET BUTTON
  if (isSet())
  {
    while (isSet()) delay(2);

    selected_mode = (selected_mode - 1) < 0 ? 6 : (selected_mode - 1);
    setColonLEDs(false);
  }

  // MODE DISPLAY
  if (isTimestamp(lastTimestamp, 100))
  {
    switch (selected_mode)
    {
      // hh:mm
      case 0:
        disp.setNumber(now.hour * 100 + now.minute, true);
        if (isInterval(1000))
          setColonLEDs(true);
        else
          setColonLEDs(false);
        break;

      // __ss
      case 1:
        disp.setNumber(now.second); 
        break;

      // mo, tu, we, th, fr, sa, su
      case 2:
        disp.setSegments(SEG_Weekday[now.dow - 1]);
        break;

      // dd.mm
      case 3:
        disp.setNumber(now.day * 100 + now.month, true);
        disp.setDigitSegment(1, 7, true);
        break;

      // yyyy
      case 4:
        disp.setNumber(2000 + now.year);
        break;

      // ldr value
      case 5:
        disp.setNumber(analogRead(LDR_PIN));
        break;

      // seconds since boot
      case 6:
        disp.setNumber(millis() / 1000);
        break;
    }

    disp.show();
  }

  printData(now);
  setBlueLED(true);
}

void printData(Ds1302::DateTime& now)
{
  // PRINT TIME
  Serial.print(now.day);     // 01-31
  Serial.print('.');
  Serial.print(now.month);   // 01-12
  Serial.print('.');
  Serial.print(now.year);    // 00-99
  Serial.print("-");
  Serial.print(now.dow); // 1-7
  Serial.print("-");
  Serial.print(now.hour);    // 00-23
  Serial.print(':');
  Serial.print(now.minute);  // 00-59
  Serial.print(':');
  Serial.print(now.second);  // 00-59
  Serial.print('\t');

  // PRINT BUTTONS - NOTE: BUTTON PRESS BLOCKS SERIAL PRINT
  Serial.print(isSet());
  Serial.print('\t');
  Serial.print(isMode());
  Serial.print('\t');

  // PRINT LDR & BRIGHTNESS
  Serial.print(analogRead(LDR_PIN));
  Serial.print('\t');
  Serial.print(getBrightness());
  Serial.print('\t');

  Serial.print(battery.GetBatteryVoltage());
  Serial.print('\t');
  Serial.print(battery.GetBatteryPercent());
  Serial.print('\t');
  Serial.println(battery.IsChargingBattery());
}
