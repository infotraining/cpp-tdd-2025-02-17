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

struct Snake
{
    bool operator==(const Snake&) const = default;
};

struct Board 
{
    bool operator==(const Board&) const = default;
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

public:
    explicit SnakeGame(TTerminal& terminal, TSnake& snake) : terminal_{terminal}, snake_{snake}
    {}
    
    void run()
    {
        while(true)
        {
            std::optional<Key> key_pressed = terminal_.read_key();

            if (key_pressed == Key::Q)
                    return;

            if (key_pressed == Key::P)
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
};

[[no_discard]] auto set_default_expectations(MockSnake& snake)
{
    return std::tuple{
        NAMED_ALLOW_CALL(snake, is_alive()).RETURN(true),
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

TEST_CASE("SnakeGame - rendering menu") 
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

TEST_CASE("SnakeGame - game loop")
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

TEST_CASE("SnakeGame - game over")
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