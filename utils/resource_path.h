#pragma once

#include <SDL.h>

#include <string>

namespace ResourcePath
{
	inline bool file_exists(const std::string& path)
	{
		SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
		if (file == nullptr)
			return false;

		SDL_RWclose(file);
		return true;
	}

	inline std::string base_path()
	{
		char* path = SDL_GetBasePath();
		if (path == nullptr)
			return {};

		std::string result(path);
		SDL_free(path);
		return result;
	}

	inline std::string asset(const char* relative_path)
	{
		if (relative_path == nullptr)
			return {};

		std::string relative(relative_path);
		const std::string assets_prefix = "assets/";
		if (relative.rfind(assets_prefix, 0) == 0)
			relative.erase(0, assets_prefix.size());

		const std::string base = base_path();
		if (!base.empty())
		{
			const std::string from_executable = base + assets_prefix + relative;
			if (file_exists(from_executable))
				return from_executable;
		}

		return assets_prefix + relative;
	}
}
