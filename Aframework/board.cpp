#include "board.h"
#include <cmath>
#include <SDL_image.h>//图像库

SDL_Texture* Board::tile_select = nullptr;

Board::Board()
{
    _board_render_pos.x = 250;
    _board_render_pos.y = 20;
}

Board::~Board()
{


}

void Board::init(SDL_Renderer* renderer)
{

    auto load_texture = [&](const char* path) -> SDL_Texture*
        {
            SDL_Surface* surface = IMG_Load(path);
            if (!surface)
            {
                SDL_Log("IMG_Load failed: %s", IMG_GetError());
                return nullptr;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            if (!texture)
            {
                SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
            }

            return texture;
        };


    tile_select = load_texture("assets/texture/tile_select.png");

}

void Board::on_render(SDL_Renderer* renderer)
{
	draw_board(renderer);
    draw_mouse_pos_tile(renderer, _mouse_pos);
}

void Board::on_update(double delta)
{

}

void Board::on_input(const SDL_Event& event)
{
    on_mouse_move(event);
    on_mouse_click(event);
}


void Board::set_size(int row, int col)
{

	}
void Board::set_board_pos(SDL_Point point)
{

}

bool Board::is_inside(int x, int y) const
{
    return x >= _board_render_pos.x
        && x < _board_render_pos.x + _col * SIZE_TILE
        && y >= _board_render_pos.y
        && y < _board_render_pos.y + _row * SIZE_TILE;

}


void Board::draw_board(SDL_Renderer* renderer)
{
    // 棋盘底色
    SDL_Rect board_rect =
    {
        _board_render_pos.x, _board_render_pos.y,
        _col * SIZE_TILE, _row * SIZE_TILE
    };

    SDL_SetRenderDrawColor(renderer, 205, 205, 205, 255);
    SDL_RenderFillRect(renderer, &board_rect);

    // 网格线
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    // 竖线
    for (int i = 0; i <= _col; ++i)
    {
        int x = _board_render_pos.x + i * SIZE_TILE;
        SDL_RenderDrawLine(
            renderer, x, _board_render_pos.y, x,
            _board_render_pos.y + _row * SIZE_TILE
        );
    }

    // 横线
    for (int j = 0; j <= _row; ++j)
    {
        int y = _board_render_pos.y + j * SIZE_TILE;
        SDL_RenderDrawLine(
            renderer, _board_render_pos.x, y,
            _board_render_pos.x + _col * SIZE_TILE, y
        );
    }
}

void Board::on_mouse_click(const SDL_Event& event)
{

}

void Board::on_mouse_move(const SDL_Event& event)
{
    if (is_inside(event.motion.x, event.motion.y))
    {
        _move_in_board = true;
        _mouse_pos.x = event.motion.x;
        _mouse_pos.y = event.motion.y;
        SDL_ShowCursor(SDL_DISABLE);
    }
    else
    {
        _move_in_board = false;
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void Board::draw_mouse_pos_tile(SDL_Renderer* renderer, SDL_Point pos)
{
    if (!_move_in_board)
        return;

    SDL_Point grid_pos = { 0 };

    if (pos.x - _board_render_pos.x < -(SIZE_TILE / 2) || pos.y - _board_render_pos.y < -(SIZE_TILE / 2))
    {
        int g_pos_x = std::floor(double(pos.x - _board_render_pos.x) / SIZE_TILE);
        int g_pos_y = std::floor(double(pos.y - _board_render_pos.y) / SIZE_TILE);

        grid_pos = { g_pos_x,g_pos_y };
    }
    else
    {
        grid_pos =
        {
            (pos.x - _board_render_pos.x) / SIZE_TILE,
            (pos.y - _board_render_pos.y) / SIZE_TILE
        };
    }
    SDL_Rect rect;

    SDL_RenderCopy(renderer, tile_select, nullptr, &rect);
    SDL_RenderFillRect(renderer, &rect);
}
