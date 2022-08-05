#pragma once

#include <AsyncTimer.h>

typedef std::function<void(int)> AnimationHandler;
typedef std::function<void()> AnimationCompleteHandler;

class Animation {
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
    Animation(AsyncTimer& timer) : timer(timer) {
        this->from = 0;
        this->to = 0;
        this->current = 0;
        this->timerId = 0;
        this->timeMs = 200;
        this->onChange = nullptr;
        this->onComplete = nullptr;
        this->step = 0;
        this->delayMs = 10;
    }

    void transition(uint8 from, uint8 to, AnimationHandler onChange, AnimationCompleteHandler onComplete) {
        this->current = from;
        this->from = from;
        this->to = to;

        this->onChange = onChange;
        this->onComplete = onComplete;

        int stepSize = abs(to - from) / (timeMs / delayMs);
        stepSize = max(stepSize, 1);
        this->step = from > to ? -stepSize : stepSize;
        // this->timeMs = timeMs; // TODO: use timeMs

        this->timerId = this->timer.setInterval([this]() {
            this->current = constrain(
                this->current + this->step,
                min(this->from, this->to),
                max(this->from, this->to)
            );

            if (this->onChange != nullptr) {
                this->onChange(this->current);
            }
            // Stop the timer
            if ((this->step > 0 && this->current >= this->to) ||
                (this->step < 0 && this->current <= this->to)) {
                this->timer.cancel(this->timerId);
                if (this->onComplete != nullptr) {
                    this->onComplete();
                }
            }
            }, delayMs);
    }

};
