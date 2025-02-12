#include "button.hpp"
#include "led_light.hpp"
#include "led_switch.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;

// create mock for ISwitch
class MockISwitch : public ISwitch
{
public:
    MOCK_METHOD(void, on, (), (override));
    MOCK_METHOD(void, off, (), (override));
};

TEST(Tests, WhenButtonIsClicked_SwitchIsOn)
{
    auto mock_switch = make_shared<MockISwitch>();
    
    // Arrange
    Button btn{1, mock_switch};
   
    EXPECT_CALL(*mock_switch, on()).Times(1);

    // Act
    btn.click();
}