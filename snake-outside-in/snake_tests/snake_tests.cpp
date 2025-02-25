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
    auto mock_terminal_expectations = set_default_expectations(*mock_terminal);

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
        REQUIRE_CALL(*mock_terminal, render_board(ANY(Board))).IN_SEQUENCE(seq);
        REQUIRE_CALL(*mock_terminal, render_snake(ANY(MockSnakeType))).IN_SEQUENCE(seq);
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

TEST_CASE("Snake - constructed with board")
{
    Board board{20, 10};
 
    Snake snake{board};

    SECTION("has head in the middle of the board")
    {
        REQUIRE(snake.head() == Point{10, 5});
    }

    SECTION("is directed up")
    {
        REQUIRE(snake.direction() == Direction::Up);
    }
    
    SECTION("is alive")
    {
        REQUIRE(snake.is_alive() == true);
    }
}

TEST_CASE("Snake - moving on the board", "[Snake]")
{
    Board board{20, 10};
    Point head{5, 5};
    
    auto [snake_direction, move_direction, expected_segments] = GENERATE(
        table<Direction, Direction, std::vector<Point>>(
            {
                { Direction::Up, Direction::Up, { Point{5, 4}, Point{5, 5} } },                
                { Direction::Up, Direction::Left, { Point{4, 5}, Point{5, 5} } },
                { Direction::Up, Direction::Right, { Point{6, 5}, Point{5, 5} } },
                { Direction::Down, Direction::Down, { Point{5, 6}, Point{5, 5} } },
                { Direction::Down, Direction::Left, { Point{4, 5}, Point{5, 5} } },
                { Direction::Down, Direction::Right, { Point{6, 5}, Point{5, 5} } },
                { Direction::Left, Direction::Up, { Point{5, 4}, Point{5, 5} } },
                { Direction::Left, Direction::Down, { Point{5, 6}, Point{5, 5} } },
                { Direction::Left, Direction::Left, { Point{4, 5}, Point{5, 5} } },
                { Direction::Right, Direction::Up, { Point{5, 4}, Point{5, 5} } },
                { Direction::Right, Direction::Down, { Point{5, 6}, Point{5, 5} } },
                { Direction::Right, Direction::Right, { Point{6, 5}, Point{5, 5} } }
                
            }
        )
    );
            
    DYNAMIC_SECTION("Snake's head: " << head 
        << ", directed: " <<  Catch::StringMaker<Direction>::convert(snake_direction)
        << ", moving: " << Catch::StringMaker<Direction>::convert(move_direction))
    {
        
        Snake snake{board, head, snake_direction};
        snake.move(move_direction);

        REQUIRE(snake.segments() == expected_segments);
    }
}

TEST_CASE("Snake - moving backward is ignored", "[Snake]")
{
    Board board{20, 10};

    auto [snake_direction, move_direction, expected_segments] = GENERATE(
        table<Direction, Direction, std::vector<Point>>(
            {
                { Direction::Up, Direction::Down, { Point{5, 4}, Point{5, 5} } },
                { Direction::Down, Direction::Up, { Point{5, 6}, Point{5, 5} } },
                { Direction::Left, Direction::Right, { Point{4, 5}, Point{5, 5} } },
                { Direction::Right, Direction::Left, { Point{6, 5}, Point{5, 5} } }
            }
        )
    );

    DYNAMIC_SECTION("Snake directed: " << Catch::StringMaker<Direction>::convert(snake_direction)
        << ", moving: " << Catch::StringMaker<Direction>::convert(move_direction))
    {
        Snake snake{board, Point{5, 5}, snake_direction};
        snake.move(move_direction);

        SECTION("snake moves in the previously set direction")
        {
            REQUIRE(snake.segments() == expected_segments);
        }

        SECTION("direction is not changed")
        {
            REQUIRE(snake.direction() == snake_direction);
        }
    }
}

TEST_CASE("Snake - hitting the wall", "[Snake][Board]")
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

TEST_CASE("Snake - eating itself", "[Snake]")
{
    Board board{40, 30};

    Snake snake{board, {Point{20, 15}, Point{20, 16}, Point{20, 17}, Point{21, 17}, Point{21, 16}, Point{21, 15}}, Direction::Up};

    snake.move(Direction::Right);

    SECTION("snake is dead")
    {
        REQUIRE_FALSE(snake.is_alive());
    }
}

TEST_CASE("Board - constructed with apples count")
{
    auto stub_rnd = [seed = 0](int min, int max) mutable {
        auto value = ++seed;
        return value;
    };

    Board board{10, 20, 3, stub_rnd};

    SECTION("has apples with randomly assigned coordinates")
    {
        REQUIRE(board.apples() == std::vector{Point{1, 2}, Point{3, 4}, Point{5, 6}});
    }
}

TEST_CASE("Snake - eating apples from the board")
{
    auto stub_rnd =  [seed = 20](int min, int max) mutable {
        return seed--;
    };

    Board board{40, 40, 1, stub_rnd};
    Point apple{20, 19};
    CHECK(board.apples() == std::vector{apple});

    Snake snake{board};
    CHECK(snake.head() == Point{20, 20});
    CHECK(snake.direction() == Direction::Up);

    SECTION("when snake eats apple")
    {
        snake.move(Direction::Up);
        
        SECTION("snake grows by one segment")
        {
            REQUIRE(snake.segments() == std::vector{Point{20, 19}, Point{20, 20}, Point{20, 21}});
        }
        
        SECTION("apple is removed from the board")
        {
            REQUIRE_FALSE(board.has_apple(apple));
        }

        SECTION("two new apples are randomly added to the board")
        {
            REQUIRE(board.apples() == std::vector{Point{18, 17}, Point{16, 15}});
        }
    }    
}
