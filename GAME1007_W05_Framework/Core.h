#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "imgui/imgui.h"
#include "Math.h"

using Points = std::vector<Point>;
using Rects = std::vector<Rect>;

using Texture = SDL_Texture;
using Textures = std::vector<Texture>;

using Color = SDL_Color;
using Colors = std::vector<Color>;

using Sound = Mix_Chunk;
using Sounds = std::vector<Sound>;

using Music = Mix_Music;
using Musics = std::vector<Music>;

// This is just driving home the point of "using". We have no need for a vector of gui callbacks currently...
using GuiCallback = void(*)(void*);
using GuiCallbacks = std::vector<GuiCallback>;

void SetGuiCallback(GuiCallback callback, void* data);

void AppInit(int width, int height);
void AppExit();

void RenderBegin();
void RenderEnd();

Texture* LoadTexture(const char* path);
void UnloadTexture(Texture* texture);

Sound* LoadSound(const char* path);
void UnloadSound(Sound* sound);
void PlaySound(Sound* sound, bool loop = false);

Music* LoadMusic(const char* path);
void UnloadMusic(Music* music);
void PlayMusic(Music* music, bool loop = true);
void PauseMusic();
void ResumeMusic();

int GetFps();			// Average frame rate
void SetFps(int fps);	// Desired (maximum) frame rate

float FrameTime();			// Time duration for frame update + frame render
float FrameTimeSmoothed();	// Time duration for frame update + frame render over 10 frames

double TotalTime();			// Time since program start in seconds
void Wait(double seconds);	// Halts the program for seconds

bool IsRunning();
bool IsKeyDown(SDL_Scancode key);
bool IsKeyPressed(SDL_Scancode key);
Point MousePosition();

void DrawPoint(const Point& point, const Color& color);
void DrawPoints(const Points& points, const Color& color);

void DrawLine(const Point& start, const Point& end, const Color& color);
void DrawLines(const Points& points, const Color& color);

void DrawRect(const Rect& rect, const Color& color);
void DrawRects(const Rects& rects, const Color& color);

void DrawTexture(Texture* texture, const Rect& rect, float degrees = 0.0f);