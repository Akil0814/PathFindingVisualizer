#include "number_renderer.h"

#include <algorithm>
#include <string>
#include <vector>

NumberRenderer::NumberRenderer(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color)
	: _renderer(renderer), _font(font), _color(color)
{
	if (_renderer == nullptr || _font == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "NumberRenderer needs a valid renderer and font.");
		return;
	}

	for (int digit = 0; digit < 10; ++digit)
		_digit_textures[digit] = create_digit_texture(digit);
}

NumberRenderer::~NumberRenderer()
{
	clear();
}

void NumberRenderer::render_number(int value, const SDL_Rect& rect) const
{
	if (!is_valid() || rect.w <= 0 || rect.h <= 0)
		return;

	long long number = value;
	if (number < 0)
		number = -number;

	std::string text = std::to_string(number);
	if (text.empty())
		text = "0";

	std::vector<int> widths;
	std::vector<int> heights;
	widths.reserve(text.size());
	heights.reserve(text.size());

	int total_width = 0;
	int max_height = 0;

	for (char ch : text)
	{
		const int digit = ch - '0';
		if (digit < 0 || digit > 9 || _digit_textures[digit] == nullptr)
			return;

		int digit_width = 0;
		int digit_height = 0;
		if (SDL_QueryTexture(_digit_textures[digit], nullptr, nullptr, &digit_width, &digit_height) != 0)
			return;

		widths.push_back(digit_width);
		heights.push_back(digit_height);
		total_width += digit_width;
		max_height = std::max(max_height, digit_height);
	}

	if (total_width <= 0 || max_height <= 0)
		return;

	const float scale = std::min(
		1.0f,
		std::min(
			static_cast<float>(rect.w) / static_cast<float>(total_width),
			static_cast<float>(rect.h) / static_cast<float>(max_height)
		)
	);

	const int render_width = static_cast<int>(total_width * scale);
	int x = rect.x + (rect.w - render_width) / 2;

	for (std::size_t i = 0; i < text.size(); ++i)
	{
		const int digit = text[i] - '0';
		const int digit_width = std::max(1, static_cast<int>(widths[i] * scale));
		const int digit_height = std::max(1, static_cast<int>(heights[i] * scale));
		const SDL_Rect dst =
		{
			x,
			rect.y + (rect.h - digit_height) / 2,
			digit_width,
			digit_height
		};

		SDL_RenderCopy(_renderer, _digit_textures[digit], nullptr, &dst);
		x += digit_width;
	}
}

bool NumberRenderer::is_valid() const
{
	if (_renderer == nullptr || _font == nullptr)
		return false;

	for (SDL_Texture* texture : _digit_textures)
	{
		if (texture == nullptr)
			return false;
	}

	return true;
}

void NumberRenderer::clear()
{
	for (SDL_Texture*& texture : _digit_textures)
	{
		if (texture != nullptr)
		{
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
	}
}

SDL_Texture* NumberRenderer::create_digit_texture(int digit) const
{
	const std::string text = std::to_string(digit);
	SDL_Surface* surface = TTF_RenderUTF8_Blended(_font, text.c_str(), _color);
	if (surface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to render digit %d: %s", digit, TTF_GetError());
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_FreeSurface(surface);

	if (texture == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create digit texture %d: %s", digit, SDL_GetError());
		return nullptr;
	}

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}
