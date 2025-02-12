#include "rover.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <ranges>
#include <string>

using namespace std;
using namespace TDD;

TEST_CASE("simple test")
{
    REQUIRE(1 == 1);
}