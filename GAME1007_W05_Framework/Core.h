#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "imgui/imgui.h"
#include "Math.h"

using Texture = SDL_Texture;
using Color = SDL_Color;
using Sound = Mix_Chunk;
using Music = Mix_Music;
using GuiCallback = void(*)(void*);
using Font = TTF_Font;

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

Font* DefaultFont();
Font* LoadFont(const char* path, int size = 20);
void UnloadFont(Font* font);

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

void DrawLine(Point start, Point end);
void DrawRect(const Rect& rect, const Color& color);
void DrawTexture(Texture* texture, const Rect& rect, float degrees = 0.0f);

void DrawText(const char* text, float x, float y, float degrees = 0.0f,
	Color color = { 255, 255, 255, 255 }, Font* font = nullptr);