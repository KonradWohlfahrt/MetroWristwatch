#ifndef TM1637Driver_h
#define TM1637Driver_h

#include "Arduino.h"

class TM1637
{
  public:
    //! Create an TM1637 object with data and clock pins for the two-wire-interface.
    //!
    //! @param pinDIO The digital pin connected to data of the chip
    //! @param pinCLK The digital pin connected to clock of the chip
    TM1637(uint8_t pinDIO, uint8_t pinCLK);


    //! Sets the brightness of the display.
    //!
    //! @param brightness The new brightness (0 = lowest, 7 = highest)
    //! @param refresh Update brightness directly, otherwise new data has to be displayed for change
    void setBrightness(uint8_t brightness, bool refresh = true);
    //! Get the current brightness of the display.
    //!
    //! @return Current brightness from 0 to 7
    uint8_t getBrightness();


    //! Enable or disable the display.
    //! If turned on, last data will be displayed.
    //!
    //! @param enable Turn on display (default true)
    //! @param refresh Turn display on/off directly, otherwise new data has to be displayed for change
    void enableDisplay(bool enable = true, bool refresh = true);
    //! Get the state of the display (false = off, true = on).
    //!
    //! @return False if off, true if on
    bool enabled();


    //! Clears the display completely.
    void clear();
    //! Shift the segments to the left or right by a specific amount.
    //! Empty spaces will be filled with 0x00.
    //!
    //! @param shift Shift segments to the left (> 0) or right (< 0) by the absolute value
    void move(int8_t shift);
    //! Set the digit segments.
    //!
    //! @param segments The different display segments
    void setSegments(const uint8_t* segments);
    //! Show a number on the display.
    //!
    //! @param number The number to display
    //! @param leadZeros Should the remaining space be filled with zeros
    void setNumber(int16_t number, bool leadZeros = false);
    //! Set a specific digit.
    //!
    //! @param index The index of the digit
    //! @param digit The digit to set
    void setDigit(uint8_t index, uint8_t digit);
    //! Get a digit from the display buffer.
    //!
    //! @param index The index of the digit
    //! @return The digit in the display buffer
    uint8_t getDigit(uint8_t index);
    //! Activate/Deactivate one segment of one digit.
    //!
    //! @param digitIndex The index of the digit
    //! @param segmentIndex The index of the segment (0=A, ..., 7=DP)
    //! @param value Enable or disable the segment
    void setDigitSegment(uint8_t digitIndex, uint8_t segmentIndex, bool value);
    //! Get the state of one segment of one digit.
    //!
    //! @param digitIndex The index of the digit
    //! @param segmentIndex The index of the segment (0=A, ..., 7=DP)
    //! @return True if enabled, false if inactive
    bool getDigitSegment(uint8_t digitIndex, uint8_t segmentIndex);

    //! Display the data in the data buffer.
    void show();

    //! Get the encoded segments of a number.
    //!
    //! @param number The number to encode
    //! @return The encoded segments
    uint8_t getNumber(uint8_t number);
    //! Get the encoded segments of a number.
    //!
    //! @param character The character to encode
    //! @return The encoded segments
    uint8_t getCharacter(char character);

    //! Check if a digit is in range.
    //!
    //! @param digit Index of the digit
    //! @return True if in range, false if not
    bool digitInRange(int8_t digit);
    //! Check if a digit is in range.
    //!
    //! @param segment Index of the segment
    //! @return True if in range, false if not
    bool segmentInRange(int8_t segment);

  private:

    uint8_t _pinDIO;
    uint8_t _pinCLK;

    uint8_t _brightness = 7;
    bool _enabled = true;

    uint8_t _displayBuffer[4] = { 0x00, 0x00, 0x00, 0x00 };
    
    void start();
    void stop();
    bool writeByte(uint8_t data);
    void writeDelay();

    const uint8_t _segments[8] = {
      0b00000001,   // A
      0b00000010,   // B
      0b00000100,   // C
      0b00001000,   // D
      0b00010000,   // E
      0b00100000,   // F
      0b01000000,   // G
      0b10000000    // DP
    };
    const uint8_t _numbers[10] = {
      0b00111111,   // 0
      0b00000110,   // 1
      0b01011011,   // 2
      0b01001111,   // 3
      0b01100110,   // 4
      0b01101101,   // 5
      0b01111101,   // 6
      0b00000111,   // 7
      0b01111111,   // 8
      0b01101111,   // 9
    };
    const uint8_t _alphabet[26] = { 
      0b01110111,   // A
      0b01111100,   // b
      0b01011000,   // c
      0b01011110,   // d
      0b01111001,   // E
      0b01110001,   // F
      0b00111101,   // G
      0b01110100,   // h
      0b00000100,   // i
      0b00011110,   // J
      0b01110101,   // k
      0b00111000,   // L
      0b01010101,   // m
      0b01010100,   // n
      0b01011100,   // o
      0b01110011,   // P
      0b01100111,   // q
      0b01010000,   // r
      0b00101101,   // S
      0b01111000,   // t
      0b00011100,   // u
      0b00101010,   // v
      0b01101010,   // w
      0b01110110,   // X
      0b01101110,   // y
      0b00011011    // Z
    };
    const uint8_t _specialCharacters[12] = {
      0b00001100,   // ,
      0b10000010,   // !
      0b01010011,   // ?
      0b01001000,   // =
      0b01001100,   // >
      0b01011000,   // <
      0b00111001,   // (
      0b00001111,   // )
      0b01010010,   // /
      0b01100011,   // *
      0b00100010,   // "
      0b00100011    // ^
    };
};
#endif