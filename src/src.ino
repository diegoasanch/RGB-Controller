#include <Wire.h>

#include "env.h"
#include "config.h"
#include "Screen.hpp"
#include "Weather.hpp"
#include "Led.hpp"

Screen screen(refreshRate::SCREEN);
Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);

Led led(D5, refreshRate::LED);

float temperature = 0;
float humidity = 0;
int brightness = 0;

uint8_t potPin = A0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(potPin, INPUT);

    startup();
}

void loop() {
    brightness = analogRead(potPin);
    led.updateLed(map(brightness, 0, 1023, 0, 255));
    if (screen.shouldUpdate()) {
        screen.updateReadings(temperature, humidity, brightness);
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
    led.turnOn();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    screen.print("led off");
    led.turnOff();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
}
