#pragma once

#include <Arduino.h>

class Led {
    private:
        uint8_t pin;
        int brightness;
        bool isOn;

        unsigned long lastCycleStart;
        unsigned long refreshRateHz;
        unsigned long millisBetweenRefresh;

    public:
        Led(uint8_t pin, unsigned long refreshRateHz) {
            this->pin = pin;
            this->refreshRateHz = refreshRateHz;
            this->millisBetweenRefresh = 1000 * 1000 / refreshRateHz; // us
            this->lastCycleStart = 0;
            this->brightness = 0;
            this->isOn = false;

            pinMode(pin, OUTPUT);
        }

        void updateLed(int brightness) {
            this->brightness = brightness;

            if (shouldTurnOn()) {
                turnOn();
                this->lastCycleStart = micros();
            } else if (shouldTurnOff()) {
                turnOff();
            }
        }

        void turnOn() {
            this->isOn = true;
            digitalWrite(pin, HIGH);
        }

        void turnOff() {
            this->isOn = false;
            digitalWrite(pin, LOW);
        }

        bool shouldTurnOn() {
            return !isOn && micros() - lastCycleStart > millisBetweenRefresh;
        }

        bool shouldTurnOff() {
            return isOn && micros() - lastCycleStart > activeTime();
        }

        unsigned long activeTime() {
            return map(brightness, 0, 255, 1, millisBetweenRefresh - 1);
        }
};
