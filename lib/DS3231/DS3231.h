#ifndef DS3231_h
#define DS3231_h

#include <Arduino.h>

#define DS3231_READ 0xD1
#define DS3231_WRITE 0xD0
#define DS3231_ADDR 0x68

// DS3231 Registers
#define DS3231_SECONDS 0x00
#define DS3231_MINUTES 0x01
#define DS3231_HOURS 0x02
#define DS3231_DAY 0x03
#define DS3231_DATE 0x04
#define DS3231_CEN_MONTH 0x05
#define DS3231_DEC_YEAR 0x06
#define DS3231_ALARM1_SECONDS 0x07
#define DS3231_ALARM1_MINUTES 0x08
#define DS3231_ALARM1_HOURS 0x09
#define DS3231_ALARM1_DAY_DATE 0x0a
#define DS3231_ALARM2_MINUTES 0x0b
#define DS3231_ALARM2_HOURS 0x0c
#define DS3231_ALARM2_DAY_DATE 0x0d
#define DS3231_CONTROL 0x0e
#define DS3231_CTL_STATUS 0x0f
#define DS3231_AGING_OFFSET 0x10
#define DS3231_TEMP_MSB 0x11
#define DS3231_TEMP_LSB 0x12

class Time
{
public:
  // Time();
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
};

class Date
{
public:
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

class DS3231
{
public:
  Time getTime();
  Date getDate();
  void setDateTimeAuto();
  void setTime(uint8_t hour, uint8_t min, uint8_t sec);
  void setDate(uint8_t day, uint8_t month, uint16_t year);
  void _findMonth(char seek);

private:
  uint8_t _readRegister(uint8_t reg);             // accounts for BCD
  void _writeRegister(uint8_t reg, uint8_t data); // accounts for BCD
  uint8_t _toBcd(uint8_t num);                    // decimal -> BCD conversion
  uint8_t _fromBcd(uint8_t bcd);                  // BCD -> decimal conversion
  // int8_t _findMonth(const char *seek);   // still in progress
};

#endif