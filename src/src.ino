#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "env.h"
#include "config.h"
#include "Screen.hpp"
#include "Weather.hpp"
#include "Led.hpp"
#include "RGBLed.hpp"
#include "Api.hpp"
#include "WiFi.hpp"

Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);

RGBLed rgb(pins::RGB_RED, pins::RGB_GREEN, pins::RGB_BLUE, true, refreshRate::LED);
Api api(80, &rgb);

float temperature = 0;
float humidity = 0;

void setup() {
    Serial.begin(9600);
    startup();
    setupWifi();
    api.configure();
}


void loop() {
    rgb.update();
    api.handleRequests();

    if (weatherSensor.shouldUpdate()) {
        temperature = weatherSensor.getTemperature();
        humidity = weatherSensor.getHumidity();
    }
}

void startup() {
    Serial.println("Booting up...");
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
}
