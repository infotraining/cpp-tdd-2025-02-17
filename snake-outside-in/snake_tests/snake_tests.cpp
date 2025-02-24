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

template <typename TTerminal = class TerminalParam>
class SnakeGame
{
    enum class GameState { Menu, Playing };

    TTerminal& terminal_;
    GameState game_state_{GameState::Menu};

public:
    explicit SnakeGame(TTerminal& terminal) : terminal_{terminal}
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
    {}

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
        }
    }

    void render_menu()
    {
        terminal_.render_text({"~~Snake~~", "Play game [P]", "Quit [Q]"});
    }

    void render_game()
    {
        terminal_.clear();
        terminal_.render_snake(Snake{});
        terminal_.render_board(Board{});
        terminal_.flush();
    }
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MockTerminal
{
    MAKE_MOCK1(render_text,  auto (const std::vector<std::string>&) -> void);
    MAKE_MOCK0(read_key,     auto () -> std::optional<Key>);
    MAKE_MOCK0(clear,        auto () -> void);
    MAKE_MOCK1(render_snake, auto (const Snake&) -> void);
    MAKE_MOCK1(render_board, auto (const Board&) -> void);
    MAKE_MOCK0(flush,        auto () -> void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CATCH_REGISTER_ENUM(Key, Key::P, Key::Q, Key::ArrowUp, Key::ArrowDown, Key::ArrowLeft, Key::ArrowRight)

CATCH_REGISTER_ENUM(Direction, Direction::Up, Direction::Down, Direction::Left, Direction::Right)

TEST_CASE("SnakeGame - rendering menu") 
{
    MockTerminal mock_terminal;

    std::vector<std::string> menu_items = {"~~Snake~~", "Play game [P]", "Quit [Q]"};

    REQUIRE_CALL(mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(mock_terminal, read_key()).RETURN(std::nullopt);
    
    SECTION("when game starts")
    {
        SnakeGame game{mock_terminal};
        
        SECTION("menu is rendered")
        {
            REQUIRE_CALL(mock_terminal, render_text(menu_items));
            game.run();
        }
    }
}

TEST_CASE("SnakeGame - game loop")
{
    MockTerminal mock_terminal;

    ALLOW_CALL(mock_terminal, render_text(trompeloeil::_));
    REQUIRE_CALL(mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(mock_terminal, read_key()).RETURN(Key::P);

    SECTION("clears, renders snake & board and flushes in sequence")
    {
        trompeloeil::sequence seq;
        REQUIRE_CALL(mock_terminal, clear()).IN_SEQUENCE(seq);
        REQUIRE_CALL(mock_terminal, render_snake(ANY(Snake))).IN_SEQUENCE(seq);
        REQUIRE_CALL(mock_terminal, render_board(ANY(Board))).IN_SEQUENCE(seq);
        REQUIRE_CALL(mock_terminal, flush()).IN_SEQUENCE(seq);

        SnakeGame game{mock_terminal};
        game.run();
    }
}