#pragma once

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <vector>
#include <stack>
#include <cmath>
#include <time.h>
#include <atlImage.h>
#include <map>
#include <chrono>
#include <algorithm>

#include <Eigen/Dense>
#include "ConsoleDebug.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include "lib/nlohmann/json.hpp"

#include "GameLogic.h"

class OutGame;
class InGame;
class World;
class EnemyEntityComponent;
class ProjectI;
class GlobalData;
class NodeData;

class GlobalData
{
public:
	IDWriteTextFormat* scoreFont;
	IDWriteTextFormat* comboFont;
	IDWriteTextFormat* soundFont;

	ID2D1SolidColorBrush* perfectBrush;
	ID2D1SolidColorBrush* goodBrush;
	ID2D1SolidColorBrush* normalBrush;
	ID2D1SolidColorBrush* badBrush;

	ID2D1SolidColorBrush* whiteBrush;
};


class ProjectI
{
public:
	std::shared_ptr<OutGame> outGame = nullptr;
	std::shared_ptr<InGame> inGame = nullptr;
	nlohmann::json songJsonData;
	static float hardLatency;
	bool autoAttack = false;
	void Init();

	GlobalData globalData;
};

class NodeData
{
public:
	bool isCreateed = false;
	int id;
	int type;
	double time;
	double speed;
	int direction;
	double reTime;
};

class OutGame : public World
{
public:
	virtual void Init();
	virtual void Update();
};
class InGame : public World
{
public:
	virtual void Init();
	virtual void Update();

	void StartNode(int musicCode, nlohmann::json jsonData);

	std::shared_ptr<SpriteRenderer> deadLine_L_SR;
	std::shared_ptr<SpriteRenderer> deadLine_R_SR;

	std::vector<NodeData> nodeDataList;
	std::vector<std::weak_ptr<EnemyEntityComponent>> EECList;
	std::shared_ptr<NodeSystem> nodeSystem;

	std::weak_ptr<GameObject> comboObj;
	std::weak_ptr<GameObject> scoreObj;
	std::weak_ptr<GameObject> songObj;

	std::weak_ptr<TextComponent> comboText;
	std::weak_ptr<TextComponent> scoreText;
	std::weak_ptr<TextComponent> songText;

	std::weak_ptr<SpriteRenderer> progressGaugeSR;
	std::weak_ptr<SpriteRenderer> progressBarSR;

	Eigen::Vector2d playerPosition = Eigen::Vector2d(0,100);
	float globalSpeed = 200;
	float HitDistance = 175;
	int combo = 0;
	float score = 0;
	float scoreHitAdd = 0;
	float scoreComboAdd = 0;
	int hitPerfact = 0;
};


class TestComponent : public Component
{
public:
	virtual void Update();
	virtual void LateUpdate();
	virtual void BeforeRender();
	virtual void Start();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
};

class EnemyEntityComponent : public Component
{
public:
	int hp = 1;
	Eigen::Vector2d targetPosition;
	Eigen::Vector2d offset;
	std::weak_ptr<Sprite> move;
	std::weak_ptr<Sprite> destroy;

	NodeData nodeData;

	static int TypeToHP(int type);
	void Setting(NodeData data, Eigen::Vector2d targetPosition, Eigen::Vector2d offset);
	void Hit(int hitPre);
	virtual void Update();
	virtual void LateUpdate();
	virtual void BeforeRender();
	virtual void Start();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
};

class EffectComponent : public Component
{
public:
	Eigen::Vector2d targetPosition;
	Eigen::Vector2d shockPower;
	std::chrono::steady_clock::time_point startTime;
	int type = 0;
	int dir = 0;
	float limitTime = 2;

	std::weak_ptr<SpriteRenderer> effectSR;
	void Execute(int type)
	{
		this->type = type;
		switch(this->type)
		{
		case 0:
		{
			shockPower = Eigen::Vector2d(0, 50);
			break;
		}
		case 10:
		{
			shockPower = Eigen::Vector2d(5 + (rand() % 200)/10.0f, -10 + (-(rand() % 120)/10.0));
			limitTime = 1.5 + (rand() % 15) / 10.0f;
			break;
		}
		case 15:
		{
		
			break;
		}
		}
	}
	void SetDirection(int dir)
	{
		this->dir = dir;
	}
	virtual void Update();
	virtual void LateUpdate() {}
	virtual void BeforeRender() {}
	virtual void Start()
	{
		targetPosition = gameObject.lock()->transform->position;
		if (this->type == 10)
			targetPosition += Eigen::Vector2d(-20 + (rand() % 40), -20 + (rand() % 40));
		startTime = std::chrono::steady_clock::now();
	}
	virtual void Enable() {}
	virtual void Disable() {}
	virtual void Destroy() {}
};

class CameraControl : public Component
{
public:
	Eigen::Vector2d targetPosition;
	Eigen::Vector2d shockPower;
	std::weak_ptr<Camera> camera;
	int shockType = 0;
	int shockDir = 0;
	bool shockSwing = true;
	bool shockDirSwing = false;

	std::weak_ptr<GameObject> holyEffectObj;
	std::weak_ptr<SpriteRenderer> holyEffectSR;

	virtual void Start();

	virtual void Update()
	{
		if (!camera.expired())
		{
			std::shared_ptr<Camera> camera = this->camera.lock();
			auto trans = this->gameObject.lock()->transform;
			
			auto currentPos = trans->position + ((targetPosition - trans->position) / 5.0f) * (GameManager::deltaTime * 60);
			if (shockPower.norm() <= 1)
				shockPower = Eigen::Vector2d(0, 0);
			shockPower = shockPower + ((Eigen::Vector2d(0,0) - shockPower) / 2.5f) * (GameManager::deltaTime * 60);
			trans->position = currentPos + shockPower;
			trans->rotationZ = -shockDir * (shockPower.norm()/4.0f*D2R);
			if (shockSwing)
			{
				shockPower.x() = -shockPower.x();
				shockPower.y() = -shockPower.y();
			}
			if (shockDirSwing)
				shockDir = -shockDir;
		}
	}
	void Shock(int type, int shockDir, Eigen::Vector2d shockPower)
	{
		this->shockType = type;
		this->shockPower = shockPower;
		this->shockDir = shockDir;
		switch (type)
		{
		case 0:
		case 1:
			shockDirSwing = false;
			shockSwing = true;
			break;
		case 2:
			shockDirSwing = true;
			shockSwing = true;
			break;
		default:
			shockDirSwing = false;
			shockSwing = true;
			break;
		}
	}

	void HolyEffect(float speed, float alpha, int type);

	virtual void LateUpdate() {}
	virtual void BeforeRender() {}
	virtual void Enable() {}
	virtual void Disable() {}
	virtual void Destroy() {}
};
