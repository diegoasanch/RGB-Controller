#pragma once

#include "Led.hpp"

const uint8 MAX_RGB_BRIGHTNESS = 100;

struct RGB_Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

const RGB_Color RGB_WHITE = {
    MAX_RGB_BRIGHTNESS,
    MAX_RGB_BRIGHTNESS,
    MAX_RGB_BRIGHTNESS
};

class RGBLed {
private:
    Led red;
    Led green;
    Led blue;

    RGB_Color color;
    uint8 brightness;
    bool isOn;

    void updateLedDisplayValues() {
        red.setBrightness(this->getDisplayValue(color.red));
        green.setBrightness(this->getDisplayValue(color.green));
        blue.setBrightness(this->getDisplayValue(color.blue));

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
    RGBLed(
        uint8_t redPin,
        uint8_t greenPin,
        uint8_t bluePin,
        bool isOn,
        uint8 brightness,
        RGB_Color color,
        unsigned long refreshRateHz
    ) :
        red(redPin, refreshRateHz),
        green(greenPin, refreshRateHz),
        blue(bluePin, refreshRateHz)
    {

        this->color = color;
        this->brightness = brightness;
        this->isOn = isOn;

        this->updateLedDisplayValues();
    }

    void update() {
        if (!isOn) return;
        this->updateLeds();
    }

    void turnOn() {
        isOn = true;
        this->updateLedDisplayValues();
    }

    void turnOff() {
        isOn = false;
        this->updateLedDisplayValues();
    }

    bool toggle() {
        isOn = !isOn;
        this->updateLedDisplayValues();
        return isOn;
    }

    uint8 setBrightness(uint8 brightness) {
        this->brightness = constrain(brightness, 0, MAX_RGB_BRIGHTNESS);
        this->updateLedDisplayValues();
        return this->brightness;
    }

    uint16 getBrightness() {
        return this->brightness;
    }

    void setRGBColor(uint8 red, uint8 green, uint8 blue) {
        color = RGB_Color{ red, green, blue };
        this->updateLedDisplayValues();
    }

    /**
     * Sets current color
     *
     * @param color In format `RRGGBB` (hex)
     */
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

    /**
         * Get current color in format `RRGGBB` (hex)
         */
    String getHexColor() {
        String red = String(color.red, HEX);
        String green = String(color.green, HEX);
        String blue = String(color.blue, HEX);

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
