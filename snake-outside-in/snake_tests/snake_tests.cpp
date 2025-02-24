#include "snake/snake.hpp"

#include <algorithm>
#include <boost/di.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/trompeloeil.hpp>
#include <ranges>
#include <trompeloeil.hpp>

using namespace std;
using namespace std::literals;

struct Point
{
    int x, y;

    bool operator==(const Point&) const = default;
};

std::ostream& operator<<(std::ostream& os, const Point& pt)
{
    return os << "Point(" << pt.x << ", " << pt.y << ")";
}


struct Board 
{
    int width_, height_;
public:
    Board(int width = 20, int height = 10) : width_{width}, height_{height}
    {}

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
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
    Snake(Board& board, Point head, Direction direction)
        : board_{board}, direction_{direction}
    {
        segments_.push_back(head);
        segments_.push_back(new_segment_from(head, opposite_direction(direction_)));
    }

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

    void move(Direction direction)
    {
        auto new_head = new_segment_from(segments_.front(), direction);

        if ( new_head.x < 0 
                || new_head.x > board_.width() 
                || new_head.y < 0 
                || new_head.y > board_.height())
                {
                    is_alive_ = false;
                }
    }

    bool operator==(const Snake&) const = default;
private:

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

    Direction opposite_direction(Direction direction)
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
};

class TerminalParam;
class SnakeParam;

template <typename TTerminal = class TerminalParam, typename TSnake = SnakeParam>
class SnakeGame
{
    enum class GameState { Menu, Playing, GameOver };

    TTerminal& terminal_;
    TSnake& snake_;
    GameState game_state_{GameState::Menu};
    std::optional<Key> key_pressed_ = std::nullopt;
    Direction direction_;

public:
    explicit SnakeGame(TTerminal& terminal, TSnake& snake) : terminal_{terminal}, snake_{snake}
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

            update();

            render();
        }
    }
private:
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
        switch(game_state_)
        {
            case GameState::Menu:
                render_menu();
                break;
            case GameState::Playing:
                render_game();
                break;
            case GameState::GameOver:
                render_game_over();
                break;
        }
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
        terminal_.render_board(Board{});
        terminal_.flush();
    }

    void render_game_over()
    {
        terminal_.render_text({"Game Over!!!"});
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TSnake = class SnakeParam>
struct MockTerminal
{
    MAKE_MOCK1(render_text,  auto (const std::vector<std::string>&) -> void);
    MAKE_MOCK0(read_key,     auto () -> std::optional<Key>);
    MAKE_MOCK0(clear,        auto () -> void);
    MAKE_MOCK1(render_snake, auto (const TSnake&) -> void);
    MAKE_MOCK1(render_board, auto (const Board&) -> void);
    MAKE_MOCK0(flush,        auto () -> void);
};


template <typename TSnake>
[[no_discard]] auto set_default_expectations(MockTerminal<TSnake>& terminal)
{
    return std::tuple{
        NAMED_ALLOW_CALL(terminal, read_key()).RETURN(std::nullopt),
        NAMED_ALLOW_CALL(terminal, clear()),
        NAMED_ALLOW_CALL(terminal, render_text(ANY(std::vector<std::string>))),
        NAMED_ALLOW_CALL(terminal, render_board(ANY(Board))),
        NAMED_ALLOW_CALL(terminal, render_snake(ANY(TSnake))),
        NAMED_ALLOW_CALL(terminal, flush())
    };
}

struct MockSnake
{
    MAKE_MOCK0(is_alive, auto () -> bool);
    MAKE_MOCK1(move,     auto(Direction) -> void);
};

[[no_discard]] auto set_default_expectations(MockSnake& snake)
{
    return std::tuple{
        NAMED_ALLOW_CALL(snake, is_alive()).RETURN(true),
        NAMED_ALLOW_CALL(snake, move(ANY(Direction)))
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CATCH_REGISTER_ENUM(Key, Key::P, Key::Q, Key::ArrowUp, Key::ArrowDown, Key::ArrowLeft, Key::ArrowRight)

CATCH_REGISTER_ENUM(Direction, Direction::Up, Direction::Down, Direction::Left, Direction::Right)

namespace di = boost::di;

auto default_injector = [] {
    return di::make_injector(
        di::bind<class TerminalParam>.to<MockTerminal>(),
        di::bind<class SnakeParam>.to<MockSnake>()
    );
};

TEST_CASE("SnakeGame - rendering menu", "[SnakeGame]") 
{
    auto injector = default_injector();

    using MockTerminalType = decltype(injector.create<MockTerminal>());
    auto mock_terminal = injector.create<std::shared_ptr<MockTerminalType>>();

    std::vector<std::string> menu_items = {"~~Snake~~", "Play game [P]", "Quit [Q]"};

    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);
    
    SECTION("when game starts")
    {
        SnakeGame game = injector.create<SnakeGame>();
        
        SECTION("menu is rendered")
        {
            REQUIRE_CALL(*mock_terminal, render_text(menu_items));
            game.run();
        }
    }
}

TEST_CASE("SnakeGame - game loop", "[SnakeGame]")
{
    auto injector = default_injector();

    using MockTerminalType = decltype(injector.create<MockTerminal>());
    auto mock_terminal = injector.create<std::shared_ptr<MockTerminalType>>();

    ALLOW_CALL(*mock_terminal, render_text(trompeloeil::_));
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::P);

    using MockSnakeType = decltype(injector.create<MockSnake>());
    auto mock_snake = injector.create<std::shared_ptr<MockSnakeType>>();
    auto mock_snake_expectations = set_default_expectations(*mock_snake);

    SECTION("clears, renders snake & board and flushes in sequence")
    {
        trompeloeil::sequence seq;
        REQUIRE_CALL(*mock_terminal, clear()).IN_SEQUENCE(seq);
        REQUIRE_CALL(*mock_terminal, render_snake(ANY(MockSnakeType))).IN_SEQUENCE(seq);
        REQUIRE_CALL(*mock_terminal, render_board(ANY(Board))).IN_SEQUENCE(seq);
        REQUIRE_CALL(*mock_terminal, flush()).IN_SEQUENCE(seq);

        SnakeGame game = injector.create<SnakeGame>();
        game.run();
    }
}

TEST_CASE("SnakeGame - game over", "[SnakeGame]")
{
    auto injector = default_injector();

    using MockTerminalType = decltype(injector.create<MockTerminal>());
    auto mock_terminal = injector.create<std::shared_ptr<MockTerminalType>>();
    auto mock_terminal_expectations = set_default_expectations(*mock_terminal);

    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::P);

    SECTION("when snake dies")
    {
        using MockSnakeType = decltype(injector.create<MockSnake>());
        auto mock_snake = injector.create<std::shared_ptr<MockSnakeType>>();
        auto mock_snake_expectations = set_default_expectations(*mock_snake);

        REQUIRE_CALL(*mock_snake, is_alive()).RETURN(false);
        REQUIRE_CALL(*mock_snake, is_alive()).RETURN(true);
        REQUIRE_CALL(*mock_snake, is_alive()).RETURN(true);

        SECTION("game over is rendered")
        {
            REQUIRE_CALL(*mock_terminal, render_text(std::vector<std::string>{"Game Over!!!"}));

            SnakeGame game = injector.create<SnakeGame>();
            game.run();
        }
    }
}

TEST_CASE("SnakeGame - pressing keys-arrows", "[SnakeGame]")
{
    auto injector = default_injector();

    using MockTerminalType = decltype(injector.create<MockTerminal>());
    auto mock_terminal = injector.create<std::shared_ptr<MockTerminalType>>();
    auto mock_terminal_expectations = set_default_expectations(*mock_terminal);

    using MockSnakeType = decltype(injector.create<MockSnake>());
    auto mock_snake = injector.create<std::shared_ptr<MockSnakeType>>();
    auto mock_snake_expectations = set_default_expectations(*mock_snake);

    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::Q);

    // clang-format off
    auto [key, expected_direction] = GENERATE(
        table<Key, Direction>(
            {
                {Key::ArrowUp, Direction::Up},
                {Key::ArrowDown, Direction::Down},
                {Key::ArrowLeft, Direction::Left},
                {Key::ArrowRight, Direction::Right}
            }
        )
    );
    // clang-format on

    DYNAMIC_SECTION("when " << Catch::StringMaker<Key>::convert(key) << " is pressed")
    {
        trompeloeil::sequence seq;
        REQUIRE_CALL(*mock_terminal, read_key()).RETURN(key).IN_SEQUENCE(seq);
        REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::P);

        SnakeGame game = injector.create<SnakeGame>();

        DYNAMIC_SECTION("snake moves " << Catch::StringMaker<Direction>::convert(expected_direction))
        {
            REQUIRE_CALL(*mock_snake, move(expected_direction)).IN_SEQUENCE(seq);
            game.run();
        }
    }
}

TEST_CASE("SnakeGame - snake follows the last set direction", "[SnakeGame]")
{
    auto injector = boost::di::make_injector(
        default_injector()
    );

    using MockTerminalType = decltype(injector.create<MockTerminal>());
    auto mock_terminal = injector.create<std::shared_ptr<MockTerminalType>>();
    auto expectations = set_default_expectations(*mock_terminal);
    
    using MockSnakeType = decltype(injector.create<MockSnake>());
    auto mock_snake = injector.create<std::shared_ptr<MockSnakeType>>();
    auto snake_expectations = set_default_expectations(*mock_snake);

    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);
    REQUIRE_CALL(*mock_terminal, read_key()).RETURN(std::nullopt);

    // clang-format off
    auto [key, expected_direction] = GENERATE(
        table<Key, Direction>(
            {
                {Key::ArrowUp, Direction::Up},
                {Key::ArrowDown, Direction::Down},
                {Key::ArrowLeft, Direction::Left},
                {Key::ArrowRight, Direction::Right}
            }
        )
    );
    // clang-format on

    DYNAMIC_SECTION(Catch::StringMaker<Key>::convert(key) << " is pressed")
    {
        REQUIRE_CALL(*mock_terminal, read_key()).RETURN(key);
        REQUIRE_CALL(*mock_terminal, read_key()).RETURN(Key::P);

        REQUIRE_CALL(*mock_snake, move(expected_direction)).TIMES(3);

        SnakeGame game = injector.create<SnakeGame>();
        game.run();
    }
}

TEST_CASE("Snake - constructor with head coordinates and direction", "[Snake]")
{
    Board board{20, 10};
    Snake snake{board, Point{5, 5}, Direction::Up};

    REQUIRE(snake.segments() == std::vector<Point>{{5, 5}, {5, 6}});
}

TEST_CASE("Snake - constructed with board, point & direction", "[Snake]")
{
    Board board{20, 10};

    auto [direction, expected_segments] = GENERATE(
        table<Direction, std::vector<Point>>(
            {{Direction::Up, {Point{5, 5}, Point{5, 6}}},
                {Direction::Left, {Point{5, 5}, Point{6, 5}}},
                {Direction::Right, {Point{5, 5}, Point{4, 5}}},
                {Direction::Down, {Point{5, 5}, Point{5, 4}}}}));

    Snake snake{board, Point{5, 5}, direction};

    SECTION("has two segments - head & tail")
    {
        REQUIRE(snake.direction() == direction);
        REQUIRE(snake.segments() == expected_segments);
    }

    SECTION("is alive")
    {
        REQUIRE(snake.is_alive() == true);
    }
}

TEST_CASE("Snake - hitting the wall", "[Snake]")
{
    Board board{40, 30};

    const auto [head, direction] = GENERATE(
        table<Point, Direction>(
            {
                {Point{20, 0}, Direction::Up},
                {Point{0, 15}, Direction::Left},
                {Point{20, 30}, Direction::Down},
                {Point{40, 15}, Direction::Right},
            }));

    DYNAMIC_SECTION("Snake's head: " << head << " moving: " << Catch::StringMaker<Direction>::convert(direction))
    {
        Snake snake{board, head, direction};
        CHECK(snake.is_alive());

        snake.move(direction);

        SECTION("snake is dead")
        {
            REQUIRE_FALSE(snake.is_alive());
        }
    }
}