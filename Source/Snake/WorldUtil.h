#pragma once

class SDLAppRenderer;
struct Vector2;

class WorldUtil
{
public:
	// Draws a rect around the midpoint of a cell in the world
	static void DrawRectAtCell(const SDLAppRenderer&, const Vector2& cellPos, float rectScale);

	// Returns the angle (in degrees) between a world-space vector and the world +x axis
	static float WorldVecToAngle(const Vector2& v);
};
