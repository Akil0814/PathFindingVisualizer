#pragma once
#include<SDL.h>
#include "../Aframework/board.h"
#include "../Aframework/button_manager.h"

class Application
{
public:
	static Application* instance();

	int run(int argc, char** argv);
	
private:

	void init();

	void on_render();

	void on_update(double delta);
	void on_input();

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
	Board* _board;
	ButtonManager* _button_manager;

private:
	static Application* _instance;

	bool _active = { true };

	int _width = 1080;
	int _height = 720;
	double FPS = 60;

	SDL_Event _event;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	SDL_Color back_ground_color = { 175,175,175,255 };
};
