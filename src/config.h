#pragma once

namespace refreshRate {
    /**
     * All values represent Hz (times per second)
    */
    const unsigned short SCREEN = 30;
    const unsigned short WEATHER = 1;
    const unsigned short LED = 1;
}

namespace pins {
    const uint8_t SCREEN_SCL = D1;
    const uint8_t SCREEN_SDA = D2;

    const uint8_t WEATHER_DATA = D5;

    // const uint8_t RED;
    // const uint8_t GREEN;
    // const uint8_t BLUE;

}
