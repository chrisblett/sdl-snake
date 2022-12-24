#pragma once

#include <SDL/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>

class Sprite;
class SDLAppRenderer;

typedef std::unique_ptr<Sprite> UniqueSpritePtr;

class Graphics
{
public:
	Graphics(SDL_Renderer* pRenderer);
	~Graphics();

	void LoadTexture(const std::string& filename);
	
	// Creates a sprite from a texture
	static Sprite* CreateSprite(const std::string& texturePath);

	// Creates a sprite, assigning a pointer to it into the given smart pointer
	static void LoadSprite(UniqueSpritePtr& pSprite, const std::string& texturePath);

	// Retrieve a loaded texture by name
	static SDL_Texture* GetTexture(const std::string& filename);

	SDLAppRenderer& GetRenderer() { return *m_pRenderer.get(); }

private:
	typedef std::unordered_map<std::string, SDL_Texture*> TexturesMap;

	static TexturesMap s_textures;

	std::unique_ptr<SDLAppRenderer> m_pRenderer;
	SDL_Renderer* m_pSDLRenderer;
};

class Sprite
{
public:
	Sprite(SDL_Texture* pTexture) : m_pTexture(pTexture) {}

	// Draw a sprite on the screen with a specified transform and rotation
	void Draw(const SDLAppRenderer& renderer, const SDL_Rect& destRect, float rotation) const;

	SDL_Texture* GetTexture() { return m_pTexture; }

private:
	SDL_Texture* m_pTexture;
};
