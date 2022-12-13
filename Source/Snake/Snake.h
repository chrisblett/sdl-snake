#pragma once

#include "../Engine/Math/Vector2.h"
#include <vector>

class SDLAppRenderer;
class World;

class Snake
{
public:
	Snake(World& game, const Vector2& dir, int worldWidth, int worldHeight);

	void Update(World& game, const Vector2& inputDir);
	void Render(const SDLAppRenderer&) const;

	void EatFood(int growValue);

	const Vector2& GetHeadPosition() const { return m_segments[0].position; }
	const Vector2& GetDirection() const { return *m_pDir; }

private:
	struct Segment
	{
		Vector2 position;
	};

	void Move(const Vector2& inputDir);
	void Grow();
	void RecordOccupiedCells(World& game);
	Segment& GetHead();

	std::vector<Segment> m_segments;
	size_t               m_numSegments;
	const Vector2*       m_pDir;

	int m_growCounter; // Remaining number of times the snake must grow
};
