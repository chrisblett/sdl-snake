#include "World.h"
#include "../Engine/Math/Random.h"
#include "../Engine/SDLAppRenderer.h"
#include "../Engine/Math/Math.h"

#include <memory>

const int FOOD_VALUE = 5;

bool g_won = false;

World::World(int width, int height)
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

	m_pSnake = std::make_unique<Snake>(*this, m_worldWidth, m_worldHeight);

	GenerateFood();
}

void World::Update(SnakeBrain& brain)
{
	assert(m_pFoodLocation);

	// Check for game over, for now just assert.
	if (g_won)
	{
		assert(0 && "You win!");
	}

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

	m_pSnake->Update(brain, *this);

	// Has the food been eaten?
	if (m_pSnake->GetHeadPosition() == m_pFoodLocation->position)
	{
		m_pSnake->EatFood(FOOD_VALUE);
		GenerateFood();
	}
}

void World::OccupyCell(int x, int y)
{
	//TODO: Should change this to an assert in the final game
	if (!InBounds(x, y)) return;

	m_cells.Get(x, y).free = false;
}

bool World::InBounds(int x, int y) const
{
	return (x >= 0 && x < m_worldWidth) &&
		(y >= 0 && y < m_worldHeight);
}

void World::DrawRectAtCell(const SDLAppRenderer& renderer, const Vector2& cellPos, float rectScale)
{
	// Calculate rect top-left pos relative to cell top-left
	Vector2 centeredPos = Math::GetCenteredPosition(cellPos, rectScale, rectScale);

	// Want to draw around the center of the cell so add half a unit
	// in each axis so we are centered around its midpoint
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		rectScale,
		rectScale)
	);
}

void World::Render(const SDLAppRenderer& renderer) const
{
	// Draw the world
#define GROUND_COLOUR 159, 122, 86, 255
	renderer.SetDrawColour(GROUND_COLOUR);
	renderer.FillRect(renderer.WorldToScreen(0, 0, m_worldWidth, m_worldHeight));

	// Draw food
#define FOOD_COLOUR 128, 0, 0, 255
	renderer.SetDrawColour(FOOD_COLOUR);
	DrawRectAtCell(renderer, m_pFoodLocation->position, .55f);
	
	// Debug drawing
	//RenderCellInfo(renderer);
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

	if (pFreeCells.empty())
	{
		g_won = true;
		return;
	}

	assert(!pFreeCells.empty() && "No free cells available");

	// Select a random free cell 
	size_t index = Random::GetInt(0, pFreeCells.size() - 1);

	// Place food at chosen cell
	m_pFoodLocation = pFreeCells[index];
	m_pFoodLocation->free = false;
}