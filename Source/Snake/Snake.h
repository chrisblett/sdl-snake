#pragma once

#include "../Engine/Math/Vector2.h"

class SDLAppRenderer;

class Snake
{
public:
	Snake(const Vector2& dir, int worldWidth, int worldHeight);

	void Update(const Vector2& inputDir, float deltaTime);
	void Render(const SDLAppRenderer&) const;

	const Vector2& GetDirection() const { return *m_pDir; }

private:
	const Vector2* m_pDir;
	Vector2        m_pos;

	float          m_timeSinceLastMove;
};
