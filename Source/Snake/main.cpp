#pragma comment (lib, "SDL2.lib")
#pragma comment (lib, "SDL2main.lib")
#pragma comment (lib, "SDL2_image.lib")

#include "SnakeGame.h"
#include "../Engine/Util.h"

#include <SDL/SDL.h>
#include <cstdlib>

int main(int argc, char** argv)
{
	SDLApp* pApp = new SnakeGame;
	if(!pApp->Init())
		return EXIT_FAILURE;

	DBG_PRINT_SEPARATOR("RUNNING");

	pApp->Run();
	pApp->Shutdown();

	delete pApp;

	return EXIT_SUCCESS;
} 