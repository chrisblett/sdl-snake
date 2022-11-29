#pragma once

#include <SDL/SDL.h>

class SDLWindow
{
public:
	// Holds the dimensions of a window
	struct WindowSize
	{
		int w;
		int h;
	};

	SDLWindow(SDL_Window* pWin);
	~SDLWindow();

	void SetTitle(const char* pTitle);

	WindowSize GetSize() const;

private:
	SDL_Window* m_pWindow;
	const char* m_title;
};