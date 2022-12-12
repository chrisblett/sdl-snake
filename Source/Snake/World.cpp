#include "World.h"
#include "../Engine/Math/Random.h"
#include "../Engine/SDLAppRenderer.h"

#include <memory>

World::World(const Vector2& snakeInputDir, int width, int height)
	: m_cells(width, height)
	, m_pFoodLocation(nullptr)
	, m_worldWidth(width)
	, m_worldHeight(height)
{
	// Create cells
	for (int y = 0; y < m_cells.Height(); y++)
	{
		for (int x = 0; x < m_cells.Width(); x++)
		{
			Cell& curCell = m_cells.Get(x, y);
			curCell.position.x = static_cast<float>(x);
			curCell.position.y = static_cast<float>(y);
			curCell.free = true;
		}
	}

	m_pSnake = std::make_unique<Snake>(*this, snakeInputDir, m_worldWidth, m_worldHeight);

	GenerateFood();
}

void World::Update(const Vector2& snakeInputDir, bool snakeShouldGrow)
{
	// Clear all cells
	for (int y = 0; y < m_cells.Height(); y++)
	{
		for (int x = 0; x < m_cells.Width(); x++)
		{
			m_cells.Get(x, y).free = true;
		}
	}

	// Don't lose data about the food
	m_pFoodLocation->free = false;

	m_pSnake->Update(*this, snakeInputDir, snakeShouldGrow);
}

void World::OccupyCell(int x, int y)
{
	if (!InBounds(x, y)) return;

	m_cells.Get(x, y).free = false;
}

bool World::InBounds(int x, int y) const
{
	return (x >= 0 && x < m_worldWidth &&
		(y >= 0 && y < m_worldHeight));
}

void World::Render(const SDLAppRenderer& renderer) const
{
	RenderCellInfo(renderer);
	RenderGrid(renderer);
	m_pSnake->Render(renderer);
}

void World::RenderCellInfo(const SDLAppRenderer& renderer) const
{
	for (int y = 0; y < m_cells.Height(); y++)
	{
		for (int x = 0; x < m_cells.Width(); x++)
		{
			// Colour each cell differently depending on if it's occupied
			const Cell& cell = m_cells.Get(x, y);
			if (cell.free)
			{
				// Free cells are blue
				renderer.SetDrawColour(0, 0, 32, 255);
			}
			else
			{
				// Occupied cells are red
				renderer.SetDrawColour(32, 0, 0, 255);
			}

			renderer.FillRect(renderer.WorldToScreen(
				static_cast<float>(x),
				static_cast<float>(y), 1, 1)
			);
		}
	}
}

void World::RenderGrid(const SDLAppRenderer& renderer) const
{
	// Set grid colour
	renderer.SetDrawColour(255, 255, 255, 255);

	// Draw row lines
	for (int i = 0; i <= m_worldHeight; i++)
	{
		Vector2 start(0, 1.0f * i);
		Vector2 end(1.0f * m_worldWidth, 1.0f * i);

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}

	// Draw column lines
	for (int i = 0; i <= m_worldWidth; i++)
	{
		Vector2 start(1.0f * i, 0);
		Vector2 end(1.0f * i, 1.0f * m_worldHeight);

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}
}

void World::GenerateFood()
{
	std::vector<Cell*> pFreeCells;
	pFreeCells.reserve(m_cells.Size());

	// Get all the cells that are not occupied
	for (int y = 0; y < m_cells.Height(); y++)
	{
		for (int x = 0; x < m_cells.Width(); x++)
		{
			Cell& curCell = m_cells.Get(x, y);
			if (curCell.free)
			{
				pFreeCells.push_back(&curCell);
			}
		}
	}
	pFreeCells.shrink_to_fit();

	assert(!pFreeCells.empty() && "No free cells available");

	// Select a random free cell 
	size_t index = Random::GetInt(0, pFreeCells.size() - 1);

	// Place the food at that cell
	m_pFoodLocation = pFreeCells[index];
}