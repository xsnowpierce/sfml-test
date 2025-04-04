#include "stdafx.h"
#include "Player.h"

void Player::initTexture()
{
    // Load a texture from file
    if (!this->textureSheet.loadFromFile("images/sprite_sheet.png"))
    {
        // Handle loading error
        std::cout << "ERROR: Couldn't load image." << "\n";
    }
}

void Player::initSprite()
{
    // Set the texture to the sprite
    this->sprite.setTexture(this->textureSheet);
    this->sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
}

Player::Player()
{
    this->initTexture();
    this->initSprite();
}

Player::~Player()
{
}

void Player::updateMovement()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) // Left
    {
        this->sprite.move(-1.f, 0.f);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) // right
    {
        this->sprite.move(1.0f, 0.0f);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) // up
    {
        this->sprite.move(0.0f, -1.0f);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) // down
    {
        this->sprite.move(0.0f, 1.0f);
    }
}

void Player::update()
{
    this->updateMovement();
}

void Player::render(sf::RenderTarget& target)
{
	target.draw(this->sprite);
}
