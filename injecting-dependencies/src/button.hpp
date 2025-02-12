#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <iostream>
#include <memory>
#include "led_light.hpp"

class ISwitch
{
public:
    virtual void on() = 0;
    virtual void off() = 0;
    virtual ~ISwitch() = default;
};

auto btn_number = [] {};

class Button
{
    int btn_number_;
    std::shared_ptr<ISwitch> light_switch_;
    bool is_on_;

public:
    Button(int btn_number, std::shared_ptr<ISwitch> light_switch)
        : btn_number_ {btn_number}
        , light_switch_ {light_switch}
        , is_on_ {false}
    {
    }

    void click()
    {
        std::cout << "Button " << btn_number_ << " clicked...\n";
        if (!is_on_)
        {
            light_switch_->on();
            is_on_ = true;
        }
        else
        {
            light_switch_->off();
            is_on_ = false;
        }
    }
};

namespace HighPerfDI
{
    class SwitchProvider;

    template <typename SwitchType = class SwitchProvider>
    class Button
    {
        int btn_number_;
        SwitchType& light_switch_;
        bool is_on_;

    public:
        Button(int btn_number, SwitchType& light_switch)
            : btn_number_ {btn_number}
            , light_switch_ {light_switch}
            , is_on_ {false}
        {
        }

        void click()
        {
            std::cout << "Button " << btn_number_ << " clicked...\n";
            if (!is_on_)
            {
                light_switch_.on();
                is_on_ = true;
            }
            else
            {
                light_switch_.off();
                is_on_ = false;
            }
        }
    };
}

namespace FactoryMethod
{
    class ILogger
    {
    public:
        virtual void log(const std::string& message) = 0;
        virtual ~ILogger() = default;
    };

    class Logger : public ILogger
    {
    public:
        static Logger& instance()
        {
            static Logger logger;
            return logger;
        }

        void log(const std::string& message) override
        {
            std::cout << "Log: " << message << "\n";
        }
    };

    class Button
    {
        int btn_number_;
        bool is_on_;
    protected:
        virtual std::shared_ptr<ISwitch> get_light_switch();        

        virtual ILogger& get_logger()
        {
            return Logger::instance();
        }
    public:
        Button(int btn_number) : btn_number_{}, is_on_{false}
        {}

        void click()
        {
            auto light_switch = get_light_switch();

            std::cout << "Button " << btn_number_ << " clicked...\n";
            if (!is_on_)
            {
                light_switch->on();
                is_on_ = true;
            }
            else
            {
                light_switch->off();
                is_on_ = false;
            }

            get_logger().log("clicked");    
        }
    };

    class MockSwitch : public ISwitch
    {};

    class MockLogger : public ILogger
    {
    public:
        void log(const std::string& message) override
        {            
        }
    };

    class TestableButton : public Button
    {
        std::shared_ptr<MockSwitch> mq_switch_;
        MockLogger mq_logger_;
    protected:
        std::shared_ptr<ISwitch> get_light_switch() override
        {
            return mq_switch_;
        }

        ILogger& get_logger() override
        {
            return mq_logger_;
        }
    };
}

#endif