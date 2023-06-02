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

class ProjectI
{
public:
	std::shared_ptr<OutGame> outGame = nullptr;
	std::shared_ptr<InGame> inGame = nullptr;

	void Init();
};

class GlobalData
{
public:
	
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