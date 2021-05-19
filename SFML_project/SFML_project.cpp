#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

struct enemy: sf::CircleShape
{
    float dx;
    float dy;

    enemy()
    {
        setFillColor(sf::Color::Red);
    }

    bool hit(const sf::CircleShape &player)
    {
        float distance = sqrt(pow(player.getPosition().x + player.getRadius() - getPosition().x - getRadius(), 2) + pow(player.getPosition().y + player.getRadius() - getPosition().y - getRadius(), 2));
        return getRadius() + player.getRadius() > distance;
    }

    void randRadius(const sf::CircleShape& player)
    {
        setRadius(rand()%(int)player.getRadius() + (int)(player.getRadius()*0.2));
    }

    void move()
    {
        sf::Vector2<float> pos = getPosition();
        pos.x += dx;
        pos.y += dy;
        setPosition(pos);
    }

    void resetPosition(int W, int H)
    {
        int x = 0;
        int y = 0;
        if (rand() % 2)
        {
            x = -(rand()%100);
            dx = 1;
        }
        else
        {
            x = rand() % (W + 100);
            dx = -1;
        }
        if (rand() % 2)
        {
            y = -(rand() % 100);
            dy = 1;
        }
        else
        {
            y = rand() % (H + 100);
            dy = -1;
        }
        setPosition({ (float)x, (float)y });
    }
};

template<typename T>
struct deleter : std::unary_function<const T*, void>
{
    void operator() (const T* ptr) const
    {
        delete ptr;
    }
};

int main()
{
    int W = 1024;
    int H = 768;

    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode(W, H), "SFML works!");
    sf::CircleShape player(10.f);
    player.setFillColor(sf::Color::Green);
    window.setMouseCursorVisible(false);


    std::vector<enemy*> enemies;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "arial.ttf not found" << std::endl;
        return 1;
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    bool lose = false;
    int points = 0;
    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        int radius = player.getRadius();
        worldPos.x -= radius;
        worldPos.y -= radius;
        player.setPosition(worldPos);

        if (enemies.size() < 10)
        {
            enemy* e = new enemy();
            e->randRadius(player);
            e->resetPosition(W, H);
            enemies.push_back(e);
        }
        
        if (lose)
        {
            text.setString("You lose: " + std::to_string(points) + " points");
            
        }
        else
        {
            for (int i = 0; i < enemies.size(); i++)
            {
                enemy*& e = enemies[i];
                e->move();
                if (e->hit(player))
                {
                    if (e->getRadius() >= player.getRadius())
                    {
                        lose = true;
                    }
                    else
                    {
                        points++;

                        e->randRadius(player);
                        player.setRadius(player.getRadius() + 2);
                    }
                    e->resetPosition(W, H);
                }
            }
        }


        window.clear();
        for (auto e : enemies) {
            window.draw(*e);
        }
        
        if (lose)
        {
            window.draw(text);
        }
        else
        {
            window.draw(player);
        }
        window.display();
    }
    std::for_each(enemies.begin(), enemies.end(), deleter<enemy>());
    enemies.clear();
    return 0;
}