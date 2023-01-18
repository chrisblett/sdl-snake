#pragma once

#include "../Engine/Math/Vector2.h"
#include "SnakeGraphics.h"

#include <vector>
#include <memory>

struct Segment
{
	Vector2 position;
};

struct SnakeTurnData
{
	Vector2 fromParent;
	Vector2 fromChild;
};

class SDLAppRenderer;
class World;
class SnakeBrain;

class Snake
{
public:
	Snake(World& world, int worldWidth, int worldHeight);
	~Snake();

	void Reset();

	void Update(SnakeBrain& brain);
	void Render(const SDLAppRenderer&) const;
	
	void Simulate(const Vector2* pInputDir);
	void EatFood(int growValue);

	const Vector2& GetHeadPosition()          const { return m_segments[HEAD_INDEX].position; }
	const Vector2& GetDirection()             const { return *m_pDir; }
	const std::vector<Segment>& GetSegments() const { return m_segments; }
	size_t GetLength()                        const { return m_numSegments; }

	bool IsDead() const { return m_dead; }

	static constexpr size_t HEAD_INDEX = 0;
	static constexpr size_t NECK_INDEX = 1;
	static constexpr size_t INITIAL_LENGTH = 3;

private:
	// Sets snake to its starting state
	void Init();

	void Move(const Vector2* pInputDir, const Vector2*& pPrevDir);
	void HandleGrowth();
	void Grow();

	// Marks any cells the snake is over as being occupied
	void MarkOccupiedCells();

	// Tests conditions for snake death. If a condition was triggered, m_dead is set to true. 
	void CheckForDeath();

	Segment& GetHead();

	SnakeGraphics        m_graphics;
	std::vector<Segment> m_segments;
	const Vector2        m_startPos;
	World&               m_world;
	const Vector2*       m_pDir;
	size_t               m_numSegments;

	// Tracks the remaining number of times the snake has to grow
	// since growing to a particular length spans multiple updates
	int                  m_growCounter; 
	bool                 m_dead;
};