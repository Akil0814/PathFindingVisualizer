#include "application.h"

#include<iostream>
#include<SDL.h>
#include <SDL_ttf.h>//字体库
#include <SDL_mixer.h>//音频库
#include <SDL_image.h>//图像库

Application* Application::_instance = nullptr;


Application::Application()
{
	init_assert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL2 Error");
	init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_img Error");
	init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer Error");
	init_assert(!TTF_Init(), u8"SDL_ttf Error");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	init_assert(_window, u8"SDL_CreateWindow Error");

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);//硬件加速 垂直同步 目标纹理
	init_assert(_renderer, u8"SDL_CreateRenderer Error");

	_board = new Board();
	_button_manager = new ButtonManager();
}

Application::~Application()
{

}

Application* Application::instance()
{
	if (_instance == nullptr)
		_instance = new Application();

	return _instance;
}

int Application::run(int argc, char** argv)
{
	Uint64 last_counter = SDL_GetPerformanceCounter();
	const Uint64 counter_freq = SDL_GetPerformanceFrequency();//获取高性能计数器的频率
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	init();

	while (_active)
	{
		while (SDL_PollEvent(&_event))
		{
			if (_event.type == SDL_QUIT)
				_active = false;
			on_input();
		}

		Uint64 current_counter = SDL_GetPerformanceCounter();//实现动态延时
		double delta = (double)(current_counter - last_counter) / counter_freq;
		last_counter = current_counter;

		if (delta * 1000 < 1000.0 / FPS)
			SDL_Delay((Uint32)(1000.0 / FPS - delta * 1000));

		on_update(delta);

		SDL_SetRenderDrawColor(_renderer, back_ground_color.r, back_ground_color.g, back_ground_color.b, back_ground_color.a);
		SDL_RenderClear(_renderer);

		on_render();

		SDL_RenderPresent(_renderer);
	}

	return 0;
}


void Application::on_input()
{
	_board->on_input(_event);
	_button_manager->on_input(_event);
}

void Application::on_render()
{
	_board->on_render(_renderer);
	_button_manager->on_render(_renderer);
}

void Application::on_update(double delta)
{
	_board->on_update(delta);
	_button_manager->on_update(delta);
}

void Application::init()
{
	_board->init(_renderer);
	_button_manager->add_button(Button(_renderer, { 10,10,30,30 }));

}

