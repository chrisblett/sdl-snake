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

Texture::Texture(SDL_Texture* pTexture)
	: m_pTexture(pTexture)
{
	assert(pTexture);

	SDL_QueryTexture(pTexture, NULL, NULL, &m_width, &m_height);
}

Texture::~Texture()
{
	SDL_DestroyTexture(m_pTexture);
}

Graphics::Graphics(SDL_Renderer* pRenderer)
	: m_pSDLRenderer(pRenderer)
{
	m_pRenderer = std::make_unique<SDLAppRenderer>(pRenderer);
}

Graphics::~Graphics()
{
	// Free all textures
	printf("Freeing all textures\n");

	auto it = s_textures.begin();
	while (it != s_textures.end())
	{
		delete it->second;
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
		s_textures[filename] = new Texture(pTexture);
	}
	else
	{
		SDL_Log("Failed to load the texture '%s'", filename.c_str());
	}
}

Texture* Graphics::GetTexture(const std::string& filename)
{
	Texture* pTexture = nullptr;

	auto it = s_textures.find(filename);

	// Found texture?
	if (it != s_textures.end())
	{
		pTexture = it->second;
	}

	return pTexture;
}