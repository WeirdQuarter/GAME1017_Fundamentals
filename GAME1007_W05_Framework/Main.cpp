#include <SDL_main.h>
#include "Core.h"
#include <array>
#include <iostream>
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;

struct Game
{
	Texture* shipTex = nullptr;
	Rect shipRec;
	float shipSpeed;

	Sound* sound1;
};

void Update(Game& game, float dt)
{
	//game.shipRec.x += game.shipSpeed * dt;
	if (IsKeyDown(SDL_SCANCODE_A))
	{
		game.shipRec.x -= game.shipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_D))
	{
		game.shipRec.x += game.shipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_W))
	{
		game.shipRec.y -= game.shipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_S))
	{
		game.shipRec.y += game.shipSpeed * dt;
	}
}

void Render(const Game& game)
{
	DrawTexture(game.shipTex, game.shipRec);
}

void SaveCar()
{
	XMLDocument doc;	//DOM tree (in-memory representation of xml document)
	XMLNode* root = doc.NewElement("CarDefinition");
	doc.InsertEndChild(root);

	XMLElement* acceleration = doc.NewElement("Acceleration");
	acceleration->SetAttribute("a", 20.0f);
	root->InsertEndChild(acceleration);

	XMLElement* wheel = doc.NewElement("Wheel");
	XMLElement* radius = doc.NewElement("Radius");
	radius->SetAttribute("r", 10.0f);
	wheel->InsertEndChild(radius);
	root->InsertEndChild(wheel);

	XMLElement* mass = doc.NewElement("Mass");
	mass->SetAttribute("m", 30.0f);
	root->InsertEndChild(mass);

	doc.SaveFile("Car.xml");
}

void SaveGame(const Game& game)
{
	XMLDocument doc;
	XMLNode* root = doc.NewElement("Game");
	doc.InsertEndChild(root);

	XMLElement* ship = doc.NewElement("Ship");
	ship->SetAttribute("x", game.shipRec.x);
	ship->SetAttribute("y", game.shipRec.y);
	ship->SetAttribute("w", game.shipRec.w);
	ship->SetAttribute("h", game.shipRec.h);
	ship->SetAttribute("speed", game.shipSpeed);
	root->InsertEndChild(ship);

	doc.SaveFile("Game.xml");
}

void LoadGame(Game& game)
{
	XMLDocument doc;
	doc.LoadFile("Game.xml");

	XMLElement* gameData = doc.FirstChildElement();
	XMLElement* shipData = gameData->FirstChildElement();
	shipData->QueryAttribute("x", &game.shipRec.x);
	shipData->QueryAttribute("y", &game.shipRec.y);
	shipData->QueryAttribute("w", &game.shipRec.w);
	shipData->QueryAttribute("h", &game.shipRec.h);
	shipData->QueryAttribute("speed", &game.shipSpeed);

	cout << "Loaded " << gameData->Value() << endl;
}

Uint32 Pause(Uint32 interval, void* param)
{
	cout << (const char*)param << endl;
	PauseMusic();
	return 0;
}

Uint32 Resume(Uint32 interval, void* param)
{
	cout << (const char*)param << endl;
	ResumeMusic();
	return 0;
}

struct Custom
{
	float f = 1.1f;
	int i = 5;
	const char* msg = "hello";
};

Uint32 CustomTimer(Uint32 interval, void* param)
{
	Custom* custom = (Custom*)param;
	cout << custom->f << endl;
	cout << custom->i << endl;
	cout << custom->msg << endl;
	return 0;
}

void OnGui(void* data)
{
	Game& game = *(Game*)data;

	if (ImGui::Button("Fire!"))
	{
		PlaySound(game.sound1);
	}
}

int main(int argc, char* argv[])
{
	AppInit(1024, 768);

	Music* music = LoadMusic("../Assets/aud/Wings.mp3");
	//PlayMusic(music);

	Game game;
	LoadGame(game);

	game.shipTex = LoadTexture("../Assets/img/enterprise.png");
	game.sound1 = LoadSound("../Assets/aud/Fire.wav");

	//Custom custom;
	//SDL_AddTimer(1000, CustomTimer, &custom);
	//SDL_AddTimer(2000, Pause, (void*)"Music paused.");
	//SDL_AddTimer(4000, Resume, (void*)"Music resumed!");

	SetGuiCallback(OnGui, &game);

	while (IsRunning())
	{
		Update(game, FrameTime());
		RenderBegin();
		Render(game);
		RenderEnd();
	}

	SaveGame(game);

	UnloadTexture(game.shipTex);
	UnloadMusic(music);
	UnloadSound(game.sound1);
	AppExit();
	return 0;
}