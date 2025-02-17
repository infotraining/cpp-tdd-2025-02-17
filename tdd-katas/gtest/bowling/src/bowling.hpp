#ifndef BOWLING_HPP
#define BOWLING_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <numeric>
#include <tuple>
#include <stdexcept>
#include <cassert>

namespace TDD
{
    class BowlingGame
    {
    public:
        size_t score() const
        {
            size_t sum{};

            for (int frame_index = 0, i = 0; frame_index < 10; ++frame_index)
            {
                auto [current_frame_score, frame_size] = frame_score(i);
                sum += current_frame_score;

                if (is_strike(i))
                    sum += strike_bonus(i);
                else if (is_spare(i))
                    sum += spare_bonus(i);

                i += frame_size;
            }

            return sum;
        }

        void roll(size_t pins)
        {
            if (pins > number_of_pins_in_frame)
                throw std::invalid_argument("Cannot knock down more than 10 pins");

            assert(m_roll_index < m_rolls.size());

            m_rolls[m_roll_index] = pins;
            ++m_roll_index;
        }

        constexpr static size_t number_of_pins_in_frame = 10;

    private:
        size_t m_score{0};
        size_t m_roll_index{};
        std::array<size_t, 22> m_rolls{};

        std::tuple<size_t, size_t> frame_score(size_t roll_index) const
        {
            if (is_strike(roll_index))
                return {number_of_pins_in_frame, 1};

            return {m_rolls[roll_index] + m_rolls[roll_index + 1], 2};
        }

        bool is_spare(size_t roll_index) const
        {
            return m_rolls[roll_index] + m_rolls[roll_index + 1] == number_of_pins_in_frame;
        }

        size_t spare_bonus(size_t roll_index) const
        {
            return m_rolls[roll_index + 2];
        }

        bool is_strike(size_t roll_index) const
        {
            return m_rolls[roll_index] == number_of_pins_in_frame;
        }

        size_t strike_bonus(size_t roll_index) const
        {
            return m_rolls[roll_index + 1] + m_rolls[roll_index + 2];
        }
    };
}

#endif