#include <SDL_main.h>
#include "Core.h"
#include <array>
#include "tinyxml2.h"
using namespace tinyxml2;

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
	ship->SetAttribute("x", 0.0f);
	ship->SetAttribute("y", 0.0f);
	ship->SetAttribute("w", 60.0f);
	ship->SetAttribute("h", 40.0f);
	ship->SetAttribute("speed", 100.0f);
	root->InsertEndChild(ship);

	doc.SaveFile("Game.xml");
}

int main(int argc, char* argv[])
{
	AppInit(1024, 768);

	Music* music = LoadMusic("../Assets/aud/Wings.mp3");
	PlayMusic(music);

	Game game;
	game.shipTex = LoadTexture("../Assets/img/enterprise.png");
	SaveGame(game);

	while (IsRunning())
	{
		Update(game, FrameTime());
		RenderBegin();
		Render(game);
		RenderEnd();
	}

	UnloadTexture(game.shipTex);
	UnloadMusic(music);
	AppExit();
	return 0;
}