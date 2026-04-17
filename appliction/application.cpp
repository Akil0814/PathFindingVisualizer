#include "application.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl2.h"
#include "../imgui/imgui_impl_sdlrenderer2.h"
#include "../Aframework/txt_texture_manager.h"
#include "../utils/display_string.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include<iostream>
#include <string>
#include<SDL.h>
#include <SDL_ttf.h>//字体库
#include <SDL_mixer.h>//音频库
#include <SDL_image.h>//图像库

namespace
{
	bool s_imgui_initialized = false;
	constexpr const char* kMoreInformationUrl = "https://akil0814.github.io/projects/PathFindingVisualizer/PathFindingVisualizer.html";
	constexpr const char* kAboutAuthorUrl = "https://akil0814.github.io/index.html";

	SDL_Rect make_centered_rect(SDL_Rect outer, SDL_Texture* texture, int padding = 8)
	{
		if (texture == nullptr)
			return { outer.x, outer.y, 0, 0 };

		int texture_width = 0;
		int texture_height = 0;
		if (SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height) != 0)
			return { outer.x, outer.y, 0, 0 };

		const int max_width = std::max(1, outer.w - padding * 2);
		const int max_height = std::max(1, outer.h - padding * 2);
		const float scale = std::min(
			1.0f,
			std::min(
				static_cast<float>(max_width) / static_cast<float>(texture_width),
				static_cast<float>(max_height) / static_cast<float>(texture_height)
			)
		);

		const int render_width = std::max(1, static_cast<int>(texture_width * scale));
		const int render_height = std::max(1, static_cast<int>(texture_height * scale));

		return
		{
			outer.x + (outer.w - render_width) / 2,
			outer.y + (outer.h - render_height) / 2,
			render_width,
			render_height
		};
	}

	void set_button_label(Button* button, SDL_Rect button_rect, SDL_Texture* texture)
	{
		if (button == nullptr || texture == nullptr)
			return;

		button->set_message_texture(texture);
		button->set_message_rect(make_centered_rect(button_rect, texture));
	}

	SDL_Rect make_label_rect(SDL_Point pos, SDL_Texture* texture)
	{
		if (texture == nullptr)
			return { pos.x, pos.y, 0, 0 };

		int texture_width = 0;
		int texture_height = 0;
		if (SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height) != 0)
			return { pos.x, pos.y, 0, 0 };

		return { pos.x, pos.y, texture_width, texture_height };
	}

	SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path)
	{
		if (renderer == nullptr || path == nullptr)
			return nullptr;

		SDL_Surface* surface = IMG_Load(path);
		if (surface == nullptr)
		{
			SDL_Log("IMG_Load failed for %s: %s", path, IMG_GetError());
			return nullptr;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		if (texture == nullptr)
			SDL_Log("SDL_CreateTextureFromSurface failed for %s: %s", path, SDL_GetError());

		return texture;
	}

	bool init_imgui_for_sdl_renderer(SDL_Window* window, SDL_Renderer* renderer)
	{
		if (s_imgui_initialized)
			return true;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer))
			return false;

		if (!ImGui_ImplSDLRenderer2_Init(renderer))
		{
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			return false;
		}

		s_imgui_initialized = true;
		return true;
	}

	void shutdown_imgui()
	{
		if (!s_imgui_initialized)
			return;

		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		s_imgui_initialized = false;
	}
}

Application::Application()
{
	init_assert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL2 Error");
	init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_img Error");
	init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer Error");
	init_assert(!TTF_Init(), u8"SDL_ttf Error");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_SHOWN);
	init_assert(_window, u8"SDL_CreateWindow Error");

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);//硬件加速 垂直同步 目标纹理
	init_assert(_renderer, u8"SDL_CreateRenderer Error");
}

Application::~Application()
{
	shutdown();
}

void Application::shutdown()
{
	if (_shutdown_done)
		return;

	_shutdown_done = true;

	shutdown_imgui();

	_number_renderer.reset();
	_error_message.reset();
	_edit_button_manager.reset();
	_alg_button_manager.reset();
	_dev_button_manager.reset();
	_button_manager.reset();
	_controller.reset();
	_board.reset();

	TxtTextureManager::instance().clear();

	if (_dev_button_texture != nullptr)
	{
		SDL_DestroyTexture(_dev_button_texture);
		_dev_button_texture = nullptr;
	}

	if (_button_font != nullptr)
	{
		TTF_CloseFont(_button_font);
		_button_font = nullptr;
	}

	if (_title_font != nullptr)
	{
		TTF_CloseFont(_title_font);
		_title_font = nullptr;
	}

	if (_renderer != nullptr)
	{
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}

	if (_window != nullptr)
	{
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}

	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

Application* Application::instance()
{
	static Application instance;
	return &instance;
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

	shutdown();
	return 0;
}

void Application::on_input()
{
	clear_error_on_operation(_event);

	if (_is_dev_mod && ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplSDL2_ProcessEvent(&_event);
		const ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse || io.WantCaptureKeyboard)
			return;
	}

	if (_controller != nullptr &&
		_controller->is_board_edit_locked() &&
		(_event.type == SDL_MOUSEBUTTONDOWN || _event.type == SDL_MOUSEMOTION))
	{
		const int mouse_x = _event.type == SDL_MOUSEBUTTONDOWN ? _event.button.x : _event.motion.x;
		const int mouse_y = _event.type == SDL_MOUSEBUTTONDOWN ? _event.button.y : _event.motion.y;
		const bool is_edit_action = _event.type == SDL_MOUSEBUTTONDOWN ||
			(_event.type == SDL_MOUSEMOTION && (_event.motion.state & SDL_BUTTON_LMASK));

		if (is_edit_action && _board != nullptr && _board->is_inside(mouse_x, mouse_y))
		{
			if (_error_message != nullptr)
				_error_message->show("Board is locked. Reset or Restart first.");
			return;
		}
	}

	_board->on_input(_event);

	_button_manager->on_input(_event);

	if (_is_dev_mod)
		_dev_button_manager->on_input(_event);
	_alg_button_manager->on_input(_event);
	_edit_button_manager->on_input(_event);
}

void Application::on_render()
{
	_board->on_render(_renderer);

	render_status_titles();
	_button_manager->on_render(_renderer);

	if(_is_dev_mod)
		_dev_button_manager->on_render(_renderer);

	_edit_button_manager->on_render(_renderer);
	_alg_button_manager->on_render(_renderer);

	if (_error_message != nullptr)
		_error_message->render(_renderer, _title_font, _width);

	rend_imgui();
}

void Application::on_update(double delta)
{
	_board->on_update(delta,_current_input);
	_controller->on_update(delta);

	if (Button* pause_button = _button_manager->get_button(_pause_button_index))
		pause_button->set_enabled(_controller != nullptr && _controller->is_auto_running());

	_button_manager->on_update(static_cast<float>(delta));

	if (_is_dev_mod)
		_dev_button_manager->on_update(static_cast<float>(delta));

	_alg_button_manager->on_update(static_cast<float>(delta));
	_edit_button_manager->on_update(static_cast<float>(delta));
}

void Application::clear_error_on_operation(const SDL_Event& event)
{
	if (_error_message == nullptr)
		return;

	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN)
		_error_message->clear();
}

bool Application::validate_unlocked_operation(const char* message)
{
	if (_controller == nullptr || !_controller->is_board_edit_locked())
		return true;

	if (_error_message != nullptr)
		_error_message->show(message != nullptr ? message : "Reset or Restart first.");

	return false;
}

bool Application::validate_path_request()
{
	if (_board == nullptr)
		return false;

	if (!_board->in_bounds(_board->get_start_point()))
	{
		if (_error_message != nullptr)
			_error_message->show("Missing start tile.");
		return false;
	}

	if (!_board->in_bounds(_board->get_end_point()))
	{
		if (_error_message != nullptr)
			_error_message->show("Missing goal tile.");
		return false;
	}

	return true;
}

void Application::init()
{
	_board = std::make_unique<Board>();
	_controller = std::make_unique<SimulationController>(_board.get());
	_controller->set_auto_run_speed(_auto_run_speed);
	_button_manager = std::make_unique<ButtonManager>();
	_dev_button_manager = std::make_unique<ButtonManager>();
	_alg_button_manager = std::make_unique<ButtonManager>();
	_edit_button_manager = std::make_unique<ButtonManager>();
	_error_message = std::make_unique<ErrorMessage>();

	_button_font = TTF_OpenFont("assets/font/Frick.otf", 22);
	init_assert(_button_font != nullptr, TTF_GetError());
	_title_font = TTF_OpenFont("assets/font/Frick.otf", 16);
	init_assert(_title_font != nullptr, TTF_GetError());
	_board->init(_renderer, _title_font);
	_number_renderer = std::make_unique<NumberRenderer>(_renderer, _title_font, SDL_Color{ 15, 15, 15, 255 });

	init_button();
}

void Application::rend_imgui()
{
	if (!_is_dev_mod)
		return;

	if (!init_imgui_for_sdl_renderer(_window, _renderer))
	{
		SDL_Log("ImGui init failed: %s", SDL_GetError());
		return;
	}

	ImGui_ImplSDL2_NewFrame();
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui::NewFrame();

	int mouse_x = 0;
	int mouse_y = 0;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	ImGui::SetNextWindowPos(ImVec2(15.0f, 15.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(360.0f, 390.0f), ImGuiCond_FirstUseEver);

	bool dev_mode_open = _is_dev_mod;
	if (ImGui::Begin("Dev Debug", &dev_mode_open))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(250.0f / 255.0f, 142.0f / 255.0f, 225.0f / 255.0f, 1.0f));
		ImGui::Text("More information:");
		if (ImGui::Selectable(kMoreInformationUrl, false))
			SDL_OpenURL(kMoreInformationUrl);
		ImGui::Text("About author:");
		if (ImGui::Selectable(kAboutAuthorUrl, false))
			SDL_OpenURL(kAboutAuthorUrl);
		ImGui::PopStyleColor();
		ImGui::Separator();

		ImGui::Text("Application");
		ImGui::Separator();
		ImGui::Text("Input mode: %s", DisplayString::input_type(_current_input));
		ImGui::Text("Algorithm: %s", DisplayString::algorithm(_controller != nullptr ? _controller->algorithm() : Algorithm::AStart));
		ImGui::Text("Move mode: %s", DisplayString::move_mode(_controller != nullptr ? _controller->move_mode() : MoveMode::FourWay));
		int algorithm_index = _controller != nullptr ? static_cast<int>(_controller->algorithm()) : 0;
		if (ImGui::Combo("Algorithm", &algorithm_index, "A Star\0Dijkstra\0BFS\0Greedy\0\0"))
		{
			if (validate_unlocked_operation("Reset or Restart before changing algorithm.") && _controller != nullptr)
				_controller->set_algorithm(static_cast<Algorithm>(algorithm_index));
		}

		ImGui::Spacing();
		ImGui::Text("State Machine");
		ImGui::Separator();
		if (_controller != nullptr)
		{
			ImGui::Text("Sim state: %s", DisplayString::sim_state(_controller->sim_state()));
			ImGui::Text("Play mode: %s", DisplayString::play_mode(_controller->play_mode()));
			ImGui::Text("Board edit: %s", _controller->is_board_edit_locked() ? "Locked" : "Unlocked");
			ImGui::Text("Auto run: %s", _controller->is_auto_running() ? "true" : "false");
			ImGui::Text("Pathfinder: %s", _controller->is_pathfinder_finished() ? "Finished" : "Active");
			ImGui::Text("Found path: %s", _controller->found_path() ? "true" : "false");
			ImGui::TextWrapped("Flow: Editing -> Running -> Finished -> Restart/Reset -> Editing");
		}
		else
		{
			ImGui::Text("Controller: null");
		}

		if (_controller != nullptr && _controller->algorithm() == Algorithm::AStart)
		{
			ImGui::Spacing();
			ImGui::Text("A* Heuristic");
			ImGui::Separator();
			int heuristic_index = static_cast<int>(_controller->a_star_heuristic());
			if (ImGui::Combo("Heuristic", &heuristic_index, "Manhattan\0Euclidean\0Octile\0Chebyshev\0\0"))
			{
				if (validate_unlocked_operation("Reset or Restart before changing heuristic."))
					_controller->set_a_star_heuristic(static_cast<HeuristicMode>(heuristic_index));
			}
			ImGui::Text("Current: %s", DisplayString::a_star_heuristic(_controller->a_star_heuristic()));
			ImGui::TextWrapped("Formula: %s", DisplayString::a_star_heuristic_formula(_controller->a_star_heuristic()));
		}

		ImGui::Spacing();
		ImGui::Text("Movement");
		ImGui::Separator();
		int move_mode_index = (_controller != nullptr && _controller->move_mode() == MoveMode::EightWay) ? 1 : 0;
		if (ImGui::Combo("Move mode", &move_mode_index, "Four Way\0Eight Way\0\0"))
		{
			if (validate_unlocked_operation("Reset or Restart before changing move mode."))
			{
				if (_controller != nullptr)
					_controller->set_move_mode(move_mode_index == 1 ? MoveMode::EightWay : MoveMode::FourWay);
			}
		}

		ImGui::Spacing();
		ImGui::Text("Auto Run");
		ImGui::Separator();
		if (ImGui::SliderFloat("Speed", &_auto_run_speed, 1.0f, 100.0f, "%.0f steps/s") && _controller != nullptr)
			_controller->set_auto_run_speed(_auto_run_speed);

		ImGui::Spacing();
		ImGui::Text("Weight Brush");
		ImGui::Separator();
		if (ImGui::SliderInt("Input weight", &_input_weight, 1, 10) && _board != nullptr)
			_board->set_weight(_input_weight);
		if (ImGui::Button("Use Weight Brush"))
			_current_input = InPutType::Weight;

		ImGui::Spacing();
		ImGui::Text("Mouse");
		ImGui::Separator();
		ImGui::Text("Position: %d, %d", mouse_x, mouse_y);
		ImGui::Text("Inside board: %s", (_board != nullptr && _board->is_inside(mouse_x, mouse_y)) ? "true" : "false");

		ImGui::Spacing();
		ImGui::Text("Tools");
		ImGui::Separator();
		if (ImGui::Button("Reset Board") && _board != nullptr)
		{
			_board->reset();
			if (_controller != nullptr)
				_controller->restart();
		}
		ImGui::SameLine();
		if (ImGui::Button("Quit"))
			_active = false;
	}
	ImGui::End();

	_is_dev_mod = dev_mode_open;

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _renderer);
}

void Application::render_status_titles()
{
	TxtTextureManager& txt_manager = TxtTextureManager::instance();
	const SDL_Color title_text_color = { 15, 15, 15, 255 };

	auto render_title = [&](const std::string& text, SDL_Point pos)
		{
			SDL_Texture* title_texture = txt_manager.get_txt_texture(_renderer, _title_font, text, true, title_text_color);
			if (title_texture == nullptr)
				return;

			const SDL_Rect rect = make_label_rect(pos, title_texture);
			SDL_RenderCopy(_renderer, title_texture, nullptr, &rect);
	};

	render_title(std::string("Edit mode: ") + DisplayString::edit_mode(_current_input), { 900, 20 });
	render_title(std::string("Alg using: ") + DisplayString::algorithm(_controller != nullptr ? _controller->algorithm() : Algorithm::AStart), { 20, 212 });

	render_title("Control", { 900, 180 });
	render_title("Reset", { 900, 460 });

	std::string status_text = "Unknown";
	if (_controller != nullptr)
	{
		if (_controller->is_pathfinder_finished())
			status_text = _controller->found_path() ? "Finished Found" : "Finished No Path";
		else if (_controller->play_mode() == PlayMode::Pause && _controller->is_board_edit_locked())
			status_text = "Paused";
		else
			status_text = DisplayString::sim_state(_controller->sim_state());
	}
	render_title("Status:", { 900, 620 });
	render_title(status_text, { 900, 640 });

	const int total_steps = _controller != nullptr ? _controller->total_steps() : 0;
	const int total_cost = _controller != nullptr ? _controller->total_cost() : 0;
	render_title("Total Steps:", { 20, 170 });//一共走了多少格子
	render_title("Total Cost:", { 20, 190 });//这条路径的总花费
	if (_number_renderer != nullptr)
	{
		_number_renderer->render_number(total_steps, { 150, 168, 42, 18 });
		_number_renderer->render_number(total_cost, { 140, 188, 52, 18 });
	}

	if(_is_dev_mod)
		render_title("Advance", { 20,474 });
}

void Application::init_button()
{
	Button* tmp;
	TxtTextureManager& txt_manager = TxtTextureManager::instance();
	const SDL_Color button_text_color = { 25, 25, 25, 255 };
	const SDL_Color title_text_color = { 15, 15, 15, 255 };

	auto make_text = [&](const char* text, bool is_bold = false) -> SDL_Texture*
		{
			return txt_manager.get_txt_texture(_renderer, _button_font, text, is_bold, button_text_color);
		};

	//input type
	SDL_Rect rect_button = { 900,40,70,50 };
	tmp = _edit_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Start", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before editing board."))
			return;

		_current_input = InPutType::Start;
		});

	rect_button = { 980,40,70,50 };
	tmp = _edit_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Goal", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before editing board."))
			return;

		_current_input = InPutType::Goal;
		});

	rect_button = { 900,100,70,50 };
	tmp = _edit_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Wall", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before editing board."))
			return;

		_current_input = InPutType::Wall;

		});

	rect_button = { 980,100,70,50 };
	tmp = _edit_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("ERASE", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before editing board."))
			return;

		_current_input = InPutType::Empty;
		});

	rect_button = { 20,230,150,50 };
	tmp = _alg_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("A Star", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before changing algorithm."))
			return;

		_controller->set_algorithm(Algorithm::AStart);
		});

	rect_button = { 20,290,150,50 };
	tmp = _alg_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Dijkstra", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before changing algorithm."))
			return;

		_controller->set_algorithm(Algorithm::Dijkstar);
		});

	rect_button = { 20,350,150,50 };
	tmp = _alg_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("BFS", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before changing algorithm."))
			return;

		_controller->set_algorithm(Algorithm::BFS);
		});

	rect_button = { 20,410,150,50 };
	tmp = _alg_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Greedy", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before changing algorithm."))
			return;

		_controller->set_algorithm(Algorithm::Greedy);
		});

	//run time
	rect_button = { 900,200,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Auto Run", true));
	tmp->set_on_click([this] {
		if (!validate_path_request())
			return;

		std::cout << "start " << std::endl;
		_controller->set_auto_run(true);
		});

	rect_button = { 900,260,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	_pause_button_index = _button_manager->size() - 1;
	tmp->set_enabled(false);
	set_button_label(tmp, rect_button, make_text("Pause", true));
	tmp->set_on_click([this] {
		if (_controller == nullptr || !_controller->is_auto_running())
			return;

		_controller->pause();
		});

	rect_button = { 900,320,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Next Step", true));
	tmp->set_on_click([this] {
		if (!validate_path_request())
			return;

		std::cout << "Next Step" << std::endl;
		_controller->next_step();
		});

	rect_button = { 900,380,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Prev Step", true));
	tmp->set_on_click([this] {
		if (_controller != nullptr && _controller->is_auto_running())
		{
			if (_error_message != nullptr)
				_error_message->show("Pause before undo.");
			return;
		}

		std::cout << " Prev Step " << std::endl;
		if (_controller == nullptr || !_controller->previous_step())
		{
			if (_error_message != nullptr)
				_error_message->show("No previous step.");
		}
		});

	//board statse
	rect_button = { 900,480,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Restart", true));
	tmp->set_on_click([this] {
		std::cout << "restart " << std::endl;
		_controller->restart();
		});

	rect_button = { 900,540,150,50 };
	tmp = _button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Reset", true));
	tmp->set_on_click([this] {
		std::cout << "reset " << std::endl;
		_board->reset();
		_controller->restart();
		});


	rect_button = { 20,678,22,22 };

	if (_dev_button_texture == nullptr)
		_dev_button_texture = load_texture(_renderer, "assets/texture/dev_button.png");

	if (_dev_button_texture != nullptr)
	{
		tmp = _button_manager->add_button(Button(
			_renderer,
			rect_button,
			{ 21,679,20,20 },
			_dev_button_texture,
			nullptr,nullptr));
	}
	else
	{
		tmp = _button_manager->add_button(Button(_renderer, rect_button));
		set_button_label(tmp, rect_button, make_text("Dev", true));
	}
	tmp->set_on_click([this] {
		_is_dev_mod = !_is_dev_mod;
		});

	rect_button = { 20,490,150,50 };
	tmp = _dev_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Show Cost", true));
	tmp->set_on_click([this] {
		_board->toggle_show_cost();
		});


	rect_button = { 20,550,150,50 };
	tmp = _dev_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Edit Weight", true));
	tmp->set_on_click([this] {
		if (!validate_unlocked_operation("Reset or Restart before editing weight."))
			return;

		_current_input = InPutType::Weight;
		});

	rect_button = { 20,610,150,50 };
	tmp = _dev_button_manager->add_button(Button(_renderer, rect_button));
	set_button_label(tmp, rect_button, make_text("Weight Graph", true));
	tmp->set_on_click([this] {
		_board->toggle_show_weight();
		});
}

