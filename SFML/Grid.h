#include "stdafx.h"
#include "Block.h"

// do i even need these anymore?...
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
	/// <summary>
	/// The current state of the game.
	/// </summary>
	enum GAME_STATE {
		NEW_ROUND,
		PLAYER_BLOCK_FALLING,
		MATCH_CHECK,
		MATCH_BLOCK_DELETION,
		MATCH_BLOCK_FALL,
		MATCH_ATTEMPT_STARFALL,
		SPAWNING_NEW_BOTTOM_ROW,
		PUSHING_UP_BLOCKS,
		GAME_END
	};

	GAME_STATE current_game_state = Grid::GAME_STATE::NEW_ROUND;

	std::mt19937 gen; // rng engine

	sf::Sprite backgroundSprite;
	sf::Texture backgroundTexture;

	sf::Sprite blockSprite;
	sf::Texture blockTexture;

	std::unique_ptr<Block> grid[6][12];
	std::unique_ptr<Block> new_grid_line[6];

	Block* main_block;
	Block* child_block;

	int current_player_score;

	void initBackgroundTexture();
	void initBackgroundSprite();

	void start_round();
	BLOCK_TYPE pick_random_block(bool other_block_is_default);
	Block* spawn_block(BLOCK_TYPE type, sf::Vector2f grid_spawn_position, bool isLocked);

	int get_random_int_range(int min, int max);

	const float time_until_pushdown = 0.5f;
	float current_time_until_pushdown = 0.5f;
	const float time_until_pushdown_increment = 0.01666666666f;

	int current_rotation_index = 0;

	float blocks_locked_waiting_time = 0.2f;
	float current_blocks_locked_waiting_time;

	float block_deletion_wait_time = 0.3f;
	float current_block_deletion_wait_time;

	float block_fall_wait_time = 0.1f;
	float current_block_fall_wait_time;

	int rounds_required_to_spawn_new_row = 4;
	int current_rounds_to_spawn_new_row = 4;

	std::vector<sf::Vector2f> rotation_offsets = {
		sf::Vector2f(-1, 0), // 0° (left of main block)
		sf::Vector2f(0, 1),  // 90° (below)
		sf::Vector2f(1, 0),  // 180° (right)
		sf::Vector2f(0, -1), // 270° (above)
	};

	void pushdown_block();
	void check_blocks();
	void pushup_blocks();
	void spawn_bottom_row();

	// input
	bool input_left;
	bool input_right;
	bool input_rotate_right;
	bool input_rotate_left;
	bool input_down;

	void query_inputs();
	sf::Vector2f get_next_child_rotate_position(int direction, sf::Vector2f current_offset);
	void rotate_block(int direction);

	void print_debug_array();

	std::vector<Block*> find_matches();
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
	const float single_frame_value = 0.01666666666f;
};
