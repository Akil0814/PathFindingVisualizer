#include "error_message.h"

#include "txt_texture_manager.h"

#include <algorithm>
#include <utility>

void ErrorMessage::show(std::string message)
{
	_message = std::move(message);
	_visible = !_message.empty();
}

void ErrorMessage::clear()
{
	_message.clear();
	_visible = false;
}

bool ErrorMessage::visible() const
{
	return _visible;
}

void ErrorMessage::render(SDL_Renderer* renderer, TTF_Font* font, int window_width) const
{
	if (!_visible || renderer == nullptr || font == nullptr || _message.empty())
		return;

	const SDL_Color error_color = { 190, 30, 35, 255 };
	SDL_Texture* texture = TxtTextureManager::instance().get_txt_texture(renderer, font, _message, true, error_color);
	if (texture == nullptr)
		return;

	int texture_width = 0;
	int texture_height = 0;
	if (SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height) != 0)
		return;

	const SDL_Rect rect =
	{
		std::max(0, (window_width - texture_width) / 2),
		8,
		texture_width,
		texture_height
	};

	SDL_RenderCopy(renderer, texture, nullptr, &rect);
}
