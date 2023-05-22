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

#include "GameLogic.h"

class GlobalData
{
public:
};

class OutGame : public World
{
public:
	virtual void Init()
	{
		World::Init();

	}
};
class InGame : public World
{
public:
	virtual void Init()
	{
		World::Init();

	}
};

class ProjectI
{
public:
	OutGame outGame;
	InGame inGame;
	void Init();
};