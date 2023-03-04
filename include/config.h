#define BUTTON_PIN 7
#define RELAY_PIN 4
#define PHOTO_PIN A0
#define INVERT_RELAY_PIN true

#define startHour 6
#define endHour 20

#define DEBUG_MODE // comment for actual use
// #define DEBUG_EEPROM

// auto calced
#ifdef DEBUG_MODE
#define REFRESH_RATE 1000
#else
#define REFRESH_RATE 600000UL
#endif