#pragma once

#include "stdafx.h"
#include "Scene.h"

class Game
{
private:
	sf::RenderWindow window;
	sf::Event event;
	sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);
	sf::View view;
	sf::Vector2i resolution = sf::Vector2i(256, 224);

	Scene* currentScene;

	void initWindow();
	void initScene();

public:
	Game();
	virtual ~Game();

	void updateScene();
	void update();
	void renderScene();
	void render();

	const sf::RenderWindow& getWindow() const;
};

