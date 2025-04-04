#pragma once
#include "stdafx.h"
#include "Grid.h"
class Scene
{
private:
	sf::Sprite backgroundSprite;
	sf::Texture backgroundTexture;

	void initBackgroundTexture();
	void initBackgroundSprite();

	Grid* grid;

public:
	Scene();
	~Scene();
	void update();
	void render(sf::RenderTarget& target);
};

