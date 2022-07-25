#pragma once

#include <Arduino.h>


const uint8 MAX_LED_BRIGHTNESS = 255;

class Led {
private:
    uint8_t pin;
    uint8 brightness;
    bool isOn;

public:
    Led(uint8_t pin) {
        this->pin = pin;
        this->brightness = 0; // 0 - MAX_LED_BRIGHTNESS
        this->isOn = false;

        pinMode(pin, OUTPUT);
    }

    void setBrightness(uint8 brightness) {
        this->brightness = constrain(brightness, 0, MAX_LED_BRIGHTNESS);
        analogWrite(this->pin, this->brightness);
    }

    uint8 getBrightness() {
        return this->brightness;
    }

    void run() {
        if (shouldTurnOn()) {
            turnOn();
        }
        else if (shouldTurnOff()) {
            turnOff();
        }
    }

    void turnOn() {
        this->isOn = true;
        analogWrite(pin, this->brightness);
    }

    void turnOff() {
        this->isOn = false;
        analogWrite(pin, 0);
    }

    bool shouldTurnOn() {
        return !isOn && this->brightness != 0;
    }

    bool shouldTurnOff() {
        return isOn && this->brightness == 0;
    }
};
