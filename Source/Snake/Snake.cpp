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

#define HEAD_INDEX 0
#define NECK_INDEX 1

// Calculates the angle to rotate the corner graphic for a corner segment
static float CalculateCornerSpriteRotation(const Vector2& fromParent, const Vector2& fromChild);

// Returns the angle (in degrees) between a world-space vector and the world +x axis
float WorldVecToAngle(const Vector2& v);

Snake::Snake(World& world, int worldWidth, int worldHeight)
	: m_graphics(worldWidth * worldHeight)
	, m_pDir(&SnakeGame::EAST)
	, m_numSegments(1)
	, m_growCounter(0)
{
	// Allocate segments
	m_segments.resize(worldWidth * worldHeight);

	// Center snake in world
	GetHead().position = Vector2(static_cast<float>(worldWidth / 2), static_cast<float>(worldHeight / 2));
	printf("Snake starting pos: (%f, %f)\n", GetHead().position.x, GetHead().position.y);

	// Snake starts with 3 segments (head, body, and tail)
	EatFood(2);
	Grow();
	Grow();

	RecordOccupiedCells(world);
}

void Snake::Update(SnakeBrain& brain, World& world)
{
	// Update behaviour
	brain.Update(this);

	RecordOccupiedCells(world);
}

void Snake::Render(const SDLAppRenderer& renderer) const
{
	m_graphics.Render(renderer, *this);
}

void Snake::Simulate(const Vector2* pInputDir)
{
	// Does the snake need to grow?
	if (m_growCounter > 0)
	{
		Grow();
	}

	const Vector2* pPrevSnakeDir = 0;
	Move(pInputDir, pPrevSnakeDir);

	// Update snake graphics
	m_graphics.SetSegmentGraphic(SEGMENT_HEAD, WorldVecToAngle(GetDirection()), HEAD_INDEX); // Head
	m_graphics.Update(m_numSegments); // Body

	int tailIndex = m_numSegments - 1;
	m_graphics.SetSegmentGraphic(SEGMENT_TAIL, 
		WorldVecToAngle(m_segments[tailIndex - 1].position - m_segments[tailIndex].position), // Get direction to parent segment
		tailIndex);

	bool turnMade = (pPrevSnakeDir != nullptr) && (pPrevSnakeDir != m_pDir);
	if (turnMade)
	{
		// Override neck segment graphic with a turn graphic.
		// The dir from the parent segment (the head in this case) would be in the opposite direction to where it is going.
		m_graphics.SetTurnGraphic(-1.0f * GetDirection(), *pPrevSnakeDir);
	}
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

Segment& Snake::GetHead()
{
	assert(m_numSegments > 0 && m_segments.size() > 0);

	return m_segments[HEAD_INDEX];
}

SnakeGraphics::SnakeGraphics(int maxSegments)
{
	m_segmentGraphics.resize(maxSegments);

	// Load all snake sprites
	Graphics::LoadSprite(m_pCorner, Assets::SNAKE_CORNER_TEXTURE_PATH);
	Graphics::LoadSprite(m_pHead, Assets::SNAKE_HEAD_TEXTURE_PATH);
	Graphics::LoadSprite(m_pBody, Assets::SNAKE_BODY_TEXTURE_PATH);
	Graphics::LoadSprite(m_pTail, Assets::SNAKE_TAIL_TEXTURE_PATH);
}

Sprite* SnakeGraphics::GetSprite(SegmentType type) const
{
	switch (type)
	{
		case SEGMENT_HEAD: return m_pHead.get();
		case SEGMENT_TAIL: return m_pTail.get();
		case SEGMENT_BODY: return m_pBody.get();
		case SEGMENT_TURN: return m_pCorner.get();

		default:
			assert(0); // Should never get here!
	}
}

void SnakeGraphics::SetSegmentGraphic(SegmentType type, float angle, int index)
{
	m_segmentGraphics[index].type = type;
	m_segmentGraphics[index].angle = angle;
}

void SnakeGraphics::SetTurnGraphic(const Vector2& fromParent, const Vector2& fromChild)
{
	// Calculate correct orientation and set
	SetSegmentGraphic(SEGMENT_TURN, CalculateCornerSpriteRotation(fromParent, fromChild), NECK_INDEX);
}

void SnakeGraphics::Update(int numSegments)
{
	// Iterate in reverse between the tail and the neck
	for (int i = numSegments - 2; i > NECK_INDEX; i--)
	{
		// Take on the graphic of their parent segment
		m_segmentGraphics[i] = m_segmentGraphics[i - 1];
	}

	// Manually set neck segment graphic to the body sprite by default.
	SetSegmentGraphic(SEGMENT_BODY, m_segmentGraphics[HEAD_INDEX].angle, NECK_INDEX);
}

void SnakeGraphics::Render(const SDLAppRenderer& renderer, const Snake& snake) const
{
	const size_t numSegments = snake.GetNumSegments();
	const auto& segments = snake.GetSegments();

	for (size_t i = 0; i < numSegments; i++)
	{
		const Sprite* pSprite = GetSprite(m_segmentGraphics[i].type);
		auto destRect = renderer.WorldToScreen(segments[i].position.x, segments[i].position.y, 1, 1);
		pSprite->Draw(renderer, destRect, m_segmentGraphics[i].angle);
	}
}

float WorldVecToAngle(const Vector2& v)
{
	// Flip the y-coord to work with trig coord system
	return Math::ToDegrees(atan2f(-v.y, v.x));
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