![Cover](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/Cover_1.jpg)

# METRO WRISTWATCH
**Artyom’s Watch - Metro Series Inspired Wristwatch**

Ever wanted to build a piece of your favorite video game in real life? This project brings Artyom’s iconic wristwatch from the Metro game series into the real world! Designed to be both nostalgic and functional, this DIY gadget keeps time, adapts screen brightness to the environment, and stays true to the post-apocalyptic style of the original. Some design tweaks were made to simplify the build: 7-segment displays instead of nixie tubes, microcontroller is housed separately to save PCB space.
A full build guide is available at instructables: [https://www.instructables.com/Artyoms-Wristwatch-From-Metro/](https://www.instructables.com/Artyoms-Wristwatch-From-Metro/)

If you build your own version, feel free to share it or fork this project - I’d love to see what you come up with!


***
# FEATURES
- Low power microcontroller (XIAO nRF52840)
- 7-segment LED display
- DS1302 real time clock IC, date and time display
- 5-minute timer
- Adjustable display brightness with LDR
- 6+ days battery life in testing


***
# PROGRAMMING
Programmed with the Arduino IDE and [XIAO nRF52840 board manager](https://wiki.seeedstudio.com/XIAO_BLE/#getting-started). Only one external library is required, everything else is included: [Ds1302](https://github.com/Treboada/Ds1302).


***
# ELECTRIC COMPONENTS:
| Component | Amount | Silkscreen label |
|:----------|:------:|-----------------:|
| Custom PCB | 1 | - |
| TH CR927 Battery Clip | 1 | BT1 |
| 100nF 0805 | 2 | C1,C2 |
| PH2.0 2 Pin Connector | 1 | CN1 |
| 4x 7-segment LED, Yellow (Everlight ELSS406) | 4 | LED1-4 |
| 0805 SMD LED, Yellow | 2 | LED5,LED6 |
| 5mm LED, Blue | 1 | - |
| MMBT2222A | 2 | Q1,Q2 |
| 7mm LDR | 2 | - |
| 1k 0805 | 3 | R2,R6,R9 |
| 10k 0805 | 2 | R3,R4 |
| 220r 0805 | 3 | R5,R7,R8 |
| Horizontal tactile switch 6x6x3mm | 2 | SW1,SW2 |
| SK12D07VG3 | 1 | SW3 |
| TM1637 | 1 | U1 |
| DS1302 | 1 | U2 |
| 32.768kHz Crystal 6pF | 1 | X1 |
| XIAO nRF52840 | 1 | - |
| 100mAh LiPo Battery | 1 | - |


***
# GALLERY
![Schematic](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/Schematic_MetroWristwatch.png)
_Schematic of the Focus Wheel_
![PCB Lables Front](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/PCB_Front_Labled.png)
_SMD component placement reference (front)_
![PCB Lables Back](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/PCB_Back_Labled.png)
_SMD component placement reference (back)_
![Housing](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/Housing.jpg)
_3D printed and spray painted housing_


***
# FUNCTIONS
The button facing towards you is the **MODE** button, and the one facing away from you is the **SET** button. The **MODE** button is used to navigate the different menues (Time -> Date -> Timer) and changing the selected settings. The **SET** button switches between the displayed information (Time: HH:MM -> xx:SS; Date: Weekday -> DD.MM -> YYYY), starts the timer and changes the selected setting (hold for fast-forward). Press and hold **MODE**, then press **SET** to enter or exit the settings menu of the current mode.

The watch will return to deep sleep after a few seconds to preserve power, it can be woken up by pressing any button. Once the battery runs low, a message will appear before entering sleep mode. If too low, the watch will instantly go back to sleep mode. Charging the battery through the USB-C port will show the current percentage and an charging animation.


![Metro Wristwatch](https://github.com/KonradWohlfahrt/MetroWristwatch/blob/main/img/Cover_2.jpg)