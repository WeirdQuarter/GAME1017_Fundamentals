#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
using namespace std;
using namespace tinyxml2;

Scene::Type Scene::sCurrent;
std::array<Scene*, Scene::COUNT> Scene::sScenes;

void Scene::Init()
{
	sScenes[TITLE] = new TitleScene;
	sScenes[GAME] = new GameScene;
	sCurrent = GAME;
	sScenes[sCurrent]->OnEnter();
}

void Scene::Exit()
{
	for (size_t i = 0; i < sScenes.size(); i++)
		delete sScenes[i];
}

void Scene::Update(float dt)
{
	sScenes[sCurrent]->OnUpdate(dt);
}

void Scene::Render()
{
	sScenes[sCurrent]->OnRender();
}

void TitleScene::OnUpdate(float dt)
{
}

void TitleScene::OnRender()
{
	DrawRect(mBackRec, { 0, 0, 0, 255 });
	DrawRect(mFrontRec, { 255, 255, 255, 255 });
}

GameScene::GameScene()
{
	mShipTex = LoadTexture("../Assets/img/enterprise.png");
}

GameScene::~GameScene()
{
	UnloadTexture(mShipTex);
}

void GameScene::OnEnter()
{
	XMLDocument doc;
	doc.LoadFile("Game.xml");

	XMLElement* gameData = doc.FirstChildElement();
	XMLElement* shipData = gameData->FirstChildElement();
	shipData->QueryAttribute("x", &mShipRec.x);
	shipData->QueryAttribute("y", &mShipRec.y);
	shipData->QueryAttribute("w", &mShipRec.w);
	shipData->QueryAttribute("h", &mShipRec.h);
	shipData->QueryAttribute("speed", &mShipSpeed);

	cout << "Loaded " << gameData->Value() << endl;
}

void GameScene::OnExit()
{
	XMLDocument doc;
	XMLNode* root = doc.NewElement("Game");
	doc.InsertEndChild(root);

	XMLElement* ship = doc.NewElement("Ship");
	ship->SetAttribute("x", mShipRec.x);
	ship->SetAttribute("y", mShipRec.y);
	ship->SetAttribute("w", mShipRec.w);
	ship->SetAttribute("h", mShipRec.h);
	ship->SetAttribute("speed", mShipSpeed);
	root->InsertEndChild(ship);

	doc.SaveFile("Game.xml");
}

void GameScene::OnUpdate(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_A))
	{
		mShipRec.x -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_D))
	{
		mShipRec.x += mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_W))
	{
		mShipRec.y -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_S))
	{
		mShipRec.y += mShipSpeed * dt;
	}
}

void GameScene::OnRender()
{
	DrawTexture(mShipTex, mShipRec);
}
