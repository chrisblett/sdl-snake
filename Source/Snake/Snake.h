#pragma once

#include "../Engine/Math/Vector2.h"
#include "../Engine/Graphics.h"

#include <vector>

class SDLAppRenderer;
class World;
class SnakeBrain;

class Snake
{
public:
	Snake(World& world, int worldWidth, int worldHeight);
	~Snake() { printf("Snake destroyed\n"); }

	void Update(SnakeBrain& brain, World& world);
	void Render(const SDLAppRenderer&) const;

	void Simulate(const Vector2* pInputDir);
	void EatFood(int growValue);

	const Vector2& GetHeadPosition() const { return m_segments[0].position; }
	const Vector2& GetDirection() const { return *m_pDir; }

private:
	struct Segment
	{
		Vector2 position;
	};

	void Move(const Vector2* pInputDir);
	void Grow();
	void RecordOccupiedCells(World& game);

	Segment& GetHead();

	std::vector<Segment> m_segments;
	size_t               m_numSegments;
	const Vector2*       m_pDir;

	std::unique_ptr<Sprite> m_pCorner;

	int m_growCounter; // Remaining number of times the snake must grow
};
