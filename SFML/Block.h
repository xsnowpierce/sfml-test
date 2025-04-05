#pragma once
#include "BLOCK_TYPE.h"
class Block
{
private:
	sf::Sprite sprite;
	
	BLOCK_TYPE block_type;
	
	sf::IntRect get_texture_bounds_from_enum(BLOCK_TYPE type);
	void initSprite();
	sf::Vector2f grid_position;
	bool is_position_locked;
public:
	void push_down();
	void move_block(int x, int y);
	void set_position(float x, float y);
	void update();
	void render(sf::RenderTarget &target);
	Block(Block::BLOCK_TYPE type, sf::Vector2f position);
	~Block();
	sf::Vector2f get_current_grid_position();
	bool getPositionLocked();
	void setPositionLocked(bool lockPosition);
	BLOCK_TYPE get_block_type();

	static sf::Texture texture;
	static void initTexture();
	static int get_type_as_int(BLOCK_TYPE type);
	static BLOCK_TYPE get_int_as_type(int value);
};

