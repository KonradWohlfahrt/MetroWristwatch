#ifndef MODES_h
#define MODES_h

// BASE CLASS
class WristwatchMode 
{
  public:
    WristwatchMode() {}
    virtual void onStart() = 0;
    virtual void onUpdate() = 0;
    virtual void onSetButton() = 0;
    virtual void onBothButtons() = 0;
};


// TIME MODE
class TimeDisplay : public WristwatchMode
{
    public:
        TimeDisplay() { }

        void onStart() 
        {
            showSeconds = false;
            anim(true);
        }
        void onUpdate() 
        { 
            Ds1302::DateTime now;
            rtc.getDateTime(&now);

            if (!showSeconds)
            {
                disp.setNumber(100 * now.hour + now.minute, true);
                
                if (isInterval(1000))
                    setColonLEDs(true);
                else
                    setColonLEDs(false);
            }
            else
            {
                disp.setNumber(now.second, false);
                if (now.second < 10)
                    disp.setDigit(2, disp.getNumber(0));
            }
            disp.show();

            if (isTimestamp(swapTimestamp, showSeconds ? 2500 : 4000))
            {
                showSeconds = !showSeconds;
                anim(true);
            }
        }
        void onSetButton() 
        { 
            showSeconds = !showSeconds;
            anim(true);
        }
        void onBothButtons() 
        { 
            showSeconds = false;
            settingsIndex = 0;
            anim(true);

            Ds1302::DateTime now;
            rtc.getDateTime(&now);
            settings[0] = now.minute;
            settings[1] = now.hour;
            settings[2] = now.second;

            uint32_t start = millis();
            uint32_t refreshDisp;
            while (millis() - start <= SLEEP_TIME)
            {
                if (isMode())
                {
                    bool both = false;
                    while (isMode())
                    {
                        if (!both && isSet())
                            both = true;
                        updateSettingsDisplay();
                        delay(2);
                    }

                    if (both)
                        break;

                    settingsIndex = (settingsIndex + 1) % 3;
                    if (settingsIndex != 1) 
                    {
                        showSeconds = !showSeconds;
                        anim(false);
                    }
                    start = millis();
                }

                if (isSet())
                {
                    changeSettings();
                    
                    uint32_t setStart = millis();
                    while (isSet())
                    {
                        if (millis() - setStart > 1000)
                        {
                            changeSettings();
                            delay(40);
                        }
                        updateSettingsDisplay();
                        delay(2);
                    }

                    start = millis();
                }
                
                if (isTimestamp(refreshDisp, DISP_REFRESH_TIME))
                    updateSettingsDisplay();
            }

            if (millis() - start <= SLEEP_TIME)
            {
                // set new rtc time
                rtc.getDateTime(&now);
                Ds1302::DateTime dt = {
                    .year = now.year,
                    .month = now.month,
                    .day = now.day,
                    .hour = settings[1],
                    .minute = settings[0],
                    .second = settings[2],
                    .dow = now.dow
                };
                rtc.setDateTime(&dt);
            }

            changeMode();
        }

    private:
        bool showSeconds = false;
        uint32_t swapTimestamp;

        void anim(bool useNow)
        {
            setColonLEDs(false);
            if (useNow)
            {
                Ds1302::DateTime now;
                rtc.getDateTime(&now);
                if (!showSeconds)
                    playAnimation(new uint8_t[4] { now.hour / 10 % 10, now.hour % 10, now.minute / 10 % 10, now.minute % 10 });
                else
                    playAnimation(new uint8_t[4] { 10, 10, now.second / 10 % 10, now.second % 10 });
            }
            else
            {
                if (!showSeconds)
                    playAnimation(new uint8_t[4] { settings[1] / 10 % 10, settings[1] % 10, settings[0] / 10 % 10, settings[0] % 10 });
                else
                    playAnimation(new uint8_t[4] { 10, 10, settings[0] / 10 % 10, settings[0] % 10 });
            }
            
            swapTimestamp = millis();
        }

        uint8_t settingsIndex; // 0=minutes, 1=hours, 2=seconds
        uint8_t settings[3];
        void updateSettingsDisplay()
        {
            if (settingsIndex != 2)
            {
                disp.setNumber(100 * settings[1] + settings[0], true);
                
                if (isInterval(1000))
                    setColonLEDs(true);
                else
                    setColonLEDs(false);

                if (!isInterval(500))
                {
                    if (settingsIndex == 1)
                    {
                        disp.setDigit(0, 0);
                        disp.setDigit(1, 0);
                    }
                    else
                    {
                        disp.setDigit(2, 0);
                        disp.setDigit(3, 0);
                    }
                }               
            }
            else
            {
                if (isInterval(500))
                {
                    disp.setNumber(settings[2], false);
                    if (settings[2] < 10)
                        disp.setDigit(2, disp.getNumber(0));
                }
                else
                    disp.clear();
            }

            disp.show();
            refreshPeripherals();
        }
        void changeSettings()
        {
            if (settingsIndex == 0)
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 60;
            else if (settingsIndex == 1)
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 24;
            else
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 60;
            updateSettingsDisplay();
        }
};

// DATE MODE
class DateDisplay : public WristwatchMode
{
    public:
        DateDisplay() { }

        void onStart() 
        { 
            currentDateDisp = 0;
            anim(true);
        }
        void onUpdate() 
        {
            Ds1302::DateTime now;
            rtc.getDateTime(&now);

            if (currentDateDisp == 0)
            {
                disp.setDigit(0, disp.getCharacter(SEG_Weekday[now.dow - 1][0]));
                disp.setDigit(1, disp.getCharacter(SEG_Weekday[now.dow - 1][1]));
            }
            else if (currentDateDisp == 1)
            {
                disp.setNumber(now.day * 100 + now.month, true);
                disp.setDigitSegment(1, 7, true);
            }
            else
            {
                disp.setNumber(2000 + now.year);
            }
            disp.show();

            if (isTimestamp(swapTimestamp, 1500))
            {
                currentDateDisp = (currentDateDisp + 1) % 3;
                anim(true);
            }
        }
        void onSetButton() 
        { 
            currentDateDisp = (currentDateDisp + 1) % 3;
            anim(true);
        }
        void onBothButtons() 
        {
            currentDateDisp = 0;
            settingsIndex = 0;
            anim(true);

            Ds1302::DateTime now;
            rtc.getDateTime(&now);
            settings[0] = now.dow - 1;
            settings[1] = now.day - 1;
            settings[2] = now.month - 1;
            settings[3] = now.year;

            uint32_t start = millis();
            uint32_t refreshDisp;
            while (millis() - start <= SLEEP_TIME)
            {
                if (isMode())
                {
                    bool both = false;
                    while (isMode())
                    {
                        if (!both && isSet())
                            both = true;
                        updateSettingsDisplay();
                        delay(2);
                    }

                    if (both)
                        break;

                    settingsIndex = (settingsIndex + 1) % 4;
                    if (settingsIndex != 2) 
                    {
                        currentDateDisp = (currentDateDisp + 1) % 3;
                        anim(false);
                    }
                    start = millis();
                }

                if (isSet())
                {
                    changeSettings();
                    
                    uint32_t setStart = millis();
                    while (isSet())
                    {
                        if (settingsIndex != 0 && millis() - setStart > 1000)
                        {
                            changeSettings();
                            delay(40);
                        }
                        updateSettingsDisplay();
                        delay(2);
                    }

                    start = millis();
                }
                
                if (isTimestamp(refreshDisp, DISP_REFRESH_TIME))
                    updateSettingsDisplay();
            }

            if (millis() - start <= SLEEP_TIME)
            {
                // set new rtc time
                rtc.getDateTime(&now);
                Ds1302::DateTime dt = {
                    .year = settings[3],
                    .month = settings[2] + 1,
                    .day = settings[1] + 1,
                    .hour = now.hour,
                    .minute = now.minute,
                    .second = now.second,
                    .dow = settings[0] + 1
                };
                rtc.setDateTime(&dt);
            }

            changeMode();
        }

    private:
        uint8_t currentDateDisp;
        uint32_t swapTimestamp;

        void anim(bool useNow)
        {
            if (useNow)
            {
                Ds1302::DateTime now;
                rtc.getDateTime(&now);
                if (currentDateDisp == 0)
                    playAnimation(new uint8_t[4] { SEG_Weekday[now.dow - 1][0], SEG_Weekday[now.dow - 1][1], 10, 10 });
                else if (currentDateDisp == 1)
                    playAnimation(new uint8_t[4] { now.day / 10 % 10, now.day % 10, now.month / 10 % 10, now.month % 10 });
                else
                    playAnimation(new uint8_t[4] { 2, 0, now.year / 10 % 10, now.year % 10 });
            }
            else
            {
                if (currentDateDisp == 0)
                    playAnimation(new uint8_t[4] { SEG_Weekday[settings[0]][0], SEG_Weekday[settings[0]][1], 10, 10 });
                else if (currentDateDisp == 1)
                    playAnimation(new uint8_t[4] { (settings[1] + 1) / 10 % 10, (settings[1] + 1) % 10, (settings[2] + 1) / 10 % 10, (settings[2] + 1) % 10 });
                else
                    playAnimation(new uint8_t[4] { 2, 0, settings[3] / 10 % 10, settings[3] % 10 });
            }
            
            swapTimestamp = millis();
        }

        uint8_t settingsIndex; // 0=weekday, 1=day, 2=month, 3=year
        uint8_t settings[4];
        void updateSettingsDisplay()
        {
            if (settingsIndex == 0)
            {
                if (isInterval(500))
                {
                    disp.setDigit(0, disp.getCharacter(SEG_Weekday[settings[0]][0]));
                    disp.setDigit(1, disp.getCharacter(SEG_Weekday[settings[0]][1]));
                }
                else
                    disp.clear();                
            }
            else if (settingsIndex == 1 || settingsIndex == 2)
            {
                disp.setNumber((settings[1] + 1) * 100 + (settings[2] + 1), true);
                disp.setDigitSegment(1, 7, true);

                if (!isInterval(500))
                {
                    if (settingsIndex == 1)
                    {
                        disp.setDigit(0, 0);
                        disp.setDigit(1, 0);
                    }
                    else
                    {
                        disp.setDigit(2, 0);
                        disp.setDigit(3, 0);
                    }
                }
            }
            else
            {
                if (isInterval(500))
                    disp.setNumber(2000 + settings[3]);
                else
                    disp.clear();
            }

            disp.show();
            refreshPeripherals();
        }
        void changeSettings()
        {
            if (settingsIndex == 0)
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 7;
            else if (settingsIndex == 1)
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 31;
            else if (settingsIndex == 2)
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 12;
            else
                settings[settingsIndex] = (settings[settingsIndex] + 1) % 100;
            updateSettingsDisplay();
        }
};

// TIMER MODE
class TimerDisplay : public WristwatchMode
{
    public:
        TimerDisplay() { }

        void onStart() 
        { 
            playAnimation(new uint8_t[4] { 0, 5, 0, 0 });
        }
        void onUpdate() 
        { 
            if (isInterval(500))
            {
                disp.setNumber(500, true);
                setColonLEDs(true);
            }
            else
            {
                disp.clear();
                setColonLEDs(false);
            }
            disp.show();
        }
        void onSetButton() 
        {
            uint32_t start = millis();
            uint32_t end = start + _timerValue;
            uint32_t refreshDisp;
            
            while (millis() - start <= _timerValue)
            {
                if (isSet() || isMode())
                {
                    while (isSet() || isMode())
                    {
                        showTimer(end - millis());
                        delay(2);
                    }
                    break;
                }

                if (isTimestamp(refreshDisp, 50))
                    showTimer(end - millis());

                if (millis() >= end)
                    break;

                delay(2);
            }

            if (millis() >= end)
            {
                setColonLEDs(false);
                for (uint8_t i = 0; i < 10; i++)
                {
                    if (i % 2 == 0)
                        disp.setSegments(new const uint8_t[4] { 0b01000000, 0b01000000, 0b01000000, 0b01000000 });
                    else
                        disp.clear();
                    disp.show();
                    delay(150);
                }
            }

            changeMode();
        }
        void onBothButtons() { }

    private:
        const uint32_t _timerValue = 300000;

        void showTimer(uint32_t remaining)
        {
            uint8_t seconds = remaining / 1000 % 60;
            uint8_t minutes = remaining / 1000 / 60 % 10;
            if (minutes > 0)
                disp.setNumber(minutes * 100 + seconds, true);
            else
                disp.setNumber(seconds * 100 + (remaining % 1000 / 10), true);
            
            uint16_t interval = 1000;
            if (minutes == 0 && seconds < 15)
                interval = 500;

            if (isInterval(interval))
                setColonLEDs(true);
            else 
            {
                setColonLEDs(false);
                if (interval == 500)
                    disp.clear();
            }

            disp.show();
            refreshPeripherals();
        }
};

#endif