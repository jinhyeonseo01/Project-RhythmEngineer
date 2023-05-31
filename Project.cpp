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
#define image_player_idle2 400
#define image_player_Attack1 200
#define image_player_Attack2 300

#define sprite_player_idle 10
#define sprite_player_idle2 11
#define sprite_player_Attack1 15
#define sprite_player_Attack2 16

FMOD::Channel* channel;

TCHAR path[1000];
std::shared_ptr<Sprite> sprite;
std::weak_ptr<Sprite> spriteWeak;
ID2D1Bitmap* image;

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
	
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Player\\Attack2\\", L"", 80, L".png",
		11, image_player_Attack2, sprite_player_Attack2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.73333333);
	
	Resources::SoundLoading(music_Urgency, ".\\Resources\\Sound\\Urgency.mp3");
	Resources::SoundLoading(music_Abiogenesis, ".\\Resources\\Sound\\Abiogenesis.mp3");

	this->inGame = std::make_shared<InGame>();
	this->outGame = std::make_shared<OutGame>();

	GameManager::mainWorld = this->outGame;
}
std::shared_ptr<SpriteRenderer> sr;
void OutGame::Init()
{
	World::Init();
	
	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	mainCameraObject->transform->position = Eigen::Vector2d(960, 540);

	auto obj = GameManager::mainWorld->CreateGameObject();
	sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr->SetSprite(Resources::GetSprite(sprite_player_idle));
	sr->zIndex = 1;
	sr->renderSize = Eigen::Vector2d(450, 360);
	obj->transform->position = Eigen::Vector2d(100, 100);
	/*
	std::shared_ptr<GameObject> obj2 = GameManager::mainWorld->CreateGameObject();
	auto sr2 = obj2->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr2->renderSize = Eigen::Vector2d(1920, 1080);
	sr2->SetSprite(Resources::GetImage(1));
	sr2->pivot = Eigen::Vector2d(0, 0);
	sr2->alpha = 0;
	obj2->transform->position = Eigen::Vector2d(0, 0);
	*/

	GameManager::soundSystem->playSound(Resources::GetSound(music_Urgency), 0, false, &channel);
}
void OutGame::Update()
{
	//sr->gameObject.lock()->transform->rotationZ += 3.0f * GameManager::deltaTime;
	sr->gameObject.lock()->transform->position += (GameManager::mainCamera->ScreenToWorld(InputManager::mousePos) - sr->gameObject.lock()->transform->position) / (5.0f / (GameManager::deltaTime * 60));
	if (InputManager::GetKeyDown('R'))
	{
		sr->SetSprite(Resources::GetSprite(sprite_player_idle2));
		sr->animationLoop = true;
		sr->Play();
	}
	if (InputManager::GetKeyDown('E'))
	{
		sr->SetSprite(Resources::GetSprite(sprite_player_Attack1));
		sr->animationLoop = false;
		sr->Play();
	}

	World::Update();
}

void InGame::Init()
{
	World::Init();


}
void InGame::Update()
{
	World::Update();


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