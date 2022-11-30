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

	// Grow the snake a bit
	Grow();
	Grow();
	Grow();
}

void Snake::Update(const Vector2& inputDir, float deltaTime)
{
	/*
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
	*/
}

static void RenderSegment(const Vector2& segmentPos, const SDLAppRenderer& renderer)
{
	// R, G, B, A
	#define OUTER_COLOUR 0, 103, 65, 255
	#define INNER_COLOUR 0, 146, 64, 255

	// Draw outer
	renderer.SetDrawColour(OUTER_COLOUR);
	renderer.FillRect(renderer.WorldToScreen(segmentPos.x, segmentPos.y, 1, 1));

	// Draw inner
	const float innerScale = .8f;
	Vector2 centeredPos = Math::GetCenteredPosition(segmentPos, innerScale, innerScale);

	renderer.SetDrawColour(INNER_COLOUR);

	// Move from top-left to midpoint of the cell and draw
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		innerScale,
		innerScale)
	);
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	for (size_t i = 0; i < m_numSegments; i++)
	{
		RenderSegment(m_segments[i].position, renderer);
	}
}

void Snake::Grow()
{
	assert(m_numSegments > 0);
	
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
	m_segments[m_numSegments].position = lastSegmentPos + (-1.0f * segmentDir);
	m_numSegments++;

	printf("Snake length is now: %d\n", m_numSegments);
}

Snake::Segment& Snake::GetHead()
{
	assert(m_numSegments > 0 && m_segments.size() > 0);

	return m_segments[0];
}