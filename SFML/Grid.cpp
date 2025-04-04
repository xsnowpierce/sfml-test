#include "stdafx.h"
#include "Grid.h"

void Grid::initBackgroundTexture()
{
	if (!this->backgroundTexture.loadFromFile("images/grid_outline.png")) {
		std::cout << "ERROR: Couldn't load grid image." << "\n";
	}
}

void Grid::initBackgroundSprite()
{
	this->backgroundSprite.setTexture(this->backgroundTexture);
	this->backgroundSprite.move(sf::Vector2(0.f, -5.f));
}

Grid::Grid()
{
	this->initBackgroundTexture();
	this->initBackgroundSprite();
}

Grid::~Grid()
{
}

void Grid::update()
{
}

void Grid::render(sf::RenderTarget& target)
{
	target.draw(this->backgroundSprite);
}
