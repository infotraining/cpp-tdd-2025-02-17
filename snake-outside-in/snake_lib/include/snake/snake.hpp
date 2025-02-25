#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <optional>
#include <functional>

enum class Key {
    P,
    Q,
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

struct Point
{
    int x, y;

    bool operator==(const Point&) const = default;
};

std::ostream& operator<<(std::ostream& os, const Point& pt)
{
    return os << "Point(" << pt.x << ", " << pt.y << ")";
}

using IRandomGenerator = std::function<int(int, int)>;

auto RandomGenerator = [](int min, int max) {
    static std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
};

struct Board 
{
    int width_, height_;
    std::vector<Point> apples_;
    IRandomGenerator rnd_generator_;
public:
    Board(int width = 20, int height = 10, size_t apple_count = 0, IRandomGenerator rnd_gen = RandomGenerator) 
        : width_{width}, height_{height}, rnd_generator_{rnd_gen}
    {
        for (size_t i = 0; i < apple_count; ++i)
        {
            apples_.push_back({rnd_generator_(1, width - 1), rnd_generator_(1, height - 1)});
        }
    }

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
    }

    const std::vector<Point>& apples() const
    {
        return apples_;
    }

    bool has_apple(const Point& apple) const
    {
        return std::ranges::find(apples_, apple) != apples_.end();
    }

    void add_apple()
    {
        apples_.push_back(Point{rnd_generator_(1, width_ - 1), rnd_generator_(1, height_ - 1)});
    }

    bool try_eat_apple(const Point& point)
    {
        auto it = std::ranges::find(apples_, point);
        if (it != apples_.end())
        {
            apples_.erase(it);
            return true;
        }

        return false;
    }


    bool is_hitting_walls(const Point& point) const
    {
        return point.x <= 0 || point.x >= width_ || point.y <= 0 || point.y >= height_;
    }

    bool operator==(const Board&) const = default;
};

struct Snake
{
    Board& board_;
    std::vector<Point> segments_;
    Direction direction_;
    bool is_alive_{true};
public:
    explicit Snake(Board& board)
        : Snake{board, Point{board.width() / 2, board.height() / 2}, Direction::Up}
    {}

    Snake(Board& board, Point head, Direction direction)
        : board_{board}, direction_{direction}
    {
        segments_.push_back(head);
        segments_.push_back(new_segment_from(head, opposite_direction(direction_)));
    }

    Snake(Board& board, std::vector<Point> segments, Direction direction)
        : board_{board}, segments_{std::move(segments)}, direction_{direction}
    {}
    
    const std::vector<Point> segments() const
    {
        return segments_;
    }

    Direction direction() const
    {
        return direction_;
    }

    bool is_alive() const
    {
        return is_alive_;
    }

    const Point& head() const
    {
        return segments_.front();
    }

    void move(Direction direction)
    {
        direction_ = update_direction(direction);

        auto new_head = new_segment_from(segments_.front(), direction_);

        if (board_.is_hitting_walls(new_head) || is_eating_itself(new_head))
        {
            is_alive_ = false;
            return;
        }

        segments_.insert(segments_.begin(), new_head);

        if (board_.try_eat_apple(new_head))
        {            
            board_.add_apple();
            board_.add_apple();
        }
        else
        {
            segments_.pop_back();
        }
    }

    bool operator==(const Snake&) const = default;
private:   
    Direction update_direction(Direction new_direction) const
    {
        if (new_direction == opposite_direction(direction_))
            return direction_;

        return new_direction;
    }

    Point new_segment_from(Point old, Direction direction)
    {
        Point new_segment{};
        switch (direction)
        {
        case Direction::Up:
            new_segment = {old.x, old.y - 1};
            break;
        case Direction::Down:
            new_segment = {old.x, old.y + 1};
            break;
        case Direction::Left:
            new_segment = {old.x - 1, old.y};
            break;
        case Direction::Right:
            new_segment = {old.x + 1, old.y};
            break;
        }

        return new_segment;
    }

    Direction opposite_direction(Direction direction) const
    {
        switch (direction)
        {
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        default:
            throw std::runtime_error("Invalid direction");
        }
    }

    bool is_eating_itself(const Point& new_head) const
    {
        return std::ranges::find(segments_, new_head) != segments_.end();
    }
};

class TerminalParam;
class SnakeParam;

template <typename TTerminal = class TerminalParam, typename TSnake = SnakeParam>
class SnakeGame
{
    enum class GameState { Menu, Playing, GameOver };

    TTerminal& terminal_;
    TSnake& snake_;
    Board& board_;
    GameState game_state_{GameState::Menu};
    std::optional<Key> key_pressed_ = std::nullopt;
    Direction direction_{Direction::Up};

public:
    explicit SnakeGame(TTerminal& terminal, TSnake& snake, Board& board) : terminal_{terminal}, snake_{snake}, board_{board}
    {}
    
    void run()
    {
        while(true)
        {
            key_pressed_ = terminal_.read_key();

            if (key_pressed_ == Key::Q)
                    return;

            if (key_pressed_ == Key::P)
                game_state_ = GameState::Playing;

            clear();

            update();

            render();

            flush();
        }
    }
private:
    void clear()
    {
        terminal_.clear();
    }

    void update()
    {
        if (game_state_ == GameState::Playing)        
        {
            direction_ = current_direction(key_pressed_);

            snake_.move(direction_);
            
            if (!snake_.is_alive())
            {
                game_state_ = GameState::GameOver;            
            }        
        }
    }

    void render()
    {
        switch (game_state_)
        {
        case GameState::Menu:
            render_menu();
            break;
        case GameState::Playing:
            terminal_.render_board(board_);
            terminal_.render_snake(snake_);
            break;
        case GameState::GameOver:
            render_game_over();
            break;
        }
    }

    void flush()
    {
        terminal_.flush();
    }

    Direction current_direction(std::optional<Key> key_pressed) const
    {
        if (key_pressed_.has_value())
        {
            switch (*key_pressed_)
            {
            case Key::ArrowDown:
                return Direction::Down;
            case Key::ArrowUp:
                return Direction::Up;
            case Key::ArrowLeft:
                return Direction::Left;
            case Key::ArrowRight:
                return Direction::Right;
            }
        }

        return direction_;
    }

    void render_menu()
    {
        terminal_.render_text({"~~Snake~~", "Play game [P]", "Quit [Q]"});
    }

    void render_game()
    {
        terminal_.clear();
        terminal_.render_snake(snake_);
        terminal_.render_board(board_);
        terminal_.flush();
    }

    void render_game_over()
    {
        terminal_.render_text({"Game Over!!!"});
    }
};

#endif