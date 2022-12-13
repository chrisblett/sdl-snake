#pragma once

#include "../Engine/SDLApp.h"
#include "../Engine/Array2D.h"
#include "../Engine/Math/Vector2.h"
#include "World.h"

class SnakeGame : public SDLApp
{
public:
	SnakeGame();

	virtual bool Init()         override;
	virtual void Shutdown()     override;
	virtual void ProcessInput() override;
	virtual void Update()       override;
	virtual void Render()       override;

private:
	// Calculate the top-left pos that the renderer will draw the world from
	Vector2 CalculateRenderOrigin(int renderAreaW, int renderAreaH,
		int worldWidth, int worldHeight) const;

	static const Vector2* GetInputDirection(const Uint8* pKeyState);
	bool ValidInputDirection(const Vector2& input) const;

	// Size of an individual cell in pixels
	static const int CELL_SIZE;

	// Direction vectors representing the four cardinal directions
	// that the snake can move in
	static const Vector2 NORTH;
	static const Vector2 EAST;
	static const Vector2 SOUTH;
	static const Vector2 WEST;

	std::unique_ptr<World> m_pWorld;
	const Vector2*         m_pInputDir;
	float                  m_nextUpdateTime; // Time until the next update
};