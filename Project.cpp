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

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "TotalLoader.h"

#define music_Urgency 0001
#define music_Abiogenesis 0002

#define image_player_idle 100
#define image_player_idle2 1000
#define image_player_Attack1 200
#define image_player_Attack2 300
#define image_player_Attack3 400

#define image_enemy_1_move 2000
#define image_enemy_1_move_hit 2100

#define image_enemy_2_move 3000
#define image_enemy_2_move_hit 3100

#define image_enemy_3_move 4000
#define image_enemy_3_move_hit 4100

#define image_DeadLine_1 10000

#define sprite_player_idle 10
#define sprite_player_idle2 11
#define sprite_player_Attack1 15
#define sprite_player_Attack2 16
#define sprite_player_Attack3 17

#define sprite_enemy_1_move 30
#define sprite_enemy_1_move_hit 35

#define sprite_enemy_2_move 40
#define sprite_enemy_2_move_hit 45

#define sprite_enemy_3_move 50
#define sprite_enemy_3_move_hit 55

#define sprite_DeadLine_1 100

float ProjectI::hardLatency = 0.01f;

FMOD::Channel* channel;

TCHAR path[1000];
std::shared_ptr<Sprite> sprite;
std::weak_ptr<Sprite> spriteWeak;
ID2D1Bitmap* image;

ProjectI* project;

std::weak_ptr<Sprite> SpriteGroupLoad(const TCHAR* mainPath, const TCHAR* SubPath, const TCHAR* FName, int startIndex, const TCHAR* LName, int count, int resourceIndex, int spriteIndex)
{
	sprite = std::make_shared<Sprite>();
	for (int i = 0; i < count; i++)
	{
		_stprintf_s(path, _countof(path), L"%s%s%s%d%s", mainPath, SubPath, FName, i + startIndex, LName);
		image = Resources::ImageLoading(resourceIndex + i, path);
		sprite->PushImage(image);
	}
	sprite->spriteSize = Eigen::Vector2i(image->GetSize().width, image->GetSize().height);
	return Resources::PushSprite(spriteIndex, sprite);
}

void ProjectI::Init()
{
	project = this;

	Resources::ImageLoading(0, L".\\Resources\\Image\\Back.png");
	Resources::ImageLoading(1, L".\\Resources\\Image\\Back4.png");
	
	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Idle\\", L"", 31, L".png",
		40, image_player_idle, sprite_player_idle);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	sprite->time = 0.8f;
	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Idle2\\", L"", 0, L".png",
		31, image_player_idle2, sprite_player_idle2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	sprite->time = 0.8f;
	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Attack1\\", L"", 71, L".png",
		10, image_player_Attack1, sprite_player_Attack1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	sprite->time = 0.25f;

	//-----------------------------------------------------------

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Attack2\\", L"", 81, L".png",
		10, image_player_Attack2, sprite_player_Attack2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	sprite->time = 0.25f;
	//-----------------------------------------------------------

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Attack3\\", L"", 101, L".png",
		20, image_player_Attack3, sprite_player_Attack3);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	sprite->time = 0.5f;
	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy1\\Normal\\", L"Walk", 1, L".png",
		4, image_enemy_1_move, sprite_enemy_1_move);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 0.5f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy2\\Normal\\", L"Enemy", 1, L".png",
		6, image_enemy_2_move, sprite_enemy_2_move);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.75);
	sprite->time = 0.35f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy3\\Normal\\", L"Enemy", 1, L".png",
		4, image_enemy_3_move, sprite_enemy_3_move);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 0.35f;

	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"DeadLine", 0, L".png",
		1, image_DeadLine_1, sprite_DeadLine_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0);
	sprite->time = 1.0f;

	
	Resources::SoundLoading(music_Urgency, ".\\Resources\\Sound\\Urgency.mp3");
	Resources::SoundLoading(music_Abiogenesis, ".\\Resources\\Sound\\Abiogenesis.mp3");

	this->inGame = std::make_shared<InGame>();
	this->outGame = std::make_shared<OutGame>();

	GameManager::mainWorld = this->inGame;
}

std::shared_ptr<SpriteRenderer> sr;
std::shared_ptr<EnemyEntityComponent> enemyScript;

JsonReader jr;
class Data
{
public:
	int a = 10;
	float b = 10;
	std::string s = "Hello";
};
void OutGame::Init()
{
	World::Init();
}

int TestAttackStack = 0;
bool TestAttackAnim = false;
std::chrono::steady_clock::time_point playerAnimTime;

void OutGame::Update()
{
	World::Update();
}



void InGame::Init()
{
	World::Init();

	nodeSystem = std::make_shared<NodeSystem>();

	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	mainCameraObject->transform->position = Eigen::Vector2d(0, 0);

	auto obj = GameManager::mainWorld->CreateGameObject();
	sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr->SetSprite(Resources::GetSprite(sprite_player_idle));
	sr->zIndex = 1;
	sr->renderSize = Eigen::Vector2d(sr->sprite.lock()->spriteSize.x() * 3.0f, sr->sprite.lock()->spriteSize.y() * 3.0f);
	obj->transform->position = playerPosition;
	/*
	std::shared_ptr<GameObject> obj2 = GameManager::mainWorld->CreateGameObject();
	auto sr2 = obj2->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr2->renderSize = Eigen::Vector2d(1920, 1080);
	sr2->SetSprite(Resources::GetImage(1));
	sr2->pivot = Eigen::Vector2d(0, 0);
	sr2->alpha = 0;
	obj2->transform->position = Eigen::Vector2d(0, 0);
	*/

	//--------DeadLine--------

	auto deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_L_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_L_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_1));
	deadLine_L_SR->zIndex = 10;
	deadLine_L_SR->flip.x() = 1;
	deadLine_L_SR->renderSize = Eigen::Vector2d(deadLine_L_SR->sprite.lock()->spriteSize.x() * 3, deadLine_L_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() - HitDistance, playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_1));
	deadLine_R_SR->zIndex = 10;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x() * 3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() + HitDistance, playerPosition.y());

	
	project->songJsonData = jr.Read(".\\Resources\\GameData\\Test.json");
	nlohmann::json jsonData = project->songJsonData["Node"]["Song_A"]["Datas"];
	for (int i = 0; i < jsonData.size(); i++)
	{
		if (!jsonData.at(i)["Type"].is_null())
		{
			NodeData data;

			int type = jsonData.at(i)["Type"].get<int>();
			int id = jsonData.at(i)["ID"].get<int>();
			double time = jsonData.at(i)["Time"].is_null()?-1.0:jsonData.at(i)["Time"].get<double>();
			double speed = jsonData.at(i)["Speed"].is_null() ? 3 : jsonData.at(i)["Speed"].get<double>();
			std::string direction = jsonData.at(i)["Direction"].is_null() ? "L" : jsonData.at(i)["Direction"].get<std::string>();
			double reTime = jsonData.at(i)["ReTime"].is_null() ? 3.0 : jsonData.at(i)["ReTime"].get<double>();

			data.id = id;
			data.type = type;
			data.time = time;
			data.reTime = reTime;
			data.speed = speed;
			if (direction._Equal("L"))
				data.direction = 1;
			if (direction._Equal("R"))
				data.direction = -1;
			nodeDataList.push_back(data);
		}
	}
	//GameManager::soundSystem->playSound(Resources::GetSound(music_Urgency), 0, false, &channel);
}
int attackDir = 0;
void InGame::Update()
{
	World::Update();

	for (int i = 0; i < nodeDataList.size(); i++)
	{
		if (!nodeDataList[i].isCreateed)
		{
			if (nodeDataList[i].time - nodeSystem->GetMusicTime() <= 10.0)
			{
				nodeDataList[i].isCreateed = true;
				
				auto obj = GameManager::mainWorld->CreateGameObject();
				auto sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
				sr->SetSprite(Resources::GetSprite(sprite_enemy_2_move));
				sr->zIndex = 1;
				sr->flip.x() = 0;
				sr->renderSize = Eigen::Vector2d(sr->sprite.lock()->spriteSize.x() * 3.0f, sr->sprite.lock()->spriteSize.y() * 3.0f);
				auto enemyScript2 = obj->AddComponent<EnemyEntityComponent>(std::make_shared<EnemyEntityComponent>());
				enemyScript2->Setting(nodeDataList[i], playerPosition, Eigen::Vector2d(0, 0));
				obj->transform->position = playerPosition;
	
				EECList.push_back(enemyScript2);
			}
		}
	}

	//sr->gameObject.lock()->transform->position += (GameManager::mainCamera->ScreenToWorld(InputManager::mousePos) - sr->gameObject.lock()->transform->position) / (5.0f / (GameManager::deltaTime * 60));
	if (InputManager::GetKeyDown('R'))
	{
		sr->SetSprite(Resources::GetSprite(sprite_player_idle2));
		sr->animationLoop = true;
		sr->Reset();
		sr->Play();
	}

	attackDir = 0;
	if (InputManager::GetKeyDown(VK_RIGHT) || InputManager::GetKeyDown(VK_LEFT))
	{
		//*ConsoleDebug::console << (nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_LEFT)) - (enemyScript->targetTime + ProjectI::hardLatency)) << "\n";
		if (TestAttackStack == 0)
		{
			sr->SetSprite(Resources::GetSprite(sprite_player_Attack1));
			playerAnimTime = std::chrono::steady_clock::now();
			TestAttackAnim = true;
		}
		if (TestAttackStack == 1)
		{
			sr->SetSprite(Resources::GetSprite(sprite_player_Attack2));
			playerAnimTime = std::chrono::steady_clock::now();
			TestAttackAnim = true;
		}
		if (TestAttackStack == 2)
		{
			sr->SetSprite(Resources::GetSprite(sprite_player_Attack3));
			playerAnimTime = std::chrono::steady_clock::now();
			TestAttackAnim = true;
		}
		sr->flip.x() = InputManager::GetKeyDown(VK_RIGHT) ? 0 : 1;
		TestAttackStack++;
		TestAttackStack = TestAttackStack % 3;
		sr->animationLoop = false;
		sr->Reset();
		sr->Play();

		double keyDeltaTime = 0;
		if (InputManager::GetKey(VK_RIGHT))
		{
			attackDir = 1;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_RIGHT));
		}
		else if (InputManager::GetKey(VK_LEFT))
		{
			attackDir = -1;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_LEFT));
		}
		if (attackDir != 0)
		{
			for (int i = 0; i < EECList.size(); i++)
			{
				if (!EECList[i].expired())
				{
					double dTime = (keyDeltaTime - (EECList[i].lock()->nodeData.time + ProjectI::hardLatency));
					if (EECList[i].lock()->nodeData.direction == -attackDir)
					{
						if (EECList[i].lock()->hp > 0)
						{
							if (abs(dTime) <= 0.15)
							{
								*ConsoleDebug::console << (int)EECList.size() << "\n";
								EECList[i].lock()->Hit();
								break;
							}
							else if (abs(dTime) <= 0.25)
							{
								EECList[i].lock()->hp = 0;
								EECList[i].lock()->gameObject.lock()->isDestroy = true;
								EECList.erase(EECList.begin() + i);
								i--;
							}
						}
						else
						{
							EECList.erase(EECList.begin() + i);
							i--;
						}
					}
				}
				else
				{
					EECList.erase(EECList.begin() + i);
					i--;
				}
			}
		}
	}

	for (int i = 0; i < EECList.size(); i++)
	{
		if (!EECList[i].expired())
		{
			double dTime = (nodeSystem->GetMusicTime() - (EECList[i].lock()->nodeData.time + ProjectI::hardLatency));
			if (abs(dTime) > 0.2f && dTime > 0)
			{
				//EECList[i].lock()->hp = 1;
				//EECList[i].lock()->Hit();
				//EECList.erase(EECList.begin() + i);
				//i--;
			}
		}
		else
		{
			EECList.erase(EECList.begin() + i);
			i--;
		}
	}
	if (InputManager::GetKeyDown('T'))
	{
		nodeSystem->Start(Resources::GetSound(music_Urgency));
	}
	if (InputManager::GetKeyDown('Y'))
	{
		nodeSystem->Pause();
	}
	if (InputManager::GetKeyDown('U'))
	{
		nodeSystem->DisPause();
	}
	if (InputManager::GetKeyDown('I'))
	{
		nodeSystem->Stop();
	}

	if (TestAttackAnim)
	{
		if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - playerAnimTime).count() / 1000000.0 >= 0.51)
		{
			TestAttackAnim = false;
			TestAttackStack = 0;
			sr->SetSprite(Resources::GetSprite(sprite_player_idle));
			sr->animationLoop = true;
			sr->Reset();
			sr->Play();
		}
	}

	//*ConsoleDebug::console << nodeSystem->GetMusicTime() << "\n" << "\n";
}

void TestComponent::Enable()
{
	Component::Enable();
	this->gameObject.lock()->Destroy();
}

void TestComponent::Disable()
{
	Component::Disable();
	this->gameObject.lock()->isActive = true;
}

void TestComponent::Update()
{
	Component::Update();
}
void TestComponent::LateUpdate()
{
	Component::LateUpdate();
}
void TestComponent::Start()
{
	Component::Start();
	this->gameObject.lock()->isActive = false;
}
void TestComponent::BeforeRender()
{
	Component::BeforeRender();
}
void TestComponent::Destroy()
{
	Component::Destroy();
}

//--------

void EnemyEntityComponent::Enable()
{
	Component::Enable();
}

void EnemyEntityComponent::Disable()
{
	Component::Disable();
}

void EnemyEntityComponent::Update()
{
	Component::Update();
	auto mySR = this->gameObject.lock()->GetComponent<SpriteRenderer>();
	if(nodeData.direction == 1)
		mySR->flip.x() = 0;
	if (nodeData.direction == -1)
		mySR->flip.x() = 1;
	if (nodeData.type == 0)
	{
		mySR->SetSprite(Resources::GetSprite(sprite_enemy_2_move));
		offset.y() = 0;
	}
	if (nodeData.type == 1)
	{
		mySR->SetSprite(Resources::GetSprite(sprite_enemy_3_move));
		offset.y() = -50;
	}
	if (nodeData.type == 2)
	{
		mySR->SetSprite(Resources::GetSprite(sprite_enemy_1_move));
		offset.y() = -75;
	}
	
	mySR->renderSize = Eigen::Vector2d(mySR->sprite.lock()->spriteSize.x() * 3.0f, mySR->sprite.lock()->spriteSize.y() * 3.0f);

	float dTime = this->nodeData.time - project->inGame->nodeSystem->GetMusicTime();
	float totalSpeed = this->nodeData.speed * project->inGame->globalSpeed;
	float hitDistance = project->inGame->HitDistance;
	gameObject.lock()->transform->position = Eigen::Vector2d(targetPosition.x() + ((dTime * totalSpeed) + hitDistance) * -this->nodeData.direction, targetPosition.y() + offset.y());

	if ((targetPosition.x() - gameObject.lock()->transform->position.x()) * this->nodeData.direction < 0)
	{
		this->gameObject.lock()->isDestroy = true;
	}
}

void EnemyEntityComponent::LateUpdate()
{
	Component::LateUpdate();
}
void EnemyEntityComponent::Start()
{
	Component::Start();
}
void EnemyEntityComponent::BeforeRender()
{
	Component::BeforeRender();
}
void EnemyEntityComponent::Destroy()
{
	Component::Destroy();
}

void EnemyEntityComponent::Setting(NodeData data, Eigen::Vector2d targetPosition, Eigen::Vector2d offset)
{
	this->nodeData = data;
	this->offset = offset;
	this->targetPosition = targetPosition;
	
	if (this->nodeData.type == 0)
		this->hp = 1;
	if (this->nodeData.type == 1)
		this->hp = 2;
	if (this->nodeData.type == 2)
		this->hp = 2;
}

void EnemyEntityComponent::Hit()
{
	(this->hp)--;
	if (this->hp > 0)
	{
		if (this->nodeData.type == 1)
		{
			nodeData.time += nodeData.reTime;
		}
		if (this->nodeData.type == 2)
		{
			nodeData.direction = -nodeData.direction;
			nodeData.time += nodeData.reTime;
		}
	}
	else
	{
		this->gameObject.lock()->isDestroy = true;
	}
}
/*
void Component::Enable()
{
	Component::Enable();
}

void Component::Disable()
{
	Component::Disable();
}

void Component::Update()
{
	Component::Update();
}
void Component::LateUpdate()
{
	Component::LateUpdate();
}
void Component::Start()
{
	Component::Start();
}
void Component::BeforeRender()
{
	Component::BeforeRender();
}
void Component::Destroy()
{
	Component::Destroy();
}
*/