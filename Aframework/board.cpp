#include "board.h"
#include <cmath>
#include <SDL_image.h>//图像库

SDL_Texture* Board::tile_select = nullptr;
SDL_Texture* Board::tile_start = nullptr;
SDL_Texture* Board::tile_end = nullptr;

Board::Board()
{
    _board.assign(_row, std::vector<Tile>(_col));
    _board_render_pos.x = 200;
    _board_render_pos.y = 20;
}

Board::~Board()
{


}

void Board::reset()
{
    _board.assign(_row, std::vector<Tile>(_col));

    _move_in_board = false;
    _click_in_board = false;
    _on_process = false;

    _index_x = -1;
    _index_y = -1;

    _start_pos_index = { -1, -1 };
    _end_pos_index = { -1, -1 };

    _mouse_click_tile_center = { 0, 0 };
}

void Board::clear_path_data()
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
    tile_start = load_texture("assets/texture/tile_start_pos.png");
    tile_end = load_texture("assets/texture/tile_end_pos.png");

}

void Board::on_render(SDL_Renderer* renderer)
{
    draw_board(renderer);
    draw_mouse_pos_tile(renderer, _mouse_pos);

    SDL_Rect detail = { 20,20,150,150 };
    SDL_RenderFillRect(renderer, &detail);

    for (int y = 0; y < _row; ++y)
    {
        for (int x = 0; x < _col; ++x)
        {
            Tile::Status status = _board[y][x].get_status();

            if (status == Tile::Status::Empty)
                continue;

            SDL_Rect rect =
            {
                _board_render_pos.x + x * SIZE_TILE + 2,
                _board_render_pos.y + y * SIZE_TILE + 2,
                SIZE_TILE - 4,
                SIZE_TILE - 4
            };

            switch (status)
            {
            case Tile::Status::Wall:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);       // black
                break;

            case Tile::Status::Start:
                SDL_RenderCopy(renderer, tile_start, nullptr, &rect);    // red
                continue;

            case Tile::Status::Goal:
                SDL_RenderCopy(renderer, tile_end, nullptr, &rect);   // green
                continue;

            case Tile::Status::Open:
                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);   // cyan
                break;

            case Tile::Status::Closed:
                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);   // orange
                break;

            case Tile::Status::Path:
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);     // blue
                break;

            default:
                continue;
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Board::on_update(double delta, InPutType input)
{
    _current_input = input;
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
    if (_on_process)
        return;

    int mouse_x = 0;
    int mouse_y = 0;
    bool should_paint = false;

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            mouse_x = event.button.x;
            mouse_y = event.button.y;
            should_paint = true;
        }
        break;

    case SDL_MOUSEMOTION:
        if (event.motion.state & SDL_BUTTON_LMASK)
        {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            should_paint = true;
        }
        break;

    default:
        return;
    }

    if (!should_paint || !is_inside(mouse_x, mouse_y))
        return;

    int x = (mouse_x - _board_render_pos.x) / SIZE_TILE;
    int y = (mouse_y - _board_render_pos.y) / SIZE_TILE;

    if (x < 0 || x >= _col || y < 0 || y >= _row)
        return;

    _index_x = x;
    _index_y = y;

    _mouse_click_tile_center =
    {
        _board_render_pos.x + x * SIZE_TILE + SIZE_TILE / 2,
        _board_render_pos.y + y * SIZE_TILE + SIZE_TILE / 2
    };

    auto& tile = _board[y][x];

    switch (_current_input)
    {
    case InPutType::Empty:
        if (_start_pos_index.x == x && _start_pos_index.y == y)
            _start_pos_index = { -1, -1 };

        if (_end_pos_index.x == x && _end_pos_index.y == y)
            _end_pos_index = { -1, -1 };

        tile.change_status(Tile::Status::Empty);
        break;

    case InPutType::Wall:
        if (_start_pos_index.x == x && _start_pos_index.y == y)
            _start_pos_index = { -1, -1 };

        if (_end_pos_index.x == x && _end_pos_index.y == y)
            _end_pos_index = { -1, -1 };

        tile.change_status(Tile::Status::Wall);
        break;

    case InPutType::Start:
        if (_end_pos_index.x == x && _end_pos_index.y == y)
            _end_pos_index = { -1, -1 };

        if (_start_pos_index.x >= 0 && _start_pos_index.y >= 0 &&
            !(_start_pos_index.x == x && _start_pos_index.y == y))
        {
            _board[_start_pos_index.y][_start_pos_index.x].change_status(Tile::Status::Empty);
        }

        _start_pos_index = { x, y };
        tile.change_status(Tile::Status::Start);
        break;

    case InPutType::Goal:
        if (_start_pos_index.x == x && _start_pos_index.y == y)
            _start_pos_index = { -1, -1 };

        if (_end_pos_index.x >= 0 && _end_pos_index.y >= 0 &&
            !(_end_pos_index.x == x && _end_pos_index.y == y))
        {
            _board[_end_pos_index.y][_end_pos_index.x].change_status(Tile::Status::Empty);
        }

        _end_pos_index = { x, y };
        tile.change_status(Tile::Status::Goal);
        break;

    default:
        break;
    }
}

void Board::on_mouse_move(const SDL_Event& event)
{
    if (event.type != SDL_MOUSEMOTION)
        return;

    if (is_inside(event.motion.x, event.motion.y))
    {
        _move_in_board = true;
        _mouse_pos.x = event.motion.x;
        _mouse_pos.y = event.motion.y;
    }
    else
        _move_in_board = false;
}

void Board::draw_mouse_pos_tile(SDL_Renderer* renderer, SDL_Point pos)
{
    if (!_move_in_board || tile_select == nullptr)
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
    SDL_Rect rect =
    {
        _board_render_pos.x + grid_pos.x * SIZE_TILE,
        _board_render_pos.y + grid_pos.y * SIZE_TILE,
        SIZE_TILE,
        SIZE_TILE
    };
    SDL_RenderCopy(renderer, tile_select, nullptr, &rect);
}
