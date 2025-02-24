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

class TerminalParam;

template <typename TTerminal = class TerminalParam>
class SnakeGame
{
    TTerminal& terminal_;
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

            render_menu();
        }
    }
private:
    void render_menu()
    {
        terminal_.render_text({"~~Snake~~", "Play game [P]", "Quit [Q]"});
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MockTerminal
{
    MAKE_MOCK1(render_text, auto (const std::vector<std::string>&) -> void);
    MAKE_MOCK0(read_key,    auto () -> std::optional<Key>);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CATCH_REGISTER_ENUM(Key, Key::P, Key::Q, Key::ArrowUp, Key::ArrowDown, Key::ArrowLeft, Key::ArrowRight)

CATCH_REGISTER_ENUM(Direction, Direction::Up, Direction::Down, Direction::Left, Direction::Right)

TEST_CASE("SnakeGame - when game starts menu is rendered") 
{
    MockTerminal mock_terminal;

    std::vector<std::string> menu_items = {"~~Snake~~", "Play game [P]", "Quit [Q]"};

    REQUIRE_CALL(mock_terminal, read_key()).RETURN(Key::Q);
    REQUIRE_CALL(mock_terminal, read_key()).RETURN(std::nullopt);
    REQUIRE_CALL(mock_terminal, render_text(menu_items));

    SnakeGame game{mock_terminal};

    game.run();
}