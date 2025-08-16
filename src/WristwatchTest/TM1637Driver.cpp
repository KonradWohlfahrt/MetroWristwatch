#include "Arduino.h"
#include "TM1637Driver.h"

#define DISPLAY_LENGTH 4

#define TM1637_WRITE_DELAY 100

#define TM1637_DATA_COMMAND 0x40
#define TM1637_DISPLAY_CONTROL_COMMAND 0x80 
#define TM1637_ADDRESS_COMMAND 0xC0


TM1637::TM1637(uint8_t pinDIO, uint8_t pinCLK)
{
  _pinDIO = pinDIO;
  _pinCLK = pinCLK;

  pinMode(pinCLK, OUTPUT);
  pinMode(pinDIO, OUTPUT);
	digitalWrite(pinCLK, LOW);
	digitalWrite(pinDIO, LOW);
}

void TM1637::setBrightness(uint8_t brightness, bool refresh)
{
  _brightness = brightness & 0x07;
  if (refresh)
    show();
}
uint8_t TM1637::getBrightness()
{
  return _brightness;
}

void TM1637::enableDisplay(bool enable, bool refresh)
{
  _enabled = enable;
  if (refresh)
    show();
}
bool TM1637::enabled()
{
  return _enabled;
}

void TM1637::clear()
{
  for (int i = 0; i < DISPLAY_LENGTH; i++)
    _displayBuffer[i] = 0x00;
}
void TM1637::move(int8_t shift)
{
  if (shift == 0)
    return;

  if (shift > 0)
  {
    for (int8_t i = 0; i < DISPLAY_LENGTH; i++)
    {
      int8_t newDigit = i + shift;
      if (digitInRange(newDigit))
        _displayBuffer[i] = _displayBuffer[newDigit];
      else
        _displayBuffer[i] = 0x00;
    }
  }
  else
  {
    for (int8_t i = DISPLAY_LENGTH - 1; i >= 0; i--)
    {
      int8_t newDigit = i + shift;
      if (digitInRange(newDigit))
        _displayBuffer[i] = _displayBuffer[newDigit];
      else
        _displayBuffer[i] = 0x00;
    }
  }
}
void TM1637::setSegments(const uint8_t* segments)
{
  uint8_t size = sizeof(segments) / sizeof(segments[0]);
  for (uint8_t i = 0; i < DISPLAY_LENGTH; i++)
  {
    if (i <= size)
      _displayBuffer[i] = segments[i];
    else
      _displayBuffer[i] = 0x00;
  }
}
void TM1637::setNumber(int16_t number, bool leadZeros)
{
  bool negativ = number < 0;
  if (negativ)
    number = -number;
  bool begun = false;

  for (uint8_t i = 0; i < DISPLAY_LENGTH; i++)
  {
    if (negativ && leadZeros && i == 0)
    {
      _displayBuffer[i] = _segments[6]; // minus
      negativ = false;
      continue;
    }

    uint8_t digit = (number / (uint16_t)pow(10, DISPLAY_LENGTH - 1 - i)) % 10;
    if (!begun && digit == 0 && i < DISPLAY_LENGTH - 1)
    {
      if (leadZeros)
        _displayBuffer[i] = _numbers[digit];
      else 
      {
        digit = (number / (uint16_t)pow(10, DISPLAY_LENGTH - 2 - i)) % 10;
        if (negativ && digit != 0)
          _displayBuffer[i] = _segments[6];
        else
          _displayBuffer[i] = 0x00;
      }
    }
    else
    {
      _displayBuffer[i] = _numbers[digit];
      begun = true;
    }
  }
}
void TM1637::setDigit(uint8_t index, uint8_t digit)
{
  if (digitInRange(index))
    _displayBuffer[index] = digit;
}
uint8_t TM1637::getDigit(uint8_t index)
{
  if (digitInRange(index))
    return _displayBuffer[index];
  return 0x00;
}
void TM1637::setDigitSegment(uint8_t digitIndex, uint8_t segmentIndex, bool value)
{
  if (!digitInRange(digitIndex) || !segmentInRange(segmentIndex))
    return;
  
  if (value)
    _displayBuffer[digitIndex] = _displayBuffer[digitIndex] | _segments[segmentIndex];
  else
    _displayBuffer[digitIndex] = _displayBuffer[digitIndex] & (~_segments[segmentIndex]);
}
bool TM1637::getDigitSegment(uint8_t digitIndex, uint8_t segmentIndex)
{
  if (!digitInRange(digitIndex) || !segmentInRange(segmentIndex))
    return false;
  return (_displayBuffer[digitIndex] & _segments[segmentIndex]) != 0;
}

void TM1637::show()
{
  start();
  writeByte(TM1637_DATA_COMMAND); // indicate data write to display register
  stop();

  start();
  writeByte(TM1637_ADDRESS_COMMAND); // write display address command (start: C0H)
	for (uint8_t i = 0; i < DISPLAY_LENGTH; i++)
	  writeByte(_displayBuffer[i]); // write each digit data
	stop();

  start();
  writeByte(TM1637_DISPLAY_CONTROL_COMMAND | _brightness | (_enabled ? 0x08 : 0x00)); // write brightness & on/off
  stop();
}


uint8_t TM1637::getNumber(uint8_t number)
{
  if (number < 0 || number > 9)
    return 0x00;
  return _numbers[number];
}
uint8_t TM1637::getCharacter(char character)
{
  character = tolower(character);

  if (isAlpha(character))
    return _alphabet[(character - 97)];
  if (isDigit(character))
    return _numbers[(character - 48)];

  switch (character)
  {
    case '-':
      return _segments[6];
    case '.':
      return _segments[7];
    case '_':
      return _segments[3];
    case ',':
      return _specialCharacters[0];
    case '!':
      return _specialCharacters[1];
    case '?':
      return _specialCharacters[2];
    case '=':
      return _specialCharacters[3];
    case '>':
      return _specialCharacters[4];
    case '<':
      return _specialCharacters[5];
    case '(':
      return _specialCharacters[6];
    case ')':
      return _specialCharacters[7];
    case '/':
      return _specialCharacters[8];
    case '*':
      return _specialCharacters[9];
    case '"':
      return _specialCharacters[10];
    case '^':
      return _specialCharacters[11];
  }

  return 0x00;
}


bool TM1637::digitInRange(int8_t digit)
{
  return 0 <= digit && digit < DISPLAY_LENGTH;
}
bool TM1637::segmentInRange(int8_t segment)
{
  return 0 <= segment && segment < 8;
}

void TM1637::start()
{
  digitalWrite(_pinDIO, LOW); // pull dio low: indicate data input
  writeDelay();
}
void TM1637::stop()
{
  digitalWrite(_pinDIO, LOW); // keep dio low for one clk pulse
	writeDelay();

	digitalWrite(_pinCLK, HIGH); // clk high
	writeDelay();
	digitalWrite(_pinDIO, HIGH); // dio high
	writeDelay();
}
bool TM1637::writeByte(uint8_t data)
{
  // write byte to tm1637 via two-wire-interface
  uint8_t pointer = 1;
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(_pinCLK, LOW); // clk pulse low
    writeDelay();

    digitalWrite(_pinDIO, (data & pointer) > 0 ? HIGH : LOW); // write bit on dio
    writeDelay();

    digitalWrite(_pinCLK, HIGH); // clk pulse high
    writeDelay();

    pointer = pointer << 1; // move to next bit pointer
  }

  // wait for ack, dio high (pulled down by tm1637)
  digitalWrite(_pinCLK, LOW);
  digitalWrite(_pinDIO, HIGH);
  writeDelay();

  digitalWrite(_pinCLK, HIGH); // clk pulse high
  writeDelay();

  // read ack signal
  uint8_t ack = digitalRead(_pinDIO);
  if (ack == 0)
    digitalWrite(_pinDIO, LOW);
  writeDelay();

  digitalWrite(_pinCLK, LOW);
  writeDelay();

  return ack;
}
void TM1637::writeDelay()
{
  delayMicroseconds(TM1637_WRITE_DELAY);
}
