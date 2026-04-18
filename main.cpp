#include "application/application.h"

//很多项目都是这样，开头像泡面，做到一半发现是满汉全席，还是自己点的 - by ChatGPT

int main(int argc, char** argv)
{
#if defined(SDL_MAIN_HANDLED)
	SDL_SetMainReady();
#endif

	Application* instance = Application::instance();
	
	return instance->run(argc, argv);
}
