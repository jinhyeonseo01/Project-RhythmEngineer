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
#define music_Gloxinia_by_Ruxxi 0003

#define Sound_Attack_1 0100
#define Sound_Attack_2 0101
#define Sound_Attack_3 0102

#define Sound_Hit_1 0110

#define image_player_idle 100
#define image_player_idle2 1000
#define image_player_Attack1 200
#define image_player_Attack2 300
#define image_player_Attack3 400

#define image_enemy_1_move 2000
#define image_enemy_1_move_hit 2100
#define image_enemy_1_part 2150

#define image_enemy_2_move 3000
#define image_enemy_2_move_hit 3100
#define image_enemy_2_part 3150

#define image_enemy_3_move 4000
#define image_enemy_3_move_hit 4100
#define image_enemy_3_part 4150

#define image_DeadLine_1 10000
#define image_DeadLine_2 10002
#define image_DeadLine_3 10003
#define image_DeadLine_4 10004

#define image_ProgressBar_1 10010
#define image_ProgressGauge_1 10020
#define image_ViewEffect_White 11020
#define image_ViewEffect_Red 11030

#define image_Background_1 20010
#define image_Background_2 20020
#define image_Vignette_1 20100

#define image_FFT_1 30000

#define sprite_player_idle 10
#define sprite_player_idle2 11
#define sprite_player_Attack1 15
#define sprite_player_Attack2 16
#define sprite_player_Attack3 17

#define sprite_enemy_1_move 30
#define sprite_enemy_1_move_hit 35
#define sprite_enemy_1_Part 37

#define sprite_enemy_2_move 40
#define sprite_enemy_2_move_hit 45
#define sprite_enemy_2_Part 47

#define sprite_enemy_3_move 50
#define sprite_enemy_3_move_hit 55
#define sprite_enemy_3_Part 57

#define sprite_DeadLine_1 100
#define sprite_DeadLine_2 102
#define sprite_DeadLine_3 103
#define sprite_DeadLine_4 104
#define sprite_ProgressBar_1 105
#define sprite_ProgressGauge_1 110
#define sprite_ViewEffect_White 200
#define sprite_ViewEffect_Red 201

#define sprite_FFT_1 1001

#define sprite_Background_1 501
#define sprite_Background_2 502

float ProjectI::hardLatency = 0.01f;

FMOD::Channel* channel;

TCHAR path[1000];
std::shared_ptr<Sprite> sprite;
std::weak_ptr<Sprite> spriteWeak;
ID2D1Bitmap* image;

ProjectI* project;
TCHAR tempText[MAX_TEXT_SIZE];

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

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy1\\Part\\", L"Part", 1, L".png",
		21, image_enemy_1_part, sprite_enemy_1_Part);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1000.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy2\\Part\\", L"Part", 1, L".png",
		16, image_enemy_2_part, sprite_enemy_2_Part);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1000.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Enemy3\\Part\\", L"Part", 1, L".png",
		26, image_enemy_3_part, sprite_enemy_3_Part);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1000.0f;

	//-----------------------------------------------------------
	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"DeadLine", 0, L".png",
		1, image_DeadLine_1, sprite_DeadLine_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"DeadLine", 2, L".png",
		1, image_DeadLine_2, sprite_DeadLine_2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 1);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"DeadLine", 3, L".png",
		1, image_DeadLine_3, sprite_DeadLine_3);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 1);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"DeadLine", 4, L".png",
		1, image_DeadLine_4, sprite_DeadLine_4);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 1);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Background_Floor_", 1, L".png",
		1, image_Background_1, sprite_Background_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Background_", 1, L".png",
		1, image_Background_2, sprite_Background_2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"ProgressBar", 0, L".png",
		1, image_ProgressBar_1, sprite_ProgressBar_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"ProgressGauge", 0, L".png",
		1, image_ProgressGauge_1, sprite_ProgressGauge_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"ViewEffect_White", 0, L".png",
		1, image_ViewEffect_White, sprite_ViewEffect_White);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"ViewEffect_Red", 0, L".png",
		1, image_ViewEffect_Red, sprite_ViewEffect_Red);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Vignette_", 0, L".png",
		1, image_Vignette_1, sprite_Vignette_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"FFT_Effect_", 1, L".png",
		1, image_FFT_1, sprite_FFT_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0, 0);
	sprite->time = 1.0f;

	Resources::SoundLoading(music_Urgency, ".\\Resources\\Sound\\Urgency.mp3");
	Resources::SoundLoading(music_Abiogenesis, ".\\Resources\\Sound\\Abiogenesis.mp3");
	Resources::SoundLoading(music_Gloxinia_by_Ruxxi, ".\\Resources\\Sound\\Gloxinia by Ruxxi.mp3");

	Resources::SoundLoading(Sound_Attack_1, ".\\Resources\\Sound\\Attack\\Attack1.mp3");
	Resources::SoundLoading(Sound_Attack_2, ".\\Resources\\Sound\\Attack\\Attack2.mp3");
	Resources::SoundLoading(Sound_Attack_3, ".\\Resources\\Sound\\Attack\\Attack3.mp3");
	Resources::SoundLoading(Sound_Hit_1, ".\\Resources\\Sound\\Hit\\Hit1.ogg");

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(246 / 255.0, 241 / 255.0, 134.0 / 255.0, 1.0)
		), &(project->globalData.perfectBrush));
	
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(100 / 255.0, 155 / 255.0, 255.0 / 255.0, 1.0)
		), &(project->globalData.goodBrush));

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(188 / 255.0, 183 / 255.0, 190.0 / 255.0, 1.0)
		), &(project->globalData.normalBrush));

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(220 / 255.0, 58 / 255.0, 73.0 / 255.0, 1.0)
		), &(project->globalData.badBrush));

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF::White
		), &(project->globalData.whiteBrush));

	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		108.0f,
		L"ko-kr",
		&project->globalData.comboFont);
	
	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		72.0f,
		L"ko-kr",
		&project->globalData.scoreFont);
	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		34.0f,
		L"ko-kr",
		&project->globalData.soundFont);


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

int WindowSize = 256;
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
	auto CC = mainCameraObject->AddComponent<CameraControl>(std::make_shared<CameraControl>());
	CC->targetPosition = Eigen::Vector2d(0, 0);
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

	songObj = GameManager::mainWorld->CreateGameObject();
	songText = songObj.lock()->AddComponent<TextComponent>(std::make_shared<TextComponent>());
	songObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y()+300);
	songText.lock()->SetFont(project->globalData.soundFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	songText.lock()->SetText(L"Cytus2 : Urgency-SIHanatsuka");
	songText.lock()->SetBrush(project->globalData.whiteBrush);
	songText.lock()->zIndex = -5;

	comboObj = GameManager::mainWorld->CreateGameObject();
	comboText = comboObj.lock()->AddComponent<TextComponent>(std::make_shared<TextComponent>());
	comboObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y() + -250);
	comboText.lock()->SetFont(project->globalData.comboFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	comboText.lock()->SetBrush(project->globalData.perfectBrush);
	comboText.lock()->zIndex = -5;
	comboObj.lock()->AddComponent<EffectComponent>(std::make_shared<EffectComponent>());

	scoreObj = GameManager::mainWorld->CreateGameObject();
	scoreText = scoreObj.lock()->AddComponent<TextComponent>(std::make_shared<TextComponent>());
	scoreObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y() + 130);
	scoreText.lock()->SetFont(project->globalData.scoreFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	scoreText.lock()->SetText(L"000,000");
	scoreText.lock()->SetBrush(project->globalData.whiteBrush);
	scoreText.lock()->zIndex = -5;
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

	
	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_L_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_L_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_4));
	deadLine_L_SR->zIndex = -10;
	deadLine_L_SR->flip.x() = 1;
	deadLine_L_SR->renderSize = Eigen::Vector2d(deadLine_L_SR->sprite.lock()->spriteSize.x() * 3, deadLine_L_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() - HitDistance, playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_4));
	deadLine_R_SR->zIndex = -10;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x() * 3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() + HitDistance, playerPosition.y());
	
	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_Background_1));
	deadLine_R_SR->zIndex = -99;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x() * 3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_Background_2));
	deadLine_R_SR->zIndex = -100;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x()*3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	auto PP_OR = deadLineObj->AddComponent<OverlayRenderer>(std::make_shared<OverlayRenderer>());
	PP_OR->SetSprite(Resources::GetSprite(sprite_Vignette_1));
	PP_OR->zIndex = 80;
	PP_OR->alphaValue = 0.87f;
	deadLineObj->transform->position = Eigen::Vector2d(0, 0);

	auto progressBarObj = GameManager::mainWorld->CreateGameObject();
	progressBarSR = progressBarObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	progressBarSR.lock()->SetSprite(Resources::GetSprite(sprite_ProgressBar_1));
	progressBarSR.lock()->zIndex = 10;
	progressBarSR.lock()->renderSize = Eigen::Vector2d(progressBarSR.lock()->sprite.lock()->spriteSize.x() * 3, progressBarSR.lock()->sprite.lock()->spriteSize.y() * 3);
	progressBarObj->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y()+200);

	auto progressGaugeObj = GameManager::mainWorld->CreateGameObject();
	progressGaugeSR = progressGaugeObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	progressGaugeSR.lock()->SetSprite(Resources::GetSprite(sprite_ProgressGauge_1));
	progressGaugeSR.lock()->zIndex = 10;
	progressGaugeSR.lock()->renderSize = Eigen::Vector2d(progressGaugeSR.lock()->sprite.lock()->spriteSize.x() * 3, progressGaugeSR.lock()->sprite.lock()->spriteSize.y() * 3);
	progressGaugeObj->transform->position = Eigen::Vector2d(progressBarObj->transform->position.x() - (progressBarSR.lock()->renderSize.x() - 12.0) / 2, playerPosition.y() + 200);

	for (int i = 0; i < WindowSize; i++)
	{
		deadLineObj = GameManager::mainWorld->CreateGameObject();
		deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
		deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_FFT_1));
		deadLine_R_SR->zIndex = -50;
		deadLine_R_SR->renderSize = Eigen::Vector2d(GameManager::viewSize.right/((float)WindowSize) + 1, 0);
		FFTList.push_back(deadLine_R_SR);
		deadLineObj->transform->position = Eigen::Vector2d(((playerPosition.x() - GameManager::viewSize.right / 2.0)) + (i * (GameManager::viewSize.right / (float)WindowSize)), playerPosition.y() + 3);
		//deadLineObj->transform->position = Eigen::Vector2d(0, 0);
	}

	project->songJsonData = jr.Read(".\\Resources\\GameData\\Test.json");
	//GameManager::soundSystem->playSound(Resources::GetSound(music_Urgency), 0, false, &channel);
}


FMOD::DSP* dsp;
FMOD_DSP_PARAMETER_FFT* data = NULL;
void InGame::StartNode(int musicCode, std::string name, nlohmann::json jsonData)
{
	float ChanPitch;
	
	FMOD::ChannelGroup* master;
	GameManager::soundSystem->getMasterChannelGroup(&master);
	GameManager::channelGroup = master;
	GameManager::soundSystem->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
	//result = channel->addDSP(FMOD_DSP_PARAMETER_DATA_TYPE_FFT, dsp);
	master->addDSP(0, dsp);
	dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_HANNING);
	dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, WindowSize);
	dsp->setBypass(false);
	dsp->setActive(true);

	nodeSystem->Start(Resources::GetSound(musicCode));
	nodeDataList.clear();

	//result = LowlevelSystem->createDSPByType(FMOD_DSP_TYPE_FFT, &fftDSP);
	//result = channelGroup->addDSP(0 /* = DSP 체인의 헤드. */, fftDSP);
	
	//void* data;
	//unsigned int length;
	//result = fft->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, &data, &length, 0, 0);

	songText.lock()->SetText(std::wstring(name.begin(), name.end()).c_str());

	for (int i = 0; i < jsonData.size(); i++)
	{
		if (!jsonData.at(i)["Type"].is_null())
		{
			NodeData data;

			int type = jsonData.at(i)["Type"].get<int>();
			int id = jsonData.at(i)["ID"].get<int>();
			double time = jsonData.at(i)["Time"].is_null() ? -1.0 : jsonData.at(i)["Time"].get<double>();
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
	for (int i = 0; i < EECList.size(); i++)
		EECList[i].lock()->gameObject.lock()->isDestroy = true;
	EECList.clear();
	int count = 0;
	for (int i = 0; i < nodeDataList.size(); i++)
		count += EnemyEntityComponent::TypeToHP(nodeDataList[i].type);
	scoreHitAdd = (100000.0 / count);
	scoreComboAdd = 0;
	score = 0;
	combo = 0;
	maxComboLevel = -1;
	hitPerfact = 0;
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
	progressGaugeSR.lock()->renderSize = Eigen::Vector2d((progressBarSR.lock()->renderSize.x() - (4.0 * 3)) * min((nodeSystem->GetMusicTime() / nodeSystem->GetMusicTotalTime()), 1.0), progressGaugeSR.lock()->renderSize.y());

	//sr->gameObject.lock()->transform->position += (GameManager::mainCamera->ScreenToWorld(InputManager::mousePos) - sr->gameObject.lock()->transform->position) / (5.0f / (GameManager::deltaTime * 60));
	if (InputManager::GetKeyDown('L'))
	{
		project->autoAttack = !project->autoAttack;
	}
	
	dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)(&data), 0, 0, 0);

	if (data != nullptr)
	{
		//*ConsoleDebug::console << data->spectrum[0][10] << "\n";
		//*ConsoleDebug::console << data->length << "\n";
		for (int i = 0; i < data->numchannels; i++)
		{
			float prevH = 0;
			for (int i = 0; i < FFTList.size()/2; i++)
			{
				float h, nextH = 0;
				if (i < FFTList.size() - 1)
					nextH = data->spectrum[0][i+1];
				h = data->spectrum[0][i];
				h = sqrt(sqrt(h)) * 400;
				float h2 = (abs(h - prevH) * 0.5 + abs(h - nextH) * 0.5);

				float prevX = FFTList[i].lock()->renderSize.x();
				float prevY = FFTList[i].lock()->renderSize.y();
				FFTList[i].lock()->renderSize = Eigen::Vector2d(prevX,
					prevY + (h2 - prevY) / 5.0);
				prevH = h;
			}

			prevH = 0;
			for (int i = 0; i < FFTList.size()/2; i++)
			{
				float h, nextH = 0;
				if (i < FFTList.size() - 1)
					nextH = data->spectrum[0][i + 1];
				h = data->spectrum[0][i];
				h = sqrt(sqrt(h)) * 400;
				float h2 = (abs(h - prevH) * 0.5 + abs(h - nextH) * 0.5);

				float prevX = FFTList[WindowSize / 2 + (FFTList.size() / 2 - 1 - i)].lock()->renderSize.x();
				float prevY = FFTList[WindowSize / 2 + (FFTList.size() / 2 - 1 - i)].lock()->renderSize.y();
				FFTList[WindowSize/2 + (FFTList.size()/2 - 1 - i)].lock()->renderSize = Eigen::Vector2d(prevX,
					prevY + (h2 - prevY) / 5.0);
				prevH = h;
			}
		}
		//*ConsoleDebug::console << data->numchannels << "\n";
	}
	
	if(data == nullptr || !nodeSystem->isStart)
	{
		for (int i = 0; i < FFTList.size(); i++)
		{
			float prevX = FFTList[i].lock()->renderSize.x();
			FFTList[i].lock()->renderSize = Eigen::Vector2d(prevX, 0);
		}
	}

	//GameManager::mainCamera->gameObject.lock()->
	//	GetComponent<CameraControl>()->targetPosition += Eigen::Vector2d(10, 10);

	//*ConsoleDebug::console << (GameManager::mainCamera->gameObject.lock()->
	//	GetComponent<CameraControl>()->targetPosition.x()) << "\n";
	attackDir = 0;
	if (InputManager::GetKeyDown(VK_RIGHT) || InputManager::GetKeyDown(VK_LEFT) || InputManager::GetKeyDown(VK_DOWN) || project->autoAttack)
	{
		//*ConsoleDebug::console << (nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_LEFT)) - (enemyScript->targetTime + ProjectI::hardLatency)) << "\n";
		double keyDeltaTime = 0;
		
		float power = 22;
		float hitTimeRange = 0.16f;
		float hitBadTimeRange = 0.28f;

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
		else if (InputManager::GetKey(VK_DOWN))
		{
			attackDir = 2;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_DOWN));
		}

		if (project->autoAttack) // autoAttack
		{
			float autoHitPer = 0.035;
			attackDir = 0;
			keyDeltaTime = nodeSystem->GetDeltaTime(std::chrono::steady_clock::now());

			double minLeftTime = 1000000.0;
			double minRightTime = 1000000.0;

			for (int i = 0; i < EECList.size(); i++)
			{
				if (!EECList[i].expired())
				{
					double dTime = (keyDeltaTime - EECList[i].lock()->nodeData.time);
					if (EECList[i].lock()->nodeData.direction == 1)
					{
						if (minLeftTime > abs(dTime))
							minLeftTime = abs(dTime);
					}
					else
					{
						if (minRightTime > abs(dTime))
							minRightTime = abs(dTime);
					}
				}
			}
			if (minLeftTime == 1000000.0 && minRightTime == 1000000.0)
			{
				attackDir = 0;
			}
			else if (minLeftTime != 1000000.0 && minRightTime != 1000000.0)
			{
				if (minLeftTime <= autoHitPer && minRightTime <= autoHitPer && abs(minLeftTime - minRightTime) <= 0.035)
					attackDir = 2;
				else if (minLeftTime < minRightTime)
				{
					if (minLeftTime <= autoHitPer)
						attackDir = -1;
				}
				else
				{
					if (minRightTime <= autoHitPer)
						attackDir = 1;
				}
			}
			else if (minLeftTime != 1000000.0)
			{
				if(minLeftTime <= autoHitPer)
					attackDir = -1;
			}
			else if (minRightTime != 1000000.0)
			{
				if (minRightTime <= autoHitPer)
					attackDir = 1;
			}
		}

		//--------Player Animation--------
		if (attackDir == -1 || attackDir == 1)
		{
			if (TestAttackStack == 0)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack1));
				playerAnimTime = std::chrono::steady_clock::now();
				TestAttackAnim = true;

				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, 0, -Eigen::Vector2d(power * attackDir, -power));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
				channel->setPitch(0.75f);
			}
			if (TestAttackStack == 1)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack2));
				playerAnimTime = std::chrono::steady_clock::now();
				TestAttackAnim = true;
				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, 0, -Eigen::Vector2d(power * 1.5 * attackDir, power / 0.8f));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_2), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.6f);
				channel->setPitch(0.75f);
			}
			if (TestAttackStack == 2)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack3));
				playerAnimTime = std::chrono::steady_clock::now();
				TestAttackAnim = true;
				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, attackDir, Eigen::Vector2d(0, -power * 2));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_3), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.6f);
				channel->setPitch(0.75f);
			}
			TestAttackStack++;
			TestAttackStack = TestAttackStack % 3;
			sr->animationLoop = false;
			sr->flip.x() = (attackDir == 1) ? 0 : 1;
			sr->Reset();
			sr->Play();
		}
		else if (attackDir == 2)
		{
			TestAttackStack = 0;

			sr->SetSprite(Resources::GetSprite(sprite_player_idle2));
			playerAnimTime = std::chrono::steady_clock::now();
			TestAttackAnim = true;
			GameManager::mainCamera->gameObject.lock()->
				GetComponent<CameraControl>()->Shock(2, attackDir, Eigen::Vector2d(0, -power * 2));
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_3), 0, false, &channel);
			channel->setVolume(GameManager::masterSound* GameManager::SFXSound * 0.6f);
			channel->setPitch(0.75f);
			sr->Reset();
			sr->Play();
		}
		
		//--------Hit Check--------
		if (attackDir == -1 || attackDir == 1 || attackDir == 2)
		{
			bool leftHit = false;
			bool rightHit = false;
			*ConsoleDebug::console << nodeSystem->GetMusicTime() << "\n";
			for (int i = 0; i < EECList.size(); i++)
			{
				if (!EECList[i].expired())
				{
					double dTime = (keyDeltaTime - (EECList[i].lock()->nodeData.time + ProjectI::hardLatency));
					if (EECList[i].lock()->nodeData.direction == -attackDir || attackDir == 2)
					{
						if (leftHit && EECList[i].lock()->nodeData.direction >= 0 ? true : false)
							continue;
						if (rightHit && EECList[i].lock()->nodeData.direction < 0 ? true : false)
							continue;
						if (EECList[i].lock()->hp > 0)
						{
							if (abs(dTime) <= hitTimeRange)
							{
								if (!leftHit)
									leftHit = EECList[i].lock()->nodeData.direction >= 0 ? true : false;
								if (!rightHit)
									rightHit = EECList[i].lock()->nodeData.direction < 0 ? true : false;

								if (abs(dTime) < hitTimeRange / 3)
									hitPerfact = 0;
								else if (abs(dTime) < hitTimeRange * (2.0/3))
									hitPerfact = 1;
								else
									hitPerfact = 2;

								EECList[i].lock()->Hit(hitPerfact);

								FMOD::Channel* channel;
								GameManager::soundSystem->playSound(Resources::GetSound(Sound_Hit_1), 0, false, &channel);
								channel->setVolume(GameManager::masterSound* GameManager::SFXSound * 0.75f);
								combo++;
								comboObj.lock()->GetComponent<EffectComponent>()->Execute(0);
								GameManager::mainCamera->gameObject.lock()->
									GetComponent<CameraControl>()->HolyEffect(14.0, 0.10f, 0);
								score += scoreHitAdd * (0.5 + 0.5 * ((2.0 - hitPerfact) / 2.0f));

								//*ConsoleDebug::console << hitPerfact << "\n";

								if(attackDir == 2 && leftHit && rightHit)
									break;
								if (attackDir == -1 || attackDir == 1)
									break;
							}
							else if (abs(dTime) <= hitBadTimeRange)
							{
								if (!leftHit)
									leftHit = EECList[i].lock()->nodeData.direction >= 0 ? true : false;
								if (!rightHit)
									rightHit = EECList[i].lock()->nodeData.direction < 0 ? true : false;

								EECList[i].lock()->Hit(0);
								combo = 0;
								GameManager::mainCamera->gameObject.lock()->
									GetComponent<CameraControl>()->HolyEffect(12.0, 0.15f, 1);
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

			if (attackDir == 2 && (!leftHit || !rightHit))
			{
				combo = 0;
				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->HolyEffect(12.0, 0.15f, 1);
			}
		}
	}

	if (combo != 0)
	{
		if (hitPerfact == 0)
			comboText.lock()->SetBrush(project->globalData.perfectBrush);
		if (hitPerfact == 1)
			comboText.lock()->SetBrush(project->globalData.goodBrush);
		if (hitPerfact == 2)
			comboText.lock()->SetBrush(project->globalData.normalBrush);

		_stprintf_s(tempText, L"%d", combo);
		comboText.lock()->SetText(tempText);
	}
	else
	{
		comboText.lock()->SetText(L"");
	}


	if (maxComboLevel < hitPerfact)
	{
		maxComboLevel = hitPerfact;
		if (maxComboLevel == 0)
			scoreText.lock()->SetBrush(project->globalData.perfectBrush);
		if (maxComboLevel == 1)
			scoreText.lock()->SetBrush(project->globalData.goodBrush);
		if (maxComboLevel == 2)
			scoreText.lock()->SetBrush(project->globalData.whiteBrush);
	}
	_stprintf_s(tempText, L"%03d,%03d", (int)((int)round(score) / 1000), (int)round(score) % 1000);
	scoreText.lock()->SetText(tempText);

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
		StartNode(music_Urgency, project->songJsonData["Node"]["Song_A"]["Name"].get<std::string>(), project->songJsonData["Node"]["Song_A"]["Datas"]);
	}
	if (InputManager::GetKeyDown('G'))
	{
		StartNode(music_Gloxinia_by_Ruxxi, project->songJsonData["Node"]["Song_B"]["Name"].get<std::string>(), project->songJsonData["Node"]["Song_B"]["Datas"]);
	}
	if (InputManager::GetKeyDown('Y'))
	{
		nodeSystem->Pause();
		*ConsoleDebug::console << project->inGame->nodeSystem->GetMusicTime() << "\n";
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
		project->inGame->combo = 0;
		GameManager::mainCamera->gameObject.lock()->
			GetComponent<CameraControl>()->HolyEffect(12.0, 0.15f, 1);
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
	this->hp = TypeToHP(this->nodeData.type);
}

int EnemyEntityComponent::TypeToHP(int type)
{
	if (type == 0)
		return 1;
	if (type == 1)
		return 2;
	if (type == 2)
		return 2;
}

void EnemyEntityComponent::Hit(int hitPer)
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

		int partCount = 10;
		if (this->nodeData.type == 0)
			partCount = 10;
		if (this->nodeData.type == 1)
			partCount = 18;
		if (this->nodeData.type == 2)
			partCount = 26;
		for (int i = 0; i < partCount; i++)
		{
			auto obj = GameManager::mainWorld->CreateGameObject();
			auto sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			if (this->nodeData.type == 0)
				sr->SetSprite(Resources::GetSprite(sprite_enemy_1_Part));
			if (this->nodeData.type == 1)
				sr->SetSprite(Resources::GetSprite(sprite_enemy_2_Part));
			if (this->nodeData.type == 2)
				sr->SetSprite(Resources::GetSprite(sprite_enemy_3_Part));
			sr->zIndex = -2;
			sr->flip.x() = 0;
			sr->animationTime = sr->sprite.lock()->time * ((rand()%999)/1000.0);
			sr->renderSize = Eigen::Vector2d(sr->sprite.lock()->spriteSize.x() * 3.0f, sr->sprite.lock()->spriteSize.y() * 3.0f);
			sr->Play();
			auto effectScript = obj->AddComponent<EffectComponent>(std::make_shared<EffectComponent>());
			effectScript->Execute(10);
			effectScript->SetDirection(-this->nodeData.direction);
			obj->transform->position = this->gameObject.lock()->transform->position;
		}
	}
}

void EffectComponent::Update()
{
	auto trans = this->gameObject.lock()->transform;
	if (this->type == 0)
	{
		auto currentPos = trans->position + ((targetPosition - trans->position) / 5.0f) * (GameManager::deltaTime * 60);
		trans->position = currentPos + shockPower;

		if (shockPower.norm() <= 1)
			shockPower = Eigen::Vector2d(0, 0);
		shockPower = shockPower + ((Eigen::Vector2d(0, 0) - shockPower) / 1.35f) * (GameManager::deltaTime * 60);
	}
	if (this->type == 10)
	{
		trans->position += shockPower * (GameManager::deltaTime * 60);
		trans->rotationZ += ((shockPower.x() * 3.14f * (GameManager::deltaTime * 60)) * D2R);
		shockPower.x() = (abs(shockPower.x()) + ((0 - abs(shockPower.x())) / 28.0) * (GameManager::deltaTime * 60)) * this->dir;
		shockPower.y() += 0.981 * (GameManager::deltaTime * 60);
		if (trans->position.y() >= project->inGame->playerPosition.y())
		{
			trans->position.y() = project->inGame->playerPosition.y();
			shockPower.y() = -abs(shockPower.y()/3.5f);
		}
		if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count() / 1000000.0 >= limitTime)
		{
			this->gameObject.lock()->isDestroy = true;
		}
	}
	if (this->type == 15)
	{
		if (!effectSR.expired())
		{
			auto sr = effectSR.lock();

			if (effectSR.lock()->alphaValue <= 0.005)
				effectSR.lock()->alphaValue = 0;
			effectSR.lock()->alphaValue = effectSR.lock()->alphaValue + ((0 - effectSR.lock()->alphaValue) / limitTime) * (GameManager::deltaTime * 60);
		}
	}
}

void CameraControl::Start()
{
	camera = this->gameObject.lock()->GetComponent<Camera>();
	holyEffectObj = GameManager::mainWorld->CreateGameObject();
	holyEffectSR = holyEffectObj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	auto effectScript = holyEffectObj.lock()->AddComponent<EffectComponent>(std::make_shared<EffectComponent>());

	holyEffectSR.lock()->SetSprite(Resources::GetSprite(sprite_ViewEffect_White));
	holyEffectSR.lock()->zIndex = 100;
	holyEffectSR.lock()->flip.x() = 0;
	holyEffectSR.lock()->renderSize = Eigen::Vector2d(GameManager::RenderSize.x() * 1.25f, GameManager::RenderSize.y() * 1.25f);
	holyEffectSR.lock()->alphaValue = 0.0;
}

void CameraControl::HolyEffect(float speed, float alpha, int type)
{
	auto effectScript = holyEffectObj.lock()->GetComponent<EffectComponent>();
	if (type == 0)
		holyEffectSR.lock()->SetSprite(Resources::GetSprite(sprite_ViewEffect_White));
	if (type == 1)
		holyEffectSR.lock()->SetSprite(Resources::GetSprite(sprite_ViewEffect_Red));
	effectScript->Execute(15);
	effectScript->limitTime = speed;
	effectScript->effectSR = holyEffectSR;
	effectScript->effectSR.lock()->alphaValue = alpha;
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