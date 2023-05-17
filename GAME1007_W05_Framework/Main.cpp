#include <SDL_main.h>
#include "Core.h"

struct Game
{
	Texture* shipTex = nullptr;
	Rect shipRec{ 0.0f, 0.0f, 60.0f, 40.0f };
	float shipSpeed = 100.0f;
};

void Update(Game& game, float dt)
{
	game.shipRec.x += game.shipSpeed * dt;
}

void Render(const Game& game)
{
	DrawTexture(game.shipTex, game.shipRec);
}

int main(int argc, char* argv[])
{
	AppInit(1024, 768);

	Game game;
	game.shipTex = LoadTexture("../Assets/img/enterprise.png");

	while (IsRunning())
	{
		Update(game, FrameTime());
		RenderBegin();
		Render(game);
		RenderEnd();
	}

	UnloadTexture(game.shipTex);
	AppExit();
	return 0;
}
