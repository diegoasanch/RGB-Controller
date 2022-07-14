#pragma once

#include <DHT.h>
#include <DHT_U.h>

#include "ProtoThread.hpp"

class Weather : public ProtoThread {
    private:
        uint8_t dataPin;
        DHT sensor;

    public:
        Weather(float refreshRateHz, uint8_t dataPin) : sensor(dataPin, DHT22) {
            this->dataPin = dataPin;
            this->setRefreshRate(refreshRateHz);
            sensor.begin();
        }

        float getTemperature() {
            return sensor.readTemperature();
        }

        float getHumidity() {
            return sensor.readHumidity();
        }
};
