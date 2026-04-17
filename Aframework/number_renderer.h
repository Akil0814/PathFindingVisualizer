#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

#include <array>

class NumberRenderer
{
public:
	NumberRenderer(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color = { 0, 0, 0, 255 });
	~NumberRenderer();

	NumberRenderer(const NumberRenderer&) = delete;
	NumberRenderer& operator=(const NumberRenderer&) = delete;

	void render_number(int value, const SDL_Rect& rect) const;
	[[nodiscard]] bool is_valid() const;

private:
	void clear();
	SDL_Texture* create_digit_texture(int digit) const;

private:
	SDL_Renderer* _renderer = nullptr;
	TTF_Font* _font = nullptr;
	SDL_Color _color = { 0, 0, 0, 255 };
	std::array<SDL_Texture*, 10> _digit_textures = {};
};
