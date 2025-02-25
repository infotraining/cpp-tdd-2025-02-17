#ifndef SFML_TERMINAL_HPP
#define SFML_TERMINAL_HPP

#include <snake/snake.hpp>
#include <snake/snake.hpp>
#include <SFML/Graphics.hpp>
#include <format>

class SfmlTerminal 
{
    int columns_, rows_;
    sf::RenderWindow window_;
    constexpr static size_t segment_size_ = 30;
    Key key_;

public:
    SfmlTerminal(int columns, int rows)
        : columns_{columns}, rows_{rows}
        ,window_(sf::VideoMode((columns + 1) * segment_size_, (rows + 1) * segment_size_), "SFML Snake")
    {
    }

    ~SfmlTerminal()
    {
        sf::Event event;
        while (window_.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window_.close();
        }
    }

    std::optional<Key> read_key()
    {
        sf::Event event;
        window_.pollEvent(event);

        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            key_ = Key::Q;        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            key_ = Key::P;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            key_ = Key::ArrowLeft;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            key_ = Key::ArrowRight;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            key_ = Key::ArrowDown;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            key_ = Key::ArrowUp;

        return key_;
    }

    void clear()
    {
        window_.clear();
    }

    void render_snake(const Snake& snake)
    {
        for(const auto& segment : snake.segments())
        {            
            draw_rectangle(segment.x, segment.y, sf::Color::Green);
        }
    }

    void render_board(const Board& board)
    {        
        sf::Color gray(164, 164, 164);
        for(int i = 0; i <= columns_; ++i)
        {
            draw_rectangle(i, 0, gray);
            draw_rectangle(i, rows_, gray);
        }

        for(int i = 0; i <= rows_; ++i)
        {
            draw_rectangle(0, i, gray);
            draw_rectangle(columns_, i, gray);
        }
        

        for(const auto& apple : board.apples())
        {
            draw_apple(apple.x, apple.y);
        }
    }

    void render_text(const std::vector<std::string>& lines)
    {
        window_.clear();
        std::string message;
        for(const auto& line : lines)
        {
            message += std::format("{:^25}\n", line);
        }

        sf::Font font;
        font.loadFromFile("ibm-vga-9x16.ttf");
        sf::Text text(message, font);
        text.setCharacterSize(50);
        text.setFillColor(sf::Color::White);
        auto text_bounds = text.getGlobalBounds();
        const size_t x_offset = text_bounds.getSize().x / 2;
        const size_t y_offset = text_bounds.getSize().y / 2;
        const auto x_center = window_.getSize().x / 2;
        const auto y_center = window_.getSize().y / 2;
        text.setPosition(x_center - x_offset, y_center / 2 - y_offset);
        window_.draw(text);
    }

    void flush()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
        window_.display();
    }
private:
    void draw_rectangle(size_t i, size_t j, sf::Color color)
    {
        sf::RectangleShape rectangle(sf::Vector2f(segment_size_, segment_size_));
        rectangle.setFillColor(color);
        rectangle.setPosition(i * segment_size_, j * segment_size_);
        window_.draw(rectangle);
    }

    void draw_apple(size_t i, size_t j)
    {
        sf::CircleShape circle(segment_size_ / 2);
        circle.setFillColor(sf::Color::Red);
        circle.setPosition(i * segment_size_, j * segment_size_);
        window_.draw(circle);
    }
};

#endif // SFML_TERMINAL_HPP