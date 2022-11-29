#pragma once

struct Vector2
{
	float x;
	float y;

	explicit Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(float scalar);

	void Normalize();
	float Length();

	static float Dot(const Vector2& a, const Vector2& b);
	static float Distance(const Vector2& a, const Vector2& b);
};

Vector2 operator+(Vector2 lhs, const Vector2& rhs);
Vector2 operator-(Vector2 lhs, const Vector2& rhs);
Vector2 operator*(float scalar, Vector2 v);
Vector2 operator*(Vector2 v, float scalar);

bool operator==(const Vector2& lhs, const Vector2& rhs);