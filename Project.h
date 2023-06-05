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

class ProjectI
{
public:
	std::shared_ptr<OutGame> outGame = nullptr;
	std::shared_ptr<InGame> inGame = nullptr;
	nlohmann::json songJsonData;
	static float hardLatency;
	void Init();
};

class GlobalData
{
public:
	
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

	std::shared_ptr<SpriteRenderer> deadLine_L_SR;
	std::shared_ptr<SpriteRenderer> deadLine_R_SR;

	std::vector<NodeData> nodeDataList;
	std::vector<std::weak_ptr<EnemyEntityComponent>> EECList;
	std::shared_ptr<NodeSystem> nodeSystem;

	Eigen::Vector2d playerPosition = Eigen::Vector2d(0,100);
	float globalSpeed = 200;
	float HitDistance = 175;
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

	void Setting(NodeData data, Eigen::Vector2d targetPosition, Eigen::Vector2d offset);
	void Hit();
	virtual void Update();
	virtual void LateUpdate();
	virtual void BeforeRender();
	virtual void Start();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
};