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
	spriteSheetCoordinate = this->get_texture_bounds_from_enum(this->block_type);
	this->sprite.setTextureRect(spriteSheetCoordinate);
}

void Block::animateSprite()
{
	if(!is_player_falling_block 
		|| block_type == BLOCK_TYPE::DEFAULT 
		|| block_type == BLOCK_TYPE::BOMB 
		|| block_type == BLOCK_TYPE::EMPTY){
		sprite.setTextureRect(spriteSheetCoordinate);
		return;
	}
	
	if (currentAnimationFrameTimer > 0.f) {
		currentAnimationFrameTimer -= single_frame_value;
		return;
	}

	currentAnimationFrameTimer = spriteAnimationTimer;

	sf::IntRect box = sf::IntRect(get_texture_bounds_from_enum(block_type));
	if (currentFrame == 0) {
		
		box.left = 32;
		
	}
	else if (currentFrame == 1) {
		box.left = 48;
	}

	sprite.setTextureRect(box);

	currentFrame++;
	if (currentFrame > 1) {
		currentFrame = 0;
	}
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

void Block::move_block(sf::Vector2f move)
{
	this->move_block(move.x, move.y);
}

void Block::set_position(float x, float y)
{
	this->sprite.setPosition(x, y);
	this->grid_position = Grid::get_grid_position_from_screen_position(this->sprite.getPosition());
}

void Block::set_position(sf::Vector2f position)
{
	this->set_position(position.x, position.y);
}

void Block::update()
{
	if (is_scheduled_to_delete) {
		if (flashTimer.getElapsedTime().asMilliseconds() >= 200) {
			use_deletion_red_flash = !use_deletion_red_flash;
			flashTimer.restart();
		}
	}

	if (use_deletion_red_flash) {
		//std::cout << "Use deletion" << std::endl;
		sprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
	}
	else
		animateSprite();
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
	this->use_deletion_red_flash = false;
	this->is_scheduled_to_delete = false;
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
	this->is_player_falling_block = false;
}

BLOCK_TYPE Block::get_block_type()
{
	return this->block_type;
}

void Block::setPartOfMatch(bool partOfMatch)
{
	this->is_part_of_match = partOfMatch;
}

bool Block::getPartOfMatch()
{
	return this->is_part_of_match;
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
