#include "SDLWindow.h"

#include <cassert>
#include <cstdio>

SDLWindow::SDLWindow(SDL_Window* pWin)
	: m_pWindow(pWin)
{
	assert(pWin);

	SetTitle(SDL_GetWindowTitle(pWin));
}

SDLWindow::~SDLWindow()
{
	assert(m_pWindow && "Attempting to destroy a null SDL_Window!");

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	printf("SDLWindow has been successfully destroyed\n");
}

void SDLWindow::SetTitle(const char* pTitle)
{
	assert(pTitle && "Attempted to pass a null window title");

	m_title = pTitle;
	SDL_SetWindowTitle(m_pWindow, m_title);
}

SDLWindow::WindowSize SDLWindow::GetSize() const
{
	int w, h;
	SDL_GetWindowSize(m_pWindow, &w, &h);

	return WindowSize{w, h};
}