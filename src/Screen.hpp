#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ProtoThread.hpp"

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1; // Reset pin not, really used

class Screen : public ProtoThread {
    private:
        uint8_t dataPin;
        uint8_t clockPin;
        Adafruit_SSD1306 display;

    public:
        Screen(unsigned long refreshRateHz)
        : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
        {
            this->setRefreshRate(refreshRateHz);
            Wire.begin();
            display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
            display.setTextSize(1);
            display.setTextColor(WHITE);
        }

        void updateReadings(float temperature, float humidity) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print("Temperature: " + String(temperature) + " C");
            display.setCursor(0, 10);
            display.print("Humidity: " + String(humidity) + " %");
            display.setCursor(0, 20);
            display.display();
        }

        void print(String text) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print(text);
            display.display();
        }
};
