
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
        return 0;
    }

    void roll(size_t pins) 
    {

    }
};

TEST(BowlingGameTests, When_GameStarts_ScoreIsZero)
{
    BowlingGame game;

    ASSERT_EQ(game.score(), 0);
}

TEST(BowlingGameTests, When_AllRollsInGutter_ScoreIsZero)
{
    BowlingGame game;

    for(int i = 0; i < 20; ++i)
        game.roll(0);

    ASSERT_EQ(game.score(), 0);
}