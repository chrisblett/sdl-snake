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

// Calculates the angle to rotate the corner graphic for a corner segment
static float CalculateCornerSpriteRotation(const Vector2& fromParent, const Vector2& fromChild);

Snake::Snake(World& world, int worldWidth, int worldHeight)
	: m_pDir(&SnakeGame::EAST)
	, m_numSegments(1)
	, m_growCounter(0)
{
	// Load snake graphics
	Graphics::LoadSprite(m_pCorner, Assets::SNAKE_CORNER_TEXTURE_PATH);
	Graphics::LoadSprite(m_pHead, Assets::SNAKE_HEAD_TEXTURE_PATH);
	Graphics::LoadSprite(m_pBody, Assets::SNAKE_BODY_TEXTURE_PATH);
	Graphics::LoadSprite(m_pTail, Assets::SNAKE_TAIL_TEXTURE_PATH);

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
	/*
	for (size_t i = 0; i < m_numSegments; i++)
	{
		::RenderSegment(m_segments[i].position, renderer);
	}
	*/

	// Experimental code

	for (size_t i = 0; i < m_numSegments; i++)
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
	const Sprite* pSprite = nullptr;

	// Vector to rotate the sprite towards
	Vector2 rotateTo;

	float angle = 0.0f;
	bool isCorner = false;

	/*
	// Segment is the head
	if (!pParent)
	{
		Vector2 dir = GetDirection();

		pSprite = m_pHead.get();

		// Convert vector to angle
		angle = Math::ToDegrees(atan2f(-dir.y, dir.x)); // Flip the y to work with trig coordinate system
	}
	else if (!pChild) // Segment is the tail
	{
		Vector2 toParent = pParent->position - seg.position;
		pSprite = m_pTail.get();

		// Convert vector to angle
		angle = Math::ToDegrees(atan2f(-toParent.y, toParent.x));
	}
	else // Not the head or tail so must be either a body or corner
	{
		// Both body segments and corners must have a parent and child
		assert(pParent && pChild);

		// Calculate vectors from both parent and child segments towards this segment
		Vector2 fromParent = seg.position - pParent->position;
		Vector2 fromChild  = seg.position - pChild->position;
		
		// Flip vector to get the direction this segment is moving in
		Vector2 toParent = fromParent * -1.0f;

		// Segment is straight if both vectors are the same
		if (fromChild == toParent)
		{
			// Convert vector to angle
			angle = Math::ToDegrees(atan2f(-toParent.y, toParent.x));
		}
		else // Must be corner
		{
			// Change to corner graphic
			pSprite = m_pCorner.get();
			angle = CalculateCornerSpriteRotation(fromParent, fromChild);
		}
	}
	*/

	// Determine the type of segment (head, tail, body, corner)
	if (!pParent)
	{
		pSprite = m_pHead.get();		
		rotateTo = GetDirection();
	}
	else
	{
		// Any segment that is not the head has a parent
		assert(pParent);

		Vector2 toParent = pParent->position - seg.position;
		rotateTo = toParent;

		if (!pChild)
		{
			pSprite = m_pTail.get();
		}
		else // Not the head or tail, so must be either a body or corner
		{
			Vector2 fromChild  = seg.position - pChild->position;

			// Straight segment
			if (fromChild == toParent)
			{
				pSprite = m_pBody.get();
			}
			else
			{
				isCorner = true;
				pSprite = m_pCorner.get();
				angle = CalculateCornerSpriteRotation(-1.0f * toParent, fromChild);
			}
		}
	}

	// Calculate sprite rotation for non-corner segments
	if (!isCorner)
	{
		angle = Math::ToDegrees(atan2f(-rotateTo.y, rotateTo.x)); // Flip the y-coord to work with trig coord system
	}

	// Create the destination rect for the sprite on the screen
	auto destRect = renderer.WorldToScreen(seg.position.x, seg.position.y, 1, 1);
	pSprite->Draw(renderer, destRect, angle);
}

float CalculateCornerSpriteRotation(const Vector2& fromParent, const Vector2& fromChild)
{
	// The corner graphic image can be thought of as a single quadrant of a square (or circle depending on it's smoothness).
	// By rotating this one graphic through each quadrant of the unit-circle, all four potential orientations can be drawn.
	// This code assumes that the corner graphic with no rotation appears to lie in Quadrant I (top-right corner).
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
	return 0.0f;
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
	m_segments[m_numSegments].position = lastSegmentPos + (-1.0f * segmentDir);
	m_numSegments++;

	m_growCounter--;
	if (m_growCounter == 0)
	{
		printf("Snake length is now: %d\n", m_numSegments);
	}
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