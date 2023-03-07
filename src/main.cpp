#include "config.h"

#define between(x, a, b) (((a) <= (x)) && ((x) <= (b)))

// eeprom for threshold
#define EEPROM_ON_RTC_MODULE 0x57

#include <Arduino.h>
#include <BtButton.h>
#include <DS3231.h>
#include <Wire.h>

BtButton bnt(BUTTON_PIN);
BtButton bntDebug(DEBUG_TOGGLE_PIN);
DS3231 rtc;
Time t;

bool debugMode = false;

uint16_t light = 1023;

uint32_t previousMillis;

uint16_t thresholdFromEEPROM();

void writeEEPROM();
void readEEPROM();
void writeUint16InEEPROM(uint16_t data);

//-------------------------------------------------------------------------------------------------

void setup()
{
    Wire.begin(); // rtc
    pinMode(PHOTO_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);

    // rtc.setTime(23, 46, 00);

    Serial.begin(115200);
}

void loop()
{
    t = rtc.getTime(); // read time from the rtc

    uint32_t refreashRate = debugMode ? 1000 : 600000UL;

    bool targetState = LOW;

    light = analogRead(PHOTO_PIN);

    light = map(light, 0, 1023, 1023, 0); // brighter = +, darker = -

    // timed loop
    if (millis() - previousMillis >= refreashRate)
    {
        previousMillis = millis();

        if (between(t.hour, startHour, endHour))
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

        char buffer[69];
        sprintf(buffer, "th: %u   li: %u   startH: %u   t: %u:%u:%u    endH: %u", thresholdFromEEPROM(), light, startHour, t.hour, t.min, t.sec, endHour);
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