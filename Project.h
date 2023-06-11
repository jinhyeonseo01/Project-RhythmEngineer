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
#include <locale>
#include <codecvt>

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
class SongData;

class GlobalData
{
public:
	IDWriteTextFormat* scoreFont;
	IDWriteTextFormat* comboFont;
	IDWriteTextFormat* soundFont;
	IDWriteTextFormat* menuFont;
	IDWriteTextFormat* menuRankFont;
	IDWriteTextFormat* menuNameFont;
	IDWriteTextFormat* menuLevelFont;
	IDWriteTextFormat* menuScoreFont;

	ID2D1SolidColorBrush* perfectBrush;
	ID2D1SolidColorBrush* goodBrush;
	ID2D1SolidColorBrush* normalBrush;
	ID2D1SolidColorBrush* badBrush;

	ID2D1SolidColorBrush* SBrush;
	ID2D1SolidColorBrush* ABrush;
	ID2D1SolidColorBrush* BBrush;
	ID2D1SolidColorBrush* CBrush;
	ID2D1SolidColorBrush* DBrush;
	ID2D1SolidColorBrush* FBrush;

	ID2D1SolidColorBrush* whiteBrush;
	ID2D1SolidColorBrush* grayBrush;
};


class ProjectI
{
public:
	std::shared_ptr<OutGame> outGame = nullptr;
	std::shared_ptr<InGame> inGame = nullptr;
	static float hardLatency;
	bool autoAttack = false;
	void Init();

	JsonReader jsonReader;
	GlobalData globalData;
	nlohmann::json songTotalJsonData;
	nlohmann::json songUserJsonData;
	std::vector<SongData> songDataList;
	void LoadSong();
	void PushSong(SongData data);
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

class SongData
{
public:
	nlohmann::json songJson;
	std::wstring name;
	std::vector<NodeData> nodeDataList;
	int level = 0;
	int soundResourceCode = 0;
	int soundImageResourceCode = -1;
	int bestScore = 0;
	int bestCombo = 0;
	bool isLock = false;

	void SetData(nlohmann::json songData);
	void SaveData();
};

class OutGame : public World
{
public:
	virtual void Init();
	virtual void Update();

	float animationTime = -2;
	int animationEventIndex = 0;
	std::weak_ptr<GameObject> animObj1;
	std::weak_ptr<GameObject> animObj2;
};
class InGame : public World
{
public:
	virtual void Init();
	virtual void Update();

	void StartNode(SongData* songData);
	SongData* nowSongData;

	std::shared_ptr<SpriteRenderer> deadLine_L_SR;
	std::shared_ptr<SpriteRenderer> deadLine_R_SR;

	std::vector<NodeData> nodeDataList;
	std::vector<std::weak_ptr<EnemyEntityComponent>> EECList;
	std::shared_ptr<NodeSystem> nodeSystem;

	std::weak_ptr<GameObject> comboObj;
	std::weak_ptr<GameObject> scoreObj;
	std::weak_ptr<GameObject> songObj;

	std::weak_ptr<TextRenderer> comboText;
	std::weak_ptr<TextRenderer> scoreText;
	std::weak_ptr<TextRenderer> songText;

	std::weak_ptr<SpriteRenderer> progressGaugeSR;
	std::weak_ptr<SpriteRenderer> progressBarSR;

	std::vector<std::weak_ptr<SpriteRenderer>> FFTList;

	double globalSpeed = 200;
	double playerHitDistance = 175;
	int playerAttackStack = 0;
	bool playerAttackAnim = false;
	std::chrono::steady_clock::time_point playerAnimTime;
	Eigen::Vector2d playerPosition = Eigen::Vector2d(0,100);

	float power = 27;

	int maxCombo = 0;
	int combo = 0;
	double score = 0;
	double scoreHitAdd = 0;
	double scoreComboAdd = 0;
	int hitPerfact = 0;
	float hitTimeRange = 0.16f;
	float hitBadTimeRange = 0.28f;
	int maxComboLevel = -1;

	int attackDir = 0;

	bool syncMode = false;
	double syncTotalTime = 0;

	int maxNodeLevel = 18;
	int menuListCount = 7;
	int selectMenuIndex = -1;
	float menuAngle = 13 * D2R;
	float menuDistance = 950;
	float menuAlpha = 1;
	bool menuMode = true;

	float menuMusicTermTime = 0;

	FMOD::Channel* menuMusicChannel = NULL;
	Eigen::Vector2d menuOffset = Eigen::Vector2d(0, 0);
	std::vector<std::weak_ptr<GameObject>> menuTableList;
	std::vector<std::weak_ptr<GameObject>> menuTableImageList;
	std::vector<std::weak_ptr<GameObject>> menuTableNameList;
	std::vector<std::vector<std::weak_ptr<GameObject>>> menuLevelList;

	std::weak_ptr<GameObject> menuSongObj;
	std::weak_ptr<GameObject> menuSongInfoObj;
	std::weak_ptr<GameObject> menuSongInfoNameObj;
	std::weak_ptr<GameObject> menuSongInfoLevelObj;
	std::weak_ptr<GameObject> menuSongInfoRankObj;
	std::weak_ptr<GameObject> menuSongInfoBestScoreObj;
	std::weak_ptr<GameObject> menuSongInfoBestScoreValueObj;
	std::weak_ptr<GameObject> menuSongInfoBestComboObj;
	std::weak_ptr<GameObject> menuSongInfoBestComboValueObj;

	std::weak_ptr<GameObject> menuPause;
	std::weak_ptr<GameObject> menuSongStart;
	std::weak_ptr<GameObject> menuSongStartText;

	std::weak_ptr<GameObject> menuSongInfoImageObj;
	std::weak_ptr<GameObject> menuSongInfoImageRectObj;
	std::weak_ptr<GameObject> menuSongInfoImageRect2Obj;
	std::weak_ptr<GameObject> menuSongInfoImageRect3Obj;

	float menuSelectLevel = 0;
	std::vector<std::weak_ptr<GameObject>> menuSelectSongLevelList;
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
			shockPower = Eigen::Vector2d(0, 35);
			break;
		}
		case 10:
		{
			shockPower = Eigen::Vector2d(5 + (rand() % 200)/10.0f, -10 + (-(rand() % 120)/10.0));
			limitTime = 1.5 + (rand() % 15) / 10.0f;
			break;
		}
		case 11:
		{
			shockPower = Eigen::Vector2d(0, 0);
			limitTime = 10;
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
	float shockSpeed = 2.5;
	float shockDirSpeed = 4;
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
			shockPower = shockPower + ((Eigen::Vector2d(0,0) - shockPower) / shockSpeed) * (GameManager::deltaTime * 60);
			trans->position = currentPos + shockPower;

			trans->rotationZ = -shockDir * (shockPower.norm()/ shockDirSpeed *D2R);
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
		case 100:
			shockDirSwing = true;
			shockSwing = true;
			shockDirSpeed = 10;
			shockSpeed = 10;
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
