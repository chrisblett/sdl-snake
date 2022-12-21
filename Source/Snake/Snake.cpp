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

using std::unique_ptr;

// Calculates the angle to rotate the corner graphic for a corner segment
static float CalculateCornerSpriteRotation(const Vector2& fromParent, const Vector2& fromChild);

Snake::Snake(World& world, int worldWidth, int worldHeight)
	: m_pDir(&SnakeGame::EAST)
	, m_numSegments(1)
	, m_growCounter(0)
{
	// Create snake sprites
	m_pCorner = unique_ptr<Sprite>(Graphics::CreateSprite(Assets::SNAKE_CORNER_TEXTURE_PATH));
	assert(m_pCorner);

	m_pBody = unique_ptr<Sprite>(Graphics::CreateSprite(Assets::SNAKE_BODY_TEXTURE_PATH));
	assert(m_pBody);

	// Allocate segments
	m_segments.resize(worldWidth * worldHeight);

	// Center snake in world
	GetHead().position = Vector2(static_cast<float>(worldWidth / 2), static_cast<float>(worldHeight / 2));
	printf("Snake starting pos: (%f, %f)\n", GetHead().position.x, GetHead().position.y);

	RecordOccupiedCells(world);
}

void Snake::Update(SnakeBrain& brain, World& world)
{
	// Update behaviour
	brain.Update(this);

	RecordOccupiedCells(world);
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
	renderer.SetDrawColour(INNER_COLOUR);
	const float innerScale = .8f;

	World::DrawRectAtCell(renderer, segmentPos, .8f);
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	for (size_t i = 0; i < m_numSegments; i++)
	{
		::RenderSegment(m_segments[i].position, renderer);
	}

	// Experimental code

	for (size_t i = 1; i < m_numSegments; i++)
	{
		// If it is not the first segment
		bool hasParent = i != 0;

		// If it is not the last segment
		bool hasChild = i != m_numSegments - 1;

		const Segment* pParent = hasParent ? &m_segments[i - 1] : nullptr;
		const Segment* pChild  = hasChild  ? &m_segments[i + 1] : nullptr;

		RenderSegment(renderer, m_segments[i], pParent, pChild);
	}
}

void Snake::RenderSegment(const SDLAppRenderer& renderer, const Segment& seg,
	const Segment* pParent, const Segment* pChild) const
{
	// Create dest rect
	auto destRect = renderer.WorldToScreen(seg.position.x, seg.position.y, 1, 1);
	Sprite* pSprite = nullptr;

	// Segment is the head
	if (!pParent)
	{
		return;
	}

	// Segment is the tail
	if (!pChild)
	{
		return;
	}

	// Determine whether the segment is a regular body or corner

	// Calculate vectors from both parent and child segments towards this segment
	Vector2 fromParent = seg.position - pParent->position;
	Vector2 fromChild  = seg.position - pChild->position;
	{
		// Flip vector to get the direction this segment is moving in
		Vector2 toParent = fromParent * -1.0f;

		// Segment graphic is straight if both vectors are the same
		if (fromChild == toParent)
		{
			pSprite = m_pBody.get();

			// Convert vector to angle
			float rotRadians = atan2f(toParent.y, toParent.x);

			pSprite->Draw(renderer, destRect, Math::ToDegrees(rotRadians));

			return;
		}
	}
	
	// Segment is a corner
	pSprite = m_pCorner.get();

	float angle = CalculateCornerSpriteRotation(fromParent, fromChild);

	pSprite->Draw(renderer, destRect, angle);
}

float CalculateCornerSpriteRotation(const Vector2& fromParent, const Vector2& fromChild)
{
	// The corner graphic can be thought of as a quadrant of a square (or circle depending on it's smoothness).
	// By rotating the graphic through each quadrant of the unit-circle, all four potential orientations can be drawn.
	// This code assumes that the corner graphic with no rotation applied lies in Quadrant I.
	if ((fromParent == SnakeGame::NORTH) && (fromChild == SnakeGame::EAST) 
		|| (fromParent == SnakeGame::EAST) && (fromChild == SnakeGame::NORTH))
	{
		return 0.f;
	}

	if ((fromParent == SnakeGame::WEST)  && (fromChild == SnakeGame::NORTH) 
		|| (fromParent == SnakeGame::NORTH) && (fromChild == SnakeGame::WEST))
	{
		return 90.f;
	}

	if ((fromParent == SnakeGame::SOUTH) && (fromChild == SnakeGame::WEST) 
		|| (fromParent == SnakeGame::WEST) && (fromChild == SnakeGame::SOUTH))
	{
		return 180.f;
	}

	if ((fromParent == SnakeGame::EAST)  && (fromChild == SnakeGame::SOUTH) 
		|| (fromParent == SnakeGame::SOUTH) && (fromChild == SnakeGame::EAST))
	{
		return 270.f;
	}

	// The segment shouldn't be drawn as a corner.
	assert(0);
}

void Snake::Simulate(const Vector2* pInputDir)
{
	// Does the snake need to grow?
	if (m_growCounter > 0)
	{
		Grow();
	}
	Move(pInputDir);
}

void Snake::EatFood(int growthValue)
{
	m_growCounter += growthValue;
	printf("Food consumed\n");
}

void Snake::Move(const Vector2* pInputDir)
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
		m_pDir = pInputDir;
	}
	Segment& head = GetHead();
	head.position += *m_pDir;

	printf("Moving snake, head pos is (%f, %f)\n", head.position.x, head.position.y);
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
	m_segments[m_numSegments].position = lastSegmentPos + (-1.0f * segmentDir);
	m_numSegments++;

	printf("Snake length is now: %d\n", m_numSegments);

	m_growCounter--;
}

void Snake::RecordOccupiedCells(World& world)
{
	for (size_t i = 0; i < m_numSegments; i++)
	{
		world.OccupyCell(
			static_cast<int>(m_segments[i].position.x),
			static_cast<int>(m_segments[i].position.y)
		);
	}
}

Snake::Segment& Snake::GetHead()
{
	assert(m_numSegments > 0 && m_segments.size() > 0);

	return m_segments[0];
}