#include <Arduino.h>
#include <Wire.h>
#include "DS3231.h"

// reads DS2321 registers and returns time object
Time DS3231::getTime()
{
  Time t;

  // hours
  t.hour = _readRegister(DS3231_HOURS);

  // minuets
  t.min = _readRegister(DS3231_MINUTES);

  // seconds
  t.sec = _readRegister(DS3231_SECONDS);

  // return object
  return t;
}

Date DS3231::getDate()
{
  Date d;

  // days
  d.day = _readRegister(DS3231_DAY);

  // months
  d.month = _readRegister(DS3231_CEN_MONTH);

  // years
  d.year = _readRegister(DS3231_DEC_YEAR);

  // return object
  return d;
}

// sets hours, minutes, and seconds
void DS3231::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
  _writeRegister(DS3231_HOURS, hour);
  _writeRegister(DS3231_MINUTES, min);
  _writeRegister(DS3231_SECONDS, sec);
}

// sets days, months, and years
void DS3231::setDate(uint8_t day, uint8_t month, uint16_t year)
{
  _writeRegister(DS3231_DAY, day);
  _writeRegister(DS3231_CEN_MONTH, month);
  _writeRegister(DS3231_DEC_YEAR, year);
}

/*void DS3231::setDateTimeAuto()
{
  String timestr = F(__TIME__);

  uint8_t hour = timestr.substring(0, 2).toInt();
  uint8_t min = timestr.substring(3, 5).toInt();
  uint8_t sec = timestr.substring(7, 9).toInt();

  String datestr = F(__DATE__);

  String monthstr = datestr.substring(0, 3);
  uint8_t month = _findMonth(monthstr);

  uint8_t day = datestr.substring(0, 2).toInt();

  uint8_t year = datestr.substring(7, 9).toInt();

  setTime(hour, min, sec);
  setDate(day, month, year);
}
*/
// register funcs ----------------------------------------------------------------------------------

uint8_t DS3231::_readRegister(uint8_t reg)
{
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 1);
  return _fromBcd(Wire.read());
}

void DS3231::_writeRegister(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  Wire.write(_toBcd(data));
  Wire.endTransmission();
}

// bcd funcs---------------------------------------------------------------------------------------

uint8_t DS3231::_toBcd(uint8_t num)
{
  uint8_t bcd = ((num / 10) << 4) + (num % 10);
  return bcd;
}

uint8_t DS3231::_fromBcd(uint8_t bcd)
{
  uint8_t num = (10 * ((bcd & 0xf0) >> 4)) + (bcd & 0x0f);
  return num;
}

// indexing funcs for __DATE__ --------------------------------------------------------------------

int8_t _findMonth(const char *seek)
{
  const char *monthsarr[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

  for (int i = 0; i < 12; ++i)
  {
    if (strcmp(monthsarr[i], seek) == 0)
      return i + 1;
  }
  return -1;
}