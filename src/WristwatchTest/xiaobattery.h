#include <Arduino.h>
#include <bluefruit.h>

#define BAT_HIGH_CHARGE 22  // HIGH for 50mA, LOW for 100mA
#define BAT_CHARGE_STATE 23 // LOW for charging, HIGH not charging
#define VBAT_MV_PER_LBS (0.00087890625F) // 3.6 reference / 12 bit resolution

class Xiao 
{
  public:
    Xiao();
    float GetBatteryVoltage();
    uint8_t GetBatteryPercent();
    bool IsChargingBattery();
};

Xiao::Xiao() 
{
  pinMode(VBAT_ENABLE, OUTPUT);
  pinMode(BAT_CHARGE_STATE, INPUT);

  digitalWrite(BAT_HIGH_CHARGE, HIGH); // charge with 50mA
}
float Xiao::GetBatteryVoltage() 
{
  digitalWrite(VBAT_ENABLE, LOW);

  uint32_t adcCount = 0;
  for (uint8_t i = 0; i < 10; i++)
  {
    adcCount += analogRead(PIN_VBAT);
    delay(2);
  }
  adcCount /= 10;
  float adcVoltage = adcCount * VBAT_MV_PER_LBS;
  float vBat = adcVoltage * (1510.0 / 510.0); // voltage divider: 1M + 510k

  digitalWrite(VBAT_ENABLE, HIGH);

  return vBat;
}
uint8_t Xiao::GetBatteryPercent()
{
  // 3.3V = 0%, 4.13V = 100%
  float percent = 120.482F * GetBatteryVoltage() - 397.590F; // simple linear function f(x)=mx+b, f(3.3V)=0, f(4.13V)=100
  return constrain((uint8_t)percent, 0, 100); 
}
bool Xiao::IsChargingBattery() { return digitalRead(BAT_CHARGE_STATE) == LOW; }