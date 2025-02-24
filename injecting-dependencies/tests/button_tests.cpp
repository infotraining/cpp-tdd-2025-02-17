#include "button.hpp"
#include "led_light.hpp"
#include "led_switch.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <boost/di.hpp>

namespace di = boost::di;

using namespace std;

// create mock for ISwitch
class MockISwitch : public ISwitch
{
public:
    MOCK_METHOD(void, on, (), (override));
    MOCK_METHOD(void, off, (), (override));
};

auto default_test_injector = di::make_injector(
    di::bind<ISwitch>().to<MockISwitch>()
);

TEST(Tests, WhenButtonIsClicked_SwitchIsOn)
{
    auto mock_switch = default_test_injector.create<std::shared_ptr<MockISwitch>>();
    EXPECT_CALL(*mock_switch, on()).Times(1);
    
    // Arrange
    Button btn = default_test_injector.create<Button>();
   
    // Act
    btn.click();
}