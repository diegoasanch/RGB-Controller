#pragma once

class ProtoThread {
    private:
        unsigned long refreshRateHz;
        unsigned long millisBetweenRefresh;
        unsigned long lastRefresh;

    protected:
        void setRefreshRate(unsigned long refreshRateHz) {
            this->refreshRateHz = refreshRateHz;
            this->millisBetweenRefresh = 1000 / refreshRateHz; // ms
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
