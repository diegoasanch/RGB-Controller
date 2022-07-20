#pragma once

#include <Arduino.h>

const uint8 MAX_LED_BRIGHTNESS = 255;

class Led {
private:
    uint8_t pin;
    uint8 brightness;
    bool isOn;

    // unsigned long lastCycleStart;
    // unsigned long refreshRateHz;
    // unsigned long millisBetweenRefresh;

public:
    Led(uint8_t pin) {
        this->pin = pin;
        // this->refreshRateHz = refreshRateHz;
        // this->millisBetweenRefresh = 1000 * 1000 / refreshRateHz; // us
        // this->lastCycleStart = 0;
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
            // this->lastCycleStart = micros();
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
        // micros() - lastCycleStart > millisBetweenRefresh;
    }

    bool shouldTurnOff() {
        return isOn && this->brightness == 0;
    }

    // unsigned long activeTime() {
    //     return map(brightness, 0, MAX_LED_BRIGHTNESS, 0, millisBetweenRefresh);
    // }

    // void setRefreshRateHz(unsigned long refreshRateHz) {
    //     this->refreshRateHz = refreshRateHz;
    //     this->millisBetweenRefresh = 1000 * 1000 / refreshRateHz; // microseconds
    // }

    // unsigned long getRefreshRateHz() {
    //     return this->refreshRateHz;
    // }
};
