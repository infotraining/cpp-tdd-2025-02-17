#include <algorithm>
#include <calculator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <memory>
#include <string>

TEST_CASE("Untestable code", "[.][Calculator]")
{
    using DependencyInjection::NoDependencyInjection::Calculator;

    Calculator calculator;
    calculator.run();
}

class MockTerminal : public Dependency::ITerminal
{
public:
    MAKE_MOCK1(print, void(const std::string&), override);
    MAKE_MOCK0(read, std::string(), override);
};

struct DummyTerminal : public Dependency::ITerminal
{
    void print(const std::string& message) override
    {
    }

    std::string read() override
    {
        return "1";
    }
};

struct DummyLogger : public Dependency::ILogger
{
    void log(const std::string& message) override
    {
    }
};

struct CalculatorBuilder
{
    std::shared_ptr<Dependency::ITerminal> terminal = std::make_shared<DummyTerminal>();
    std::shared_ptr<Dependency::ILogger> logger = std::make_shared<DummyLogger>();
    
    CalculatorBuilder& with_terminal(std::shared_ptr<Dependency::ITerminal> term)
    {
        terminal = term;
        return *this;
    }

    CalculatorBuilder& with_logger(std::shared_ptr<Dependency::ILogger> logger)
    {
        this->logger = logger;
        return *this;
    }

    DependencyInjection::ConstructorDI::Calculator build()
    {
        return DependencyInjection::ConstructorDI::Calculator(*terminal, *logger);
    }
}; 

TEST_CASE("Dependency Injection with constructor", "[Calculator, DI-Constructor]")
{
    using DependencyInjection::ConstructorDI::Calculator;

    auto terminal = std::make_shared<MockTerminal>();
    ALLOW_CALL(*terminal, print(ANY(std::string)));

    REQUIRE_CALL(*terminal, read()).RETURN("1");
    REQUIRE_CALL(*terminal, read()).RETURN("2");
    REQUIRE_CALL(*terminal, print("The sum is: 3"));

    CalculatorBuilder builder;
    builder.with_terminal(terminal);
    Calculator calculator = builder.build();

    calculator.run();
}

TEST_CASE("Dependency Injection with setter", "[Calculator, DI-Setter]")
{
    using DependencyInjection::SetterDI::Calculator;

    auto terminal = std::make_unique<MockTerminal>();
    ALLOW_CALL(*terminal, print(ANY(std::string)));

    REQUIRE_CALL(*terminal, read()).RETURN("1");
    REQUIRE_CALL(*terminal, read()).RETURN("2");
    REQUIRE_CALL(*terminal, print("The sum is: 3"));

    Calculator calculator;
    calculator.set_terminal(std::move(terminal));
    calculator.run();
}

TEST_CASE("Dependency Injection with template parameter", "[Calculator, DI-Template]")
{
    using DependencyInjection::TemplateParameterDI::Calculator;

    MockTerminal terminal;
    ALLOW_CALL(terminal, print(ANY(std::string)));

    REQUIRE_CALL(terminal, read()).RETURN("1");
    REQUIRE_CALL(terminal, read()).RETURN("2");
    REQUIRE_CALL(terminal, print("The sum is: 3"));

    Calculator calculator(terminal);
    calculator.run();
}

class TestableCalculator : public DependencyInjection::FactoryMethodDI::Calculator
{
    MockTerminal terminal_;

public:
    MockTerminal& get_terminal() override
    {
        return terminal_;
    }
};

auto set_expectations(MockTerminal& terminal)
{
    return std::tuple{
        NAMED_ALLOW_CALL(terminal, print(ANY(std::string))),
        NAMED_REQUIRE_CALL(terminal, read()).RETURN("1"),
        NAMED_REQUIRE_CALL(terminal, read()).RETURN("2"),
        NAMED_REQUIRE_CALL(terminal, print("The sum is: 3"))
    };
}

TEST_CASE("Dependency Injection with Factory Method", "[Calculator, DI-FactoryMethod]")
{
    using DependencyInjection::FactoryMethodDI::Calculator;


    TestableCalculator calculator;
    auto& mock_terminal = calculator.get_terminal();
    auto mock_expectations = set_expectations(mock_terminal);

    calculator.run();
}

class MockLogger : public Dependency::ILogger
{
public:
    MAKE_MOCK1(log, void(const std::string&), override);
};

TEST_CASE("Calculator logging", "[Calculator, Log]")
{
    using namespace std::literals;

    using DependencyInjection::ConstructorDI::Calculator;

    auto mock_logger = std::make_shared<MockLogger>();
    REQUIRE_CALL(*mock_logger, log("Log: add(1, 2)"s));

    auto mock_terminal = std::make_shared<MockTerminal>();
    set_expectations(*mock_terminal);

    Calculator calculator = CalculatorBuilder{}.with_terminal(mock_terminal).with_logger(mock_logger).build();
    calculator.run();
}