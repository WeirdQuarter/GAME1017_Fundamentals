#pragma once
#include "Core.h"
#include <array>
#include <vector>
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void OnEnter() {}
	virtual void OnExit() {}

	virtual void OnUpdate(float dt) = 0;
	virtual void OnRender() = 0;

	enum Type : size_t
	{
		TITLE,
		GAME,
		COUNT
	};

	static void Init();
	static void Exit();

	static void Update(float dt);
	static void Render();

	static void Change(Type type);

private:
	static Type sCurrent;
	static std::array<Scene*, COUNT> sScenes;
};

class TitleScene : public Scene
{
public:
	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Rect mBackRec = { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
	Rect mFrontRec = { 0.0f, 0.0f, 60.0f, 40.0f };
};

class GameScene : public Scene
{
public:
	GameScene();
	virtual ~GameScene();

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Texture* mShipTex = nullptr;
	Rect mShipRec;
	float mShipSpeed;
};
