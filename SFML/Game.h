#pragma once

#include <SFML/Graphics.hpp>
#include "Grid.h"

/*
* Class that acts as the game engine
* Wrapper class.
*/
class Game
{
private:
	// Variables
	// Window
	sf::Vector2i resolution;
	sf::RenderWindow* window;
	sf::View view;

	// Game objects
	Grid grid;

	// Private functions
	void initVariables();
	void initWindow();
	sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);
public:
	// Constructors
	Game();
	~Game();

	// Accessors
	const bool getWindowIsOpen() const;


	// Functions
	void pollEvents();
	void update();
	void render();
};