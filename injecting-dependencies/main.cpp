#include "button.hpp"
#include "led_light.hpp"
#include "led_switch.hpp"

#include <boost/di.hpp>
#include <iostream>

using namespace std;

namespace di = boost::di;

template <>
struct di::ctor_traits<Button>
{
    BOOST_DI_INJECT_TRAITS((named = btn_number) int, std::shared_ptr<ISwitch>);
};

template <typename T>
using has_log = decltype(declval<T>().log("text"s));

int main()
{
    static_assert(requires(FactoryMethod::Logger&& logger) { logger.log("text"s); });

    // Button btn {make_shared<LEDSwitch>(make_unique<LEDLight>())};
    auto injector = di::make_injector(
        di::bind<ISwitch>().to<LEDSwitch>(),
        di::bind<ILEDLight>().to<LEDLight>(),
        di::bind<int>().to(42),
        di::bind<int>().named(btn_number).to(665));

    auto btn = injector.create<Button>();

    btn.click();
    btn.click();
    btn.click();
    btn.click();
    btn.click();
    btn.click();

    auto btn2 = injector.create<Button>();

    auto hp_injector = di::make_injector(
        di::bind<class HighPerfDI::SwitchProvider>().to<HighPerfDI::LEDSwitch>(),
        di::bind<ILEDLight>().to<LEDLight>(),
        di::bind<int>().to(43),
        di::bind<int>().named(btn_number).to(667));

    // LEDLight led{43};
    // auto led_switch = LEDSwitch{led};
    // auto hp_button = HighPerfDI::Button<LEDSwitch>(667, led_switch);

    auto hp_button = hp_injector.create<HighPerfDI::Button>();

    hp_button.click();
    hp_button.click();

    return 0;
}