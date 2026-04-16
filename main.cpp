#define SDL_MAIN_HANDLED
#include "appliction/application.h"

int main(int argc, char** argv)
{
	Application* instance = Application::instance();
	
	return instance->run(argc, argv);
}