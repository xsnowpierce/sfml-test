#include "stdafx.h"
#include "Grid.h"

Grid::Grid() : gen(std::random_device{}()) {
	initBackgroundTexture();
	initBackgroundSprite();
	start_round();
}

Grid::~Grid() {}

void Grid::initBackgroundTexture() {
	if (!backgroundTexture.loadFromFile("images/grid_outline.png")) {
		std::cout << "ERROR: Couldn't load grid image." << std::endl;
	}

	if (!blockTexture.loadFromFile("images/sprite_sheet.png")) {
		std::cout << "ERROR: couldn't load block sheet." << std::endl;
	}
}

void Grid::initBackgroundSprite() {
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setPosition(64.f, 18.f);
}

int Grid::get_random_int_range(int min, int max) {
	std::uniform_int_distribution<> distr(min, max);
	return distr(gen);
}

BLOCK_TYPE Grid::pick_random_block(bool other_block_is_default) {
	static const BLOCK_TYPE valid_types[4] = {
		BLOCK_TYPE::DEFAULT, BLOCK_TYPE::PENGUIN, BLOCK_TYPE::SEAL, BLOCK_TYPE::BIRD
	};
	return valid_types[get_random_int_range(other_block_is_default ? 1 : 0, 3)];
}

bool Grid::spawn_block(BLOCK_TYPE type, sf::Vector2f grid_pos, bool isLocked) {
	int x = static_cast<int>(grid_pos.x);
	int y = static_cast<int>(grid_pos.y);

	if (grid[x][y]) return false;

	auto block = std::make_unique<Block>(type, get_screen_position_from_grid_position(grid_pos));
	block->setPositionLocked(isLocked);
	grid[x][y] = std::move(block);

	return true;
}

void Grid::start_round() {
	BLOCK_TYPE type2 = pick_random_block(false);
	BLOCK_TYPE type1 = pick_random_block(type2 == BLOCK_TYPE::DEFAULT);

	if (!spawn_block(type1, { 2.f, 0.f }, false) || !spawn_block(type2, { 3.f, 0.f }, false)) {
		std::cout << "game lost." << std::endl;
		return;
	}
}

void Grid::pushdown_block() {
	bool newRoundStarted = false;

	for (int x = 0; x < grid_bounds_x; ++x) {
		for (int y = grid_bounds_y - 1; y >= 0; --y) {
			auto& block = grid[x][y];
			if (block && !block->getPositionLocked()) {
				int newY = y + 1;
				if (newY < grid_bounds_y && !grid[x][newY]) {
					grid[x][newY] = std::move(block);
					grid[x][y] = nullptr;
					grid[x][newY]->push_down();
				}
				else {
					block->setPositionLocked(true);
					newRoundStarted = true;
				}
			}
		}
	}

	if (newRoundStarted) {
		start_round();
	}
}

void Grid::update() {
	input_down = sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

	query_inputs();

	input_left = false;
	input_right = false;
	input_rotate = false;

	for (int x = 0; x < grid_bounds_x; ++x) {
		for (int y = 0; y < grid_bounds_y; ++y) {
			if (grid[x][y]) grid[x][y]->update();
		}
	}

	float increment = time_until_pushdown_increment;
	if (input_down) increment *= 3;

	current_time_until_pushdown -= increment;
	if (current_time_until_pushdown <= 0.f) {
		current_time_until_pushdown = time_until_pushdown;
		pushdown_block();
	}
}

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
			input_rotate = true;
			break;
		default:
			break;
		}
	}
}

void Grid::query_inputs() {
	// Placeholder for future input handling (move, rotate, etc.)
}

sf::Vector2f Grid::get_screen_position_from_grid_position(sf::Vector2f grid_position) {
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(16.f, 6.f);
	int sprite_size = 16;

	return sf::Vector2f(grid_position.x * sprite_size + offset.x, grid_position.y * sprite_size + offset.y) + gridStartPosition;
}

sf::Vector2f Grid::get_grid_position_from_screen_position(sf::Vector2f screen_position) {
	sf::Vector2f gridStartPosition(64.f, 18.f);
	sf::Vector2f offset(16.f, 6.f);
	int sprite_size = 16;

	sf::Vector2f local_position = screen_position - gridStartPosition;
	float grid_x = (local_position.x - offset.x) / sprite_size;
	float grid_y = (local_position.y - offset.y) / sprite_size;

	return sf::Vector2f(grid_x, grid_y);
}
