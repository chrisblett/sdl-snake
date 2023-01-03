#pragma once

#include "../Engine/Math/Vector2.h"
#include "../Engine/Graphics.h"

#include <vector>
#include <memory>

struct Segment
{
	Vector2 position;
};

enum SegmentType
{
	SEGMENT_HEAD,
	SEGMENT_TAIL,
	SEGMENT_BODY,
	SEGMENT_TURN,
};

class Snake;
class World;
class SnakeBrain;
class SDLAppRenderer;

class SnakeGraphics
{
public:
	SnakeGraphics(int maxSegments);

	void Render(const SDLAppRenderer& renderer, const Snake& snake) const;

	// Sets a particular segment's graphic data
	void SetSegmentGraphic(SegmentType type, float angle, int index);
	void SetTurnGraphic(const Vector2& fromParent, const Vector2& fromChild);

	void Update(int numSegments);

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

class Snake
{
public:
	Snake(World& world, int worldWidth, int worldHeight);
	~Snake() { printf("Snake destroyed\n"); }

	void Update(SnakeBrain& brain);
	void Render(const SDLAppRenderer&) const;
	
	void Simulate(const Vector2* pInputDir);
	void EatFood(int growValue);

	const Vector2& GetHeadPosition()          const { return m_segments[0].position; }
	const Vector2& GetDirection()             const { return *m_pDir; }
	const std::vector<Segment>& GetSegments() const { return m_segments; }
	size_t GetNumSegments()                   const { return m_numSegments; }

	bool IsDead() const { return m_dead; }

private:
	void Move(const Vector2* pInputDir, const Vector2*& pPrevDir);
	void Grow();

	// Marks any cells the snake is over as being occupied
	void MarkOccupiedCells();

	Segment& GetHead();

	SnakeGraphics        m_graphics;
	std::vector<Segment> m_segments;
	World&               m_world;
	const Vector2*       m_pDir;
	size_t               m_numSegments;
	int                  m_growCounter; // Remaining number of times the snake must grow
	bool                 m_dead;
};