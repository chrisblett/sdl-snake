#pragma once

#include "Vector2.h"

namespace Math
{
	// Returns the top-left position of a rectangle so it can be centered about 'point'
	Vector2 GetCenteredPosition(const Vector2& point, float rectW, float rectH);

	// Convert an angle in radians to degrees
	float ToDegrees(float angleRadians);
}