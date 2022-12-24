#include "Math.h"
#include "../Math/Vector2.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace Math
{
	Vector2 GetCenteredPosition(const Vector2& point, float rectW, float rectH)
	{
		return Vector2(
			point.x - (rectW / 2.0f),
			point.y - (rectH / 2.0f)
		);
	}

	float ToDegrees(float angleRadians)
	{
		return angleRadians * 180.0f / static_cast<float>(M_PI);
	}
}