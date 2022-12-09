#pragma once

#include "../Engine/Math/Vector2.h"
#include <vector>

class SDLAppRenderer;
class SnakeGame;

class Snake
{
public:
	Snake(const Vector2& dir, int worldWidth, int worldHeight);

	void Update(SnakeGame& game, const Vector2& inputDir, bool shouldGrow, float deltaTime);
	void Render(const SDLAppRenderer&) const;


	const Vector2& GetHeadPosition() const { return m_segments[0].position; }
	const Vector2& GetDirection() const { return *m_pDir; }

private:
	struct Segment
	{
		Vector2 position;
	};

	void Grow();
	void Move(const Vector2& inputDir);
	Segment& GetHead();

	std::vector<Segment> m_segments;
	size_t               m_numSegments;
	const Vector2*       m_pDir;
};
