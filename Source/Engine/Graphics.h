#pragma once

#include <SDL/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>

class SDLAppRenderer;

class Graphics
{
public:
	Graphics(SDL_Renderer* pRenderer);
	~Graphics();

	void LoadTexture(const std::string& filename);

	SDLAppRenderer& GetRenderer() { return *m_pRenderer.get(); }

private:
	typedef std::unordered_map<std::string, SDL_Texture*> TexturesMap;

	static TexturesMap s_textures;

	std::unique_ptr<SDLAppRenderer> m_pRenderer;
	SDL_Renderer* m_pSDLRenderer;
};
