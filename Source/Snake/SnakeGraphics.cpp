#include "SnakeGraphics.h"
#include "SnakeGame.h"
#include "Snake.h"
#include "World.h"
#include "WorldUtil.h"

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
	// Should only be called after snake has been initialised
	assert(snake.GetLength() == 3);

	const float snakeAngleWorld = WorldUtil::WorldVecToAngle(snake.GetDirection());

	SetSegmentGraphic(SEGMENT_HEAD, snakeAngleWorld, Snake::HEAD_INDEX);
	SetSegmentGraphic(SEGMENT_BODY, m_segmentGraphics[Snake::HEAD_INDEX].angle, Snake::NECK_INDEX);
	SetSegmentGraphic(SEGMENT_TAIL, snakeAngleWorld, snake.GetLength() - 1);
}

void SnakeGraphics::Update(const Snake& snake, SnakeTurnData* pTurnData)
{
	SetSegmentGraphic(SEGMENT_HEAD, WorldUtil::WorldVecToAngle(snake.GetDirection()), Snake::HEAD_INDEX);

	// Iterate in reverse between the tail and the neck
	for (size_t i = snake.GetLength() - 2; i > Snake::NECK_INDEX; i--)
	{
		// Take on the graphic of their parent segment
		m_segmentGraphics[i] = m_segmentGraphics[i - 1];
	}

	// Snake turned this update?
	if (pTurnData)
	{
		// Use turn sprite for neck segment
		SetTurnGraphic(pTurnData->fromParent, pTurnData->fromChild);
	}
	else
	{
		// Use body sprite for the neck segment instead
		SetSegmentGraphic(SEGMENT_BODY, m_segmentGraphics[Snake::HEAD_INDEX].angle, Snake::NECK_INDEX);
	}

	const auto& snakeSegments = snake.GetSegments();
	int tailIndex = snake.GetLength() - 1;

	SetSegmentGraphic(
		SEGMENT_TAIL,
		WorldUtil::WorldVecToAngle(
			snakeSegments[tailIndex - 1].position - snakeSegments[tailIndex].position), // Get direction to parent segment
		tailIndex);
}

void SnakeGraphics::Render(const SDLAppRenderer& renderer, const Snake& snake) const
{
	const auto& segments = snake.GetSegments();

	for (size_t i = 0; i < snake.GetLength(); i++)
	{
		const Sprite* pSprite = GetSprite(m_segmentGraphics[i].type);
		auto destRect = renderer.WorldToScreen(segments[i].position.x, segments[i].position.y, 1, 1);
		pSprite->Draw(renderer, destRect, m_segmentGraphics[i].angle);
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
	SetSegmentGraphic(SEGMENT_TURN, CalculateTurnSpriteRotation(fromParent, fromChild), Snake::NECK_INDEX);
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

	WorldUtil::DrawRectAtCell(renderer, segmentPos, .8f);
}