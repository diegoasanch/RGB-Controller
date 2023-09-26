#pragma once

#include<EEPROM.h>

namespace addresses {
    unsigned int rgbOn = 0;
    unsigned int rgbBrightness = 1;
    unsigned int rgbRed = 2;
    unsigned int rgbGreen = 3;
    unsigned int rgbBlue = 4;
}

class Settings {

public:
    Settings() {
        EEPROM.begin(10);
    }
    bool getRgbOn() {
        return bool(EEPROM.read(addresses::rgbOn));
    }
    void setRgbOn(bool rgbOn) {
        EEPROM.write(addresses::rgbOn, rgbOn);
        EEPROM.commit();
    }

    uint8_t getRgbBrightness() {
        return EEPROM.read(addresses::rgbBrightness);
    }
    void setRgbBrightness(uint8_t brightness) {
        EEPROM.write(addresses::rgbBrightness, brightness);
        EEPROM.commit();
    }

    uint8_t getRgbRed() {
        return EEPROM.read(addresses::rgbRed);
    }
    void setRgbRed(uint8_t red) {
        EEPROM.write(addresses::rgbRed, red);
        EEPROM.commit();
    }

    uint8_t getRgbGreen() {
        return EEPROM.read(addresses::rgbGreen);
    }
    void setRgbGreen(uint8_t green) {
        EEPROM.write(addresses::rgbGreen, green);
        EEPROM.commit();
    }

    uint8_t getRgbBlue() {
        return EEPROM.read(addresses::rgbBlue);
    }
    void setRgbBlue(uint8_t blue) {
        EEPROM.write(addresses::rgbBlue, blue);
        EEPROM.commit();
    }

    /*
    * Reboot the device
    */
    void reboot() {
        Serial.println("Rebooting device");
        EEPROM.end();
        ESP.restart();
    }
};
