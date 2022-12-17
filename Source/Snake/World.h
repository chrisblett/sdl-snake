#pragma once

#include "../Engine/Array2D.h"
#include "Snake.h"

#include <memory>

class SDLAppRenderer;
class SnakeBrain;

class World
{
public:
	World(int width, int height);
	~World() { printf("World destroyed\n"); }

	void Update(SnakeBrain& brain);
	void OccupyCell(int x, int y);

	// Returns true if the position is within the world limits
	bool InBounds(int x, int y) const;

	void Render(const SDLAppRenderer&) const;
	void RenderGrid(const SDLAppRenderer& renderer) const;
	void RenderCellInfo(const SDLAppRenderer& renderer) const;

	Snake* GetSnake() { return m_pSnake.get(); }

	// Draws a rect around the midpoint of a cell in the world
	static void DrawRectAtCell(const SDLAppRenderer&, const Vector2& cellPos, float rectScale);

private:
	void GenerateFood();

	struct Cell
	{
		Vector2 position;
		bool free; // Not occupied by the snake or food
	};

	std::unique_ptr<Snake> m_pSnake;
	Array2D<Cell>          m_cells;
	Cell*                  m_pFoodLocation; // Cell that is holding the food
	int                    m_worldWidth;
	int                    m_worldHeight;
};
