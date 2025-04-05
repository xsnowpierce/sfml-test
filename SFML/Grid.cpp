#include "stdafx.h"
#include "Grid.h"

void Grid::initBackgroundTexture()
{
	if (!this->backgroundTexture.loadFromFile("images/grid_outline.png")) {
		std::cout << "ERROR: Couldn't load grid image." << std::endl;
	}
	
	if (!this->blockTexture.loadFromFile("images/sprite_sheet.png")) {
		std::cout << "ERROR: couldn't load block sheet." << std::endl;
	}
}

void Grid::initBackgroundSprite()
{
	this->backgroundSprite.setTexture(this->backgroundTexture);
	this->backgroundSprite.setPosition(64.f, 18.f);
}

void Grid::spawn_block(BLOCK_TYPE type, sf::Vector2f grid_spawn_position)
{
	Block block(type, get_screen_position_from_grid_position(sf::Vector2f(grid_spawn_position.x, grid_spawn_position.y)));
	blocks.push_back(block);
	this->block_data[(int)grid_spawn_position.x][(int)grid_spawn_position.y] = Block::get_type_as_int(type) + 1; // add 1 so that 0 is empty
	this->blockMap[grid_spawn_position] = &block;
}

void Grid::pushdown_block()
{
	for (Block &block : blocks) {
		sf::Vector2f destinedPosition(block.get_current_grid_position() + sf::Vector2f(0.f, 1.f));
		if (destinedPosition.y < Grid::grid_bounds_y && blockMap.find(destinedPosition) == blockMap.end()) {
			block.push_down();
		}
		else {
			// has block at position
			continue;
		}
	}
}

Grid::Grid()
{
	this->initBackgroundTexture();
	this->initBackgroundSprite();
	spawn_block(BLOCK_TYPE::PENGUIN, sf::Vector2f(2.f, 0.f));
	spawn_block(BLOCK_TYPE::SEAL, sf::Vector2f(3.f, 0.f));
}

Grid::~Grid()
{
}

void Grid::update()
{
	if (!this->blocks.empty()) {
		for (Block &block : blocks) {
			block.update();
		}
	}
	current_time_until_pushdown -= 0.01666666666f;
	if (current_time_until_pushdown <= 0.f) {
		current_time_until_pushdown = time_until_pushdown;
		this->pushdown_block();
	}
}

void Grid::render(sf::RenderTarget& target)
{
	target.draw(this->backgroundSprite);

	if (!this->blocks.empty()) {
		for (Block &block : blocks) {
			block.render(target);
		}
	}
}

sf::Vector2f Grid::get_screen_position_from_grid_position(sf::Vector2f grid_position)
{
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(sf::Vector2f(16.f, 6.f));
	int sprite_size = 16;

	return sf::Vector2f(grid_position.x * sprite_size + offset.x, grid_position.y * sprite_size + offset.y) + gridStartPosition;
}

sf::Vector2f Grid::get_grid_position_from_screen_position(sf::Vector2f screen_position)
{
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(16.f, 6.f);
	int sprite_size = 16;

	sf::Vector2f local_position = screen_position - gridStartPosition;

	float grid_x = (local_position.x - offset.x) / sprite_size;
	float grid_y = (local_position.y - offset.y) / sprite_size;

	return sf::Vector2f(grid_x, grid_y);
}
