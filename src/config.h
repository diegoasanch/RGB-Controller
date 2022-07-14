#pragma once

namespace refreshRate {
    /**
     * All values represent Hz (times per second)
    */
    const float SCREEN = 0.2;
    const float WEATHER = 0.1;
    const unsigned short LED = 60;
}

namespace pins {
    const uint8_t SCREEN_SCL = D1;
    const uint8_t SCREEN_SDA = D2;

    const uint8_t WEATHER_DATA = D3;

    // const uint8_t RED;
    // const uint8_t GREEN;
    // const uint8_t BLUE;

}
