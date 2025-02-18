#include <iostream>

#include "calculator.hpp"

using namespace std;

int main()
{
    using DependencyInjection::NoDependencyInjection::Calculator;

    Calculator calculator;
    calculator.run();
}