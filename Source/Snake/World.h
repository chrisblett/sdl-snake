#pragma once

#include "../Engine/Array2D.h"
#include "Snake.h"
#include "SnakeGame.h"

#include <memory>

struct Cell
{
	Vector2 position;
	bool free{}; // Not occupied by the snake or food
};

class SDLAppRenderer;
class SnakeBrain;
class Sprite;

class World
{
public:
	World(int width, int height);
	~World();

	void Reset();
	SnakeStatus Update(SnakeBrain& brain);
	void Render(const SDLAppRenderer&) const;

	void OccupyCell(int x, int y);
	const Cell& GetCell(int x, int y) const;

	// Returns true if the position is within the world limits
	bool InBounds(int x, int y) const;

	// Returns true if the cell located at position (x, y) is free
	bool IsFree(int x, int y) const;

	Snake* GetSnake() { return m_pSnake.get(); }
	int GetWidth() const { return m_worldWidth; }
	int GetHeight() const { return m_worldHeight; }
private:
	void GenerateFood();

	// Clears all cells in the world to empty
	void ClearAll();

	std::unique_ptr<Snake>  m_pSnake;
	std::unique_ptr<Sprite> m_pFood;
	Array2D<Cell>           m_cells;
	Cell*                   m_pFoodLocation; // Cell that is holding the food
	int  m_worldWidth;
	int  m_worldHeight;
	bool m_noFoodLeft;
};

class WorldDebugDraw
{
public:
	// Colours each cell in the world according to whether it is occupied
	static void RenderCellFreeStatus(const World& world, const SDLAppRenderer& renderer);

	// Overlays a grid on top of the world, indicating world and individual cell boundaries
	static void RenderGrid(const World& world, const SDLAppRenderer& renderer);
};