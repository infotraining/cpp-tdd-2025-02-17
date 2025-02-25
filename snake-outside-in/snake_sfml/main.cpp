#include "sfml_terminal.hpp"
#include <snake/snake.hpp>

#include <boost/di.hpp>

namespace di = boost::di;

namespace ParamNames
{
    auto width = [] {};
    auto height = [] {};
    auto apple_count = [] {};

    auto columns = []{};
    auto rows = []{};
}
namespace boost::ext::di
{
    template <>
    struct ctor_traits<Board>
    {
        BOOST_DI_INJECT_TRAITS((named = ParamNames::width)int, (named = ParamNames::height)int, (named = ParamNames::apple_count)size_t, IRandomGenerator rnd_gen);
    };

    template <>
    struct ctor_traits<SfmlTerminal>
    {
        BOOST_DI_INJECT_TRAITS((named = ParamNames::width)int, (named = ParamNames::height)int);
    };
}

int main()
{
    auto board_injector = [] { 
        return di::make_injector(
            di::bind<int>.named(ParamNames::width).to(40),
            di::bind<int>.named(ParamNames::height).to(30),
            di::bind<size_t>.named(ParamNames::apple_count).to(size_t{5}),
            di::bind<IRandomGenerator>.to(RandomGenerator)
        );
    };

    auto injector = di::make_injector(
        board_injector(),
        di::bind<class TerminalParam>.to<SfmlTerminal>(),
        di::bind<class SnakeParam>.to<Snake>()
    );

    auto game = injector.create<SnakeGame>();
    game.run();

    /////////////////
    // hand-wired

    constexpr size_t rows = 30;
    constexpr size_t columns = 40;

    //Board board(columns, rows, 5);
    //Snake snake(board);
    //SfmlTerminal terminal(columns, rows);
    //SnakeGame<SfmlTerminal, Snake> game(terminal, snake, board);
    //game.run();
}
