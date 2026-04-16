#include "txt_texture_manager.h"

#include <cstdint>

TxtTextureManager& TxtTextureManager::instance()
{
	static TxtTextureManager manager;
	return manager;
}

SDL_Texture* TxtTextureManager::get_txt_texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
	bool is_bold, const SDL_Color& color)
{
	if (has_invalid_args(renderer, font, text))
		return nullptr;

	const int old_style = TTF_GetFontStyle(font);
	int render_style = old_style;

	if (is_bold)
		render_style |= TTF_STYLE_BOLD;

	std::string key = make_text_key(renderer, font, text, render_style, color);
	
	auto it = text_texture_pool.find(key);
	if (it != text_texture_pool.end())
		return it->second;

	TTF_SetFontStyle(font, render_style);

	SDL_Surface* suf_text = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	TTF_SetFontStyle(font, old_style);

	if (!suf_text)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			"TTF_RenderUTF8_Blended failed for text \"%s\": %s",
			text.c_str(), TTF_GetError());
		return nullptr;
	}

	SDL_Texture* tex_text = SDL_CreateTextureFromSurface(renderer, suf_text);
	SDL_FreeSurface(suf_text);

	if (!tex_text)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
			"SDL_CreateTextureFromSurface failed for text \"%s\": %s",
			text.c_str(), SDL_GetError());
		return nullptr;
	}

	SDL_SetTextureBlendMode(tex_text, SDL_BLENDMODE_BLEND);

	text_texture_pool[key] = tex_text;

	return tex_text;
}

TxtTextureManager::~TxtTextureManager()
{
	clear();
}

void TxtTextureManager::clear()
{
	for (auto& kv : text_texture_pool)
		SDL_DestroyTexture(kv.second);

	text_texture_pool.clear();
}

bool TxtTextureManager::has_invalid_args(SDL_Renderer* renderer, TTF_Font* font, const std::string& text) const
{
	if (!renderer)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Renderer is null for text \"%s\"", text.c_str());
		return true;
	}

	if (!font)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Font is null for text \"%s\"", text.c_str());
		return true;
	}

	if (text.empty())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Text must not be empty.");
		return true;
	}

	return false;
}

std::string TxtTextureManager::make_text_key(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
	int style, const SDL_Color& color) const
{
	return std::to_string(reinterpret_cast<std::uintptr_t>(renderer))
		+ "|" + std::to_string(reinterpret_cast<std::uintptr_t>(font))
		+ "|" + text
		+ "|" + std::to_string(style)
		+ "|" + std::to_string(color.r)
		+ "," + std::to_string(color.g)
		+ "," + std::to_string(color.b)
		+ "," + std::to_string(color.a);
}

