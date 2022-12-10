#pragma once

#include "../Engine/SDLApp.h"
#include "../Engine/Array2D.h"
#include "../Engine/Math/Vector2.h"
#include "../Snake/Snake.h"

class SnakeGame : public SDLApp
{
public:
	SnakeGame();

	virtual bool Init()         override;
	virtual void Shutdown()     override;
	virtual void ProcessInput() override;
	virtual void Update()       override;
	virtual void Render()       override;

	void OccupyCell(int x, int y);

	// Returns true if the position is within the game area
	bool InWorldBounds(int x, int y) const;

private:
	void RenderGrid(const SDLAppRenderer&) const;
	void RenderCellInfo(const SDLAppRenderer&) const;

	// Calculate the dimensions of the world given the width and height
	// of the render region
	void CalculateWorldDimensions(int renderAreaW, int renderAreaH);

	// Calculate the top-left pos that the renderer will draw the world from
	Vector2 CalculateRenderOrigin(int renderAreaW, int renderAreaH) const;

	static const Vector2* GetInputDirection(const Uint8* pKeyState);
	bool ValidInputDirection(const Vector2& input) const;

	void GenerateFood();

	// Size of an individual cell in pixels
	static const int CELL_SIZE;

	// Direction vectors representing the four cardinal directions
	// that the snake can move in
	static const Vector2 NORTH;
	static const Vector2 EAST;
	static const Vector2 SOUTH;
	static const Vector2 WEST;

	struct Cell
	{
		Vector2 position;
		bool free;
	};

	std::unique_ptr<Snake> m_pSnake;
	Array2D<Cell>		   m_cells;
	const Vector2*         m_pInputDir;
	Cell*                  m_pFoodLocation;  // Cell that is holding the food
	float                  m_nextUpdateTime; // Time until the next update
	bool                   m_snakeCanGrow;
	int                    m_worldWidth;
	int                    m_worldHeight;
};