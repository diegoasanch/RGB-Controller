#pragma once

#include <AsyncTimer.h>

typedef std::function<void(uint16 h, float s, float l)> ColorChangeHandler;

class ColorAnimation {
private:
    AsyncTimer& timer;
    uint16 hue; // [0, 360]
    float saturation; // [0, 1]
    float lightness; // [0, 1]

    unsigned short timerId;
    unsigned long cycleTimeMs;
    int step;
    int delayMs;

    ColorChangeHandler onChange;

public:
    ColorAnimation(AsyncTimer& timer) : timer(timer) {
        this->hue = 0;
        this->saturation = 1;
        this->lightness = 0.5;
        this->timerId = 0;
        this->cycleTimeMs = 10000;
        this->step = 0;
        this->delayMs = 10;
        this->onChange = nullptr;
    }

    void start(ColorChangeHandler onChange) {
        this->stop();
        this->onChange = onChange;
        this->timerId = this->timer.setInterval([&]() {
            this->hue = (this->hue + this->step) % 360;
            if (this->onChange != nullptr) {
                this->onChange(this->hue, this->saturation, this->lightness);
            }
            }, this->delayMs);
    }

    void stop() {
        if (this->timerId) {
            this->timer.cancel(this->timerId);
            this->timerId = 0;
        }
        this->onChange = nullptr;
    }
};


// input: h as an angle in [0,360] and s,l in [0,1] - output: r,g,b in [0,1]
// function hsl2rgb(h,s,l)
// {
//    let a=s*Math.min(l,1-l);
//    let f= (n,k=(n+h/30)%12) => l - a*Math.max(Math.min(k-3,9-k,1),-1);
//    return [f(0),f(8),f(4)];
// }
