#ifndef LED_LIGHT_HPP
#define LED_LIGHT_HPP

#include <iostream>

class ILEDLight
{
public:
    virtual void set_rgb(int r, int g, int b) = 0;
    virtual ~ILEDLight() = default;
};

class LEDLight : public ILEDLight
{
    int id_;
public:
    LEDLight(int id) : id_{id}
    {}

    void set_rgb(int r, int g, int b) override
    {
        std::cout << "Setting(id: " << id_ << "; " << r << ", " << g << ", " << b << ")\n";
    }
};

#endif //LED_LIGHT_HPP
