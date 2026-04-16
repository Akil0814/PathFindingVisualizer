#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

class TxtTextureManager
{
public:
	typedef std::unordered_map<std::string, SDL_Texture*> TextTexturePool;

	static TxtTextureManager& instance();

	SDL_Texture* get_txt_texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
		bool is_bold = false, const SDL_Color& color = { 0, 0, 0, 255 });

	void clear();

private:
	TxtTextureManager() = default;
	~TxtTextureManager();

	TxtTextureManager(const TxtTextureManager&) = delete;
	TxtTextureManager& operator=(const TxtTextureManager&) = delete;

	std::string make_text_key(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
		int style, const SDL_Color& color) const;

	bool has_invalid_args(SDL_Renderer* renderer, TTF_Font* font, const std::string& text) const;

private:
	TextTexturePool text_texture_pool;
};
