#pragma once
#include<vector>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

#include"../status.h"
#include "grid_point.h"
#include "number_renderer.h"
#include "tile.h"

class Board
{
	typedef std::vector<std::vector<Tile>> TileBoard;
	struct BoardState
	{
		TileBoard board;
		Point start_pos_index;
		Point end_pos_index;
		Point info_tile_index;
	};

public:
	Board();
	~Board();

	void init(SDL_Renderer* renderer, TTF_Font* info_font);

	void on_render(SDL_Renderer* renderer);
	void on_update(double delta,InPutType input);
	void on_input(const SDL_Event& event);

	void reset();
	void clear_path_data();

	void toggle_show_weight();
	void toggle_show_cost();
	void set_edit_locked(bool locked);

	bool undo();

	void set_weight(int weight);

	[[nodiscard]] Point get_start_point() const;
	[[nodiscard]] Point get_end_point() const;

	[[nodiscard]] int row_count() const;
	[[nodiscard]] int col_count() const;
	[[nodiscard]] bool in_bounds(Point index) const;
	Tile& tile_at(Point index);
	const Tile& tile_at(Point index) const;
	[[nodiscard]] std::vector<Point> neighbors(Point index, MoveMode move_mode) const;
	[[nodiscard]] int path_cost() const;

	bool is_inside(int x, int y) const;
	void save_snapshot();

private:


	void draw_board(SDL_Renderer* renderer);
	void draw_mouse_pos_tile(SDL_Renderer* renderer, SDL_Point pos);
	bool draw_directed_tile(SDL_Renderer* renderer, Tile::Status status, const Tile& tile, int x, int y, const SDL_Rect& rect);
	void draw_tile_cost(Tile::Status status, const Tile& tile, const SDL_Rect& rect) const;
	SDL_Texture* get_directed_tile_texture(Tile::Status status, bool diagonal) const;
	void draw_tile_info_panel(SDL_Renderer* renderer);
	void render_info_label(SDL_Renderer* renderer, const char* text, SDL_Point pos);
	void render_info_number(int value, const SDL_Rect& rect) const;

	void on_mouse_click(const SDL_Event& event);
	void on_mouse_move(const SDL_Event& event);
	Point get_tile_index_at(int x, int y) const;
	bool is_valid_tile_index(Point index) const;

private:
	static SDL_Texture* tile_select;
	static SDL_Texture* tile_start;
	static SDL_Texture* tile_end;
	static SDL_Texture* tile_current;
	static SDL_Texture* tile_current_rot45;
	static SDL_Texture* tile_open;
	static SDL_Texture* tile_open_rot45;
	static SDL_Texture* tile_path;
	static SDL_Texture* tile_path_rot45;
	static SDL_Texture* tile_stop;
	static SDL_Texture* tile_stop_rot45;

private:
	static void destroy_static_textures();

private:
	bool _move_in_board = false;
	bool _click_in_board = false;

	bool _show_weight = false;
	bool _show_cost = false;

	bool _edit_locked = false;

	int _input_weight = 1;

	int _index_x = -1;
	int _index_y = -1;

	int _row = 20;
	int _col = 20;

	TileBoard _board;
	std::vector<BoardState> _board_snapshot;
	InPutType _current_input = InPutType::Empty;
	std::unique_ptr<NumberRenderer> _number_renderer;
	TTF_Font* _info_font = nullptr;

	Point _start_pos_index = { -1, -1 };
	Point _end_pos_index = { -1, -1 };

	SDL_Point _board_render_pos = { 0, 0 };
	SDL_Point _mouse_pos = { 0, 0 };
	SDL_Point _mouse_click_tile_center = { 0, 0 };
	Point _info_tile_index = { -1, -1 };
};
