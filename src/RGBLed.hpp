#pragma once

#include "Led.hpp"

const uint8 MAX_RGB_BRIGHTNESS = 100;

class RGBLed {
private:
    Led red;
    Led green;
    Led blue;

    uint8 redValue;
    uint8 greenValue;
    uint8 blueValue;

    uint8 brightness;
    bool isOn;

    void updateLedDisplayValues() {
        red.setBrightness(this->getDisplayValue(this->redValue));
        green.setBrightness(this->getDisplayValue(this->greenValue));
        blue.setBrightness(this->getDisplayValue(this->blueValue));

        this->updateLeds();
    }

    void updateLeds() {
        this->red.updateLed();
        this->green.updateLed();
        this->blue.updateLed();
    }

    uint8 getDisplayValue(uint8 color) {
        if (!isOn) return 0;
        return color * this->brightness / MAX_RGB_BRIGHTNESS;
    }

public:
    RGBLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, bool isOn, unsigned long refreshRateHz)
        : red(redPin, refreshRateHz), green(greenPin, refreshRateHz), blue(bluePin, refreshRateHz) {

        this->redValue = 0;
        this->greenValue = 0;
        this->blueValue = 0;

        this->brightness = 0;
        this->isOn = isOn;
    }

    void update() {
        if (!this->isOn) return;
        this->updateLeds();
    }

    void turnOn() {
        this->isOn = true;
        this->updateLedDisplayValues();
    }

    void turnOff() {
        this->isOn = false;
        this->updateLedDisplayValues();
    }

    void setBrightness(uint8 brightness) {
        this->brightness = constrain(brightness, 0, MAX_RGB_BRIGHTNESS);
        this->updateLedDisplayValues();
    }

    uint16 getBrightness() {
        return this->brightness;
    }

    void setRGBColor(uint8 red, uint8 green, uint8 blue) {
        this->redValue = red;
        this->greenValue = green;
        this->blueValue = blue;

        this->updateLedDisplayValues();
    }

    void setHexColor(String color) {
        String red = color.substring(0, 2);
        String green = color.substring(2, 4);
        String blue = color.substring(4, 6);

        this->setRGBColor(
            (uint8)strtol(red.c_str(), NULL, 16),
            (uint8)strtol(green.c_str(), NULL, 16),
            (uint8)strtol(blue.c_str(), NULL, 16)
        );
    }

    String getHexColor() {
        String red = String(this->redValue, HEX);
        String green = String(this->greenValue, HEX);
        String blue = String(this->blueValue, HEX);

        if (red.length() == 1) {
            red = "0" + red;
        }
        if (green.length() == 1) {
            green = "0" + green;
        }
        if (blue.length() == 1) {
            blue = "0" + blue;
        }

        return red + green + blue;
    }
};
