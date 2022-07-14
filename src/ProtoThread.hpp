#pragma once

class ProtoThread {
    private:
        float refreshRateHz;
        unsigned long millisBetweenRefresh;
        unsigned long lastRefresh;

    protected:
        void setRefreshRate(float refreshRateHz) {
            this->refreshRateHz = refreshRateHz;
            this->millisBetweenRefresh = static_cast<unsigned long>(1000.0 / refreshRateHz); // ms
            Serial.begin(9600);
            Serial.println("ProtoThread initialized");
            Serial.println("Refresh rate: " + String(refreshRateHz) + " Hz");
            Serial.println("Millis between ref: " + String(millisBetweenRefresh) + " ms");
            this->lastRefresh = 0;
        }
    public:

        boolean shouldUpdate() {
            if (millis() - lastRefresh > millisBetweenRefresh) {
                lastRefresh = millis();
                return true;
            }
            return false;
        }
};
