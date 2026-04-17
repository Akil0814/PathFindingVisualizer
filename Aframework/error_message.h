#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

class ErrorMessage
{
public:
	void show(std::string message);
	void clear();

	[[nodiscard]] bool visible() const;

	void render(SDL_Renderer* renderer, TTF_Font* font, int window_width) const;

private:
	std::string _message;
	bool _visible = false;
};
