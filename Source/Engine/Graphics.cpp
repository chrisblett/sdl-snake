#include "Graphics.h"
#include "SDLAppRenderer.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <memory>
#include <cstdio>
#include <unordered_map>
#include <string>
#include <cassert>

Graphics::TexturesMap Graphics::s_textures;

Graphics::Graphics(SDL_Renderer* pRenderer)
	: m_pSDLRenderer(pRenderer)
{
	m_pRenderer = std::make_unique<SDLAppRenderer>(pRenderer);
}

Graphics::~Graphics()
{
	// Free all textures
	auto it = s_textures.begin();
	while (it != s_textures.end())
	{
		SDL_DestroyTexture(it->second);
		it++;
	}
	s_textures.clear();

	printf("Graphics destroyed\n");
}

void Graphics::LoadTexture(const std::string& filename)
{
	SDL_Texture* pTexture = IMG_LoadTexture(m_pSDLRenderer, filename.c_str());
	if (pTexture)
	{
		s_textures[filename] = pTexture;
	}
	else
	{
		SDL_Log("Failed to load the texture '%s'", filename.c_str());
	}
}