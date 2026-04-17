#pragma once
#include<SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <vector>

#include "../status.h"
#include "../Aframework/board.h"
#include "../Aframework/button_manager.h"
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
	Board* _board = nullptr;
	SimulationController* _controller = nullptr;
	ButtonManager* _button_manager = nullptr;
	ButtonManager* _edit_button_manager = nullptr;
	ButtonManager* _dev_button_manager = nullptr;
	std::unique_ptr<NumberRenderer> _number_renderer;

private:
	static Application* _instance;

	InPutType _current_input = InPutType::Empty;
	Algorithm _current_algorithm = Algorithm::AStart;
	PlayMode _current_play_mod = PlayMode::Idle;

	int _input_weight = 1;
	float _auto_run_speed = 10.0f;

	bool _active = { true };
	bool _is_dev_mod = { false };
	bool _show_weight_graph = { false };

	int _width = 1080;
	int _height = 720;
	double FPS = 60;

	SDL_Event _event;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	TTF_Font* _button_font = nullptr;
	TTF_Font* _title_font = nullptr;

	SDL_Color back_ground_color = { 175,175,175,255 };
};
