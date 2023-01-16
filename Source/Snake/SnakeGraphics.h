#pragma once

#include "../Engine/Graphics.h"

enum SegmentType
{
	SEGMENT_HEAD,
	SEGMENT_TAIL,
	SEGMENT_BODY,
	SEGMENT_TURN,
};

class SDLAppRenderer;
class Sprite;
class Snake;
struct SnakeTurnData;
struct Vector2;

class SnakeGraphics
{
public:
	SnakeGraphics(int maxSegments);

	void Init(const Snake& snake);
	void Update(const Snake& snake, SnakeTurnData* pTurnData);
	void Render(const SDLAppRenderer& renderer, const Snake& snake) const;

private:
	Sprite* GetSprite(SegmentType type) const;

	// Sets a particular segment's graphic data
	void SetSegmentGraphic(SegmentType type, float angle, int index);
	void SetTurnGraphic(const Vector2& fromParent, const Vector2& fromChild);

	struct SegmentGraphic
	{
		SegmentType type;
		float angle;
	};

	std::vector<SegmentGraphic> m_segmentGraphics;
	UniqueSpritePtr m_pHead;
	UniqueSpritePtr m_pTail;
	UniqueSpritePtr m_pBody;
	UniqueSpritePtr m_pTurn;
};