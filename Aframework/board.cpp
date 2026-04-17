#include "board.h"
#include "txt_texture_manager.h"
#include "../utils/display_string.h"

#include <cmath>
#include <SDL_image.h>//图像库
#include <algorithm>

SDL_Texture* Board::tile_select = nullptr;
SDL_Texture* Board::tile_start = nullptr;
SDL_Texture* Board::tile_end = nullptr;
SDL_Texture* Board::tile_open = nullptr;
SDL_Texture* Board::tile_open_rot45 = nullptr;
SDL_Texture* Board::tile_path = nullptr;
SDL_Texture* Board::tile_path_rot45 = nullptr;
SDL_Texture* Board::tile_stop = nullptr;
SDL_Texture* Board::tile_stop_rot45 = nullptr;

namespace
{
    SDL_Rect make_label_rect(SDL_Point pos, SDL_Texture* texture)
    {
        if (texture == nullptr)
            return { pos.x, pos.y, 0, 0 };

        int texture_width = 0;
        int texture_height = 0;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height) != 0)
            return { pos.x, pos.y, 0, 0 };

        return { pos.x, pos.y, texture_width, texture_height };
    }

    int direction_sign(int value)
    {
        if (value > 0)
            return 1;

        if (value < 0)
            return -1;

        return 0;
    }
}

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

    _board_snapshot.clear();

    _move_in_board = false;
    _click_in_board = false;
    _on_process = false;

    _index_x = -1;
    _index_y = -1;

    _start_pos_index = { -1, -1 };
    _end_pos_index = { -1, -1 };
    _info_tile_index = { -1, -1 };

    _mouse_click_tile_center = { 0, 0 };
}

void Board::clear_path_data()
{
    _board_snapshot.clear();

    for (auto& row : _board)
    {
        for (auto& tile : row)
        {
            tile._g_cost = 0;
            tile._h_cost = 0;
            tile._f_cost = 0;
            tile._parent = { -1, -1 };

            const Tile::Status status = tile.get_status();
            if (status == Tile::Status::Open ||
                status == Tile::Status::Closed ||
                status == Tile::Status::Path)
            {
                tile.change_status(Tile::Status::Empty);
            }
        }
    }

    if (is_valid_tile_index(_start_pos_index))
        _board[_start_pos_index.y][_start_pos_index.x].change_status(Tile::Status::Start);

    if (is_valid_tile_index(_end_pos_index))
        _board[_end_pos_index.y][_end_pos_index.x].change_status(Tile::Status::Goal);

    _on_process = false;
}

void Board::init(SDL_Renderer* renderer, TTF_Font* info_font)
{
    _info_font = info_font;
    _number_renderer = std::make_unique<NumberRenderer>(renderer, info_font);

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
    tile_open = load_texture("assets/texture/tile_open.png");
    tile_open_rot45 = load_texture("assets/texture/tile_open_rot45.png");
    tile_path = load_texture("assets/texture/tile_path.png");
    tile_path_rot45 = load_texture("assets/texture/tile_path_rot45.png");
    tile_stop = load_texture("assets/texture/tile_stop.png");
    tile_stop_rot45 = load_texture("assets/texture/tile_stop_rot45.png");

}

void Board::on_render(SDL_Renderer* renderer)
{
    draw_board(renderer);
    draw_mouse_pos_tile(renderer, _mouse_pos);

    for (int y = 0; y < _row; ++y)
    {
        for (int x = 0; x < _col; ++x)
        {
            Tile::Status status = _board[y][x].get_status();



            SDL_Rect rect =
            {
                _board_render_pos.x + x * SIZE_TILE + 2,
                _board_render_pos.y + y * SIZE_TILE + 2,
                SIZE_TILE - 4,
                SIZE_TILE - 4
            };
            

            if (_show_weight && status != Tile::Status::Wall)
            {
                int w = _board[y][x]._weight;

                SDL_Color color = { 0, 0, 0, 255 };

                if (w <= 1)       color = { 116, 174, 98, 255 };
                else if (w == 2)  color = { 139, 184, 91, 255 };
                else if (w == 3)  color = { 164, 190, 82, 255 };
                else if (w == 4)  color = { 191, 190, 73, 255 };
                else if (w == 5)  color = { 211, 178, 65, 255 };
                else if (w == 6)  color = { 224, 157, 58, 255 };
                else if (w == 7)  color = { 220, 128, 56, 255 };
                else if (w == 8)  color = { 211, 98, 58, 255 };
                else if (w == 9)  color = { 196, 70, 64, 255 };
                else              color = { 176, 50, 58, 255 };

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &rect);
            }


            switch (status)
            {
            case Tile::Status::Wall:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                break;

            case Tile::Status::Start:
                SDL_RenderCopy(renderer, tile_start, nullptr, &rect);
                draw_tile_cost(status, _board[y][x], rect);
                continue;

            case Tile::Status::Goal:
                SDL_RenderCopy(renderer, tile_end, nullptr, &rect);
                draw_tile_cost(status, _board[y][x], rect);
                continue;

            case Tile::Status::Open:
                if (draw_directed_tile(renderer, status, _board[y][x], x, y, rect))
                {
                    draw_tile_cost(status, _board[y][x], rect);
                    continue;
                }

                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
                break;

            case Tile::Status::Closed:
                if (draw_directed_tile(renderer, status, _board[y][x], x, y, rect))
                {
                    draw_tile_cost(status, _board[y][x], rect);
                    continue;
                }

                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
                break;

            case Tile::Status::Path:
                if (draw_directed_tile(renderer, status, _board[y][x], x, y, rect))
                {
                    draw_tile_cost(status, _board[y][x], rect);
                    continue;
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                break;

            default:
                continue;
            }

            SDL_RenderFillRect(renderer, &rect);
            draw_tile_cost(status, _board[y][x], rect);
        }
    }

    draw_tile_info_panel(renderer);
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

bool Board::is_inside(int x, int y) const
{
    return x >= _board_render_pos.x
        && x < _board_render_pos.x + _col * SIZE_TILE
        && y >= _board_render_pos.y
        && y < _board_render_pos.y + _row * SIZE_TILE;

}

Point Board::get_tile_index_at(int x, int y) const
{
    if (!is_inside(x, y))
        return { -1, -1 };

    return
    {
        (x - _board_render_pos.x) / SIZE_TILE,
        (y - _board_render_pos.y) / SIZE_TILE
    };
}

bool Board::is_valid_tile_index(Point index) const
{
    return index.x >= 0 && index.x < _col
        && index.y >= 0 && index.y < _row;
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

bool Board::draw_directed_tile(SDL_Renderer* renderer, Tile::Status status, const Tile& tile, int x, int y, const SDL_Rect& rect)
{
    if (renderer == nullptr || tile._parent.x < 0 || tile._parent.y < 0)
        return false;

    const int dx = direction_sign(tile._parent.x - x);
    const int dy = direction_sign(tile._parent.y - y);

    if (dx == 0 && dy == 0)
        return false;

    const bool diagonal = dx != 0 && dy != 0;
    SDL_Texture* texture = get_directed_tile_texture(status, diagonal);

    if (texture == nullptr)
        return false;

    double angle = 0.0;

    if (diagonal)
    {
        if (dx == 1 && dy == 1)
            angle = 90.0;
        else if (dx == -1 && dy == 1)
            angle = 180.0;
        else if (dx == -1 && dy == -1)
            angle = 270.0;
    }
    else
    {
        if (dx == 1)
            angle = 90.0;
        else if (dy == 1)
            angle = 180.0;
        else if (dx == -1)
            angle = 270.0;
    }

    SDL_RenderCopyEx(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    return true;
}

void Board::draw_tile_cost(Tile::Status status, const Tile& tile, const SDL_Rect& rect) const
{
    if (!_show_cost || _number_renderer == nullptr || status == Tile::Status::Wall)
        return;

    const int cost = tile._f_cost;
    if (cost <= 0)
        return;

    const SDL_Rect cost_rect =
    {
        rect.x + 4,
        rect.y + 8,
        rect.w - 8,
        rect.h - 12
    };
    _number_renderer->render_number(cost, cost_rect);
}

SDL_Texture* Board::get_directed_tile_texture(Tile::Status status, bool diagonal) const
{
    switch (status)
    {
    case Tile::Status::Open:
        return diagonal ? tile_open_rot45 : tile_open;

    case Tile::Status::Closed:
        return diagonal ? tile_stop_rot45 : tile_stop;

    case Tile::Status::Path:
        return diagonal ? tile_path_rot45 : tile_path;

    default:
        return nullptr;
    }
}

void Board::draw_tile_info_panel(SDL_Renderer* renderer)
{
    const SDL_Rect panel_rect = { 12, 6, 186, 152 };
    SDL_SetRenderDrawColor(renderer, 205, 205, 205, 255);
    SDL_RenderFillRect(renderer, &panel_rect);

    render_info_label(renderer, "Cur Tile:", { 20, 10 });
    render_info_label(renderer, "G:", { 20, 30 });
    render_info_label(renderer, "H:", { 20, 50 });
    render_info_label(renderer, "F:", { 20, 70 });
    render_info_label(renderer, "weight:", { 20, 90 });
    render_info_label(renderer, "Status:", { 20, 110 });
    render_info_label(renderer, "Parent:", { 20, 130 });

    if (!is_valid_tile_index(_info_tile_index))
    {
        render_info_label(renderer, "None", { 85, 110 });
        render_info_label(renderer, "None", { 85, 130 });
        return;
    }

    const Tile& tile = _board[_info_tile_index.y][_info_tile_index.x];

    render_info_label(renderer, "X:", { 120, 10 });
    render_info_number(_info_tile_index.x, { 139, 8, 22, 18 });
    render_info_label(renderer, "Y:", { 156, 10 });
    render_info_number(_info_tile_index.y, { 174, 8, 20, 18 });

    render_info_number(tile._g_cost, { 56, 28, 64, 18 });
    render_info_number(tile._h_cost, { 56, 48, 64, 18 });
    render_info_number(tile._f_cost, { 56, 68, 64, 18 });
    render_info_number(tile._weight, { 94, 88, 36, 18 });
    render_info_label(renderer, DisplayString::tile_status(tile.get_status()), { 85, 110 });
    render_info_label(renderer, "X:", { 85, 130 });
    render_info_number(tile._parent.x, { 104, 128, 28, 18 });
    render_info_label(renderer, "Y:", { 136, 130 });
    render_info_number(tile._parent.y, { 154, 128, 28, 18 });
}

void Board::render_info_label(SDL_Renderer* renderer, const char* text, SDL_Point pos)
{
    if (renderer == nullptr || _info_font == nullptr || text == nullptr)
        return;

    const SDL_Color text_color = { 15, 15, 15, 255 };
    SDL_Texture* texture = TxtTextureManager::instance().get_txt_texture(renderer, _info_font, text, true, text_color);
    if (texture == nullptr)
        return;

    const SDL_Rect rect = make_label_rect(pos, texture);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Board::render_info_number(int value, const SDL_Rect& rect) const
{
    if (_number_renderer == nullptr)
        return;

    _number_renderer->render_number(value, rect);
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

    const Point tile_index = get_tile_index_at(mouse_x, mouse_y);

    if (!is_valid_tile_index(tile_index))
        return;

    const int x = tile_index.x;
    const int y = tile_index.y;

    _index_x = x;
    _index_y = y;
    _info_tile_index = tile_index;

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

    case InPutType::Weight:
        tile._weight = _input_weight;
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
        _info_tile_index = get_tile_index_at(event.motion.x, event.motion.y);
    }
    else
        _move_in_board = false;
}

void Board::draw_mouse_pos_tile(SDL_Renderer* renderer, SDL_Point pos)
{
    if (!_move_in_board || tile_select == nullptr)
        return;

    Point grid_pos = { 0, 0 };

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

void Board::undo()
{
    if (_board_snapshot.empty())
        return;

    _board = std::move(_board_snapshot.back());
    _board_snapshot.pop_back();
}

void Board::save_snapshot()
{
    _board_snapshot.push_back(_board);
}

void Board::toggle_show_weight()
{
    _show_weight = !_show_weight;
}

void Board::toggle_show_cost()
{
    _show_cost = !_show_cost;
}

void Board::set_edit_locked(bool locked)
{
    _on_process = locked;
}

void Board::set_weight(int weight)
{
    _input_weight = weight;
}

Point Board::get_start_point()
{
    return _start_pos_index;
}

Point Board::get_end_point()
{
    return _end_pos_index;
}
