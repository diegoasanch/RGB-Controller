#pragma once

#include <DHT.h>
#include <DHT_U.h>

class Weather {
private:
    uint8_t dataPin;
    DHT sensor;

public:
    Weather(float refreshRateHz, uint8_t dataPin) : sensor(dataPin, DHT22) {
        this->dataPin = dataPin;
        sensor.begin();
    }

    float getTemperature() {
        return sensor.readTemperature();
    }

    float getHumidity() {
        return sensor.readHumidity();
    }
};
