#pragma once

#include "../Engine/SDLApp.h"
#include "../Engine/Math/Vector2.h"

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
	void RenderGrid(const SDLAppRenderer&)  const;

	// Calculate the dimensions of the world given the width and height
	// of the render region
	void CalculateWorldDimensions(int renderAreaW, int renderAreaH);

	// Calculate the top-left pos that the renderer will draw the world from
	Vector2 CalculateRenderOrigin(int renderAreaW, int renderAreaH) const;

	static const Vector2* GetInputDirection(const Uint8* pKeyState);
	bool ValidInputDirection(const Vector2& input);

	// Size of an individual cell in pixels
	static const int CELL_SIZE;

	// Direction vectors representing the four cardinal directions
	// that the snake can move in
	static const Vector2 NORTH;
	static const Vector2 EAST;
	static const Vector2 SOUTH;
	static const Vector2 WEST;

	const Vector2* m_pInputDir;
	const Vector2* m_pSnakeDir;
	int m_numRows;
	int m_numCols;
};