#include "Snake.h"
#include "SnakeBrain.h"
#include "SnakeGame.h"
#include "World.h"
#include "../Engine/Math/Math.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/SDLAppRenderer.h"
#include "../Engine/Graphics.h"

#include <SDL/SDL.h>
#include <cstdio>
#include <cassert>
#include <string>

static Vector2 CalcSnakeStartPos(int worldWidth, int worldHeight)
{
	// Places the snake's head at the centre of the world
	return Vector2(static_cast<float>(worldWidth / 2), static_cast<float>(worldHeight / 2));
}

Snake::Snake(World& world, int worldWidth, int worldHeight)
	: m_graphics(worldWidth * worldHeight)
	, m_world(world)
	, m_startPos(CalcSnakeStartPos(worldWidth, worldHeight))
{
	// Allocate segments
	m_segments.resize(worldWidth * worldHeight);

	printf("Snake starting pos is at: (%.1f, %.1f)\n", m_startPos.x, m_startPos.y);

	Init();
}

void Snake::Init()
{
	m_dead = false;
	m_numSegments = 1;
	m_growCounter = 0;

	m_pDir = &SnakeGame::EAST;
	GetHead().position = m_startPos;

	EatFood(2);
	Grow();
	Grow();

	MarkOccupiedCells();

	m_graphics.Init(*this);
}

void Snake::Reset()
{
	Init();
}

void Snake::Update(SnakeBrain& brain)
{
	// Update behaviour
	brain.Update(this);
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	m_graphics.Render(renderer, *this);
}

void Snake::Simulate(const Vector2* pInputDir)
{
	// Grow the snake if needed
	if (m_growCounter > 0)
	{
		Grow();
	}

	const Vector2* pPrevSnakeDir = 0;
	Move(pInputDir, pPrevSnakeDir);

	// Exit early if snake died this frame.
	CheckForDeath();
	if(m_dead) return;

	// Determine whether the snake changed direction (turned) this frame
	bool turnMade = (pPrevSnakeDir != nullptr) && (pPrevSnakeDir != m_pDir);
	SnakeTurnData turnData;

	if (turnMade)
	{
		// The dir from the parent segment (the head in this case) 
		// would be in the opposite direction to where it is going.
		turnData.fromParent = -1.0f * GetDirection();
		turnData.fromChild = *pPrevSnakeDir;
	}

	// Update snake graphics
	m_graphics.Update(*this, turnMade ? &turnData : nullptr);
}

void Snake::EatFood(int growthValue)
{
	m_growCounter += growthValue;
	printf("Food consumed\n");
}

void Snake::Move(const Vector2* pInputDir, const Vector2*& pPrevDir)
{
	assert(m_pDir && "Snake direction has not been set!");

	// Move the body first
	for (size_t i = m_numSegments - 1; i > 0; i--)
	{
		// Each segment moves to where its parent is
		m_segments[i].position = m_segments[i - 1].position;
	}

	// Update snake direction if an input direction was sent this frame
	if (pInputDir)
	{
		if (pInputDir != m_pDir)
		{
			//printf("Snake changed dir!\n");
			pPrevDir = m_pDir;
		}
		m_pDir = pInputDir;
	}
	Segment& head = GetHead();
	head.position += *m_pDir;

	//printf("Moving snake, head pos is (%f, %f)\n", head.position.x, head.position.y);
}

void Snake::Grow()
{
	assert(m_growCounter > 0);
	assert(m_numSegments > 0 && m_numSegments < m_segments.size());

	const size_t lastSegmentIndex = m_numSegments - 1;
	const Vector2& lastSegmentPos = m_segments[lastSegmentIndex].position;

	// Calculate the direction that the last segment is facing
	Vector2 segmentDir;

	// Last segment is the head
	if (m_numSegments == 1)
	{
		segmentDir = GetDirection();
	}
	else
	{
		// Get the position of the segment that comes before it (its parent)
		const Vector2& parentPos = m_segments[lastSegmentIndex - 1].position;

		// A segment faces towards its parent
		segmentDir = parentPos - lastSegmentPos;
	}
	// Position the new segment behind where the last segment is facing
	m_segments[m_numSegments++].position = lastSegmentPos + (-1.0f * segmentDir);

	m_growCounter--;
	if (m_growCounter == 0)
	{
		printf("Snake length is now: %d\n", m_numSegments);
	}
}

void Snake::MarkOccupiedCells()
{
	for (size_t i = 0; i < m_numSegments; i++)
	{
		m_world.OccupyCell(
			static_cast<int>(m_segments[i].position.x),
			static_cast<int>(m_segments[i].position.y)
		);
	}
}

void Snake::CheckForDeath()
{
	// The two conditions that trigger a game over:
	// 1. Snake head touched world bounds
	// 2. Snake head collided with body
	
	const int headX = static_cast<int>(GetHead().position.x);
	const int headY = static_cast<int>(GetHead().position.y);

	// Collision with world boundary
	if (!m_world.InBounds(headX, headY))
	{
		m_dead = true;
		return;
	}

	// Test self-collision
	m_world.OccupyCell(headX, headY);

	// Iterate through each segment after the head, checking if the head shares the same cell
	for (size_t i = 1; i < m_numSegments; i++)
	{
		const int segX = static_cast<int>(m_segments[i].position.x);
		const int segY = static_cast<int>(m_segments[i].position.y);

		// Head is already here!
		if (!m_world.IsFree(segX, segY))
		{
			m_dead = true;
			return;
		}

		// NOTE: Set each cell-empty flag here instead of a separate call to MarkOccupiedCells() later
		m_world.OccupyCell(segX, segY);
	}
}

Segment& Snake::GetHead()
{
	assert(m_numSegments > 0 && m_segments.size() > 0);

	return m_segments[HEAD_INDEX];
}