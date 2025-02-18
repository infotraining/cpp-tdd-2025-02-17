#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace Dependency
{
    struct ILogger
    {
        virtual void log(const std::string& message) = 0;
        virtual ~ILogger() = default;
    };

    class ITerminal
    {
    public:
        virtual ~ITerminal() = default;
        virtual void print(const std::string& message) = 0;
        virtual std::string read() = 0;
    };

    class Terminal : public ITerminal
    {
    public:
        void print(const std::string& message) override
        {
            std::cout << message;
        }

        std::string read() override
        {
            std::string input;
            std::cin >> input;
            return input;
        }
    };
} // namespace Dependency

namespace DependencyInjection
{
    namespace NoDependencyInjection
    {
        class Calculator
        {
        public:
            int add(int a, int b)
            {
                return a + b;
            }

            void run()
            {
                Dependency::Terminal terminal;
                terminal.print("Enter first number: ");
                int a = std::stoi(terminal.read());
                terminal.print("Enter second number: ");
                int b = std::stoi(terminal.read());
                terminal.print("The sum is: " + std::to_string(add(a, b)));
            }
        };
    } // namespace NoDependencyInjection

    namespace ConstructorDI
    {
        class Calculator
        {
            Dependency::ITerminal& terminal_;
            Dependency::ILogger& logger_;

        public:
            explicit Calculator(Dependency::ITerminal& terminal, Dependency::ILogger& logger)
                : terminal_{terminal}, logger_{logger}
            {
            }

            int add(int a, int b)
            {
                return a + b;
            }

            void run()
            {
                terminal_.print("Enter first number: ");
                int a = std::stoi(terminal_.read());
                std::cout << "a: " << a << std::endl;
                terminal_.print("Enter second number: ");
                int b = std::stoi(terminal_.read());
                std::cout << "b: " << b << std::endl;
                terminal_.print("The sum is: " + std::to_string(add(a, b)));
            }
        };
    } // namespace ConstructorDI

    namespace SetterDI
    {
        class Calculator
        {
            std::unique_ptr<Dependency::ITerminal> terminal_;

        public:
            Calculator()
                : terminal_{nullptr}
            {
            }

            void set_terminal(std::unique_ptr<Dependency::ITerminal> terminal)
            {
                assert(terminal != nullptr);
                terminal_ = std::move(terminal);
            }

            int add(int a, int b)
            {
                return a + b;
            }

            void run()
            {
                assert(terminal_ != nullptr);

                terminal_->print("Enter first number: ");
                int a = std::stoi(terminal_->read());
                terminal_->print("Enter second number: ");
                int b = std::stoi(terminal_->read());
                terminal_->print("The sum is: " + std::to_string(add(a, b)));
            }
        };

    } // namespace SetterDI

    namespace TemplateParameterDI
    {
        template <typename Terminal>
        class Calculator
        {
            Terminal& terminal_;

        public:
            Calculator(Terminal& terminal)
                : terminal_{terminal}
            {
            }

            int add(int a, int b)
            {
                return a + b;
            }

            void run()
            {
                terminal_.print("Enter first number: ");
                int a = std::stoi(terminal_.read());
                terminal_.print("Enter second number: ");
                int b = std::stoi(terminal_.read());
                terminal_.print("The sum is: " + std::to_string(add(a, b)));
            }
        };
    } // namespace TemplateParameterDI

    namespace FactoryMethodDI
    {
        template <typename T>
        class SingletonHolder
        {
        public:
            SingletonHolder() = delete;
            SingletonHolder(const SingletonHolder&) = delete;
            SingletonHolder& operator=(const SingletonHolder&) = delete;

            static T& instance()
            {
                static T instance;
                return instance;
            }
        };

        using SingletonTerminal = SingletonHolder<Dependency::Terminal>;

        class Calculator
        {
        public:
            Calculator()
            {
            }

            int add(int a, int b)
            {
                return a + b;
            }

            void run()
            {
                // BEWARE!!! Usage of singleton makes testing difficult!!!
                // SingletonTerminal::instance().print("Enter first number: ");

                auto& terminal = get_terminal(); // factory method creates dependency in a controlled way

                // usage of dependency
                terminal.print("Enter first number: ");
                int a = std::stoi(terminal.read());
                terminal.print("Enter second number: ");
                int b = std::stoi(terminal.read());
                terminal.print("The sum is: " + std::to_string(add(a, b)));
            }

        protected:
            virtual Dependency::ITerminal& get_terminal()
            {
                return SingletonTerminal::instance();
            }
        };
    } // namespace FactoryMethodDI
} // namespace DependencyInjection
#endif