#pragma once
class Player
{
private:
	sf::Sprite sprite;
	sf::Texture textureSheet;

	void initTexture();
	void initSprite();

public:
	Player();
	virtual ~Player();
	
	// functions
	void updateMovement();
	void update();
	void render(sf::RenderTarget& target);

};

