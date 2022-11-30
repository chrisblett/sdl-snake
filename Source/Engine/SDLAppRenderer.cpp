#include "SDLAppRenderer.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/SDLWindow.h"

#include <cassert>
#include <cstdio>

SDL_Rect CreateRect(int x, int y, int width, int height)
{
	return {x, y, width, height};
}

SDL_Rect CreateRect(const Vector2& pos, int width, int height)
{
	return CreateRect(
		static_cast<int>(roundf(pos.x)),
		static_cast<int>(roundf(pos.y)),
		width,
		height
	);
}

SDLAppRenderer::SDLAppRenderer(SDL_Renderer* pRenderer)
	: m_pRenderer(pRenderer)
	, m_worldToScreenScaleFactor(0.0f)
{
	assert(m_pRenderer);
}

SDLAppRenderer::~SDLAppRenderer()
{
	assert(m_pRenderer
		&& "Attempting to destroy a null SDL_Renderer!\n");

	SDL_DestroyRenderer(m_pRenderer);
	m_pRenderer = nullptr;

	printf("SDLAppRenderer has been successfully destroyed\n");
}

void SDLAppRenderer::SetWorldTransform(const Vector2& screenSpaceOrigin,
	float scaleFactor)
{
	m_worldOriginScreenSpace   = screenSpaceOrigin;
	m_worldToScreenScaleFactor = scaleFactor;
}

Vector2 SDLAppRenderer::WorldToScreen(const Vector2& pos) const
{
	Vector2 screenPos = m_worldOriginScreenSpace 
		+ Vector2(pos.x * m_worldToScreenScaleFactor, pos.y * m_worldToScreenScaleFactor);

	return screenPos;
}

SDL_Rect SDLAppRenderer::WorldToScreen(float x, float y,
	float width, float height) const
{
	// Calculate position and dimensions of rect in screen space
	Vector2 screenPos = WorldToScreen(Vector2(x, y));

	float screenWidth  = width  * m_worldToScreenScaleFactor;
	float screenHeight = height * m_worldToScreenScaleFactor;

	SDL_Rect screenSpaceRect = CreateRect(
		screenPos,
		static_cast<int>(roundf(screenWidth)),
		static_cast<int>(roundf(screenHeight))
	);

	return screenSpaceRect;
}

void SDLAppRenderer::SwapBuffers() const
{
	SDL_RenderPresent(m_pRenderer);
}

void SDLAppRenderer::SetDrawColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
{
	SDL_SetRenderDrawColor(m_pRenderer, r, g, b, a);
}

void SDLAppRenderer::Clear() const
{
	SDL_RenderClear(m_pRenderer);
}

void SDLAppRenderer::DrawRect(const Vector2& pos, int width, int height) const
{
	DrawRect(CreateRect(pos, width, height));
}

void SDLAppRenderer::DrawRect(const SDL_Rect& rect) const
{
	SDL_RenderDrawRect(m_pRenderer, &rect);
}

void SDLAppRenderer::FillRect(const Vector2& pos, int width, int height) const
{
	FillRect(CreateRect(pos, width, height));
}

void SDLAppRenderer::FillRect(const SDL_Rect& rect) const
{
	SDL_RenderFillRect(m_pRenderer, &rect);
}

void SDLAppRenderer::DrawLine(int x1, int y1, int x2, int y2) const
{
	SDL_RenderDrawLine(m_pRenderer, x1, y1, x2, y2);
}

void SDLAppRenderer::DrawLine(const Vector2& a, const Vector2& b) const
{
	DrawLine(
		static_cast<int>(a.x),
		static_cast<int>(a.y),
		static_cast<int>(b.x),
		static_cast<int>(b.y)
	);
}