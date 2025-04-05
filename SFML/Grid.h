#include "stdafx.h"
#include "Block.h"

struct Vector2fHash {
	std::size_t operator()(const sf::Vector2f& v) const {
		std::hash<float> hasher;
		return hasher(v.x) ^ (hasher(v.y) << 1);  // Simple hash combining
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
	sf::Sprite backgroundSprite;
	sf::Texture backgroundTexture;

	sf::Sprite blockSprite;
	sf::Texture blockTexture;

	std::list<Block> blocks;
	std::unordered_map<sf::Vector2f, Block*, Vector2fHash, Vector2fEqual> blockMap;
	int block_data[6][12];

	void initBackgroundTexture();
	void initBackgroundSprite();

	void spawn_block(BLOCK_TYPE type, sf::Vector2f grid_spawn_position);

	float time_until_pushdown = 0.5f;
	float current_time_until_pushdown = 0.5f;

	void pushdown_block();
public:
	Grid();
	~Grid();
	void update();
	void render(sf::RenderTarget &target);
	static sf::Vector2f get_screen_position_from_grid_position(sf::Vector2f grid_position);
	static sf::Vector2f get_grid_position_from_screen_position(sf::Vector2f screen_position);
	static const int grid_bounds_x = 6;
	static const int grid_bounds_y = 12;
};
