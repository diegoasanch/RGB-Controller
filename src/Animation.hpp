#pragma once

#include <AsyncTimer.h>

typedef std::function<void(int)> AnimationHandler;
typedef std::function<void()> AnimationCompleteHandler;

class BrightnessAnimation {
private:
    AsyncTimer& timer;
    uint8 from;
    uint8 to;
    uint8 current;
    unsigned short timerId;
    int timeMs;
    int step;
    int delayMs;
    AnimationHandler onChange;
    AnimationCompleteHandler onComplete;

public:
    BrightnessAnimation(AsyncTimer& timer) : timer(timer) {
        this->from = 0;
        this->to = 0;
        this->current = 0;
        this->timerId = 0;
        this->timeMs = 150;
        this->onChange = nullptr;
        this->onComplete = nullptr;
        this->step = 0;
        this->delayMs = 10;
    }

    void transition(uint8 from, uint8 to, AnimationHandler onChange, AnimationCompleteHandler onComplete) {
        // Clear previous transition if exists
        if (this->timerId) {
            this->timer.cancel(this->timerId);
            this->timerId = 0;
        }
        this->current = from;
        this->from = from;
        this->to = to;

        this->onChange = onChange;
        this->onComplete = onComplete;

        int stepSize = abs(to - from) / (this->timeMs / this->delayMs);
        // If stepSize is 0, set it to 1 to prevent infinite loop in timer.
        stepSize = max(stepSize, 1);
        this->step = from > to ? -stepSize : stepSize;
        // this->timeMs = timeMs; // TODO: use timeMs

        this->timerId = this->timer.setInterval([&]() {
            this->current = constrain(
                this->current + this->step,
                min(this->from, this->to),
                max(this->from, this->to)
            );

            if (this->onChange != nullptr)
                this->onChange(this->current);

            // Stop the timer
            if (this->shoudStopTimer()) {
                this->timer.cancel(this->timerId);
                this->transitionCleanup();
            }
            }, delayMs);
    }

    bool shoudStopTimer() {
        return (this->step > 0 && this->current >= this->to) ||
            (this->step < 0 && this->current <= this->to);
    }

    void transitionCleanup() {
        if (this->onComplete != nullptr)
            this->onComplete();
        this->timerId = 0;
        this->onChange = nullptr;
        this->onComplete = nullptr;
    }
};
