#include "stdafx.h"
#include "Block.h"
#include "Grid.h"

sf::Texture Block::texture;

sf::IntRect Block::get_texture_bounds_from_enum(BLOCK_TYPE type)
{
	switch (type) {
	case(BLOCK_TYPE::DEFAULT):
		return sf::IntRect(0, 0, 16, 16);
	case(BLOCK_TYPE::BOMB):
		return sf::IntRect(16, 0, 16, 16);
	case(BLOCK_TYPE::PENGUIN):
		return sf::IntRect(0, 16, 16, 16);
	case(BLOCK_TYPE::SEAL):
		return sf::IntRect(0, 32, 16, 16);
	case(BLOCK_TYPE::BIRD):
		return sf::IntRect(0, 48, 16, 16);
	default:
		return sf::IntRect(0, 0, 16, 16);
	}
}

void Block::initSprite()
{
	this->sprite.setTexture(texture);
	this->sprite.setTextureRect(this->get_texture_bounds_from_enum(this->block_type));
}

void Block::push_down()
{
	this->sprite.move(sf::Vector2f(0, 16.f));
	this->grid_position = Grid::get_grid_position_from_screen_position(this->sprite.getPosition());
}

void Block::move_block(int x, int y)
{
	sf::Vector2f movePosition(x * Grid::grid_cell_size, y * Grid::grid_cell_size);
	this->sprite.move(movePosition);
	this->grid_position = Grid::get_grid_position_from_screen_position(this->sprite.getPosition());
}

void Block::set_position(float x, float y)
{
	this->sprite.setPosition(x, y);
	this->grid_position = Grid::get_grid_position_from_screen_position(this->sprite.getPosition());
}

void Block::update()
{
}

void Block::render(sf::RenderTarget& target)
{
	target.draw(sprite);
}

Block::Block(BLOCK_TYPE type, sf::Vector2f position)
{
	this->block_type = type;
	this->initTexture();
	this->initSprite();
	this->set_position(position.x, position.y);
}

Block::~Block()
{
}

sf::Vector2f Block::get_current_grid_position()
{
	return this->grid_position;
}

bool Block::getPositionLocked()
{
	return this->is_position_locked;
}

void Block::setPositionLocked(bool lockPosition)
{
	this->is_position_locked = lockPosition;
}

BLOCK_TYPE Block::get_block_type()
{
	return this->block_type;
}

void Block::initTexture()
{
	if (!texture.loadFromFile("images/sprite_sheet.png")) {
		std::cout << "ERROR: Couldn't load grid image." << std::endl;
	}
}

int Block::get_type_as_int(BLOCK_TYPE type)
{
	switch (type) {
	case(BLOCK_TYPE::DEFAULT):
		return 1;
	case(BLOCK_TYPE::BOMB):
		return 2;
	case(BLOCK_TYPE::PENGUIN):
		return 3;
	case(BLOCK_TYPE::SEAL):
		return 4;
	case(BLOCK_TYPE::BIRD):
		return 5;
	}
	return 0;
}

BLOCK_TYPE Block::get_int_as_type(int value)
{
	switch (value) {
	case(1):
		return BLOCK_TYPE::DEFAULT;
	case(2):
		return BLOCK_TYPE::BOMB;
	case(3):
		return BLOCK_TYPE::PENGUIN;
	case(4):
		return BLOCK_TYPE::SEAL;
	case(5):
		return BLOCK_TYPE::BIRD;
	default:
		return BLOCK_TYPE::EMPTY;
	}
}
