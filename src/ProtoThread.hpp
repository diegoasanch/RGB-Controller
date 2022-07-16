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
