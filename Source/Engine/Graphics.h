#pragma once

#include <SDL/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <cassert>

class SDLAppRenderer;

class Texture
{
public:
	Texture(SDL_Texture* pTexture);
	~Texture();
private:
	SDL_Texture* m_pTexture;
	int m_width;
	int m_height;
};

class Graphics
{
public:
	Graphics(SDL_Renderer* pRenderer);
	~Graphics();

	void LoadTexture(const std::string& filename);
	static Texture* GetTexture(const std::string& filename);

	SDLAppRenderer& GetRenderer() { return *m_pRenderer.get(); }

private:
	typedef std::unordered_map<std::string, Texture*> TexturesMap;

	static TexturesMap s_textures;

	std::unique_ptr<SDLAppRenderer> m_pRenderer;
	SDL_Renderer* m_pSDLRenderer;
};
