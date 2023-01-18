#include "SDLApp.h"
#include "SDLAppRenderer.h"
#include "SDLWindow.h"
#include "Util.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstdio>
#include <memory>

const float SDLApp::MAX_DELTA_VALUE = 0.05f;

using Util::DebugPrint;

SDLApp::SDLApp()
	: m_isRunning(true)
	, m_ticksCount(0)
	, m_deltaTime(0.0f)
{
}

void SDLApp::Run()
{
	while (m_isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

bool SDLApp::InitSDL()
{
	// Attempt to initialise the SDL video subsystem
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialise SDL: %s", SDL_GetError());
		return false;
	}

	m_ticksCount = SDL_GetTicks();

	// Attempt to create to window
	SDL_Window* pWindow = SDL_CreateWindow(
		"SDLApp", // Title
		100,	  // Top left x-coordinate
		100,      // Top left y-coordinate
		800,      // Width
		600,      // Height
		0         // Flags (0 for no flags set)
	);

	if (!pWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Attempt to create a renderer
	SDL_Renderer* pRenderer = SDL_CreateRenderer(
		pWindow, // Window to create renderer for
		-1,      // Let SDL decide the graphics driver
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!pRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Failed to initialise SDL_image: %s", IMG_GetError());
		return false;
	}

	m_pWindow   = std::make_unique<SDLWindow>(pWindow);
	m_pGraphics = std::make_unique<Graphics>(pRenderer);

	return true;
}

void SDLApp::ShutdownSDL()
{
	// Deallocate in reverse order of initialisation
	m_pGraphics.reset();
	m_pWindow.reset();

	IMG_Quit();
	SDL_Quit();

	DebugPrint("SDL subsystems have shutdown successfully\n");
}

void SDLApp::AdvanceTimestep()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), m_ticksCount + 16))
		;

	// Delta time is the time passed since last frame
	// (converted to seconds)
	m_deltaTime = (SDL_GetTicks() - m_ticksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (m_deltaTime > MAX_DELTA_VALUE)
	{
		m_deltaTime = MAX_DELTA_VALUE;
	}

	// Update ticks count (for next frame)
	m_ticksCount = SDL_GetTicks();
}