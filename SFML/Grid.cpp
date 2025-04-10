#include "stdafx.h"
#include "Grid.h"

Grid::Grid() : gen(std::random_device{}()) {
	initBackgroundTexture();
	initBackgroundSprite();
}

Grid::~Grid() {}

/// <summary>
/// Initialises the texture used in the background of the scene.
/// </summary>
void Grid::initBackgroundTexture() {
	if (!backgroundTexture.loadFromFile("images/grid_outline.png")) {
		std::cout << "ERROR: Couldn't load grid image." << std::endl;
	}

	if (!blockTexture.loadFromFile("images/sprite_sheet.png")) {
		std::cout << "ERROR: couldn't load block sheet." << std::endl;
	}
}

/// <summary>
/// Uses the initialised texture called in initBackgroundTexture()
/// and applies it to a sprite.
/// </summary>
void Grid::initBackgroundSprite() {
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setPosition(64.f, 18.f);
}

/// <summary>
/// Generates a random number within a range.
/// </summary>
/// <param name="min">The lower bounds of the RNG.</param>
/// <param name="max">The upper bounds of the RNG.</param>
/// <returns>A random number between min and max.</returns>
int Grid::get_random_int_range(int min, int max) {
	std::uniform_int_distribution<> distr(min, max);
	return distr(gen);
}

/// <summary>
/// Picks a random block for use in creating new player blocks.
/// It makes it so that if the other block is DEFAULT, the other one can't be too.
/// </summary>
/// <param name="other_block_is_default">Whether the other block spawned is a DEFAULT block.</param>
/// <returns>A random block type that isn't a dupe if the other is default.</returns>
BLOCK_TYPE Grid::pick_random_block(bool other_block_is_default) {
	static const BLOCK_TYPE valid_types[4] = {
		BLOCK_TYPE::DEFAULT, BLOCK_TYPE::PENGUIN, BLOCK_TYPE::SEAL, BLOCK_TYPE::BIRD
	};
	return valid_types[get_random_int_range(other_block_is_default ? 1 : 0, 3)];
}

/// <summary>
/// Spawns a block.
/// </summary>
/// <param name="type">The type of block to spawn.</param>
/// <param name="grid_pos">The grid position to set the spawned block to.</param>
/// <param name="isLocked">Whether or not the block will have postitionLocked set.</param>
/// <returns>A pointer to the block created.</returns>
Block* Grid::spawn_block(BLOCK_TYPE type, sf::Vector2f grid_pos, bool isLocked) {
	int x = static_cast<int>(grid_pos.x);
	int y = static_cast<int>(grid_pos.y);

	if (grid[x][y]) return nullptr;

	auto block = std::make_unique<Block>(type, get_screen_position_from_grid_position(grid_pos));
	block->setPositionLocked(isLocked);

	Block* raw_ptr = block.get(); // Save raw pointer before moving
	grid[x][y] = std::move(block); // Store it in grid

	return raw_ptr; // Return the pointer to the caller
}

/// <summary>
/// Function to start the next round. Calls for new blocks to be spawned, checks if the game
/// is GAME_END, and sets the game state to either PLAYER_BLOCK_FALLING or GAME_END.
/// </summary>
void Grid::start_round() {
	//spawn_block(BLOCK_TYPE::DEFAULT, { 2.f, 11.f }, true);

	BLOCK_TYPE type2 = pick_random_block(false);
	BLOCK_TYPE type1 = pick_random_block(type2 == BLOCK_TYPE::DEFAULT);

	this->main_block = spawn_block(type1, { 2.f, 0.f }, false);
	this->child_block = spawn_block(type2, { 3.f, 0.f }, false);

	this->current_game_state = Grid::GAME_STATE::PLAYER_BLOCK_FALLING;

	if (main_block == nullptr or child_block == nullptr) {
		std::cout << "Game lost." << std::endl;
		this->current_game_state = Grid::GAME_STATE::GAME_END;
		return;
	}
}

/// <summary>
/// Makes the player blocks(child and main) move downwards.
/// The blocks are checked before and after moving to ensure no clipping happens.
/// </summary>
void Grid::pushdown_block() {
	/* check blocks before moving to make sure that if there is a block below another, 
		the block below will be able to be locked before the block above it moves down.
	*/ 
	this->check_blocks();
	if (main_block != nullptr && main_block->getPositionLocked() == false) {
		main_block->push_down();
	}
	if (child_block != nullptr && child_block->getPositionLocked() == false) {
		child_block->push_down();
	}

	// check blocks again so that after each pushdown_block attempt, all the blocks that can be locked are locked.
	this->check_blocks();
}

/// <summary>
/// Checks the current status of the main and child block as to whether they should be locked in position.
/// </summary>
void Grid::check_blocks()
{
	if (main_block != nullptr) {
		if (grid[(int)main_block->get_current_grid_position().x][(int)main_block->get_current_grid_position().y + 1]
			|| (int) main_block->get_current_grid_position().y == grid_bounds_y - 1) {
			// grid has spot below block
			main_block->setPositionLocked(true);
			grid[(int)main_block->get_current_grid_position().x][(int)main_block->get_current_grid_position().y] = std::move(grid[2][0]);
		}

		if (main_block->getPositionLocked()) {
			main_block = nullptr;
		}
	}
	
	if (child_block != nullptr) {
		if (grid[(int)child_block->get_current_grid_position().x][(int)child_block->get_current_grid_position().y + 1]
			|| (int) child_block->get_current_grid_position().y == grid_bounds_y - 1) {
			// grid has spot below block
			child_block->setPositionLocked(true);
			grid[(int)child_block->get_current_grid_position().x][(int)child_block->get_current_grid_position().y] = std::move(grid[3][0]);

		}

		if (child_block->getPositionLocked()) {
			child_block = nullptr;
		}
	}
}

/// <summary>
/// Pushes all the blocks upwards one grid cell.
/// </summary>
void Grid::pushup_blocks()
{
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 12; ++j) {
			if (grid[i][j] != nullptr) {
				
				if (j - 1 < 0) {
					// delete block instead
					grid[i][j].reset();
					continue;
				}

				grid[i][j]->move_block(sf::Vector2f(0.f, -1.f));
				grid[i][j - 1] = std::move(grid[i][j]);
			}
		}
	}

	for (int i = 0; i < 6; ++i) {
		grid[i][11] = std::move(new_grid_line[i]);
	}
}

/// <summary>
/// Spawns a new row of blocks at the bottom row.
/// This function will never create a match, as it checks for the blocks 
/// beside and above it before doing so, to ensure a unique block.
/// </summary>
void Grid::spawn_bottom_row()
{
	// clear the new_grid_line array
	for (int i = 0; i < 6; ++i) {
		new_grid_line[i].reset();
	}

	BLOCK_TYPE types[6] = {};

	for (int i = 0; i < 6; ++i) {
		std::unordered_set<BLOCK_TYPE> invalid_block_types;

		// check left block, right block, and above for possible block types. if any block is 
		// DEFAULT or BOMB, keep going until we reach a possible face block type.

		// check left
		if (i != 0) { // don't need to check if its 0, cause its the leftmost
			for (int j = 1; j < 5; ++j) {

				if (i - j < 0)
					break; // reached end of possibilities

				if (types[i - j] != BLOCK_TYPE::DEFAULT && types[i - j] != BLOCK_TYPE::BOMB) {
					// remove type from possible block types
					invalid_block_types.insert(types[i - j]);
					break; // removed one already, stop here.
				}
			}
		}

		// check right
		if (i != 5) { // don't need to check if its 6, cause its rightmost
			for (int j = 1; j < 5; ++j) {

				if (i + j > 5)
					break; // reached end of possibilities

				if (types[i + j] != BLOCK_TYPE::DEFAULT && types[i + j] != BLOCK_TYPE::BOMB) {
					// remove type from possible block types
					invalid_block_types.insert(types[i + j]);
					break; // removed one already, stop here.
				}
			}
		}

		// check above
		for (int j = 11; j > 0; --j) {

			if (grid[i][j] != nullptr) {
				if (grid[i][j]->get_block_type() != BLOCK_TYPE::DEFAULT && grid[i][j]->get_block_type() != BLOCK_TYPE::BOMB) {
					invalid_block_types.insert(grid[i][j]->get_block_type());
					break;
				}
			}
		}

		std::vector<BLOCK_TYPE> valid_block_types = {
			BLOCK_TYPE::DEFAULT,
			BLOCK_TYPE::BOMB,
			BLOCK_TYPE::PENGUIN,
			BLOCK_TYPE::BIRD,
			BLOCK_TYPE::SEAL
		};

		std::vector<BLOCK_TYPE> blocks_to_choose_from;
		for (BLOCK_TYPE type : valid_block_types) {
			if (invalid_block_types.find(type) == invalid_block_types.end()) {
				blocks_to_choose_from.push_back(type);
			}
		}

		// we have decided our blocks to be able to pick from. now let's randomize from the results.
		types[i] = blocks_to_choose_from[get_random_int_range(0, blocks_to_choose_from.size() - 1)];
	}

	for (int i = 0; i < 6; ++i) {
		auto block = std::make_unique<Block>(types[i], Grid::get_screen_position_from_grid_position(sf::Vector2f(i, 11.f)));
		block->setPositionLocked(true);
		new_grid_line[i] = std::move(block);
	}
}

/// <summary>
/// Called every frame.
/// </summary>
void Grid::update() {

	query_inputs();

	for (int x = 0; x < grid_bounds_x; ++x) {
		for (int y = 0; y < grid_bounds_y; ++y) {
			if (grid[x][y]) grid[x][y]->update();
		}
	}

	//this->print_debug_array();
	//std::cout << current_game_state << std::endl;

	std::vector<Block*> matched_blocks;

	if (current_game_state == Grid::GAME_STATE::NEW_ROUND) {

		// reset variables
		current_time_until_pushdown = time_until_pushdown;
		current_blocks_locked_waiting_time = blocks_locked_waiting_time;
		this->start_round();
	}

	else if (current_game_state == Grid::GAME_STATE::PLAYER_BLOCK_FALLING) {

		if (main_block == nullptr && child_block == nullptr) {
			current_game_state = Grid::GAME_STATE::MATCH_CHECK;
			return;
		}

		float increment = time_until_pushdown_increment;
		if (input_down || main_block == nullptr || child_block == nullptr) increment *= 3;

		current_time_until_pushdown -= increment;
		if (current_time_until_pushdown <= 0.f) {
			current_time_until_pushdown = time_until_pushdown;
			pushdown_block();
		}
	}
	

	else if (current_game_state == Grid::GAME_STATE::MATCH_CHECK) {
		std::cout << "Started match detection." << std::endl;

		// reset the timer for deleting blocks here
		current_block_deletion_wait_time = block_deletion_wait_time;

		matched_blocks = this->find_matches();

		std::cout << "Blocks in match: " << std::to_string(matched_blocks.size()) << std::endl;

		if (matched_blocks.size() > 0) {
			// we have matches, deal with them

			// set the blocks to be flashing
			for (Block* block : matched_blocks) {
				block->is_scheduled_to_delete = true;
				block->setPartOfMatch(true);
			}

			// delete the blocks
			this->current_game_state = Grid::GAME_STATE::MATCH_BLOCK_DELETION;
		}
		else
			// finished checking for matches, move on
			this->current_game_state = Grid::GAME_STATE::SPAWNING_NEW_BOTTOM_ROW;
	}

	else if (current_game_state == Grid::GAME_STATE::MATCH_BLOCK_DELETION) {
		std::cout << "Started block deletion." << std::endl;

		// Wait a little bit for visual clarity for the player
		if (current_block_deletion_wait_time > 0.f) {
			current_block_deletion_wait_time -= single_frame_value;
			return;
		}

		// waiting time is up, delete blocks
		for (int y = 0; y < 12; ++y) {
			for (int x = 0; x < 6; ++x) {
				// Check if the block exists and if its scheduled for deletion
				if (grid[x][y] != nullptr && grid[x][y]->is_scheduled_to_delete) {
					std::cout << "Deleting block at: " << x << ", " << y << std::endl;

					// If block type is DEFAULT or BOMB, add to the score
					if (grid[x][y]->get_block_type() == BLOCK_TYPE::DEFAULT || grid[x][y]->get_block_type() == BLOCK_TYPE::BOMB) {
						current_player_score++;
					}

					// delete block
					grid[x][y].reset();
				}
			}
		}

		this->current_game_state = Grid::GAME_STATE::MATCH_BLOCK_FALL;
	}


	else if (current_game_state == Grid::GAME_STATE::MATCH_BLOCK_FALL) {
		

		if (current_block_fall_wait_time > 0.f) {
			current_block_fall_wait_time -= single_frame_value;
			return;
		}

		std::cout << "Started making blocks fall." << std::endl;

		int blocks_to_fall = 0;
		for (int y = 11; y > 0; y--) {

			for (int x = 5; x > 0; x--) {

				if (grid[x][y] == nullptr) {

					for (int i = y; i > 0; i--) {
						if (grid[x][i] != nullptr){
							grid[x][i]->is_scheduled_to_fall = true;
							blocks_to_fall++;
						}
					}
				}
			}
		}

		current_block_fall_wait_time = block_fall_wait_time;

		if (blocks_to_fall > 0) {
			for (int x = 5; x > 0; x--) {
				for (int y = 10; y > 0; y--) { // start at 10 cause blocks at y=11 can't fall
					if (grid[x][y] != nullptr && grid[x][y]->is_scheduled_to_fall) {
						if (grid[x][y + 1] == nullptr) {

							grid[x][y]->move_block(sf::Vector2f(0, 1));
							grid[x][y]->is_scheduled_to_fall = false;
							grid[x][y + 1] = std::move(grid[x][y]);

						}
					}
				}
			}
		}
		else {
			this->current_game_state = Grid::GAME_STATE::MATCH_ATTEMPT_STARFALL;
		}
	}

	else if (current_game_state == Grid::GAME_STATE::MATCH_ATTEMPT_STARFALL) {
		std::cout << "Attemping operation starfall." << std::endl;
		this->current_game_state = Grid::GAME_STATE::MATCH_CHECK;
	}

	else if (current_game_state == Grid::GAME_STATE::SPAWNING_NEW_BOTTOM_ROW) {
		if (current_blocks_locked_waiting_time > 0.f) {
			current_blocks_locked_waiting_time -= single_frame_value;
			//std::cout << current_blocks_locked_waiting_time << std::endl;
			return;
		}

		if (current_rounds_to_spawn_new_row == 0) {
			std::cout << "Spawning new row." << std::endl;
			this->spawn_bottom_row();
			this->current_rounds_to_spawn_new_row = rounds_required_to_spawn_new_row;
			this->current_game_state = Grid::GAME_STATE::PUSHING_UP_BLOCKS;
		}
		else {
			std::cout << "Rounds required to spawn new row: " << std::to_string(current_rounds_to_spawn_new_row) << std::endl;
			current_rounds_to_spawn_new_row--;
			this->current_game_state = Grid::GAME_STATE::NEW_ROUND;
		}
	}

	else if (current_game_state == Grid::GAME_STATE::PUSHING_UP_BLOCKS) {
		std::cout << "Pushing up blocks." << std::endl;
		this->pushup_blocks();
		this->current_game_state = Grid::GAME_STATE::NEW_ROUND;
	}
}

/// <summary>
/// Render function.
/// </summary>
/// <param name="target">The target to display to.</param>
void Grid::render(sf::RenderTarget& target) {
	target.draw(backgroundSprite);
	for (int x = 0; x < grid_bounds_x; ++x) {
		for (int y = 0; y < grid_bounds_y; ++y) {
			if (grid[x][y]) grid[x][y]->render(target);
		}
	}
}

void Grid::pollEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
			input_left = true;
			break;
		case sf::Keyboard::D:
		case sf::Keyboard::Right:
			input_right = true;
			break;
		case sf::Keyboard::E:
		case sf::Keyboard::Space:
			input_rotate_right = true;
			break;
		case sf::Keyboard::Q:
		case sf::Keyboard::LControl:
			input_rotate_left = true;
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// Input handling method.
/// </summary>
void Grid::query_inputs() {
	if (main_block == nullptr || child_block == nullptr) {
		return;
	}

	input_down = sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

	if (input_left) {
		if ((int)main_block->get_current_grid_position().x - 1 < 0
			|| (int)child_block->get_current_grid_position().x - 1 < 0
			|| grid[(int)main_block->get_current_grid_position().x - 1][(int)main_block->get_current_grid_position().y] != nullptr
			|| grid[(int)child_block->get_current_grid_position().x - 1][(int)child_block->get_current_grid_position().y] != nullptr) {
			// can't move left
			return;
		}

		main_block->move_block(-1, 0);
		child_block->move_block(-1, 0);
	}

	if (input_right) {
		if ((int) main_block->get_current_grid_position().x + 1 >= Grid::grid_bounds_x 
			|| (int) child_block->get_current_grid_position().x + 1 >= Grid::grid_bounds_x
			|| grid[(int)main_block->get_current_grid_position().x + 1][(int)main_block->get_current_grid_position().y] != nullptr
			|| grid[(int)child_block->get_current_grid_position().x + 1][(int)child_block->get_current_grid_position().y] != nullptr) {
			// can't move left
			return;
		}

		main_block->move_block(1, 0);
		child_block->move_block(1, 0);
	}

	if (input_rotate_right) {
		// check for valid rotate direction
		this->rotate_block(1);
	}

	else if (input_rotate_left) {
		this->rotate_block(-1);
	}

	input_left = false;
	input_right = false;
	input_rotate_right = false;
	input_rotate_left = false;
}

/// <summary>
/// Calculates the next position the child should be in given the direction.
/// </summary>
/// <param name="direction">The direction to rotate in: -1 or 1, for CCW or CW rotations.</param>
/// <param name="current_offset">The current grid location relative to the main block.</param>
/// <returns>The grid position the child block should move to next.</returns>
sf::Vector2f Grid::get_next_child_rotate_position(int direction, sf::Vector2f current_offset)
{
	if (current_offset == sf::Vector2f(1.f, 0.f)) {
		// right of main block
		if (direction == 1) {
			return sf::Vector2f(0.f, 1.f); // go down
		}
		else if (direction == -1) {
			return sf::Vector2f(0.f, -1.f); // go up
		}
	}
	else if (current_offset == sf::Vector2f(0.f, 1.f)) {
		// below main block
		if (direction == 1) {
			return sf::Vector2f(-1.f, 0.f); // go left
		}
		else if (direction == -1) {
			return sf::Vector2f(1.f, 0.f); // go right
		}
	}
	else if (current_offset == sf::Vector2f(-1.f, 0.f)) {
		// left of main block
		if (direction == 1) {
			return sf::Vector2f(0.f, -1.f); // go up
		}
		else if (direction == -1) {
			return sf::Vector2f(0.f, 1.f); // go down
		}
	}
	else if (current_offset == sf::Vector2f(0.f, -1.f)) {
		// above main block
		if (direction == 1) {
			return sf::Vector2f(1.f, 0.f); // go right
		}
		else if (direction == -1) {
			return sf::Vector2f(-1.f, 0.f); // go left
		}
	}

	std::cout << "Error rotating, given ("
		<< current_offset.x << ", " << current_offset.y << ")" << std::endl;

	return sf::Vector2f(1.f, 0.f); // default to right
}

/// <summary>
/// Rotates the child block to a possible direction.
/// </summary>
/// <param name="direction">The direction, -1 or 1, to rotate in for CCW or CW rotations.</param>
void Grid::rotate_block(int direction)
{
	sf::Vector2f current_child_offset(this->child_block->get_current_grid_position() - this->main_block->get_current_grid_position());
	current_child_offset.y = current_child_offset.y;
	
	bool found_location = false;
	while (!found_location) {
		current_child_offset = this->get_next_child_rotate_position(direction, current_child_offset);
		
		sf::Vector2i target_position(this->main_block->get_current_grid_position() + sf::Vector2f(current_child_offset.x, current_child_offset.y));

		if (target_position.x < 0 || target_position.x == Grid::grid_bounds_x || grid[target_position.x][target_position.y] != nullptr) {
			// location has block or target is out of bounds, continue
		}
		else {
			found_location = true;
		}
	}

	// can assume that current_child_offset has been changed to reflect the next possible position
	sf::Vector2f reset_position(Grid::get_screen_position_from_grid_position(this->main_block->get_current_grid_position()));
	this->child_block->set_position(reset_position);
	this->child_block->move_block(current_child_offset);
}

/// <summary>
/// Prints the current status of the grid[][] array. 
/// Prints numbers of the block types, or 0 if the cell is empty.
/// </summary>
void Grid::print_debug_array()
{
	std::string debug;
	debug.append("-------\n");
	for (int i = 0; i < 12; ++i) {
		for (int j = 0; j < 6; ++j) {
			if (grid[j][i] != nullptr) {
				debug.append(std::to_string(Block::get_type_as_int(grid[j][i]->get_block_type())) + " ");
			}
			else {
				debug.append(std::to_string(0) + " ");
			}
		}
		debug.append("\n");
	}
	debug.append("-------\n");
	std::cout << debug;
}

std::vector<Block*> Grid::find_matches()
{
	std::vector<Block*> matched_blocks;

	std::pair<int, int> directions[2] = {
		{1, 0},  // RIGHT
		{0, 1}   // DOWN
	};

	for (int y = 0; y < 12; ++y) {
		for (int x = 0; x < 6; ++x) {
			Block* block = grid[x][y].get();
			if (!block) continue;

			BLOCK_TYPE type = block->get_block_type();
			if (type == BLOCK_TYPE::DEFAULT || type == BLOCK_TYPE::BOMB)
				continue;

			for (std::pair<int, int> direction : directions) {
				int directional_x = direction.first;  // x-direction change (1 for right, 0 for no change)
				int directional_y = direction.second; // y-direction change (1 for down, 0 for no change)

				int current_x = x + directional_x;  // Start from next block in the direction
				int current_y = y + directional_y;  // Same as above, move in y-direction

				std::vector<Block*> in_between;  // Store blocks in-between start and match

				while (current_x >= 0 && current_y >= 0 && current_x < 6 && current_y < 12) {
					Block* current_block = grid[current_x][current_y].get();
					if (!current_block) break;

					BLOCK_TYPE current_type = current_block->get_block_type();
					if (current_type == BLOCK_TYPE::DEFAULT || current_type == BLOCK_TYPE::BOMB) {
						in_between.push_back(current_block);
						current_x += directional_x;  // Move further in the same direction
						current_y += directional_y;  // Same as above for y-direction
						continue;
					}
					else if (current_type == type) {
						// Found a match, add start, in-between, and end blocks
						matched_blocks.push_back(block);  // Starting block
						matched_blocks.insert(matched_blocks.end(), in_between.begin(), in_between.end());
						matched_blocks.push_back(current_block);  // End matching block
						break;
					}
					else {
						break;  // If types don't match, stop searching further
					}
				}
			}
		}
	}
	return matched_blocks;
}

/// <summary>
/// Calculates the screen coordinate position from a given grid position.
/// </summary>
/// <param name="grid_position">The grid position to convert. (eg. 1,4)</param>
/// <returns>The converted screen position (eg. 124, -331)</returns>
sf::Vector2f Grid::get_screen_position_from_grid_position(sf::Vector2f grid_position) {
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(16.f, 6.f);
	int sprite_size = 16;

	return sf::Vector2f(grid_position.x * sprite_size + offset.x, grid_position.y * sprite_size + offset.y) + gridStartPosition;
}

/// <summary>
/// Calculates the grid position from screen coordinates.
/// </summary>
/// <param name="screen_position">The position on the screen to convert. (eg. 124, -331)</param>
/// <returns>The converted grid position. (eg.1, 4).</returns>
sf::Vector2f Grid::get_grid_position_from_screen_position(sf::Vector2f screen_position) {
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(16.f, 6.f);
	int sprite_size = 16;

	sf::Vector2f local_position = screen_position - gridStartPosition;
	float grid_x = (local_position.x - offset.x) / sprite_size;
	float grid_y = (local_position.y - offset.y) / sprite_size;

	return sf::Vector2f(grid_x, grid_y);
}
