#include <Wire.h>

#include "env.h"
#include "config.h"
#include "Screen.hpp"
#include "Weather.hpp"

Screen screen(refreshRate::SCREEN);
Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);

float temperature = 0;
float humidity = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    startup();
}

void loop() {
    if (screen.shouldUpdate()) {
        screen.updateReadings(temperature, humidity);
    }
    if (weatherSensor.shouldUpdate()) {
        temperature = weatherSensor.getTemperature();
        humidity = weatherSensor.getHumidity();
    }
}

void startup() {
    screen.print("screen on");
    delay(1000);
    screen.print("led on");
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    screen.print("led off");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
}
