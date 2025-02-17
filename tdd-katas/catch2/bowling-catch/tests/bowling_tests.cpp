#include "bowling.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

using namespace std;

TEST_CASE("simple test!", "[basic,constructor]")
{
    std::string text = "abc";

    REQUIRE(text == "abc");
    REQUIRE_FALSE(text.empty());
}

unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial, math]")
{
    REQUIRE(Factorial(0) == 0);
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

SCENARIO("std::vector - push_back", "[vector,push_back]")
{
    GIVEN("default constructed vector")
    {
        std::vector<int> vec;

        WHEN("push_back one item")
        {
            vec.push_back(42);

            THEN("size is increased by one")
            {
                REQUIRE(vec.size() == 1);
            }

            AND_THEN("capacity is increased")
            {
                REQUIRE(vec.capacity() >= 1);
            }
        }
    }

    GIVEN("given constructed with item")
    {
        std::vector<int> vec = {1, 2, 3, 4};
        CHECK(vec.size() == 4);
        CHECK(vec.capacity() == 4);

        WHEN("push_back one item")
        {
            vec.push_back(42);

            THEN("size is increased by one")
            {
                REQUIRE(vec.size() == 5);
            }

            AND_THEN("capacity is increased")
            {
                REQUIRE(vec.capacity() >= 5);
            }
        }
    }
}

void check_number(int n)
{
    REQUIRE(n < 10);
}

TEST_CASE("check numbers")
{
    check_number(1);
    check_number(5);
}

struct BowlingGame
{
    size_t score_{};

    void roll(size_t pins)
    {
        score_ += pins;
    }

    size_t score() const
    {
        return score_;
    }
};

TEST_CASE("Bowling game")
{
    BowlingGame game;

    SECTION("no extra marks - score is sum of pins")
    {
        auto [pins, expected_score] = GENERATE(
            table<size_t, size_t>(
                {{1, 2},
                    {2, 4},
                    {3, 6},
                    {4, 8}}));

        DYNAMIC_SECTION("roll(" << pins << ")")
        {
            game.roll(pins);
            game.roll(pins);

            REQUIRE(game.score() == expected_score);
        }
    }
}

TEST_CASE("Generators")
{
    auto i = GENERATE(1, 2);

    SECTION("one")
    {
        auto j = GENERATE(-3, -2);
        DYNAMIC_SECTION("test: " << i << " < " << j)
        {
            REQUIRE(j < i);
        }
    }

    SECTION("two")
    {
        auto k = GENERATE(4, 5, 6);
        REQUIRE(i != k);
    }
}

TEST_CASE("Generating random ints", "[example][generator]")
{
    SECTION("Deducing functions")
    {
        auto i = GENERATE(take(100, filter([](int i) { return i % 2 == 1; }, random(-100, 100))));
        REQUIRE(i > -100);
        REQUIRE(i < 100);
        REQUIRE(i % 2 == 1);
    }
}

TEST_CASE("Factorial") 
{
    CHECK(Factorial(0) == 0);
    // some more asserts..

    CHECK(Factorial(5) == 120);
    // some more asserts..

    // now let's benchmark:
    BENCHMARK("Factorial 0") {
        return Factorial(0);
    }; 

    BENCHMARK("Factorial 1") {
        return Factorial(1);
    };

    BENCHMARK("Factorial 3") {
        return Factorial(3);
    };

    BENCHMARK("Factorial 5") {
        return Factorial(5);
    };
}