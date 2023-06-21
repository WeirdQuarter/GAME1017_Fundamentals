#pragma once
#include "Core.h"
#include <array>
#include <vector>
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
constexpr Rect SCREEN = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };

void OnTitleGui(void* data);
void OnGameGui(void* data);
void OnLab1BGui(void* data);
void OnAsteroidsGui(void* data);

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
		LAB_1A,
		LAB_1B,
		LAB_2,
		ASTEROIDS,
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
	~GameScene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Texture* mShipTex = nullptr;
	Rect mShipRec;
	float mShipSpeed;
};

class Lab1AScene : public Scene
{
public:
	Lab1AScene();
	~Lab1AScene() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Texture* mTexEnterprise = nullptr;
	Texture* mTexD7 = nullptr;

	struct Ship
	{
		Rect rec;
		Point dir;
		Texture* tex;
	};

	std::array<Ship, 6> mShips;
};

class Lab1BScene : public Scene
{
public:
	Lab1BScene();
	~Lab1BScene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Sound* mEngine = nullptr;
	Sound* mExplode = nullptr;
	Sound* mFire = nullptr;
	Sound* mTeleport = nullptr;
	Music* mMusic = nullptr;

	bool mMusicPlaying = false;

	friend void OnLab1BGui(void* data);
};

class Lab2Scene : public Scene
{
public:
	Lab2Scene();
	~Lab2Scene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	struct Entity
	{
		Rect rec;
	};

	struct Turret : public Entity
	{
		float cooldown = 1.0f;
		int kills = 0;
	};

	struct Enemy : public Entity
	{
		float health = 100.0f;
	};

	struct Bullet : public Entity
	{
		float damage = 10.0f;
		Point direction;
		Turret* parent = nullptr;
	};

	std::vector<Turret> mTurrets;
	std::vector<Enemy> mEnemies;
	std::vector<Bullet> mBullets;
};

class AsteroidsScene : public Scene
{
public:
	AsteroidsScene();
	~AsteroidsScene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	struct Timer
	{
		float duration = 0.0f;	// max time
		float elapsed = 0.0f;	// current time

		bool Expired() { return elapsed >= duration; }
		void Reset() { elapsed = 0.0f; }
		void Tick(float dt) { elapsed += dt; }

		// Returns value from 0-1 based on completion. Useful for interpolation!
		float Normalize() { return Clamp(elapsed / duration, 0.0f, 1.0f); }
	};

	struct Rigidbody
	{
		Point position{};
		Point velocity{};
		Point acceleration{};

		Point direction{ 1.0f, 0.0f };
		float angularSpeed = 0.0f;
	};

	struct Entity : public Rigidbody
	{
		float width = 0.0f;
		float height = 0.0f;

		Rect Collider() const
		{
			return { position.x - width * 0.5f, position.y - height * 0.5f, width, height };
		}
	};

	struct Bullet : public Entity
	{
		void Draw() const
		{
			Color bulletColor = { 255, 0, 0, 255 };
			DrawRect(Collider(), bulletColor);
			DrawLine(position, position + direction * 20.0f, bulletColor);
		}
	};

	// Add on to this class if necessary
	struct Asteroid : public Entity
	{
		void Draw() const
		{
			Color asteroidColor = { 255, 0, 255, 255 };
			DrawRect(Collider(), asteroidColor);
			DrawLine(position, position + direction * 20.0f, asteroidColor);
		}
	};

	struct Ship : public Entity
	{
		float speed = 100.0f;

		void Draw() const
		{
			//DrawRect(Collider(), col);
			DrawTexture(tex, Collider());
			DrawLine(position, position + direction * 100.0f, col);
		}

		Timer bulletCooldown;
		Texture* tex = nullptr;
		Color col{};
	} mShip;

	Color mTestColor{ 255, 255, 255, 255 };

	// TODO:
	// Container of small asteroids
	// Container of medium asteroids
	// Timer to spawn asteroids
	std::vector<Bullet> mBullets;
	Timer mAsteroidTimer;

	std::vector<Asteroid> mAsteroidsLarge;
	std::vector<Asteroid> mAsteroidsMedium;
	std::vector<Asteroid> mAsteroidsSmall;
	const float mSizeLarge = 75.0f;
	const float mSizeMedium = 50.0f;
	const float mSizeSmall = 25.0f;

	friend void OnAsteroidsGui(void* data);

	void Wrap(Entity& entity);
};
