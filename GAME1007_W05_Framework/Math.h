#pragma once
#include <SDL.h>

// Hold off on this for now because this doesn't integrate nicely with SDL's collision functions
struct Vector2
{
	float x;
	float y;

	//operator SDL_FRect() { return { x, y }; }
	// This works for value-assignments like SDL_FRect r = Vector2{};
	// But not pointer assignments. That is only possible with reinterpret_cast
	// SDL_PointInFRect(reinterpret_cast<SDL_FPoint*>(&v), &rec);
};
