#ifndef LIGHT_SWITCH_HPP
#define LIGHT_SWITCH_HPP

#include "button.hpp"
#include "led_light.hpp"

class LEDSwitch : public ISwitch
{
    ILEDLight& led_;

public:
    LEDSwitch(ILEDLight& led)
        : led_ {led}
    {
    }

    void on() override
    {
        led_.set_rgb(255, 255, 255);
    }

    void off() override
    {
        led_.set_rgb(0, 0, 0);
    }
};

namespace HighPerfDI
{

    class LEDSwitch
    {
        ILEDLight& led_;

    public:
        LEDSwitch(ILEDLight& led)
            : led_ {led}
        {
        }

        void on()
        {
            led_.set_rgb(255, 255, 255);
        }

        void off()
        {
            led_.set_rgb(0, 0, 0);
        }
    };
}

#endif