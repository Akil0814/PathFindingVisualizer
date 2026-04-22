#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <memory>
#include <cstddef>
#include <cstdlib>
#include <string>

#include "../status.h"
#include "../Aframework/board.h"
#include "../Aframework/button_manager.h"
#include "../Aframework/error_message.h"
#include "../Aframework/number_renderer.h"
#include "simulation_controller.h"

class Application
{
public:
	static Application* instance();

	int run(int argc, char** argv);
	
private:
	void init();
	void init_button();
	void tick();
	static void main_loop_callback(void* user_data);

	void on_render();
	void on_update(double delta);
	void on_input();

	void render_imgui();
	void render_status_titles();
	void clear_error_on_operation(const SDL_Event& event);
	bool validate_unlocked_operation(const char* message);
	bool validate_path_request();
	bool ensure_audio_ready();
	void refresh_button_sound_effects();
	void shutdown();

	void init_assert(bool flag, const char* err_msg)
	{
		if (flag)
			return;

#if defined(__EMSCRIPTEN__)
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Game Start Error: %s (%s)", err_msg, SDL_GetError());
#else
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Game Start Error", err_msg, _window);
#endif

		exit(-1);
	}

private:
	Application();
	~Application();

private:
	std::unique_ptr<Board> _board;
	std::unique_ptr<SimulationController> _controller;
	std::unique_ptr<ButtonManager> _button_manager;
	std::unique_ptr<ButtonManager> _edit_button_manager;
	std::unique_ptr<ButtonManager> _alg_button_manager;
	std::unique_ptr<ButtonManager> _dev_button_manager;
	std::unique_ptr<ErrorMessage> _error_message;
	std::unique_ptr<NumberRenderer> _number_renderer;

	InputType _current_input = InputType::Empty;

	int _input_weight = 1;
	float _auto_run_speed = 10.0f;
	std::size_t _pause_button_index = 0;

	bool _active = { true };
	bool _is_dev_mod = { false };
	bool _shutdown_done = { false };

	int _width = 1080;
	int _height = 720;
	double FPS = 60;
	Uint64 _last_counter = 0;
	Uint64 _counter_freq = 0;

	SDL_Event _event;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	SDL_Texture* _dev_button_texture = nullptr;
	Mix_Chunk* _button_sound_down = nullptr;
	Mix_Chunk* _button_sound_up = nullptr;
	TTF_Font* _button_font = nullptr;
	TTF_Font* _title_font = nullptr;
	std::string _click_down_path;
	std::string _click_up_path;
	bool _audio_opened = false;
	bool _audio_assets_loaded = false;
	bool _audio_failed = false;

	SDL_Color back_ground_color = { 175,175,175,255 };
};
