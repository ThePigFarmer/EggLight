#include "config.h"

#define between(x, a, b) (((a) <= (x)) && ((x) <= (b)))

// eeprom for threshold
#define EEPROM_ON_RTC_MODULE 0x57

#include <Arduino.h>
#include <BtButton.h>
#include <RTClib.h>
#include <Wire.h>

BtButton bnt(BUTTON_PIN);
BtButton bntDebug(DEBUG_TOGGLE_PIN);
RTC_DS3231 rtc;

bool debugMode = false;

uint16_t light = 1023;

uint32_t previousMillis;

uint16_t thresholdFromEEPROM();

void initRTC();
void readEEPROM();
void writeEEPROM();
void writeUint16InEEPROM(uint16_t data);

//-------------------------------------------------------------------------------------------------

void setup()
{
    Wire.begin(); // rtc
    initRTC();
    pinMode(PHOTO_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);

    // rtc.setTime(23, 46, 00);

    Serial.begin(115200);
}

void loop()
{
    DateTime now = rtc.now();
    uint32_t refreashRate = debugMode ? 1000 : 600000UL;

    bool targetState = LOW;

    light = analogRead(PHOTO_PIN);

    light = map(light, 0, 1023, 1023, 0); // brighter = +, darker = -

    // timed loop
    if (millis() - previousMillis >= refreashRate)
    {
        previousMillis = millis();

        if (between(now.hour(), startHour, endHour))
        {
            if (light < thresholdFromEEPROM())
            {
                targetState = HIGH;
            }
            else
            {
                targetState = LOW;
            }
        }

        digitalWrite(RELAY_PIN, targetState ^ INVERT_RELAY_PIN);

        char timestr[9] = "hh:mm:ss";
        Serial.println(now.toString(timestr));
        char buffer[69];
        sprintf(buffer, "th: %u   li: %u   startH: %u   t: %s    endH: %u", thresholdFromEEPROM(), light, startHour, now.toString(timestr), endHour);
        Serial.println(buffer);
    }

    // set threshold (user) -----------------------------------------------------------------------------------------------
    bnt.read();

    if (bnt.changedToPressed())
    {
        Serial.println(F("\nbutton pressed\n"));
        writeUint16InEEPROM(light);
    }

    bntDebug.read();
    if (bntDebug.changedToPressed())
        debugMode = !debugMode;
} // end main loop

// function sources --------------------------------------------------------------------------------

void writeEEPROM(int16_t i2c_address, int16_t address, uint8_t val)
{
    Wire.beginTransmission(i2c_address);
    Wire.write((int16_t)(address >> 8));   // MSB
    Wire.write((int16_t)(address & 0xFF)); // LSB
    Wire.write(val);
    Wire.endTransmission();
    delay(5);
}

byte readEEPROM(int16_t i2c_address, int16_t address)
{
    byte rcvData = 0xFF;
    Wire.beginTransmission(i2c_address);
    Wire.write((int16_t)(address >> 8));   // MSB
    Wire.write((int16_t)(address & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(i2c_address, 1);
    rcvData = Wire.read();
    return rcvData;
}

void writeUint16InEEPROM(uint16_t data)
{
    Serial.println(F("\nwritting in eeprom..."));

    uint8_t byte1 = data >> 8;
    uint8_t byte2 = data & 0xFF;

    writeEEPROM(EEPROM_ON_RTC_MODULE, 1, byte1);
    writeEEPROM(EEPROM_ON_RTC_MODULE, 2, byte2);
}

uint16_t thresholdFromEEPROM()
{
#ifdef DEBUG_MODE
    Serial.println(F("\ngetting theshold from eeprom..."));
#endif
    uint8_t byte1 = readEEPROM(EEPROM_ON_RTC_MODULE, 1);
    uint8_t byte2 = readEEPROM(EEPROM_ON_RTC_MODULE, 2);
#ifdef DEBUG_EEPROM
    Serial.print(F("byte1 "));
    Serial.print(byte1, BIN);
    Serial.print(F("   byte2 "));
    Serial.print(byte2, BIN);
#endif
    uint16_t x = ((uint16_t)byte1 << 8) | byte2;
#ifdef DEBUG_EEPROM
    Serial.print(F("   th "));
    Serial.print(x);
#endif
    return x;
#ifdef DEBUG_MODE
    Serial.println(F("done\n\n"));
#endif
}

void initRTC()
{
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  /*
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  */
}
