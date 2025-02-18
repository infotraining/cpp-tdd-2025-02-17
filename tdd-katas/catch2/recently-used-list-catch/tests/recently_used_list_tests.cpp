#include <catch2/catch_test_macros.hpp>
#include "recently_used_list.hpp"
#include <algorithm>
#include <deque>
// #include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_all.hpp>

using namespace std;

SCENARIO("Initialize", "[stack]")
{
    GIVEN("Fresh stack")
    {
        TDD::RecentlyUsedList stack;
        REQUIRE(stack.size() == 0);

        WHEN("Add first item")
        {
            stack.push("first item");

            THEN("Size is increased")
            {
                AND_THEN("Size is 1")
                {
                    REQUIRE(stack.size() == 1);
                }
                
                AND_THEN("Recently added item is in index zero")
                {
                    REQUIRE(stack[0] == "first item"); 
                }
            }
        }
    }
}

SCENARIO("Initialize and add multiple items", "[stack]")
{
    GIVEN("Default stack")
    {
        TDD::RecentlyUsedList stack;

        auto [strings, size] = GENERATE(table<std::vector<std::string>, size_t>
            ( 
                {
                    {{"a1", "a2"}, 2},
                    {{"b1", "b2", "b3"}, 3},
                    {{"Gdy", "Ala", "ma", "kota", "to", "kot", "dalej", "ma", "wyrypane"}, 8}
                }
            ));

        WHEN("Add " << strings.size() << " items")
        {
            for(const auto& i : strings )
                stack.push(i);

            THEN("Size incresed to " << size)
            {
                REQUIRE(stack.size() == size);
            }
            
            AND_THEN("Recently added item is in index zero")
            {
                REQUIRE(stack[0] == strings.back()); 
            }
        }
    }
}

SCENARIO("Storing multiple items", "[stack]")
{
    TDD::RecentlyUsedList stack;
    GIVEN("Container with unique items")
    {
        stack.push("first item");
        stack.push("second item");

        WHEN("Add duplicate item")
        {
            stack.push("first item");
            THEN("Size is not changed")
            {
                REQUIRE(stack.size() == 2);
            }

            AND_THEN("Duplicated element is moved to beggining")
            {
                REQUIRE(stack[0] == "first item"); 
            }
        }

        WHEN("Add unique item") 
        {
            stack.push("third item");
            THEN("Size incresed")
            {
                REQUIRE(stack.size() == 3);
            }

            AND_THEN("Added item is on front")
            {
                REQUIRE(stack[0] == "third item");
            }
        }
    }
}

SCENARIO("Null insertions", "[stack]")
{
    TDD::RecentlyUsedList stack;
    GIVEN("Empty container")
    {
        WHEN("Add empty string")
        {
            THEN("Exception was thrown")
            {
                REQUIRE_THROWS_AS(stack.push(""), std::invalid_argument);
            }
        }
    }

    GIVEN("Container with multiple items")
    {
        stack.push("a");
        stack.push("b");
        WHEN("Add empty string")
        {
            THEN("Exception was thrown")
            {   
                REQUIRE_THROWS_AS(stack.push(""), std::invalid_argument);
            }
        }
    }
}

TDD::RecentlyUsedList create_list_from(std::initializer_list<std::string> lst)
{
    TDD::RecentlyUsedList rul;
    for(const auto& item : lst)
        rul.push(item);

    return rul;
}

TDD::RecentlyUsedList create_bounded_list_from(size_t capacity, std::initializer_list<std::string> lst)
{
    TDD::RecentlyUsedList rul(capacity);
    for(const auto& item : lst)
        rul.push(item);
    
    REQUIRE(rul.GetCapacity() == capacity);
        
    return rul;
}

SCENARIO("Overflow", "[stack]")
{
    GIVEN("Full container")
    {
        auto stack = create_bounded_list_from(5, {"a", "b", "c", "d", "e"});
        CHECK(stack.size() == stack.GetCapacity());

        auto leastElement{stack[stack.size()-1]};

        WHEN("Added one more")
        {
            stack.push("f");
            
            THEN("Size not changed")
            {
                REQUIRE(stack.size() == stack.GetCapacity()); 
                
                // AND_THEN("added item is at the beginning")
                // {
                //     REQUIRE(stack[0] == "f");
                // }

                // AND_THEN("The oldest items is dropped")
                // {
                //     REQUIRE(stack[4] != leastElement);
                // }
            } 

            AND_THEN("added item is at the beginning")
            {
                REQUIRE(stack[0] == "f");
            }

            AND_THEN("The oldest items is dropped")
            {
                REQUIRE(stack[4] != leastElement);
            }
        }
    }
}