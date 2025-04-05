#include "stdafx.h"
#include "Scene.h"

void Scene::initBackgroundTexture()
{
    // Load a texture from file
    if (!this->backgroundTexture.loadFromFile("images/background image.png"))
    {
        // Handle loading error
        std::cout << "ERROR: Couldn't load image." << "\n";
    }
}

void Scene::initBackgroundSprite()
{
    this->backgroundSprite.setTexture(this->backgroundTexture);
}

Scene::Scene()
{
    this->initBackgroundTexture();
    this->initBackgroundSprite();
    this->grid = new Grid();
}

Scene::~Scene()
{
}

void Scene::update()
{
    this->grid->update();
}

void Scene::render(sf::RenderTarget& target)
{
    target.draw(this->backgroundSprite);
    this->grid->render(target);
}

void Scene::pollEvent(sf::Event event)
{
    this->grid->pollEvent(event);
}
