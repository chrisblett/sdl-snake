#include "Vector2.h"

#include <cmath>

Vector2& Vector2::operator+=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *(this);
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *(this);
}

Vector2& Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *(this);
}

Vector2 operator+(Vector2 lhs, const Vector2& rhs)
{
	return lhs += rhs;
}

Vector2 operator-(Vector2 lhs, const Vector2& rhs)
{
	return lhs -= rhs;
}

Vector2 operator*(float scalar, Vector2 v)
{
	return v *= scalar;
}

Vector2 operator*(Vector2 v, float scalar)
{
	return v *= scalar;
}

bool operator==(const Vector2& lhs, const Vector2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

void Vector2::Normalize()
{
	float mag = Length();
	if (mag != 0)
	{
		x /= mag;
		y /= mag;
	}
}

float Vector2::Length()
{
	return sqrtf(x * x + y * y);
}

float Vector2::Dot(const Vector2& a, const Vector2& b)
{
	return (a.x * b.x + a.y * b.y);
}

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
	return (b - a).Length();
}