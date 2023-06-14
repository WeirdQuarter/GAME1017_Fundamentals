#pragma once
#include <SDL.h>
#include <cmath>

// Scalar math
float Clamp(float value, float min, float max)
{
	float result = (value < min) ? min : value;

	if (result > max) result = max;

	return result;
}

// Vector math
using Rect = SDL_FRect;
using Point = SDL_FPoint;

inline Point operator+(Point a, Point b)
{
	Point result{ a.x + b.x, a.y + b.y };

	return result;
}

inline Point operator-(Point a, Point b)
{
	Point result{ a.x - b.x, a.y - b.y };

	return result;
}

inline Point operator*(Point a, float b)
{
	Point result{ a.x * b, a.y * b };

	return result;
}

inline Point operator/(Point a, float b)
{
	Point result{ a.x / b, a.y / b };

	return result;
}

inline float Distance(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf(dx * dx + dy * dy);
}

inline float DistanceSqr(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

inline float Length(Point p)
{
	float result = sqrtf(p.x * p.x + p.y * p.y);

	return result;
}

inline float LengthSqr(Point p)
{
	float result = p.x * p.x + p.y * p.y;

	return result;
}

inline Point Normalize(Point p)
{
	float length = Length(p);
	if (length > 0.0f)
		return { p.x / length, p.y / length };
	return {};
}

inline float Dot(Point v1, Point v2)
{
	float result = (v1.x * v2.x + v1.y * v2.y);

	return result;
}

inline float Cross(Point v1, Point v2)
{
	float result = v1.x * v2.y - v1.y * v2.x;

	return result;
}

inline Point Direction(float angle)
{
	Point result = { cosf(angle), sinf(angle) };

	return result;
}

inline float Angle(Point p)
{
	float result = atan2f(p.y, p.x);

	return result;
}

// Project v1 onto v2
inline Point Project(Point v1, Point v2)
{
	float t = Dot(v1, v2) / Dot(v2, v2);
	return { t * v2.x, t * v2.y };
}

// Returns the point on line AB nearest to point P
inline Point NearestPoint(Point A, Point B, Point P)
{
	Point AB = B - A;
	float t = Dot(P - A, AB) / Dot(AB, AB);
	return A + AB * Clamp(t, 0.0f, 1.0f);
}

// Rotate vector by angle
inline Point Rotate(Point v, float angle)
{
	Point result = { 0 };

	float cosres = cosf(angle);
	float sinres = sinf(angle);

	result.x = v.x * cosres - v.y * sinres;
	result.y = v.x * sinres + v.y * cosres;

	return result;
}

// Calculate linear interpolation between two vectors
inline Point Lerp(Point v1, Point v2, float amount)
{
	Point result = { 0 };

	result.x = v1.x + amount * (v2.x - v1.x);
	result.y = v1.y + amount * (v2.y - v1.y);

	return result;
}
