#include "stdafx.h"
#include "Block.h"

struct Vector2fHash {
	std::size_t operator()(const sf::Vector2f& v) const {
		std::hash<float> hasher;
		return hasher(v.x) ^ (hasher(v.y) << 1);
	}
};

struct Vector2fEqual {
	bool operator()(const sf::Vector2f& a, const sf::Vector2f& b) const {
		return a.x == b.x && a.y == b.y;
	}
};

class Grid
{
private:
	std::mt19937 gen; // rng engine

	sf::Sprite backgroundSprite;
	sf::Texture backgroundTexture;

	sf::Sprite blockSprite;
	sf::Texture blockTexture;

	std::unique_ptr<Block> grid[6][12];

	Block* main_block;
	Block* child_block;

	void initBackgroundTexture();
	void initBackgroundSprite();

	void start_round();
	BLOCK_TYPE pick_random_block(bool other_block_is_default);
	Block* spawn_block(BLOCK_TYPE type, sf::Vector2f grid_spawn_position, bool isLocked);

	int get_random_int_range(int min, int max);

	const float time_until_pushdown = 0.5f;
	float current_time_until_pushdown = 0.5f;
	const float time_until_pushdown_increment = 0.01666666666f;

	void pushdown_block();
	void check_blocks();

	// input
	bool input_left;
	bool input_right;
	bool input_rotate;
	bool input_down;

	void query_inputs();
public:
	Grid();
	~Grid();
	void update();
	void render(sf::RenderTarget &target);
	void pollEvent(sf::Event event);

	static sf::Vector2f get_screen_position_from_grid_position(sf::Vector2f grid_position);
	static sf::Vector2f get_grid_position_from_screen_position(sf::Vector2f screen_position);
	static const int grid_bounds_x = 6;
	static const int grid_bounds_y = 12;
	static const int grid_cell_size = 16;
};
