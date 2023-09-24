#include <AsyncTimer.h>
#include <WiFiClient.h>

#include "env.h"
#include "config.h"

#include "./components/RGBLed.hpp"

#include "./domain/Animation.hpp"
#include "./domain/Api.hpp"
#include "./domain/Update.hpp"
#include "./domain/Weather.hpp"
#include "./domain/WiFi.hpp"
#include "./domain/Settings.hpp"

Settings settings;

WiFiClient client;
UpdateClient updateClient(client);

AsyncTimer timer;
BrightnessAnimation brightnessAnimation(timer);
RGBLed rgb(
    pins::RGB_RED,
    pins::RGB_GREEN,
    pins::RGB_BLUE,
    settings,
    brightnessAnimation
);

Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);
Api api(80, rgb, weatherSensor, updateClient, settings);

void setup() {
    // Turn led ON until setup is completed
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    rgb.startFromStorage();
    rgb.run();

    // TODO: activate Serial with env
    Serial.begin(115200);
    Serial.println("Booting up...");

    setupWifi();
    api.configure();

    // Setup complete, LED off
    Serial.println("Setup complete " + String(millis()) + "ms");
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    api.handleRequests();
    timer.handle();
}
