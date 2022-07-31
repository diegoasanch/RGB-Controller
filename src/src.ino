#include <AsyncTimer.h>

#include "env.h"
#include "config.h"

// #include "Animation.hpp"
#include "Api.hpp"
#include "RGBLed.hpp"
#include "Weather.hpp"
#include "WiFi.hpp"

AsyncTimer timer;
// Animation animation(timer);
RGBLed rgb(
    pins::RGB_RED,
    pins::RGB_GREEN,
    pins::RGB_BLUE,
    true,
    70,
    RGB_WHITE,
    refreshRate::LED,
    timer
);

Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);
Api api(80, rgb, weatherSensor);

void setup() {
    // Turn led ON until setup is completed
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // TODO: activate Serial with env
    Serial.begin(115200);
    Serial.println("Booting up...");

    setupWifi();
    api.configure();

    if (Serial) Serial.println("Setup complete " + String(millis()) + "ms");
    digitalWrite(LED_BUILTIN, HIGH);

    rgb.run();
}

void loop() {
    api.handleRequests();
    timer.handle();
}
