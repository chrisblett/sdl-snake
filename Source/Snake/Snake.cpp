#include "../Snake/Snake.h"
#include "../Engine/Math/Math.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/SDLAppRenderer.h"

#include <cstdio>
#include <cassert>

// How many cells it covers per second
const int SNAKE_SPEED = 5;

// Delay between moves in seconds
const float SNAKE_DELAY = 1.0f / SNAKE_SPEED;

Snake::Snake(const Vector2& dir, int worldWidth, int worldHeight)
	: m_pDir(&dir)
	, m_timeSinceLastMove(0.0f)
{
	// Allocate segments
	m_segments.resize(worldWidth * worldHeight);

	// Create head
	m_numSegments = 1;
	// Center snake in world
	GetHead().position = Vector2(worldWidth / 2.0f, worldHeight / 2.0f);

	printf("SnakePos: (%f, %f)\n", GetHead().position.x, GetHead().position.y);
}

void Snake::Update(const Vector2& inputDir, float deltaTime)
{
	// Update move timer
	m_timeSinceLastMove = m_timeSinceLastMove - deltaTime;

	// Can we move yet?
	if (m_timeSinceLastMove <= 0.0f)
	{
		m_timeSinceLastMove += SNAKE_DELAY;

		Segment& head = GetHead();

		head.position += inputDir;
		m_pDir = &inputDir;

		printf("Moving snake (%f, %f)\n", head.position.x, head.position.y);
	}
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	// R, G, B, A
	#define OUTER_COLOUR 0, 103, 65, 255
	#define INNER_COLOUR 0, 146, 64, 255

	const Vector2& headPos = GetHeadPosition();

	// Draw outer
	renderer.SetDrawColour(OUTER_COLOUR);
	renderer.FillRect(renderer.WorldToScreen(headPos.x, headPos.y, 1, 1));

	// Draw inner
	const float innerScale = .8f;
	Vector2 centeredPos = Math::GetCenteredPosition(headPos, innerScale, innerScale);
	
	renderer.SetDrawColour(INNER_COLOUR);
	
	// Move from top-left to midpoint of the cell and draw
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		innerScale,
		innerScale)
	);
}

Snake::Segment& Snake::GetHead()
{
	assert(m_numSegments > 0 && m_segments.size() > 0);

	return m_segments[0];
}