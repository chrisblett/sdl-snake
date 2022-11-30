#include "../Snake/Snake.h"
#include "../Engine/Math/Math.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/SDLAppRenderer.h"

#include <cstdio>

// How many cells it covers per second
const int SNAKE_SPEED = 5;

// Delay between moves in seconds
const float SNAKE_DELAY = 1.0f / SNAKE_SPEED;

Snake::Snake(const Vector2& dir, int worldWidth, int worldHeight)
	: m_pDir(&dir)
	, m_timeSinceLastMove(0.0f)
{
	// Center snake in world
	m_pos.x = static_cast<float>(worldWidth / 2);
	m_pos.y = static_cast<float>(worldHeight / 2);

	printf("SnakePos: (%f, %f)\n", m_pos.x, m_pos.y);
}

void Snake::Update(const Vector2& inputDir, float deltaTime)
{
	// Update move timer
	m_timeSinceLastMove = m_timeSinceLastMove - deltaTime;

	// Can we move yet?
	if (m_timeSinceLastMove <= 0.0f)
	{
		m_timeSinceLastMove += SNAKE_DELAY;

		m_pos += inputDir;
		m_pDir = &inputDir;

		printf("Moving snake (%f, %f)\n", m_pos.x, m_pos.y);
	}
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	// R, G, B, A
	#define OUTER_COLOUR 0, 103, 65, 255
	#define INNER_COLOUR 0, 146, 64, 255

	// Draw outer
	renderer.SetDrawColour(OUTER_COLOUR);
	renderer.FillRect(renderer.WorldToScreen(m_pos.x, m_pos.y, 1, 1));

	// Draw inner
	const float innerScale = .8f;
	Vector2 centeredPos = Math::GetCenteredPosition(m_pos, innerScale, innerScale);
	
	renderer.SetDrawColour(INNER_COLOUR);
	
	// Move from top-left to midpoint of the cell and draw
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		innerScale,
		innerScale)
	);
}