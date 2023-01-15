#include "SnakeGraphics.h"
#include "SnakeGame.h"
#include "Snake.h"
#include "World.h"

namespace
{
	constexpr size_t HEAD_INDEX = 0;
	constexpr size_t NECK_INDEX = 1;
}

// Calculates the angle to rotate the graphic for a turn segment
static float CalculateTurnSpriteRotation(const Vector2& fromParent, const Vector2& fromChild)
{
	// The turn sprite can be thought of as a single quadrant of a square (or circle depending on it's smoothness).
	// By rotating this one graphic through each quadrant of the unit-circle, all four potential orientations can be drawn.
	// This code assumes that the turn texture with no rotation appears to lie in Quadrant I (top-right corner).
	if ((fromParent == SnakeGame::NORTH) && (fromChild == SnakeGame::EAST)
		|| (fromParent == SnakeGame::EAST) && (fromChild == SnakeGame::NORTH))
	{
		return 0.f;
	}

	if ((fromParent == SnakeGame::WEST) && (fromChild == SnakeGame::NORTH)
		|| (fromParent == SnakeGame::NORTH) && (fromChild == SnakeGame::WEST))
	{
		return 90.f;
	}

	if ((fromParent == SnakeGame::SOUTH) && (fromChild == SnakeGame::WEST)
		|| (fromParent == SnakeGame::WEST) && (fromChild == SnakeGame::SOUTH))
	{
		return 180.f;
	}

	if ((fromParent == SnakeGame::EAST) && (fromChild == SnakeGame::SOUTH)
		|| (fromParent == SnakeGame::SOUTH) && (fromChild == SnakeGame::EAST))
	{
		return 270.f;
	}

	// The segment shouldn't be drawn as a turn.
	assert(0);
	return 0.0f;
}

SnakeGraphics::SnakeGraphics(int maxSegments)
{
	m_segmentGraphics.resize(maxSegments);

	// Load all snake sprites
	Graphics::LoadSprite(m_pTurn, Assets::SNAKE_TURN_TEXTURE_PATH);
	Graphics::LoadSprite(m_pHead, Assets::SNAKE_HEAD_TEXTURE_PATH);
	Graphics::LoadSprite(m_pBody, Assets::SNAKE_BODY_TEXTURE_PATH);
	Graphics::LoadSprite(m_pTail, Assets::SNAKE_TAIL_TEXTURE_PATH);
}

void SnakeGraphics::Init(const Snake& snake)
{
	const float snakeAngleWorld = World::WorldVecToAngle(snake.GetDirection());

	SetSegmentGraphic(SEGMENT_HEAD, snakeAngleWorld, HEAD_INDEX);
	SetSegmentGraphic(SEGMENT_BODY, m_segmentGraphics[HEAD_INDEX].angle, NECK_INDEX);
	SetSegmentGraphic(SEGMENT_TAIL, snakeAngleWorld, 2);
}

Sprite* SnakeGraphics::GetSprite(SegmentType type) const
{
	switch (type)
	{
	case SEGMENT_HEAD: return m_pHead.get();
	case SEGMENT_TAIL: return m_pTail.get();
	case SEGMENT_BODY: return m_pBody.get();
	case SEGMENT_TURN: return m_pTurn.get();

	default:
		assert(0); // Should never get here!
	}

	return nullptr;
}

void SnakeGraphics::SetSegmentGraphic(SegmentType type, float angle, int index)
{
	m_segmentGraphics[index].type = type;
	m_segmentGraphics[index].angle = angle;
}

void SnakeGraphics::SetTurnGraphic(const Vector2& fromParent, const Vector2& fromChild)
{
	// Calculate correct orientation and set
	SetSegmentGraphic(SEGMENT_TURN, CalculateTurnSpriteRotation(fromParent, fromChild), NECK_INDEX);
}

void SnakeGraphics::Update(const Snake& snake)
{
	SetSegmentGraphic(SEGMENT_HEAD, World::WorldVecToAngle(snake.GetDirection()), HEAD_INDEX); // Head

	// Iterate in reverse between the tail and the neck
	for (size_t i = snake.GetLength() - 2; i > NECK_INDEX; i--)
	{
		// Take on the graphic of their parent segment
		m_segmentGraphics[i] = m_segmentGraphics[i - 1];
	}

	// Manually set neck segment graphic to the body sprite by default.
	SetSegmentGraphic(SEGMENT_BODY, m_segmentGraphics[HEAD_INDEX].angle, NECK_INDEX);

	const auto& snakeSegments = snake.GetSegments();
	int tailIndex = snake.GetLength() - 1;

	SetSegmentGraphic(
		SEGMENT_TAIL,
		World::WorldVecToAngle(snakeSegments[tailIndex - 1].position - snakeSegments[tailIndex].position), // Get direction to parent segment
		tailIndex);
}

void SnakeGraphics::Render(const SDLAppRenderer& renderer, const Snake& snake) const
{
	const size_t numSegments = snake.GetLength();
	const auto& segments = snake.GetSegments();

	for (size_t i = 0; i < numSegments; i++)
	{
		const Sprite* pSprite = GetSprite(m_segmentGraphics[i].type);
		auto destRect = renderer.WorldToScreen(segments[i].position.x, segments[i].position.y, 1, 1);
		pSprite->Draw(renderer, destRect, m_segmentGraphics[i].angle);
	}
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