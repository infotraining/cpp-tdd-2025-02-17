
#include <algorithm>
#include <string>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bowling.hpp"

using namespace std;

class BowlingGame
{
public:
    size_t score() const
    {
        return m_score;
    }

    void roll(size_t pins) 
    {
        m_score += pins;
    }
private:
    size_t m_score {0};
};

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