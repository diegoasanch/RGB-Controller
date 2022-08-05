#pragma once

#include <AsyncTimer.h>
#include "Led.hpp"
#include "config.h"
#include "Settings.hpp"
#include "Animation.hpp"


const uint8 MAX_RGB_BRIGHTNESS = 100;

struct RGB_Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

const RGB_Color RGB_WHITE = {
    MAX_LED_BRIGHTNESS,
    MAX_LED_BRIGHTNESS,
    MAX_LED_BRIGHTNESS
};

const RGB_Color RGB_BLUE = {
    0,
    0,
    MAX_LED_BRIGHTNESS
};

class RGBLed {
private:
    Led red;
    Led green;
    Led blue;

    bool isOn;
    RGB_Color color;
    uint8 displayBrightness;
    uint8 targetBrightness;

    unsigned long refreshRateHz;
    Settings& settings;
    Animation& animation;

    // TODO: Move to Animation class
    AsyncTimer& timer;
    int from;
    int to;
    int current;
    unsigned short timerId;
    int step;
    // End TODO


    void updateLedDisplayValues() {
        red.setBrightness(this->getDisplayValue(color.red));
        green.setBrightness(this->getDisplayValue(color.green));
        blue.setBrightness(this->getDisplayValue(color.blue));

        this->run();
    }

    uint8 getDisplayValue(uint8 color) {
        if (!isOn) return 0;
        return color * this->displayBrightness / MAX_RGB_BRIGHTNESS;
    }

public:
    RGBLed(
        uint8_t redPin,
        uint8_t greenPin,
        uint8_t bluePin,
        AsyncTimer& timer,
        Settings& settings,
        Animation& animation
    ) :
        red(redPin),
        green(greenPin),
        blue(bluePin),
        timer(timer),
        settings(settings),
        animation(animation)
    {
        this->color = RGB_WHITE;
        this->displayBrightness = 70;
        this->targetBrightness = 70;
        this->isOn = false;
        this->refreshRateHz = refreshRate::LED;

        analogWriteFreq(refreshRate::LED);
        analogWriteRange(MAX_LED_BRIGHTNESS);

        this->updateLedDisplayValues();
        this->timerId = 0;
        this->step = 1;
    }

    void startFromStorage() {
        this->isOn = this->settings.getRgbOn();
        this->displayBrightness = this->settings.getRgbBrightness();
        this->targetBrightness = this->displayBrightness;

        uint8_t r = this->settings.getRgbRed();
        uint8_t g = this->settings.getRgbGreen();
        uint8_t b = this->settings.getRgbBlue();
        this->color = { r, g, b };

        this->updateLedDisplayValues();
    }

    void run() {
        this->red.run();
        this->green.run();
        this->blue.run();
    }

    void update() {
        if (!isOn) return;
        this->run();
    }

    bool getIsOn() {
        return this->isOn;
    }

    void turnOn() {
        isOn = true;
        this->updateLedDisplayValues();
        this->settings.setRgbOn(true);
    }

    void turnOff() {
        isOn = false;
        this->updateLedDisplayValues();
        this->settings.setRgbOn(false);
    }

    bool toggle() {
        isOn = !isOn;
        this->updateLedDisplayValues();
        this->settings.setRgbOn(isOn);
        return isOn;
    }


    void turnOnAnimated() {
        isOn = true;
        this->animation.transition(0, this->displayBrightness, [this](uint8 value) {
            this->setDisplayBrightness(value);
            }, [this]() {
                this->turnOn();
            });
    }

    void turnOffAnimated() {
        this->animation.transition(this->displayBrightness, 0, [this](uint8 value) {
            this->setDisplayBrightness(value);
            }, [this]() {
                this->turnOff();
                // Return the brightness to the original value
                this->setDisplayBrightness(this->targetBrightness);
            });
    }

    bool toggleAnimated() {
        if (isOn)
            this->turnOffAnimated();
        else
            this->turnOnAnimated();
        return !isOn;
    }

    /**
     * @brief Sets the target brightness, saves it to the settings and display the new value.
     *
     * @param brightness
     * @return uint8
     */
    uint8 setTargetBrightness(uint8 brightness) {
        this->displayBrightness = constrain(brightness, 0, MAX_RGB_BRIGHTNESS);
        this->targetBrightness = this->displayBrightness;
        this->updateLedDisplayValues();
        this->settings.setRgbBrightness(brightness);
        return this->displayBrightness;
    }

    /**
     * @brief Set the Display Brightness without modifying the settings.
     *
     * @param brightness
     * @return uint8
     */
    uint8 setDisplayBrightness(uint8 brightness) {
        this->displayBrightness = constrain(brightness, 0, MAX_RGB_BRIGHTNESS);
        this->updateLedDisplayValues();
        return this->displayBrightness;
    }

    uint8 getBrightness() {
        return this->displayBrightness;
    }

    void setRGBColor(uint8 red, uint8 green, uint8 blue) {
        color = RGB_Color{ red, green, blue };
        this->updateLedDisplayValues();
        this->settings.setRgbRed(red);
        this->settings.setRgbGreen(green);
        this->settings.setRgbBlue(blue);
    }

    /**
     * Sets current color
     *
     * @param color In format `RRGGBB` (hex)
     */
    void setHexColor(String color) {
        Serial.printf("> SetHexColor %s\n", color.c_str());;
        String red = color.substring(0, 2);
        String green = color.substring(2, 4);
        String blue = color.substring(4, 6);

        this->setRGBColor(
            (uint8)strtol(red.c_str(), NULL, 16),
            (uint8)strtol(green.c_str(), NULL, 16),
            (uint8)strtol(blue.c_str(), NULL, 16)
        );
    }

    /**
         * Get current color in format `RRGGBB` (hex)
         */
    String getHexColor() {
        String red = String(color.red, HEX);
        String green = String(color.green, HEX);
        String blue = String(color.blue, HEX);

        if (red.length() == 1) {
            red = "0" + red;
        }
        if (green.length() == 1) {
            green = "0" + green;
        }
        if (blue.length() == 1) {
            blue = "0" + blue;
        }

        return red + green + blue;
    }

    void setRefreshRateHz(unsigned long refreshRateHz) {
        this->refreshRateHz = constrain(refreshRateHz, 0, 40000);
        analogWriteFreq(refreshRate::LED);
    }

    unsigned long getRefreshRateHz() {
        return this->refreshRateHz;
    }

    // // TODO: make its own class
    // // TODO: receive onChange and cleanup callbacks
    // void transition(int from, int to) {
    //     // void transition(int from, int to, unsigned long timeMs, ChangeHandler onChangePtr, Cleanup onCleanupPtr) {
    //     int direction = from > to ? -1 : 1;
    //     int timeMs = 200;
    //     int fps = 100;
    //     int updateTimes = fps * timeMs / 1000;
    //     this->step = abs(from - to) / updateTimes * direction;
    //     this->from = from;
    //     this->to = to;
    //     this->current = from;
    //     // this->timeMs = timeMs; // TODO: use timeMs
    //     // this->onChangePtr = onChangePtr;
    //     // this->onCleanupPtr = onCleanupPtr;

    //     if (this->timerId != 0) {
    //         this->timer.cancel(this->timerId);
    //     }

    //     this->timerId = this->timer.setInterval([this]() {
    //         this->current += this->step;
    //         this->setBrightness(this->current);

    //         // Stop the timer
    //         if (
    //             (this->step > 0 && this->current >= this->to)
    //             || (this->step < 0 && this->current <= this->to)
    //             ) {
    //             this->timer.cancel(this->timerId);

    //             if (this->shoudlTurnOn()) {
    //                 this->turnOn();
    //             }
    //             else if (this->shoudlTurnOff()) {
    //                 this->turnOff();
    //             }
    //         }
    //         }, 1000 / fps);
    // }
};

