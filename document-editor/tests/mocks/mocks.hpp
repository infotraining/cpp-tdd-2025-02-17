#ifndef MOCK_CLIPBOARD_HPP

#include <gmock/gmock.h>
#include "clipboard.hpp"
#include "console.hpp"

class MockConsole : public Console
{
public:
    MOCK_METHOD(std::string, get_line, (), (override));
    MOCK_METHOD(void, print, (const std::string&), (override));
};

#endif // MOCK_CLIPBOARD_HPP
