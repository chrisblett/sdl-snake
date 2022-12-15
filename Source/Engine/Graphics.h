#pragma once

#include <SDL/SDL.h>
#include <cstdio>
#include <memory>

class SDLAppRenderer;

class Graphics
{
public:
	Graphics(SDL_Renderer* pRenderer)
		: m_pSDLRenderer(pRenderer)
	{
		m_pRenderer = std::make_unique<SDLAppRenderer>(pRenderer);
	}

	~Graphics()
	{
		printf("Graphics destroyed\n");
	}

	SDLAppRenderer& GetRenderer() { return *m_pRenderer.get(); }

private:
	std::unique_ptr<SDLAppRenderer> m_pRenderer;
	SDL_Renderer* m_pSDLRenderer;
};
