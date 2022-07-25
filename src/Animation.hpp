#pragma once

#include <AsyncTimer.h>
#include"RGBLed.hpp"

// typedef void (*AnimationHandler)(int);
// using AnimationHandler = float (RGBLed::*)();
// typedef void (*Cleanup)(void);

class Animation {
private:
    AsyncTimer& timer;
    int from;
    int to;
    int current;
    unsigned short timerId;
    // int timeMs;
    // AnimationHandler onChangePtr;
    // Cleanup onCleanupPtr;

    // RGBLed rgb;

public:
    Animation(AsyncTimer& timer) : timer(timer) {
        this->from = 0;
        this->to = 0;
        this->current = 0;
        this->timerId = 0;
        // this->timeMs = 0;
        // this->onChangePtr = nullptr;
        // this->onCleanupPtr = nullptr;
    }

    // TODO: receive onChange and cleanup callbacks
    void transition(int from, int to, RGBLed* rgb) {
        // void transition(int from, int to, unsigned long timeMs, ChangeHandler onChangePtr, Cleanup onCleanupPtr) {
        this->from = from;
        this->to = to;
        // this->timeMs = timeMs; // TODO: use timeMs
        // this->onChangePtr = onChangePtr;
        // this->onCleanupPtr = onCleanupPtr;

        timerId = timer.setInterval([&]() {
            current++;
            (*rgb).setBrightness(current);
            // if (this->onChangePtr != nullptr) {
            //     // (this->rgb.*onChangePtr)();
            //     this->onChangePtr(this->current);
            // }

            // Stop the timer
            if (current > to) {
                timer.cancel(timerId);
                (*rgb).update();
            }
            }, 100);
    }

};
