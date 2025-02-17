
#include <algorithm>
#include <string>
#include <memory>
#include <ranges>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bowling.hpp"
#include <array>
#include <numeric>

using namespace std;

class BowlingGame
{
public:
    size_t score() const
    {
        size_t sum{};

        for(int i = 0; i < m_rolls.size(); i += 2)
        {
            if (is_spare(i))
                sum += spare_bonus(i);
                
            sum+= frame_score(i);
        }

        return sum;
    }

    void roll(size_t pins) 
    {
        if (pins > number_of_pins_in_frame)
            throw std::invalid_argument("Cannot knock down more than 10 pins");
            
        m_rolls[m_roll_index] = pins;
        ++m_roll_index;
    }
private:
    size_t m_score {0};
    size_t m_roll_index{};
    std::array<size_t, 20> m_rolls{};

    constexpr static size_t number_of_pins_in_frame = 10;

    bool is_spare(size_t roll_index) const
    {
        return m_rolls[roll_index] + m_rolls[roll_index + 1] == number_of_pins_in_frame;
    }

    size_t spare_bonus(size_t roll_index) const
    {
        return m_rolls[roll_index + 2];
    }

    size_t frame_score(size_t roll_index) const 
    {
        return m_rolls[roll_index] + m_rolls[roll_index + 1];
    }
};

namespace Alt1
{
    class BowlingGame
    {
    public:
        size_t score() const
        {
            return std::accumulate(std::begin(m_rolls), std::end(m_rolls), 0ULL);
        }
   
        void roll(size_t pins)
        {
            if (m_rolls.size() > 2)
            {
                auto index = m_rolls.size() - 1;
                if (m_rolls[index] + m_rolls[index - 1] == 10) //spare
                {
                    pins *= 2;
                }
            }
   
            m_rolls.emplace_back(pins);
        }
   
    private:
        size_t m_score{};
   
        vector<size_t> m_rolls{};
    };
}

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

TEST_F(BowlingGameTests, WhenMoreThan10PinsKnockedDown_RollThrows)
{
    ASSERT_THROW(game.roll(11), std::invalid_argument);
}
