#include "Graphics.h"
#include "SDLAppRenderer.h"
#include "Util.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <cstdio>
#include <cassert>

Graphics::TexturesMap Graphics::s_textures;

using Util::DebugPrint;

Graphics::Graphics(SDL_Renderer* pRenderer)
	: m_pSDLRenderer(pRenderer)
{
	m_pRenderer = std::make_unique<SDLAppRenderer>(pRenderer);
}

Graphics::~Graphics()
{
	// Free all textures
	DebugPrint("Freeing all textures\n");

	auto it = s_textures.begin();
	while (it != s_textures.end())
	{
		SDL_DestroyTexture(it->second);
		it++;
	}
	s_textures.clear();

	DebugPrint("Graphics destroyed\n");
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

SDL_Texture* Graphics::GetTexture(const std::string& filename)
{
	SDL_Texture* pTexture = nullptr;

	auto it = s_textures.find(filename);

	// Found texture?
	if (it != s_textures.end())
	{
		pTexture = it->second;
	}

	return pTexture;
}

Sprite* Graphics::CreateSprite(const std::string& texturePath)
{
	SDL_Texture* pTexture = Graphics::GetTexture(texturePath);
	if (pTexture)
	{
		return new Sprite(pTexture);
	}

	DebugPrint("Could not create sprite from texture\n");

	return nullptr;
}

void Graphics::LoadSprite(UniqueSpritePtr& pSprite, const std::string& texturePath)
{
	pSprite = UniqueSpritePtr(CreateSprite(texturePath));
	assert(pSprite != nullptr);
}

void Sprite::Draw(const SDLAppRenderer& renderer, const SDL_Rect& destRect, float angle) const
{
	renderer.DrawTexture(m_pTexture, &destRect, angle);
}