#pragma once

#include "../Engine/Math/Vector2.h"

#include <SDL/SDL.h>

class SDLAppRenderer
{
public:
	SDLAppRenderer(SDL_Renderer* pRenderer);
	~SDLAppRenderer();

	// Set the world transformation
	void SetWorldTransform(const Vector2& screenSpaceOrigin, float scaleFactor);

	// Transforms a point from world space to screen space
	Vector2 WorldToScreen(const Vector2& pos) const;

	// Transforms a rect from world space to screen space
	SDL_Rect WorldToScreen(float x, float y, float width, float height) const;

	/* Drawing operations */

	// Present the rendering results to the screen
	void SwapBuffers() const;

	// Sets the colour to be used for all rendering operations
	void SetDrawColour(
		uint8_t r,
		uint8_t g,
		uint8_t b,
		uint8_t a) const;

	// Clears the screen using the draw colour
	void Clear() const;

	// Draws a rect outline in screen space
	void DrawRect(const Vector2& pos, int width, int height) const;
	void DrawRect(const SDL_Rect& rect) const;

	// Draws a filled rect in screen space
	void FillRect(const Vector2& pos, int width, int height) const;
	void FillRect(const SDL_Rect& rect) const;

	// Draws a line including the endpoints, in screen-space
	void DrawLine(int x1, int y1, int x2, int y2) const;
	void DrawLine(const Vector2& a, const Vector2& b) const;

	void DrawTexture(SDL_Texture* pTexture, const SDL_Rect* pDestRect, float angle) const;

private:
	SDL_Renderer* m_pRenderer;
	Vector2       m_worldOriginScreenSpace;
	float         m_worldToScreenScaleFactor;
};