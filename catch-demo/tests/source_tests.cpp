#include <catch2/catch_test_macros.hpp>
#include "source.hpp"

using namespace std;

TEST_CASE("pushing items to vector", "[vector][push]")
{
    vector<int> vec{ 3 };

    CHECK(vec.size() == 1);
    CHECK(vec.capacity() == 1);

    SECTION("push_back")
    {       
        vec.push_back(1);

        SECTION("size is increased")
        {
            REQUIRE(vec.size() == 2);
        }

        SECTION("capacity can be increased")
        {
            REQUIRE(vec.capacity() >= 2);
        }
    }
}

SCENARIO("pushing items to vector")
{
    GIVEN("vector with data")
    {
        vector<int> vec = { 1, 2, 3 };
        auto prev_size = vec.size();

        WHEN("push_back() an item")
        {
            vec.push_back(4);

            THEN("size is increased")
            {
                REQUIRE(vec.size() - prev_size == 1);
            }
        }
    }
}