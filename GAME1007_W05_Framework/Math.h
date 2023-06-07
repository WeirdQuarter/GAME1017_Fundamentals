#pragma once
#include <SDL.h>
#include <cmath>

using Rect = SDL_FRect;
using Point = SDL_FPoint;

// Hold off on this for now because this doesn't integrate nicely with SDL's collision functions
/*struct Vector2
{
	float x;
	float y;

	//operator SDL_FRect() { return { x, y }; }
	// This works for value-assignments like SDL_FRect r = Vector2{};
	// But not pointer assignments. That is only possible with reinterpret_cast
	// SDL_PointInFRect(reinterpret_cast<SDL_FPoint*>(&v), &rec);
};*/

float Distance(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf(dx * dx + dy * dy);
}

float DistanceSqr(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}
