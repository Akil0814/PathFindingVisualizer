#pragma once
#include<SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <cstddef>

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

	void on_render();
	void on_update(double delta);
	void on_input();

	void rend_imgui();
	void render_status_titles();
	void clear_error_on_operation(const SDL_Event& event);
	bool validate_unlocked_operation(const char* message);
	bool validate_path_request();
	void shutdown();

	void init_assert(bool flag, const char* err_msg)
	{
		if (flag)
			return;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Game Start Error", err_msg, _window);
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

	InPutType _current_input = InPutType::Empty;

	int _input_weight = 1;
	float _auto_run_speed = 10.0f;
	std::size_t _pause_button_index = 0;

	bool _active = { true };
	bool _is_dev_mod = { false };
	bool _shutdown_done = { false };

	int _width = 1080;
	int _height = 720;
	double FPS = 60;

	SDL_Event _event;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	SDL_Texture* _dev_button_texture = nullptr;
	TTF_Font* _button_font = nullptr;
	TTF_Font* _title_font = nullptr;

	SDL_Color back_ground_color = { 175,175,175,255 };
};
