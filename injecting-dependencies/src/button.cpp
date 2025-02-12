#include "button.hpp"
#include "led_switch.hpp"

std::shared_ptr<ISwitch> FactoryMethod::Button::get_light_switch()
{
    static LEDLight led {77};
    return std::make_shared<LEDSwitch>(led);
}