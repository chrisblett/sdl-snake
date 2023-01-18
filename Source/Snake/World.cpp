#include "World.h"
#include "SnakeGame.h"
#include "../Engine/Graphics.h"
#include "../Engine/Math/Math.h"
#include "../Engine/Math/Random.h"
#include "../Engine/SDLAppRenderer.h"
#include "../Engine/Util.h"

#include <memory>

constexpr int FOOD_VALUE = 5;

World::World(int width, int height)
	: m_cells(width, height)
	, m_pFoodLocation(nullptr)
	, m_worldWidth(width)
	, m_worldHeight(height)
	, m_noFoodLeft(false)
{
	// Load graphics
	Graphics::LoadSprite(m_pFood, Assets::SNAKE_FOOD_TEXTURE_PATH);

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

World::~World()
{ 
	Util::DebugPrint("World destroyed\n");
}

void World::Reset()
{
	ClearAll();
	m_pSnake->Reset();
	GenerateFood();
}

SnakeStatus World::Update(SnakeBrain& brain)
{
	assert(m_pFoodLocation);

	// Check if the player has eaten all food
	if (m_noFoodLeft)
	{
		return STATUS_DONE;
	}

	// Clear all cells, however don't clear the cell holding the food
	ClearAll();
	m_pFoodLocation->free = false;

	m_pSnake->Update(brain);

	// See if snake died this update
	if (m_pSnake->IsDead())
	{
		return STATUS_DEAD;
	}
	
	// Check if food was eaten
	if (m_pSnake->GetHeadPosition() == m_pFoodLocation->position)
	{
		m_pSnake->EatFood(FOOD_VALUE);
		Util::DebugPrint("Snake consumed food at (%.1f, %.1f)\n",
			m_pFoodLocation->position.x, m_pFoodLocation->position.y);

		GenerateFood();
	}

	return STATUS_ACTIVE;
}

void World::Render(const SDLAppRenderer& renderer) const
{
	// Draw the world
#define GROUND_COLOUR 159, 122, 86, 255
	renderer.SetDrawColour(GROUND_COLOUR);

	renderer.FillRect(
		renderer.WorldToScreen(0, 0, static_cast<float>(m_worldWidth), static_cast<float>(m_worldHeight)));

	// Draw food
	m_pFood->Draw(
		renderer,
		renderer.WorldToScreen(m_pFoodLocation->position.x, m_pFoodLocation->position.y, 1, 1),
		0.0f);

	m_pSnake->Render(renderer);
}

void World::OccupyCell(int x, int y)
{
	assert(InBounds(x, y));

	m_cells.Get(x, y).free = false;
}

const Cell& World::GetCell(int x, int y) const
{
	assert(InBounds(x, y));

	return m_cells.Get(x, y);
}

bool World::InBounds(int x, int y) const
{
	return (x >= 0 && x < m_worldWidth) &&
		(y >= 0 && y < m_worldHeight);
}

bool World::IsFree(int x, int y) const
{
	assert(InBounds(x, y));

	return m_cells.Get(x, y).free;
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

	// If this fails, there's a good chance we forgot to mark the snake's 
	// occupied cells or it is out-of-date.
	assert(pFreeCells.size() == (m_cells.Size() - m_pSnake->GetLength()));

	// No more food can be generated
	m_noFoodLeft = pFreeCells.empty();
	if (m_noFoodLeft) return;

	assert(!pFreeCells.empty());

	// Select a random free cell 
	size_t index = Random::GetInt( 0, static_cast<int>( pFreeCells.size() ) - 1 );

	// Place food at chosen cell
	m_pFoodLocation = pFreeCells[index];
	m_pFoodLocation->free = false;
}

void World::ClearAll()
{
	for (int y = 0; y < m_cells.Height(); y++)
	{
		for (int x = 0; x < m_cells.Width(); x++)
		{
			m_cells.Get(x, y).free = true;
		}
	}
}

void WorldDebugDraw::RenderCellFreeStatus(const World& world, const SDLAppRenderer& renderer)
{
	for (int y = 0; y < world.GetHeight(); y++)
	{
		for (int x = 0; x < world.GetWidth(); x++)
		{
			const Cell& cell = world.GetCell(x, y);
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

void WorldDebugDraw::RenderGrid(const World& world, const SDLAppRenderer& renderer)
{
	// Set grid colour
	renderer.SetDrawColour(255, 255, 255, 255);

	// Draw row lines
	for (int i = 0; i <= world.GetHeight(); i++)
	{
		Vector2 start(0, 1.0f * i);
		Vector2 end(1.0f * world.GetWidth(), 1.0f * i);

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}

	// Draw column lines
	for (int i = 0; i <= world.GetWidth(); i++)
	{
		Vector2 start(1.0f * i, 0);
		Vector2 end(1.0f * i, 1.0f * world.GetHeight());

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}
}