#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <functional>
using namespace std;
using namespace tinyxml2;

Scene::Type Scene::sCurrent;
std::array<Scene*, Scene::COUNT> Scene::sScenes;

void Scene::Init()
{
	sScenes[TITLE] = new TitleScene;
	sScenes[GAME] = new GameScene;
	sScenes[LAB_1A] = new Lab1AScene;
	sScenes[LAB_1B] = new Lab1BScene;
	sScenes[LAB_2] = new Lab2Scene;
	sScenes[ASTEROIDS] = new AsteroidsScene;
	sCurrent = ASTEROIDS;
	sScenes[sCurrent]->OnEnter();
}

void Scene::Exit()
{
	sScenes[sCurrent]->OnExit();
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

void Scene::Change(Type type)
{
	assert(sCurrent != type);
	sScenes[sCurrent]->OnExit();
	sCurrent = type;
	sScenes[sCurrent]->OnEnter();
}

void TitleScene::OnEnter()
{
	SetGuiCallback(OnTitleGui, this);
}

void TitleScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void TitleScene::OnUpdate(float dt)
{
}

void TitleScene::OnRender()
{
	DrawRect(mBackRec, { 0, 0, 0, 255 });
	DrawRect(mFrontRec, { 255, 255, 255, 255 });
}

void OnTitleGui(void* data)
{
	TitleScene& scene = *(TitleScene*)data;
	if (ImGui::Button("Begin!"))
	{
		Scene::Change(Scene::GAME);
	}
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
	SetGuiCallback(OnGameGui, this);

	XMLDocument doc;
	doc.LoadFile("Game.xml");

	XMLElement* gameData = doc.FirstChildElement();
	XMLElement* shipData = gameData->FirstChildElement();
	shipData->QueryAttribute("x", &mShipRec.x);
	shipData->QueryAttribute("y", &mShipRec.y);
	shipData->QueryAttribute("w", &mShipRec.w);
	shipData->QueryAttribute("h", &mShipRec.h);
	shipData->QueryAttribute("speed", &mShipSpeed);
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

	SetGuiCallback(nullptr, nullptr);
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

void OnGameGui(void* data)
{
	GameScene& scene = *(GameScene*)data;

	if (ImGui::Button("End"))
	{
		Scene::Change(Scene::TITLE);
	}
}

Lab1AScene::Lab1AScene()
{
	mTexEnterprise = LoadTexture("../Assets/img/enterprise.png");
	mTexD7 = LoadTexture("../Assets/img/d7.png");

	for (size_t i = 0; i < mShips.size(); i++)
	{
		Ship& ship = mShips[i];
		ship.rec.x = SCREEN_WIDTH * 0.1f;

		const float space = SCREEN_HEIGHT / mShips.size();
		ship.rec.y = i * space + space * 0.5f;

		ship.rec.w = 60.0f;
		ship.rec.h = 40.0f;

		ship.dir = { 1.0f, 1.0f };

		if (i % 2 == 0)
			ship.tex = mTexEnterprise;
		else
			ship.tex = mTexD7;
	}
}

Lab1AScene::~Lab1AScene()
{
	UnloadTexture(mTexD7);
	UnloadTexture(mTexEnterprise);
}

void Lab1AScene::OnUpdate(float dt)
{
	const float speed = 500.0f;
	for (Ship& ship : mShips)
	{
		Rect& rec = ship.rec;

		if (rec.x + rec.w >= SCREEN_WIDTH)
			ship.dir.x = -1.0f;
		else if (rec.x <= 0.0f)
			ship.dir.x = 1.0f;

		if (rec.y + rec.h >= SCREEN_HEIGHT)
			ship.dir.y = -1.0f;
		else if (rec.y <= 0.0f)
			ship.dir.y = 1.0f;

		rec.x += ship.dir.x * speed * dt;
		rec.y += ship.dir.y * speed * dt;
	}
}

void Lab1AScene::OnRender()
{
	for (const Ship& ship : mShips)
		DrawTexture(ship.tex, ship.rec);
}

void OnLab1BGui(void* data)
{
	Lab1BScene& scene = *(Lab1BScene*)data;

	if (scene.mMusicPlaying)
	{
		if (ImGui::Button("Pause"))
		{
			scene.mMusicPlaying = false;
			PauseMusic();
		}
	}
	else
	{
		if (ImGui::Button("Resume"))
		{
			scene.mMusicPlaying = true;
			ResumeMusic();
		}
	}

	if (ImGui::Button("Engine"))
		PlaySound(scene.mEngine);

	if (ImGui::Button("Explode"))
		PlaySound(scene.mExplode);

	if (ImGui::Button("Fire"))
		PlaySound(scene.mFire);

	if (ImGui::Button("Teleport"))
		PlaySound(scene.mTeleport);
}

Lab1BScene::Lab1BScene()
{
	mEngine = LoadSound("../Assets/aud/Engines.wav");
	mExplode = LoadSound("../Assets/aud/Explode.wav");
	mFire = LoadSound("../Assets/aud/Fire.wav");
	mTeleport = LoadSound("../Assets/aud/Teleport.wav");
	mMusic = LoadMusic("../Assets/aud/Wings.mp3");

	// Pause/Resume music() pause/resume the stream, but they don't play.
	// Hence, we need to ensure our music is playing before pausing/resuming!
	PlayMusic(mMusic);
	if (!mMusicPlaying)
		PauseMusic();
}

Lab1BScene::~Lab1BScene()
{
	UnloadMusic(mMusic);
	UnloadSound(mEngine);
	UnloadSound(mExplode);
	UnloadSound(mFire);
	UnloadSound(mTeleport);
}

void Lab1BScene::OnEnter()
{
	SetGuiCallback(OnLab1BGui, this);
}

void Lab1BScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void Lab1BScene::OnUpdate(float dt)
{
}

void Lab1BScene::OnRender()
{
}

Lab2Scene::Lab2Scene()
{
}

Lab2Scene::~Lab2Scene()
{
}

void Lab2Scene::OnEnter()
{
	XMLDocument doc;
	doc.LoadFile("Turrets.xml");

	XMLElement* element = doc.FirstChildElement();
	while (element != nullptr)
	{
		int kills = 0;
		float cooldown = 1.0f;
		Rect rec;
		element->QueryAttribute("kills", &kills);
		element->QueryAttribute("cooldown", &cooldown);
		element->QueryAttribute("x", &rec.x);
		element->QueryAttribute("y", &rec.y);
		element->QueryAttribute("w", &rec.w);
		element->QueryAttribute("h", &rec.h);
		
		Turret turret;
		turret.rec = rec;
		turret.cooldown = cooldown;
		turret.kills = kills;
		mTurrets.push_back(turret);

		element = element->NextSiblingElement();
	}
}

void Lab2Scene::OnExit()
{
	XMLDocument doc;
	for (const Turret& turret : mTurrets)
	{
		XMLElement* element = doc.NewElement("Turret");
		element->SetAttribute("x", turret.rec.x);
		element->SetAttribute("y", turret.rec.y);
		element->SetAttribute("w", turret.rec.w);
		element->SetAttribute("h", turret.rec.h);
		element->SetAttribute("kills", turret.kills);
		element->SetAttribute("cooldown", turret.cooldown);
		doc.InsertEndChild(element);
	}
	doc.SaveFile("Turrets.xml");
}

void Lab2Scene::OnUpdate(float dt)
{
	if (IsKeyPressed(SDL_SCANCODE_T))
	{
		Turret turret;
		turret.rec.x = rand() % SCREEN_WIDTH;
		turret.rec.y = rand() % SCREEN_HEIGHT;
		turret.rec.w = 100.0f;
		turret.rec.h = 100.0f;
		mTurrets.push_back(turret);
	}

	if (IsKeyPressed(SDL_SCANCODE_R))
		mTurrets.pop_back();

	if (IsKeyPressed(SDL_SCANCODE_E))
	{
		Enemy enemy;
		enemy.rec.x = rand() % SCREEN_WIDTH;
		enemy.rec.y = rand() % SCREEN_HEIGHT;
		enemy.rec.w = 60.0f;
		enemy.rec.h = 40.0f;
		mEnemies.push_back(enemy);
	}

	for (Turret& turret : mTurrets)
	{
		turret.cooldown -= dt;
		if (turret.cooldown <= 0.0f)
		{
			turret.cooldown = 1.0f;

			// Find the nearest enemy to shoot at:
			float nearestDistance = FLT_MAX;
			const Enemy* nearestEnemy = nullptr;

			for (const Enemy& enemy : mEnemies)
			{
				float distance = DistanceSqr({ turret.rec.x, turret.rec.y }, { enemy.rec.x, enemy.rec.y });
				if (distance < nearestDistance)
				{
					nearestDistance = distance;
					nearestEnemy = &enemy;
				}
			}

			if (nearestEnemy != nullptr)
			{
				// AB = B - A
				Bullet bullet;
				bullet.rec.w = 10.0f;
				bullet.rec.h = 10.0f;
				bullet.direction = Normalize({ nearestEnemy->rec.x - turret.rec.x, nearestEnemy->rec.y - turret.rec.y });
				bullet.rec.x = turret.rec.x + turret.rec.w * bullet.direction.x;
				bullet.rec.y = turret.rec.y + turret.rec.h * bullet.direction.y;
				bullet.parent = &turret;
				mBullets.push_back(bullet);
			}
		}
	}
	
	for (Bullet& bullet : mBullets)
	{
		float speed = 100.0f * dt;
		bullet.rec.x += bullet.direction.x * speed;
		bullet.rec.y += bullet.direction.y * speed;
	}

	// Remove if colliding with enemy or off-screen
	mBullets.erase(remove_if(mBullets.begin(), mBullets.end(),
		[this](const Bullet& bullet)
		{
			// Check if the bullet is on-screen before checking it against every enemy
			Rect screen{ 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
			if (!SDL_HasIntersectionF(&bullet.rec, &screen)) return true;
	
			for (Enemy& enemy : mEnemies)
			{
				if (SDL_HasIntersectionF(&bullet.rec, &enemy.rec))
				{
					enemy.health -= bullet.damage;
					if (enemy.health <= 0.0f)
						bullet.parent->kills++;
					return true;
				}
			}
	
			// Bullet is on-screen and not colliding with any enemies
			return false;
		}),
	mBullets.end());

	// Remove dead enemies
	mEnemies.erase(remove_if(mEnemies.begin(), mEnemies.end(),
		[this](const Enemy& enemy)
		{
			return enemy.health <= 0.0f;
		}),
	mEnemies.end());
}

void Lab2Scene::OnRender()
{
	for (const Turret& turret : mTurrets)
		DrawRect(turret.rec, { 0, 255, 0, 255 });
	
	for (const Enemy& enemy : mEnemies)
		DrawRect(enemy.rec, { 255, 0, 0, 255 });
	
	for (const Bullet& bullet : mBullets)
		DrawRect(bullet.rec, { 0, 0, 255, 255 });
}

AsteroidsScene::AsteroidsScene()
{
	
	mShip.tex = LoadTexture("../Assets/img/enterprise.png");
	sfxPlayerShoot = LoadSound("../Assets/aud/Fire.wav");
	bgmDefault = LoadMusic("../Assets/aud/bgm.mp3");
	sfxShipHit = LoadSound("../Assets/aud/Explode.wav");
	if (bgmDefault == NULL)
	{
		std::cout << "Mix_LoadMUS failed to load file: " << SDL_GetError() << std::endl;
	}
	PlayMusic(bgmDefault, 1);
	mBackground.texBackground = LoadTexture("../Assets/img/background.png");
}

AsteroidsScene::~AsteroidsScene()
{
	UnloadTexture(mBackground.texBackground);
	UnloadTexture(mShip.tex);
	UnloadMusic(bgmDefault);
	UnloadSound(sfxPlayerShoot);
	UnloadSound(sfxShipHit);
}

void AsteroidsScene::OnEnter()
{
	mBackground.position = Point{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
	mBackground.width = 1024.0f;
	mBackground.height = 768.0f;

	mShip.position = Point{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
	mShip.width = mShip.height = 50.0f;
	mShip.angularSpeed = 200.0f * DEG2RAD;
	mShip.bulletCooldown.duration = 0.5f;
	mShip.velocity = { 0,0 };

	mAsteroidTimer.elapsed = 0.0f;
	mAsteroidTimer.duration = 2.5f;


	//SetGuiCallback(OnAsteroidsGui, this);
}

void AsteroidsScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void AsteroidsScene::OnUpdate(float dt)
{
	mShip.velocity.x = mShip.direction.x * mShip.speed;
	mShip.velocity.y = mShip.direction.y * mShip.speed;
	//Cooldowns
	if (mShip.damageCooldown > 0)
	{
		mShip.damageCooldown = mShip.damageCooldown - 1;
	}
	if (mShip.knockbackCooldown > 0)
	{
		mShip.knockbackCooldown = mShip.knockbackCooldown - 1;
	}
	if (mShip.collsionDelay > 0)
	{
		mShip.collsionDelay = mShip.collsionDelay - 1;
	}
	
	if (IsKeyDown(SDL_SCANCODE_A))
	{
		mShip.direction = Rotate(mShip.direction, -mShip.angularSpeed * dt);
	}

	if (IsKeyDown(SDL_SCANCODE_D))
	{
		mShip.direction = Rotate(mShip.direction, mShip.angularSpeed * dt);
	}

	if (IsKeyDown(SDL_SCANCODE_W))
	{	
		if (mShip.acceleration.x < 0.5)
		{
			mShip.acceleration.x += 0.005f;
		}
	}
	else
	{
		if (mShip.acceleration.x > 0)
		{
			mShip.acceleration.x -= 0.005f;
		}
		else if (mShip.acceleration.x < 0)
		{
			mShip.acceleration.x = 0;
		}
	}

	if (IsKeyDown(SDL_SCANCODE_S))
	{	
		if (mShip.acceleration.x > 0)
		{
			mShip.acceleration.x -= 0.04f;
		}
		else if (mShip.acceleration.x < 0)
		{
			mShip.acceleration.x = 0;
		}
	}

	if (IsKeyDown(SDL_SCANCODE_SPACE))
	{
		if (mShip.bulletCooldown.Expired())
		{
			mShip.bulletCooldown.Reset();

			Bullet bullet;
			bullet.width = bullet.height = 50.0f;
			bullet.position = mShip.position + mShip.direction * sqrtf(powf(mShip.width * 0.5f + bullet.width * 0.5f, 2.0f));
			bullet.velocity = mShip.direction * 500.0f;
			bullet.direction = mShip.direction;
			mBullets.push_back(bullet);

			PlaySound(sfxPlayerShoot, 0);
		}
	}

	mShip.bulletCooldown.Tick(dt);

	if (mShip.collsionDelay <= 0)
	{
		mShip.position.x += (mShip.velocity.x * mShip.acceleration.x);
		mShip.position.y += (mShip.velocity.y * mShip.acceleration.x);
	}
	
	
	//Asteriod Collision
	for (Asteroid& asteroid : mAsteroidsLarge)
	{
		Rect asteroidRect = asteroid.Collider();
		Rect shipRect = mShip.Collider();
		if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
		{
			if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
			{
				mShip.velocity.x = 0;
				mShip.velocity.y = 0;
				mShip.acceleration.x = 0;
				mShip.collsionDelay = 10.0f;
			}
			if (mShip.damageCooldown <= 0)
			{
				//damage
				mShip.damageCooldown = 60.0f;
				mShip.health = mShip.health - asteroid.damage;
				//sound
				PlaySound(sfxShipHit, 0);
			}
			//knockback
			if (mShip.knockbackCooldown <= 0)
			{
				float r = 180.0f * DEG2RAD;
				float v = Random(100.0f, 200.0f);
				Point direction = Normalize(asteroid.velocity);
				Point direction1 = Rotate(direction, r);
				asteroid.velocity = direction1 * v;
				mShip.knockbackCooldown = 60.0f;
			}
		}
	}
	for (Asteroid& asteroid : mAsteroidsMedium)
	{
		Rect asteroidRect = asteroid.Collider();
		Rect shipRect = mShip.Collider();
		if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
		{
			if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
			{
				mShip.velocity.x = 0;
				mShip.velocity.y = 0;
				mShip.acceleration.x = 0;
				mShip.collsionDelay = 10.0f;

			}
			if (mShip.damageCooldown <= 0)
			{
				//damage
				mShip.damageCooldown = 60.0f;
				mShip.health = mShip.health - asteroid.damage;
				//sound
				PlaySound(sfxShipHit, 0);
			}

			//knockback
			if (mShip.knockbackCooldown <= 0)
			{
				float r = 180.0f * DEG2RAD;
				float v = Random(100.0f, 200.0f);
				Point direction = Normalize(asteroid.velocity);
				Point direction1 = Rotate(direction, r);
				asteroid.velocity = direction1 * v; 
				mShip.knockbackCooldown = 60.0f;
			}
		}
	}
	for (Asteroid& asteroid : mAsteroidsSmall)
	{
		Rect asteroidRect = asteroid.Collider();
		Rect shipRect = mShip.Collider();
		if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
		{
			if (SDL_HasIntersectionF(&shipRect, &asteroidRect))
			{
				mShip.velocity.x = 0;
				mShip.velocity.y = 0;
				mShip.acceleration.x = 0;
				mShip.collsionDelay = 10.0f;

			}
			if (mShip.damageCooldown <= 0)
			{
				//damage
				mShip.damageCooldown = 60.0f;
				mShip.health = mShip.health - asteroid.damage;
				//sound
				PlaySound(sfxShipHit, 0);
			}

			//knockback
			if (mShip.knockbackCooldown <= 0)
			{
				float r = 180.0f * DEG2RAD;
				float v = Random(100.0f, 200.0f);
				Point direction = Normalize(asteroid.velocity);
				Point direction1 = Rotate(direction, r);
				asteroid.velocity = direction1 * v;
				mShip.knockbackCooldown = 60.0f;
			}
		}
	}
	//Tint
	if (mShip.health < 75.0f && mShip.health > 50.0f)
	{
		Tint(mShip.tex, mShip.col3);
	}
	if (mShip.health < 50.0f && mShip.health > 25.0f)
	{
		Tint(mShip.tex, mShip.col2);
	}
	if (mShip.health < 25.0f && mShip.health > 0.0f)
	{
		Tint(mShip.tex, mShip.col1);
	}
	//Death
	if (mShip.health <= 0)
	{
		mShip.tex = LoadTexture("../Assets/img/explosion.png");
		// Temp Reset
		mShip.health = 100.0f;
	}

	for (Bullet& bullet : mBullets)
	{
		bullet.position = bullet.position + bullet.velocity * dt;
		Rect bulletRect = bullet.Collider();

		// TODO -- spawn 2 medium asteroids if a bullet hits a large asteroid
		for (Asteroid& asteroid : mAsteroidsLarge)
		{
			Rect asteroidRect = asteroid.Collider();
			if (SDL_HasIntersectionF(&bulletRect, &asteroidRect))
			{
				asteroid.health -= bullet.damage;

				Asteroid asteroid1, asteroid2;
				asteroid1.position = asteroid2.position = asteroid.position;
				asteroid1.width = asteroid2.width = mSizeMedium;
				asteroid1.height = asteroid2.height = mSizeMedium;

				float r = Random(30.0f, 45.0f) * DEG2RAD;
				float v = Random(20.0f, 200.0f);
				Point direction = Normalize(bullet.velocity);
				Point direction1 = Rotate(direction, r);
				Point direction2 = Rotate(direction, -r);
				asteroid1.velocity = direction1 * v;
				asteroid2.velocity = direction2 * v;

				// TODO -- take bullet collider and velocity into account when spawning asteroids
				asteroid1.position = asteroid1.position + direction1 * mSizeLarge;
				asteroid2.position = asteroid2.position + direction2 * mSizeLarge;

				mAsteroidsMedium.push_back(asteroid1);
				mAsteroidsMedium.push_back(asteroid2);
			}
		}

		// Spawn 2 small asteroids if a bullet hits a medium asteroid
		for (Asteroid& asteroid : mAsteroidsMedium)
		{
			Rect asteroidRect = asteroid.Collider();
			if (SDL_HasIntersectionF(&bulletRect, &asteroidRect))
			{
				asteroid.health -= bullet.damage;

				Asteroid asteroid1, asteroid2;
				asteroid1.position = asteroid2.position = asteroid.position;
				asteroid1.width = asteroid2.width = mSizeSmall;
				asteroid1.height = asteroid2.height = mSizeSmall;

				float r = Random(30.0f, 45.0f) * DEG2RAD;
				float v = Random(20.0f, 200.0f);
				Point direction = Normalize(bullet.velocity);
				Point direction1 = Rotate(direction,  r);
				Point direction2 = Rotate(direction, -r);
				asteroid1.velocity = direction1 * v;
				asteroid2.velocity = direction2 * v;

				// TODO -- take bullet collider and velocity into account when spawning asteroids
				asteroid1.position = asteroid1.position + direction1 * mSizeMedium;
				asteroid2.position = asteroid2.position + direction2 * mSizeMedium;

				mAsteroidsSmall.push_back(asteroid1);
				mAsteroidsSmall.push_back(asteroid2);
			}
		}
	}

	// TODO -- update and wrap large asteroids. Consider making a physics update function like in AI

	for (Asteroid& asteroid : mAsteroidsLarge)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	for (Asteroid& asteroid : mAsteroidsMedium)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	for (Asteroid& asteroid : mAsteroidsSmall)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	Wrap(mShip);

	//static float tt = 0.0f;
	//float r = cosf(tt + PI * 0.00f) * 0.5f + 0.5f;
	//float g = cosf(tt + PI * 0.33f) * 0.5f + 0.5f;
	//float b = cosf(tt + PI * 0.66f) * 0.5f + 0.5f;
	//Tint(mShip.tex, { Uint8(r * 255.0f), Uint8(g * 255.0f), Uint8(b * 255.0f), Uint8((sinf(tt) * 0.5f + 0.5f) * 255.0f) });
	//tt += dt;

	// Asteroid spawning -- spawn based on time
	if (mAsteroidTimer.Expired())
	{
		mAsteroidTimer.Reset();
		mAsteroidsLarge.push_back(SpawnAsteroid(mSizeLarge));
	}
	mAsteroidTimer.Tick(dt);

	// TODO:
	// Bullet collision - remove if off screen or hitting asteroid
	// Handle small vs medium asteroids accordingly
	// Hint: small.direction = Rotate(medium.direction, Random(30.0f, 45.0f) * DEG2RAD * dt);
	mBullets.erase(remove_if(mBullets.begin(), mBullets.end(), [this](Bullet& bullet)
	{
			// Test if bullet is off-screen first because that's cheaper than testing it against every asteroid
			Rect bulletRect = bullet.Collider();
			if (!SDL_HasIntersectionF(&bulletRect, &SCREEN)) return true;

			// Make this a function if you'd like to remove the duplication in these 3 loops
			for (Asteroid& asteroid : mAsteroidsLarge)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			for (Asteroid& asteroid : mAsteroidsMedium)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			for (Asteroid& asteroid : mAsteroidsSmall)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			return false;
	}), mBullets.end());

	mAsteroidsLarge.erase(remove_if(mAsteroidsLarge.begin(), mAsteroidsLarge.end(), [this](const Asteroid& asteroid)
	{
		return asteroid.health <= 0.0f;
	}), mAsteroidsLarge.end());

	mAsteroidsMedium.erase(remove_if(mAsteroidsMedium.begin(), mAsteroidsMedium.end(), [this](const Asteroid& asteroid)
	{
		return asteroid.health <= 0.0f;
	}), mAsteroidsMedium.end());

	mAsteroidsSmall.erase(remove_if(mAsteroidsSmall.begin(), mAsteroidsSmall.end(), [this](const Asteroid& asteroid)
	{
		return asteroid.health <= 0.0f;
	}), mAsteroidsSmall.end());
}

void AsteroidsScene::OnRender()
{
	mBackground.Draw();
	for (const Asteroid& asteroid : mAsteroidsLarge)
	{
		asteroid.Draw(); 
	}

	for (const Asteroid& asteroid : mAsteroidsMedium)
	{
		asteroid.Draw();
	}

	for (const Asteroid& asteroid : mAsteroidsSmall)
	{
		asteroid.Draw();
	}
		
	for (const Bullet& bullet : mBullets)
	{
		bullet.Draw();
	}

	//DrawRect({ 0, 0, 200, 200 }, mTestColor);
	mShip.Draw();
	
}

void AsteroidsScene::Wrap(Entity& entity)
{
	// Consider offsetting position by half width & half height since position is the centre of an entity
	if (entity.position.x <= 0.0f) entity.position.x = SCREEN_WIDTH;
	else if (entity.position.x >= SCREEN_WIDTH) entity.position.x = 0.0f;
	else if (entity.position.y <= 0.0f) entity.position.y = SCREEN_HEIGHT;
	else if (entity.position.y >= SCREEN_HEIGHT) entity.position.y = 0.0f;
}

AsteroidsScene::Asteroid AsteroidsScene::SpawnAsteroid(float size)
{
	Asteroid asteroid;
	asteroid.width = asteroid.height = size;

	// Ensure asteroid isn't spawned on top of player
	bool collision = true;
	while (collision)
	{
		float x = Random(0.0f, SCREEN_WIDTH - asteroid.width);
		float y = Random(0.0f, SCREEN_HEIGHT - asteroid.height);
		asteroid.position = { x, y };

		Rect asteroidRect = asteroid.Collider();
		Rect shipRect = mShip.Collider();
		shipRect.w *= 4.0f;
		shipRect.h *= 4.0f;
		collision = SDL_HasIntersectionF(&asteroidRect, &shipRect);
	}

	// Add some variance to asteroid movement by shooting them +- 10 degrees towards the player
	Point toPlayer = Normalize(mShip.position - asteroid.position);
	toPlayer = Rotate(toPlayer, Random(-10.0f, 10.0f) * DEG2RAD);
	asteroid.velocity = toPlayer * Random(20.0f, 200.0f);

	return asteroid;
}

void OnAsteroidsGui(void* data)
{
	AsteroidsScene& scene = *(AsteroidsScene*)data;

	static float colors[4]{ 1.0f, 1.0f, 1.0f, 1.0f };	// from 0 to 1
	if (ImGui::ColorPicker4("Ship Color", colors))
	{
		Color color;	// from 0 to 255
		color.r = colors[0] * 255.0f;
		color.g = colors[1] * 255.0f;
		color.b = colors[2] * 255.0f;
		color.a = colors[3] * 255.0f;
		scene.mShip.col3 = color;

		// Blending must be applied separately to textures in SDL
		// See SDL_SetTextureBlendMode
		// If running this on your own, remember that the alpha component must be set manually!
		//Tint(scene.mShip.tex, color);
	}
	
	static float colors2[4]{ 1.0f, 1.0f, 1.0f, 1.0f };	// from 0 to 1
	if (ImGui::ColorPicker4("Rect Color", colors2))
	{
		Color color;	// from 0 to 255
		color.r = colors2[0] * 255.0f;
		color.g = colors2[1] * 255.0f;
		color.b = colors2[2] * 255.0f;
		color.a = colors2[3] * 255.0f;
		scene.mTestColor = color;
	}

	static int blendMode = 0;
	if (ImGui::SliderInt("Blend Mode", &blendMode, 0, 4))
	{
		switch (blendMode)
		{
		case 0:
			BlendMode(SDL_BLENDMODE_NONE);
			break;

		case 1:
			BlendMode(SDL_BLENDMODE_BLEND);
			break;

		case 2:
			BlendMode(SDL_BLENDMODE_ADD);
			break;

		case 3:
			BlendMode(SDL_BLENDMODE_MOD);
			break;

		case 4:
			BlendMode(SDL_BLENDMODE_MUL);
			break;
		}
	}
}
