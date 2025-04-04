#include "Game.h"

void Game::initVariables()
{
	this->window = nullptr;
}

void Game::initWindow()
{
	this->resolution = sf::Vector2i(256, 224);
	this->window = new sf::RenderWindow(sf::VideoMode({ (unsigned int) resolution.x, (unsigned int) resolution.y}), "Game", (sf::Style::Resize + sf::Style::Close));
	this->view = sf::View();
	view.setSize(sf::Vector2((float)resolution.x, (float)resolution.y));
	view.setCenter(sf::Vector2(view.getSize().x / 2, view.getSize().y / 2));
	view = getLetterboxView(view, resolution.x, resolution.y);

    this->window->setFramerateLimit(60);
}

sf::View Game::getLetterboxView(sf::View view, int windowWidth, int windowHeight)
{
   // Compares the aspect ratio of the window to the aspect ratio of the view,
   // and sets the view's viewport accordingly in order to achieve a letterbox effect.
   // A new view (with a new viewport set) is returned.

    float windowRatio = (float)windowWidth / (float)windowHeight;
    float viewRatio = view.getSize().x / (float)view.getSize().y;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    // If horizontalSpacing is true, the black bars will appear on the left and right side.
    // Otherwise, the black bars will appear on the top and bottom.

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect({ posX, posY }, { sizeX, sizeY }));

    return view;
}

Game::Game()
{
    this->initVariables();
    this->initWindow();
}

Game::~Game()
{
    delete this->window;
}

const bool Game::getWindowIsOpen() const
{
    return this->window->isOpen();
}

void Game::pollEvents()
{
    while (const std::optional event = this->window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            this->window->close();

        if (const auto* resized = event->getIf<sf::Event::Resized>())
            view = this->getLetterboxView(view, resized->size.x, resized->size.y);
    }
}

void Game::update()
{
    this->pollEvents();
}

void Game::render()
{
    this->window->clear();

    // Draw game stuff

    this->window->display();
}
