#include "Math.h"
#include "../Math/Vector2.h"

namespace Math
{
	Vector2 GetCenteredPosition(const Vector2& point, float rectW, float rectH)
	{
		return Vector2(
			point.x - (rectW / 2.0f),
			point.y - (rectH / 2.0f)
		);
	}
}