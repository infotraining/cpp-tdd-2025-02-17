
#include <algorithm>
#include <string>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bowling.hpp"
#include <array>
#include <numeric>

using namespace std;

using namespace TDD;

class BowlingGameTests : public ::testing::Test
{
protected:
    BowlingGame game{};

    BowlingGameTests()
    {
         // setup
    }

    // void SetUp() override
    // {        
    // }
    
    // void TearDown() override
    // {        
    // }

    void roll_many(size_t roll, size_t pins)
    {
        for(size_t i = 0; i < roll; ++i)
            game.roll(pins);
    }

    void roll_spare()
    {
        roll_many(2, 5); 
    }

    void roll_strike()
    {
        game.roll(BowlingGame::number_of_pins_in_frame);
    }
};

TEST_F(BowlingGameTests, When_GameStarts_ScoreIsZero)
{
    ASSERT_EQ(game.score(), 0);
}

TEST_F(BowlingGameTests, When_AllRollsInGutter_ScoreIsZero)
{
    roll_many(20, 0);

    ASSERT_EQ(game.score(), 0);
}

TEST_F(BowlingGameTests, When_AllRollsWithoutSpareOrStrike_ScoreIsSumOfPins)
{
    roll_many(20, 1);

    ASSERT_EQ(game.score(), 20);
}

TEST_F(BowlingGameTests, When_Spare_NextRollIsCountedTwice)
{
    roll_spare();

    EXPECT_EQ(game.score(), 10) << "Guard Assertion for SUT"; 

    game.roll(5);    // bonus
    game.roll(1);    // normal roll
    roll_many(16, 1);

    ASSERT_EQ(game.score(), 37); // Assert
}

TEST_F(BowlingGameTests, When_Strike_TwoNextRollsAreCountedTwice)
{
    roll_strike();

    game.roll(5); // bonus - counted twice
    game.roll(1); // bonus - counted twice
    roll_many(16, 1);
    ASSERT_EQ(game.score(), 38);
}

TEST_F(BowlingGameTests, WhenMoreThan10PinsKnockedDown_RollThrows)
{
    ASSERT_THROW(game.roll(11), std::invalid_argument);
}

TEST_F(BowlingGameTests, WhenSpareInLastFrame_ExtraRollIsCounted)
{
    roll_many(18, 1);
    roll_spare();
    game.roll(6);

    ASSERT_EQ(game.score(), 34);
}

TEST_F(BowlingGameTests, WhenStrikeInLastFrame_TwoExtraRollsAreCounted)
{
    roll_many(18, 1);
    roll_strike();
    game.roll(2);
    game.roll(3);

    ASSERT_EQ(game.score(), 33);
}

TEST_F(BowlingGameTests, WithoutBonusInLastFrame_Only20RollsAreCountedForScore)
{
    roll_many(20, 1);

    game.roll(1);
    ASSERT_EQ(game.score(), 20);
}

TEST_F(BowlingGameTests, PerfectGame_ScoreIs300)
{
    roll_many(12, 10);
    
    ASSERT_EQ(game.score(), 300);
}

struct BowlingGameParams
{
    const char* test_description;
    std::vector<size_t> rolls;
    size_t expected_score;
};

std::ostream& operator<<(std::ostream& out, const BowlingGameParams& p)
{
    out << p.test_description;
    return out;
}  

struct BowlingGameParamTests : ::testing::TestWithParam<BowlingGameParams>
{
    BowlingGame game; // SUT
};

TEST_P(BowlingGameParamTests, RealGameExamplesWithScore)
{
    const BowlingGameParams param = GetParam();    

    for(size_t pins : param.rolls)
        game.roll(pins);

    ASSERT_EQ(game.score(), param.expected_score) << param.test_description;
}

BowlingGameParams params[] = {
    { "simple game - all rolls one pin", { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 20 },
    { "simple game - different rolls", {0, 8, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 27},
    { "strike & spare", {10, 4, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 47},
    { "all spares & strike", {1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 10}, 119}
};

INSTANTIATE_TEST_SUITE_P(PackOfBowlingTests, BowlingGameParamTests, ::testing::ValuesIn(params));
