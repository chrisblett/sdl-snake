#pragma once

#include "../Engine/Math/Vector2.h"
#include "../Engine/Graphics.h"

#include <vector>
#include <memory>

class Snake;
class World;
class SnakeBrain;
class SDLAppRenderer;

struct Segment
{
	Vector2 position;
};

class SnakeGraphics
{
public:
	SnakeGraphics();

	void Render(const SDLAppRenderer& renderer, const Snake& snake) const;
	void RenderSegment(const SDLAppRenderer& renderer, const Snake& snake,
		const Segment& seg, const Segment* pParent, const Segment* pChild) const;
private:
	UniqueSpritePtr m_pHead;
	UniqueSpritePtr m_pTail;
	UniqueSpritePtr m_pCorner;
	UniqueSpritePtr m_pBody;
};

class Snake
{
public:
	Snake(World& world, int worldWidth, int worldHeight);
	~Snake() { printf("Snake destroyed\n"); }

	void Update(SnakeBrain& brain, World& world);
	void Render(const SDLAppRenderer&) const;
	
	void Simulate(const Vector2* pInputDir);
	void EatFood(int growValue);

	const Vector2& GetHeadPosition()          const { return m_segments[0].position; }
	const Vector2& GetDirection()             const { return *m_pDir; }
	const std::vector<Segment>& GetSegments() const { return m_segments; }
	size_t GetNumSegments()                   const { return m_numSegments; }

private:
	void Move(const Vector2* pInputDir);
	void Grow();

	// Marks any cells the snake is over as being occupied
	void RecordOccupiedCells(World& game);

	Segment& GetHead();

	SnakeGraphics        m_graphics;
	std::vector<Segment> m_segments;
	size_t               m_numSegments;
	const Vector2*       m_pDir;

	int m_growCounter; // Remaining number of times the snake must grow
};

