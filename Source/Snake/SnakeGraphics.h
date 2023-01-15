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
struct Vector2;

class SnakeGraphics
{
public:
	SnakeGraphics(int maxSegments);

	void Init(const Snake& snake);

	void Render(const SDLAppRenderer& renderer, const Snake& snake) const;

	// Sets a particular segment's graphic data
	void SetSegmentGraphic(SegmentType type, float angle, int index);
	void SetTurnGraphic(const Vector2& fromParent, const Vector2& fromChild);

	void Update(const Snake& snake);

private:
	Sprite* GetSprite(SegmentType type) const;

	struct SegmentGraphic
	{
		SegmentType type;
		float angle;
	};

	std::vector<SegmentGraphic> m_segmentGraphics;
	UniqueSpritePtr m_pHead;
	UniqueSpritePtr m_pTail;
	UniqueSpritePtr m_pTurn;
	UniqueSpritePtr m_pBody;
};