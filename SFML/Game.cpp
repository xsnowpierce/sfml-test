#include "stdafx.h"
#include "Game.h"

void Game::initWindow()
{
	this->window.create(sf::VideoMode(resolution.x, resolution.y), "Game", sf::Style::Resize + sf::Style::Close);
    this->view = sf::View();
    view.setSize(sf::Vector2((float)resolution.x, (float)resolution.y));
    view.setCenter(sf::Vector2(view.getSize().x / 2, view.getSize().y / 2));
    view = getLetterboxView(view, resolution.x, resolution.y);
    this->window.setFramerateLimit(60);
}

void Game::initScene()
{
    Block::initTexture();
    this->currentScene = new Scene();
}

Game::Game()
{
    this->initWindow();
    this->initScene();
}

Game::~Game()
{
    delete this->currentScene;
}

void Game::updateScene()
{
    this->currentScene->update();
}

void Game::update()
{
    // polling window events
    while (this->window.pollEvent(this->event))
    {
        this->currentScene->pollEvent(event);
        if (this->event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::Resized)
            view = this->getLetterboxView(view, event.size.width, event.size.height);
    }

    this->updateScene();
}

void Game::renderScene()
{
    this->currentScene->render(this->window);
}

void Game::render()
{
    this->window.clear();
    this->window.setView(view);
    // render game
    this->renderScene();

    this->window.display();
}

const sf::RenderWindow& Game::getWindow() const
{
    return this->window;
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