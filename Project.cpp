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

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "TotalLoader.h"

#define music_Sync_Beat 0000
#define music_Urgency 0001
#define music_Abiogenesis 0002
#define music_Gloxinia_by_Ruxxi 0003
#define music_To_Where_You_Are 0004

#define music_Menu 90

#define Sound_Attack_1 0100
#define Sound_Attack_2 0101
#define Sound_Attack_3 0102

#define Sound_Hit_1 0110

#define Sound_UI_CHOOSE_1 1000
#define Sound_UI_Start_1 1001

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

#define image_Menu_SongTable_1 30100
#define image_Menu_Table_1 30200
#define image_Menu_Check_1 30250
#define image_Menu_NonCheck_1 30260
#define image_Menu_Start_1 30270

#define image_Menu_Option_1 30300
#define image_Menu_Pause_1 30310

#define image_Menu_SongTable_Info_1 30500
#define image_Menu_SongTable_Info_ImageRect1 30510
#define image_Menu_SongTable_Info_ImageRect2 30511
#define image_Menu_SongTable_Info_ImageRect3 30512

#define image_Menu_SongTable_Info_Image_0 31000
#define image_Menu_SongTable_Info_Image_1 31010
#define image_Menu_SongTable_Info_Image_2 31020
#define image_Menu_SongTable_Info_Image_3 31030
#define image_Menu_SongTable_Info_Image_Lock 123456

#define image_FFT_1 30000

#define image_LogoBack_0 100000
#define image_Logo_0 100001
#define image_Logo_1 100002

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

#define sprite_Menu_SongTable_1 300
#define sprite_Menu_Table_1 310
#define sprite_Menu_Level_Check 314
#define sprite_Menu_Level_NonCheck 315
#define sprite_Menu_Start_1 320

#define sprite_Menu_Option_1 330
#define sprite_Menu_Pause_1 331

#define sprite_Menu_SongTable_Info_1 400
#define sprite_Menu_SongTable_Info_ImageRect1 410
#define sprite_Menu_SongTable_Info_ImageRect2 411
#define sprite_Menu_SongTable_Info_ImageRect3 412

#define sprite_Menu_SongTable_Info_Image_0 420
#define sprite_Menu_SongTable_Info_Image_1 421
#define sprite_Menu_SongTable_Info_Image_2 422
#define sprite_Menu_SongTable_Info_Image_3 423
#define sprite_Menu_SongTable_Info_Image_Lock 123456

#define sprite_FFT_1 1001

#define sprite_Background_1 501
#define sprite_Background_2 502

#define sprite_LogoBack_0 2000

#define sprite_Logo_0 2001
#define sprite_Logo_1 2002

float ProjectI::hardLatency = 0.00f;

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

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"MenuTable", 1, L".png",
		1, image_Menu_Table_1, sprite_Menu_Table_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitleBase_", 0, L".png",
		1, image_Menu_SongTable_1, sprite_Menu_SongTable_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitleLevel_Check", 0, L".png",
		4, image_Menu_Check_1, sprite_Menu_Level_Check);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitleLevel_NonCheck", 0, L".png",
		4, image_Menu_NonCheck_1, sprite_Menu_Level_NonCheck);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Info_", 0, L".png",
		1, image_Menu_SongTable_Info_1, sprite_Menu_SongTable_Info_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Info_ImageRect", 0, L".png",
		1, image_Menu_SongTable_Info_ImageRect1, sprite_Menu_SongTable_Info_ImageRect1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Info_ImageRect", 1, L".png",
		1, image_Menu_SongTable_Info_ImageRect2, sprite_Menu_SongTable_Info_ImageRect2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Info_ImageRect", 2, L".png",
		1, image_Menu_SongTable_Info_ImageRect3, sprite_Menu_SongTable_Info_ImageRect3);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Image", 0, L".png",
		1, image_Menu_SongTable_Info_Image_0, sprite_Menu_SongTable_Info_Image_0);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Image", 1, L".png",
		1, image_Menu_SongTable_Info_Image_1, sprite_Menu_SongTable_Info_Image_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Image", 2, L".png",
		1, image_Menu_SongTable_Info_Image_2, sprite_Menu_SongTable_Info_Image_2);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Image", 3, L".png",
		1, image_Menu_SongTable_Info_Image_3, sprite_Menu_SongTable_Info_Image_3);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Image", 123456, L".png",
		1, image_Menu_SongTable_Info_Image_Lock, sprite_Menu_SongTable_Info_Image_Lock);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"SongTitle_Start", 0, L".png",
		1, image_Menu_Start_1, sprite_Menu_Start_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Menu_Option", 0, L".png",
		2, image_Menu_Option_1, sprite_Menu_Option_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Menu_Pause", 0, L".png",
		2, image_Menu_Pause_1, sprite_Menu_Pause_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Logo_", 0, L".png",
		1, image_LogoBack_0, sprite_LogoBack_0);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\Menu\\", L"GameLogo", 0, L".png",
		1, image_Logo_0, sprite_Logo_0);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.5);
	sprite->time = 1.0f;

	spriteWeak = SpriteGroupLoad(L".\\Resources\\Image\\", L"Object\\", L"Enemy", 1, L".png",
		1, image_Logo_1, sprite_Logo_1);
	sprite = spriteWeak.lock();
	sprite->pivot = Eigen::Vector2d(0.5, 0.7333333);
	sprite->time = 1.0f;

	Resources::SoundLoading(music_Menu, ".\\Resources\\Sound\\Title Screen Loop.mp3");
	Resources::SoundLoading(music_Sync_Beat, ".\\Resources\\Sound\\Sync_Beat.mp3");
	Resources::SoundLoading(music_Urgency, ".\\Resources\\Sound\\Urgency.mp3");
	Resources::SoundLoading(music_Abiogenesis, ".\\Resources\\Sound\\Abiogenesis.mp3");
	Resources::SoundLoading(music_Gloxinia_by_Ruxxi, ".\\Resources\\Sound\\Gloxinia by Ruxxi.mp3");
	Resources::SoundLoading(music_To_Where_You_Are, ".\\Resources\\Sound\\To Where You Are.mp3");

	Resources::SoundLoading(Sound_Attack_1, ".\\Resources\\Sound\\Attack\\Attack1.mp3");
	Resources::SoundLoading(Sound_Attack_2, ".\\Resources\\Sound\\Attack\\Attack2.mp3");
	Resources::SoundLoading(Sound_Attack_3, ".\\Resources\\Sound\\Attack\\Attack3.mp3");
	Resources::SoundLoading(Sound_Hit_1, ".\\Resources\\Sound\\Hit\\Hit1.ogg");
	
	Resources::SoundLoading(Sound_UI_CHOOSE_1, ".\\Resources\\Sound\\UI\\Choose.mp3");
	Resources::SoundLoading(Sound_UI_Start_1, ".\\Resources\\Sound\\UI\\Start.wav");

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

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(50 / 255.0, 50 / 255.0, 53.0 / 255.0, 1.0)
		), &(project->globalData.grayBrush));

	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(130 / 255.0, 177 / 255.0, 218.0 / 255.0, 1.0)
		), &(project->globalData.SBrush));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(228 / 255.0, 83 / 255.0, 83.0 / 255.0, 1.0)
		), &(project->globalData.ABrush));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(130 / 255.0, 200 / 255.0, 107.0 / 255.0, 1.0)
		), &(project->globalData.BBrush));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(209 / 255.0, 204 / 255.0, 127.0 / 255.0, 1.0)
		), &(project->globalData.CBrush));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(120 / 255.0, 96 / 255.0, 74.0 / 255.0, 1.0)
		), &(project->globalData.DBrush));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF(99 / 255.0, 101 / 255.0, 104.0 / 255.0, 1.0)
		), &(project->globalData.FBrush));

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

	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		36.0f,
		L"ko-kr",
		&project->globalData.menuFont);

	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_ITALIC,
		DWRITE_FONT_STRETCH_NORMAL,
		56.0f,
		L"ko-kr",
		&project->globalData.menuScoreFont);

	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"ko-kr",
		&project->globalData.menuNameFont);

	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		86.0f,
		L"ko-kr",
		&project->globalData.menuRankFont);

	GameManager::soundSystem->getMasterChannelGroup(&GameManager::channelGroup);
	GameManager::soundSystem->createDSPByType(FMOD_DSP_TYPE_FFT, &GameManager::dsp);
	GameManager::channelGroup->addDSP(0, GameManager::dsp);
	GameManager::dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_HANNING);
	GameManager::dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, GameManager::fftSize);
	GameManager::dsp->setBypass(false);
	GameManager::dsp->setActive(true);

	project->songTotalJsonData = jsonReader.Read(".\\Resources\\GameData\\SongInfo.json");
	project->songUserJsonData = jsonReader.Read(".\\Resources\\GameData\\SongUser.json");
	project->LoadSong();
	SongData nonSong;
	nonSong.name = L"Lock";
	nonSong.isLock = true;
	nonSong.level = 0;
	nonSong.soundImageResourceCode = image_Menu_SongTable_Info_Image_Lock;
	nonSong.bestCombo = 0;
	nonSong.bestScore = 0;
	project->PushSong(nonSong);
	project->PushSong(nonSong);
	project->PushSong(nonSong);
	project->PushSong(nonSong);
	project->PushSong(nonSong);
	project->PushSong(nonSong);

	this->inGame = std::make_shared<InGame>();
	this->outGame = std::make_shared<OutGame>();

	GameManager::mainWorld = this->outGame;
}

void ProjectI::LoadSong()
{
	auto songInfoGroup = songTotalJsonData["SongInfo"];
	int songIndex = 0;
	std::string first = "Song_";
	nlohmann::json songData;
	while (!(songData = songInfoGroup[first + std::to_string(songIndex)]).is_null())
	{
		SongData newSongData;
		newSongData.SetData(songData);
		project->PushSong(newSongData);
		songIndex++;
	}
}

void ProjectI::PushSong(SongData data)
{
	songDataList.push_back(data);
}

void SongData::SetData(nlohmann::json songData)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convertString;
	songJson = songData;
	auto nodeDatas = songData["Datas"];
	name = convertString.from_bytes(JsonReader::stringFormat(songData["Name"].get<std::string>()));
	soundResourceCode = songData["Code"].get<int>();
	soundImageResourceCode = songData["LogoCode"].get<int>();
	level = songData["Level"].get<int>();

	auto codeString = std::to_string(soundResourceCode);
	auto songCode = codeString.c_str();
	auto nowSongUserData = project->songUserJsonData["SongUser"][songCode];
	if (nowSongUserData.is_null())
	{
		project->songUserJsonData["SongUser"][songCode]["bestCombo"] = 0;
		project->songUserJsonData["SongUser"][songCode]["bestScore"] = 0;
		project->jsonReader.Write(".\\Resources\\GameData\\SongUser.json", project->songUserJsonData);
	}
	bestCombo = project->songUserJsonData["SongUser"][songCode]["bestCombo"];
	bestScore = project->songUserJsonData["SongUser"][songCode]["bestScore"];

	for (int i = 0; i < nodeDatas.size(); i++)
	{
		if (!nodeDatas.at(i)["Type"].is_null())
		{
			NodeData currentData;

			int type = nodeDatas.at(i)["Type"].get<int>();
			int id = nodeDatas.at(i)["ID"].get<int>();
			double time = nodeDatas.at(i)["Time"].is_null() ? -1.0 : nodeDatas.at(i)["Time"].get<double>();
			double speed = nodeDatas.at(i)["Speed"].is_null() ? 3 : nodeDatas.at(i)["Speed"].get<double>();
			std::string direction = nodeDatas.at(i)["Direction"].is_null() ? "L" : nodeDatas.at(i)["Direction"].get<std::string>();
			double reTime = nodeDatas.at(i)["ReTime"].is_null() ? 3.0 : nodeDatas.at(i)["ReTime"].get<double>();

			currentData.id = id;
			currentData.type = type;
			currentData.time = time;
			currentData.reTime = reTime;
			currentData.speed = speed;
			if (direction._Equal("L"))
				currentData.direction = 1;
			if (direction._Equal("R"))
				currentData.direction = -1;
			nodeDataList.push_back(currentData);
		}
	}
}

void SongData::SaveData()
{
	auto codeString = std::to_string(soundResourceCode);
	auto songCode = codeString.c_str();
	auto nowSongUserData = project->songUserJsonData["SongUser"][songCode];
	
	project->songUserJsonData["SongUser"][songCode]["bestCombo"] = bestCombo;
	project->songUserJsonData["SongUser"][songCode]["bestScore"] = bestScore;
	project->jsonReader.Write(".\\Resources\\GameData\\SongUser.json", project->songUserJsonData);
	
}

std::shared_ptr<SpriteRenderer> sr;
std::shared_ptr<EnemyEntityComponent> enemyScript;

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

	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	auto CC = mainCameraObject->AddComponent<CameraControl>(std::make_shared<CameraControl>());
	CC->targetPosition = Eigen::Vector2d(0, 0);
	mainCameraObject->transform->position = Eigen::Vector2d(0, 0);

	auto deadLineObj = GameManager::mainWorld->CreateGameObject();
	auto deadLine_L_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_L_SR->SetSprite(Resources::GetSprite(sprite_LogoBack_0));
	deadLine_L_SR->zIndex = 0;
	deadLine_L_SR->flip.x() = 1;
	deadLine_L_SR->renderSize = Eigen::Vector2d(deadLine_L_SR->sprite.lock()->spriteSize.x() * 4, deadLine_L_SR->sprite.lock()->spriteSize.y() * 6);
	deadLineObj->transform->position = Eigen::Vector2d(0, 0);
}

void OutGame::Update()
{
	World::Update();

	switch (animationEventIndex)
	{
	case 0:
	{
		if (animationTime >= 1)
		{
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_3), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);

			animationEventIndex++;
		}
		break;
	}
	case 1:
	{
		if (animationTime >= 2)
		{
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);

			animationEventIndex++;
		}
		break;
	}
	case 2:
	{
		if (animationTime >= 2.25)
		{
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);

			animationEventIndex++;
		}
		break;
	}
	case 3:
	{
		if (animationTime >= 2.5)
		{
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);

			animationEventIndex++;
		}
		break;
	}
	case 4:
	{
		if (animationTime >= 2.75)
		{
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);
			animationEventIndex++;
		}
		break;
	}
	case 5:
	{
		if (animationTime >= 4)
		{
			FMOD::Channel* channel;
			FMOD::Channel* channel2;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
			channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
			channel->setPitch(0.75f);

			GameManager::soundSystem->playSound(Resources::GetSound(Sound_Hit_1), 0, false, &channel2);
			channel2->setVolume(GameManager::masterSound * GameManager::SFXSound);

			animObj1 = GameManager::mainWorld->CreateGameObject();
			auto animObj1SR = animObj1.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			animObj1SR->SetSprite(Resources::GetSprite(sprite_Logo_1));
			animObj1SR->zIndex = 12;
			animObj1SR->flip.x() = 0;
			animObj1SR->isAnimation = false;
			animObj1SR->animationIndex = 0;
			animObj1SR->renderSize = Eigen::Vector2d(animObj1SR->sprite.lock()->spriteSize.x() * 70, animObj1SR->sprite.lock()->spriteSize.y() * 70);
			animObj1.lock()->transform->position = Eigen::Vector2d(540, 200);
			animObj1.lock()->transform->rotationZ = -20;
			GameManager::mainCamera->gameObject.lock()->GetComponent<CameraControl>()->Shock(100, 1, Eigen::Vector2d(100, 100));
			animationEventIndex++;
		}
		break;
	}
	case 6:
	{
		if (animationTime >= 5)
		{
			animObj2 = GameManager::mainWorld->CreateGameObject();
			auto animObj1SR = animObj2.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			animObj1SR->SetSprite(Resources::GetSprite(sprite_Logo_0));
			animObj1SR->zIndex = 10;
			animObj1SR->flip.x() = 0;
			animObj1SR->isAnimation = false;
			animObj1SR->animationIndex = 0;
			animObj1SR->renderSize = Eigen::Vector2d(animObj1SR->sprite.lock()->spriteSize.x() * 2, animObj1SR->sprite.lock()->spriteSize.y() * 2);
			animObj2.lock()->transform->rotationZ = 0;

			auto effectScript = animObj1.lock()->AddComponent<EffectComponent>(std::make_shared<EffectComponent>());
			effectScript->Execute(11);
			effectScript->SetDirection(1);
			animationEventIndex++;
		}
		break;
	}
	case 7:
	{
		if (animationTime >= 8)
		{
			animationEventIndex++;
		}
		break;
	}
	case 8:
	{
		animObj2.lock()->GetComponent<SpriteRenderer>()->alphaValue -= 0.02 * GameManager::deltaTime * 60;
		if (animationTime >= 9.5)
		{
			animationEventIndex++;
		}
		break;
	}
	default:
	{
		GameManager::mainWorld = project->inGame;
		GameManager::mainWorld->Init();
		auto a = GameManager::mainCamera->gameObject.lock()->GetComponent<CameraControl>();
		a->Start();
		a->isFirst = false;
		a->HolyEffect(20, 1, 0);
		break;
	}
	}

	if (InputManager::GetKeyDown('S'))
	{
		GameManager::mainWorld = project->inGame;
		GameManager::mainWorld->Init();
	}
	animationTime += GameManager::deltaTime;
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
	songText = songObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	songObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y()+300);
	songText.lock()->renderSize = Eigen::Vector2d(1000, 100);
	songText.lock()->pivot = Eigen::Vector2d(0.5, 0.5);
	songText.lock()->SetFont(project->globalData.soundFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	songText.lock()->SetText(L"Cytus2 : Urgency-SIHanatsuka");
	songText.lock()->SetBrush(project->globalData.whiteBrush);
	songText.lock()->zIndex = -5;

	comboObj = GameManager::mainWorld->CreateGameObject();
	comboText = comboObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	comboObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y() + -290);
	comboText.lock()->renderSize = Eigen::Vector2d(500, 200);
	comboText.lock()->pivot = Eigen::Vector2d(0.5, 0);
	comboText.lock()->SetFont(project->globalData.comboFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	comboText.lock()->SetBrush(project->globalData.perfectBrush);
	comboText.lock()->zIndex = -5;
	auto comboEffect = comboObj.lock()->AddComponent<EffectComponent>(std::make_shared<EffectComponent>());
	comboEffect->targetPosition = comboObj.lock()->transform->position;

	scoreObj = GameManager::mainWorld->CreateGameObject();
	scoreText = scoreObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	scoreObj.lock()->transform->position = Eigen::Vector2d(playerPosition.x(), playerPosition.y() + 90);
	scoreText.lock()->renderSize = Eigen::Vector2d(600, 200);
	scoreText.lock()->pivot = Eigen::Vector2d(0.5, 0);
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
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() - playerHitDistance, playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_1));
	deadLine_R_SR->zIndex = 10;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x() * 3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() + playerHitDistance, playerPosition.y());

	
	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_L_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_L_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_4));
	deadLine_L_SR->zIndex = -10;
	deadLine_L_SR->flip.x() = 1;
	deadLine_L_SR->renderSize = Eigen::Vector2d(deadLine_L_SR->sprite.lock()->spriteSize.x() * 3, deadLine_L_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() - playerHitDistance, playerPosition.y());

	deadLineObj = GameManager::mainWorld->CreateGameObject();
	deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_DeadLine_4));
	deadLine_R_SR->zIndex = -10;
	deadLine_R_SR->flip.x() = 1;
	deadLine_R_SR->renderSize = Eigen::Vector2d(deadLine_R_SR->sprite.lock()->spriteSize.x() * 3, deadLine_R_SR->sprite.lock()->spriteSize.y() * 3);
	deadLineObj->transform->position = Eigen::Vector2d(playerPosition.x() + playerHitDistance, playerPosition.y());
	
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
	PP_OR->alphaValue = 1.0f;
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

	for (int i = 0; i < GameManager::fftSize; i++)
	{
		deadLineObj = GameManager::mainWorld->CreateGameObject();
		deadLine_R_SR = deadLineObj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
		deadLine_R_SR->SetSprite(Resources::GetSprite(sprite_FFT_1));
		deadLine_R_SR->zIndex = -50;
		deadLine_R_SR->renderSize = Eigen::Vector2d(GameManager::viewSize.right/((float)GameManager::fftSize) + 1, 0);
		FFTList.push_back(deadLine_R_SR);
		deadLineObj->transform->position = Eigen::Vector2d(((playerPosition.x() - GameManager::viewSize.right / 2.0)) + (i * (GameManager::viewSize.right / (float)GameManager::fftSize)), playerPosition.y() + 3);
		//deadLineObj->transform->position = Eigen::Vector2d(0, 0);
	}

	menuSongObj = GameManager::mainWorld->CreateGameObject();
	auto menuSongSR = menuSongObj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_1));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 500;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;


	menuSongInfoObj = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongInfoObj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_Info_1));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 550;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;

	menuSongInfoImageObj = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongInfoImageObj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_Info_Image_1));
	menuSongSR->renderSize = Eigen::Vector2d(
		Resources::GetSprite(sprite_Menu_SongTable_Info_ImageRect1).lock()->spriteSize.x() * 3,
		Resources::GetSprite(sprite_Menu_SongTable_Info_ImageRect1).lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 560;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;

	menuSongInfoImageRectObj = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongInfoImageRectObj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_Info_ImageRect1));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 560;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;

	menuSongInfoImageRect2Obj = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongInfoImageRect2Obj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_Info_ImageRect2));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 580;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;

	menuSongInfoImageRect3Obj = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongInfoImageRect2Obj.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_SongTable_Info_ImageRect3));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 580;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;

	menuSongStart = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuSongStart.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_Start_1));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 600;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;


	menuPause = GameManager::mainWorld->CreateGameObject();
	menuSongSR = menuPause.lock()->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	menuSongSR->SetSprite(Resources::GetSprite(sprite_Menu_Pause_1));
	menuSongSR->renderSize = Eigen::Vector2d(menuSongSR->sprite.lock()->spriteSize.x() * 3, menuSongSR->sprite.lock()->spriteSize.y() * 3);
	menuSongSR->zIndex = 600;
	menuSongSR->alpha = true;
	menuSongSR->alphaValue = 1;
	menuSongSR->animationIndex = 0;
	menuSongSR->isAnimation = false;
	menuPause.lock()->transform->position = Eigen::Vector2d(GameManager::RenderSize.x()-200, 100);

	menuSongInfoLevelObj = GameManager::mainWorld->CreateGameObject();
	auto menuScoreText = menuSongInfoLevelObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuNameFont, DWRITE_TEXT_ALIGNMENT_LEADING);
	menuScoreText->SetText(L"Level :");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongInfoNameObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoNameObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuNameFont, DWRITE_TEXT_ALIGNMENT_LEADING);
	menuScoreText->SetText(L"Best Score  :");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongInfoBestScoreObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoBestScoreObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuScoreFont, DWRITE_TEXT_ALIGNMENT_LEADING);
	menuScoreText->SetText(L"Best Score  :");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongInfoBestComboObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoBestComboObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuScoreFont, DWRITE_TEXT_ALIGNMENT_LEADING);
	menuScoreText->SetText(L"Best Combo  :");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;


	menuSongInfoBestScoreValueObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoBestScoreValueObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(1, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuScoreFont, DWRITE_TEXT_ALIGNMENT_TRAILING);
	menuScoreText->SetText(L"000,000");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongInfoBestComboValueObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoBestComboValueObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(1, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuScoreFont, DWRITE_TEXT_ALIGNMENT_TRAILING);
	menuScoreText->SetText(L"0");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongInfoRankObj = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongInfoRankObj.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0.5, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuRankFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	menuScoreText->SetText(L"RANK");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 600;

	menuSongStartText = GameManager::mainWorld->CreateGameObject();
	menuScoreText = menuSongStartText.lock()->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
	menuScoreText->pivot = Eigen::Vector2d(0.5, 0.5);
	menuScoreText->renderSize = Eigen::Vector2d(750, 200);
	menuScoreText->SetFont(project->globalData.menuRankFont, DWRITE_TEXT_ALIGNMENT_CENTER);
	menuScoreText->SetText(L"Start");
	menuScoreText->SetBrush(project->globalData.whiteBrush);
	menuScoreText->zIndex = 601;

	std::vector<std::weak_ptr<GameObject>> levelList;
	for (int i = 0; i < maxNodeLevel; i++)
	{
		auto menuObject = GameManager::mainWorld->CreateGameObject();
		auto menuSprite = menuObject->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
		
		menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Level_NonCheck));
		menuSprite->renderSize = Eigen::Vector2d(menuSprite->sprite.lock()->spriteSize.x() * 3, menuSprite->sprite.lock()->spriteSize.y() * 3);
		menuSprite->zIndex = 600;
		menuSprite->alpha = true;
		menuSprite->alphaValue = 1;
		menuSprite->animationTime = (float)i / maxNodeLevel;
		menuSprite->animationIndex = (i < 3) ? 0 : (i < 8) ? 1 : (i < 14) ? 2 : 3;
		menuSprite->Pause();

		menuObject->transform->position = Eigen::Vector2d(0, 0);
		menuObject->transform->rotationZ = 0;
		menuSelectSongLevelList.push_back(menuObject);
	}

	menuListCount = project->songDataList.size();
	for (int i = 0; i < menuListCount; i++)
	{
		auto nowSong = project->songDataList[i];

		float angle = i * menuAngle;
		Eigen::Vector2d offset = menuOffset + Eigen::Vector2d(-menuDistance + cos(angle) * menuDistance, sin(angle) * menuDistance);
		
		auto menuObject = GameManager::mainWorld->CreateGameObject();
		auto menuSprite = menuObject->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
		menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Table_1));
		menuSprite->renderSize = Eigen::Vector2d(menuSprite->sprite.lock()->spriteSize.x() * 3, menuSprite->sprite.lock()->spriteSize.y() * 3);
		menuSprite->zIndex = 500 - abs(angle * R2D);
		menuSprite->alpha = true;
		menuSprite->alphaValue = 1;
		
		menuObject->transform->position = offset;
		menuObject->transform->rotationZ = angle;
		menuTableList.push_back(menuObject);


		menuObject = GameManager::mainWorld->CreateGameObject();
		if (nowSong.soundImageResourceCode != -1)
		{
			menuSprite = menuObject->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			menuSprite->SetSprite(Resources::GetSprite(nowSong.soundImageResourceCode));
			menuSprite->renderSize = Eigen::Vector2d(160, 160);
			menuSprite->zIndex = 500 - abs(angle * R2D) + 10;
			menuSprite->alpha = true;
			menuSprite->alphaValue = 1;
		}
		menuObject->transform->position = offset;
		menuObject->transform->rotationZ = angle;
		menuTableImageList.push_back(menuObject);
		

		menuObject = GameManager::mainWorld->CreateGameObject();
		auto menuText = menuObject->AddComponent<TextRenderer>(std::make_shared<TextRenderer>());
		menuText->pivot = Eigen::Vector2d(0, 0.5);
		menuText->renderSize = Eigen::Vector2d(750, 200);
		menuText->SetFont(project->globalData.menuFont, DWRITE_TEXT_ALIGNMENT_LEADING);
		menuText->SetText(nowSong.name.c_str());
		menuText->SetBrush(project->globalData.whiteBrush);
		menuText->zIndex = 600;
		menuObject->transform->position = offset;
		menuTableNameList.push_back(menuObject);

		int level = nowSong.level;

		std::vector<std::weak_ptr<GameObject>> levelList;
		for (int i = 0; i < maxNodeLevel; i++)
		{
			menuObject = GameManager::mainWorld->CreateGameObject();
			menuSprite = menuObject->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			if(level > i)
				menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Level_Check));
			else
				menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Level_NonCheck));
			menuSprite->renderSize = Eigen::Vector2d(menuSprite->sprite.lock()->spriteSize.x() * 3, menuSprite->sprite.lock()->spriteSize.y() * 3);
			menuSprite->zIndex = 500 - abs(angle * R2D) + 30;
			menuSprite->alpha = true;
			menuSprite->alphaValue = 1;
			menuSprite->animationTime = (float)i / maxNodeLevel;
			menuSprite->animationIndex = (i < 3) ? 0 : (i < 8) ? 1 : (i < 14) ? 2 : 3;
			menuSprite->Pause();

			menuObject->transform->position = offset;
			menuObject->transform->rotationZ = angle;
			levelList.push_back(menuObject);
		}
		menuLevelList.push_back(levelList);
	}

	//GameManager::soundSystem->playSound(Resources::GetSound(music_Urgency), 0, false, &channel);
}


//FMOD::DSP* dsp;
//FMOD_DSP_PARAMETER_FFT* data = NULL;
float offsetAngle = 0;
float menuMoveVelocity = 0;
float velocityAvgs[10];

void InGame::StartNode(SongData* songData)
{
	bool isPlaying;
	menuMusicChannel->isPlaying(&isPlaying);
	if (isPlaying)
	{
		menuMusicChannel->stop();
		menuMusicChannel = NULL;
	}

	nodeSystem->Start(Resources::GetSound(songData->soundResourceCode));
	songText.lock()->SetText(songData->name.c_str());
	nodeDataList = songData->nodeDataList;
	nowSongData = songData;

	if (menuMode)
		menuMode = false;

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
	maxCombo = 0;
	maxComboLevel = -1;
	hitPerfact = 0;
}

int skipMunuIndex = -1;
void InGame::Update()
{
	//-------- 메뉴 컨트롤 --------

	if (menuMode)
	{
		if (menuMusicChannel == NULL)
		{
			GameManager::soundSystem->playSound(Resources::GetSound(music_Menu), 0, false, &menuMusicChannel);
			menuMusicChannel->setVolume(0.6);
			menuMusicTermTime = 0;
		}
		else
		{
			bool isplaying = false;
			menuMusicChannel->isPlaying(&isplaying);
			if (!isplaying)
			{
				if (menuMusicTermTime >= 3)
				{
					GameManager::soundSystem->playSound(Resources::GetSound(music_Menu), 0, false, &menuMusicChannel);
					menuMusicChannel->setVolume(0.6);
					menuMusicTermTime = 0;
				}
				else
				{
					menuMusicTermTime += GameManager::deltaTime;
				}
			}
		}

		if (InputManager::GetKey('U'))
			menuOffset.x() += 10;
		if (InputManager::GetKey('I'))
			menuOffset.x() -= 10;
		float maxAngle = menuTableList.size() * menuAngle;
		offsetAngle += menuMoveVelocity * D2R;
		menuMoveVelocity = menuMoveVelocity *= 0.85;
		if (abs(menuMoveVelocity) < 0.01)
		{
			menuMoveVelocity = 0;
			int minIndex = 0;
			for (int i = 0; i < menuTableList.size(); i++)
				if (abs(minIndex * menuAngle - offsetAngle) > abs(i * menuAngle - offsetAngle))
					minIndex = i;
			skipMunuIndex = minIndex;
			selectMenuIndex = minIndex;
			offsetAngle += (((minIndex * menuAngle) - offsetAngle) / 10.0) * GameManager::deltaTime * 60;
		}
		else
		{
			int minIndex = 0;
			for (int i = 0; i < menuTableList.size(); i++)
				if (abs(minIndex * menuAngle - offsetAngle) > abs(i * menuAngle - offsetAngle))
					minIndex = i;
			if (minIndex != skipMunuIndex)
			{
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_UI_CHOOSE_1), 0, false, &channel);
			}
			skipMunuIndex = minIndex;
			selectMenuIndex = -1;
		}
		if (offsetAngle < 0 || offsetAngle >(menuListCount - 1) * menuAngle)
			menuMoveVelocity = menuMoveVelocity * 0.6;

		auto menuPauseSR = menuPause.lock()->GetComponent<SpriteRenderer>();
		menuPauseSR->SetSprite(Resources::GetSprite(sprite_Menu_Option_1));
		menuPauseSR->renderSize = Eigen::Vector2d(menuPauseSR->sprite.lock()->spriteSize.x() * 3, menuPauseSR->sprite.lock()->spriteSize.y() * 3);
	}
	else
	{
		auto menuPauseSR = menuPause.lock()->GetComponent<SpriteRenderer>();
		menuPauseSR->SetSprite(Resources::GetSprite(sprite_Menu_Pause_1));
		menuPauseSR->renderSize = Eigen::Vector2d(menuPauseSR->sprite.lock()->spriteSize.x() * 3, menuPauseSR->sprite.lock()->spriteSize.y() * 3);
		if (nodeSystem->isStart)
		{
			menuPauseSR->animationIndex = nodeSystem->isPause ? 1 : 0;
			menuPauseSR->renderSize = Eigen::Vector2d(menuPauseSR->sprite.lock()->spriteSize.x() * (3 + (nodeSystem->isPause ? 1 : 0)), menuPauseSR->sprite.lock()->spriteSize.y() * (3 + (nodeSystem->isPause ? 1 : 0)));
		}
	}

	for (int i = 0; i < menuTableList.size(); i++)
	{
		float angle = i * menuAngle - offsetAngle;
		auto menuObject = menuTableList[i];
		auto menuImageObject = menuTableImageList[i];
		auto menuNameObject = menuTableNameList[i];

		float offsetAngle = angle * 0.30f;
		Eigen::Vector2d offsetPos = menuOffset + Eigen::Vector2d(-menuDistance + cos(angle) * menuDistance, sin(angle) * menuDistance);
		Eigen::Vector2d offsetRight = Eigen::Vector2d(cos(offsetAngle), sin(offsetAngle));
		Eigen::Vector2d offsetDown = Eigen::Vector2d(cos(offsetAngle + 90 * D2R), sin(offsetAngle + 90 * D2R));

		menuObject.lock()->transform->position = offsetPos;
		menuObject.lock()->transform->rotationZ = offsetAngle;

		menuImageObject.lock()->transform->position = offsetPos + offsetRight * 96 + offsetDown * 0;
		menuImageObject.lock()->transform->rotationZ = offsetAngle;

		menuNameObject.lock()->transform->position = offsetPos + offsetRight * 200 + offsetDown * 55;
		menuNameObject.lock()->transform->rotationZ = offsetAngle;

		auto levelList = menuLevelList[i];
		for (int i = 0; i < maxNodeLevel; i++)
		{
			levelList[i].lock()->transform->position = offsetPos + offsetRight * (210 + i * 27) + offsetDown * 10;
			levelList[i].lock()->transform->rotationZ = offsetAngle;
		}
		//menuObject.lock()->transform->rotationZ = 0;
	}
	menuSongObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance, 0);
	menuSongInfoObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, 0);

	menuSongInfoNameObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 - 270 + 20, 60);
	menuSongInfoLevelObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 - 270 + 15, -250);

	menuSongInfoRankObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, 165);

	menuSongInfoBestScoreObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 - 270, 250);
	menuSongInfoBestComboObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 - 270, 305);

	menuSongInfoBestScoreValueObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 + 270, 250);
	menuSongInfoBestComboValueObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 + 270, 305);

	menuSongStart.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, 325);
	menuSongStartText.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, 390);

	menuSongInfoImageObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, -180);
	menuSongInfoImageRectObj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, -180);
	menuSongInfoImageRect2Obj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, -180);
	menuSongInfoImageRect3Obj.lock()->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400, -180);

	menuSongInfoImageObj.lock()->GetComponent<SpriteRenderer>()->alphaValue = menuAlpha;

	menuSongInfoNameObj.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;
	menuSongInfoLevelObj.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;

	menuSongInfoRankObj.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;
	menuSongInfoBestScoreValueObj.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;
	menuSongInfoBestComboValueObj.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;
	menuSongStartText.lock()->GetComponent<TextRenderer>()->alphaValue = menuAlpha;

	if (selectMenuIndex != -1)
	{
		if (project->songDataList[selectMenuIndex].soundImageResourceCode != -1)
		{
			menuSongInfoImageObj.lock()->GetComponent<SpriteRenderer>()->SetSprite(
				Resources::GetSprite(project->songDataList[selectMenuIndex].soundImageResourceCode));

			auto tempTR = menuSongInfoNameObj.lock()->GetComponent<TextRenderer>();
			tempTR->SetText(project->songDataList[selectMenuIndex].name.c_str());

			tempTR = menuSongInfoLevelObj.lock()->GetComponent<TextRenderer>();
			tempTR->SetText((L"Level :" + std::to_wstring(project->songDataList[selectMenuIndex].level)).c_str());

			tempTR = menuSongInfoBestScoreValueObj.lock()->GetComponent<TextRenderer>();
			TCHAR tempScore[20];
			float tempScoreValue = project->songDataList[selectMenuIndex].bestScore;
			_stprintf_s(tempScore, L"%03d,%03d", (int)((int)round(tempScoreValue) / 1000), (int)round(tempScoreValue) % 1000);
			tempTR->SetText(tempScore);

			tempTR = menuSongInfoRankObj.lock()->GetComponent<TextRenderer>();
			std::wstring rankC = L"N";
			if (tempScoreValue >= 95000)
			{
				rankC = L"S";
				tempTR->SetBrush(project->globalData.SBrush);
			}
			else if (tempScoreValue >= 90000)
			{
				rankC = L"A";
				tempTR->SetBrush(project->globalData.ABrush);
			}
			else if (tempScoreValue >= 80000)
			{
				rankC = L"B";
				tempTR->SetBrush(project->globalData.BBrush);
			}
			else if (tempScoreValue >= 70000)
			{
				rankC = L"C";
				tempTR->SetBrush(project->globalData.CBrush);
			}
			else if (tempScoreValue >= 65000)
			{
				rankC = L"D";
				tempTR->SetBrush(project->globalData.DBrush);
			}
			else if (tempScoreValue == 0)
			{
				rankC = L"N";
				tempTR->SetBrush(project->globalData.whiteBrush);
			}
			else
			{
				rankC = L"F";
				tempTR->SetBrush(project->globalData.FBrush);
			}
			tempTR->SetText((L"RANK  " + rankC).c_str());

			tempTR = menuSongInfoBestComboValueObj.lock()->GetComponent<TextRenderer>();
			tempTR->SetText(std::to_wstring(project->songDataList[selectMenuIndex].bestCombo).c_str());

			tempTR = menuSongStartText.lock()->GetComponent<TextRenderer>();
			tempTR->SetText(project->songDataList[selectMenuIndex].isLock ? L"Lock" : L"Start");

			menuSelectLevel += (project->songDataList[selectMenuIndex].level - menuSelectLevel) / 5.0 * GameManager::deltaTime * 60;
			if (abs(project->songDataList[selectMenuIndex].level - menuSelectLevel) < 0.01)
				menuSelectLevel = project->songDataList[selectMenuIndex].level;
			for (int i = 0; i < menuSelectSongLevelList.size(); i++)
			{
				auto menuObject = menuSelectSongLevelList[i].lock();
				auto menuSprite = menuObject->GetComponent<SpriteRenderer>();
				if ((int)menuSelectLevel > i)
					menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Level_Check));
				else
					menuSprite->SetSprite(Resources::GetSprite(sprite_Menu_Level_NonCheck));
				menuObject->transform->position = menuOffset + Eigen::Vector2d(-menuDistance + 400 - 270 + 40 + i * 27, 30);
				menuObject->transform->rotationZ = 0;
			}
		}

		menuAlpha += ((1 - menuAlpha) / 10.0) * GameManager::deltaTime * 60;
	}
	else
	{
		menuAlpha = 0;
	}
	
	menuOffset += Eigen::Vector2d((((menuMode ? 0 : -2000) - menuOffset.x()) / 10), 0) * GameManager::deltaTime * 60;

	//-------- 노드 생성 --------
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
	//-------- 진행도 표기 --------
	progressGaugeSR.lock()->renderSize = Eigen::Vector2d((progressBarSR.lock()->renderSize.x() - (4.0 * 3)) * min((nodeSystem->GetMusicTime() / nodeSystem->GetMusicTotalTime()), 1.0), progressGaugeSR.lock()->renderSize.y());
	menuPause.lock()->transform->position = GameManager::mainCamera->gameObject.lock()->transform->position + Eigen::Vector2d(GameManager::RenderSize.x() / 2 - 75, -GameManager::RenderSize.y() / 2 + 25);

	if (!menuMode && nodeSystem->isStart)
	{
		if (min((nodeSystem->GetMusicTime() / nodeSystem->GetMusicTotalTime()), 1.0) >= 0.99999)
		{
			nodeSystem->Stop();
			menuMode = true;
			syncMode = false;
			syncTotalTime = 0.0;
			if (nowSongData->bestScore <= score)
				nowSongData->bestScore = (int)round(score);
			if (nowSongData->bestCombo <= maxCombo)
				nowSongData->bestCombo = maxCombo;
			nowSongData->SaveData();
			nowSongData = NULL;
			FMOD::Channel* channel;
			GameManager::soundSystem->playSound(Resources::GetSound(Sound_UI_Start_1), 0, false, &channel);
			GameManager::mainCamera->gameObject.lock()->
				GetComponent<CameraControl>()->HolyEffect(20.0, 1.0f, 0);
		}
	}

	//sr->gameObject.lock()->transform->position += (GameManager::mainCamera->ScreenToWorld(InputManager::mousePos) - sr->gameObject.lock()->transform->position) / (5.0f / (GameManager::deltaTime * 60));
	
	//--------FFT SOUND 비쥬얼라이저--------
	GameManager::dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)(&GameManager::fftData), 0, 0, 0);
	if (GameManager::fftData != nullptr) 
	{
		//*ConsoleDebug::console << data->spectrum[0][10] << "\n";
		//*ConsoleDebug::console << data->length << "\n";
		for (int i = 0; i < GameManager::fftData->numchannels; i++)
		{
			float prevH = 0;
			float prevH2 = 0;
			for (int i = 0; i < FFTList.size() / 2; i++)
			{
				float h, nextH = 0, nextH2 = 0;
				if (i < FFTList.size() - 1) nextH = GameManager::fftData->spectrum[0][i + 1];
				if (i < FFTList.size() - 2) nextH2 = GameManager::fftData->spectrum[0][i + 2];
				h = GameManager::fftData->spectrum[0][i];
				float h2 = abs(h - prevH) * 0.3 + abs(h - nextH) * 0.3 + abs(prevH2 - prevH) * 0.2 + abs(nextH2 - nextH) * 0.2;
				h2 = sqrt(sqrt(h2));
				h2 = min(max((h2 - 0.045) * 1.06, 0), 1);
				h2 = h2 * 600;
				float prevX = FFTList[i].lock()->renderSize.x();
				float prevY = FFTList[i].lock()->renderSize.y();
				FFTList[i].lock()->renderSize = Eigen::Vector2d(prevX,
					prevY + (h2 - prevY) / 3.0);
				prevH2 = prevH;
				prevH = h;
			}

			prevH = 0;
			prevH2 = 0;
			for (int i = 0; i < FFTList.size() / 2; i++)
			{
				float h, nextH = 0, nextH2 = 0;
				if (i < FFTList.size() - 1) nextH = GameManager::fftData->spectrum[0][i + 1];
				if (i < FFTList.size() - 2) nextH2 = GameManager::fftData->spectrum[0][i + 2];
				h = GameManager::fftData->spectrum[0][i];
				float h2 = abs(h - prevH) * 0.3 + abs(h - nextH) * 0.3 + abs(prevH2 - prevH) * 0.2 + abs(nextH2 - nextH) * 0.2;
				h2 = sqrt(sqrt(h2));
				h2 = min(max((h2 - 0.045) * 1.06, 0), 1);
				h2 = h2 * 600;
				float prevX = FFTList[GameManager::fftSize / 2 + (FFTList.size() / 2 - 1 - i)].lock()->renderSize.x();
				float prevY = FFTList[GameManager::fftSize / 2 + (FFTList.size() / 2 - 1 - i)].lock()->renderSize.y();
				FFTList[GameManager::fftSize /2 + (FFTList.size()/2 - 1 - i)].lock()->renderSize = Eigen::Vector2d(prevX,
					prevY + (h2 - prevY) / 3.0);
				prevH2 = prevH;
				prevH = h;
			}
		}
		//*ConsoleDebug::console << data->numchannels << "\n";
	}
	else
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

		if (InputManager::GetKeyDown(VK_RIGHT))
		{
			attackDir = 1;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_RIGHT)) - ProjectI::hardLatency;
		}
		if (InputManager::GetKeyDown(VK_LEFT))
		{
			attackDir = -1;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_LEFT)) - ProjectI::hardLatency;
		}
		if (InputManager::GetKeyDown(VK_DOWN))
		{
			attackDir = 2;
			keyDeltaTime = nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_DOWN)) - ProjectI::hardLatency;
		}
		if (InputManager::GetKeyDown(VK_RIGHT) && InputManager::GetKeyDown(VK_LEFT))
		{
			attackDir = 2;
			keyDeltaTime = (nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_RIGHT)) + nodeSystem->GetDeltaTime(InputManager::GetKeyDownTime(VK_LEFT))) / 2.0;
			keyDeltaTime = keyDeltaTime - ProjectI::hardLatency;
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
			if (playerAttackStack == 0)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack1));
				playerAnimTime = std::chrono::steady_clock::now();
				playerAttackAnim = true;

				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, 0, -Eigen::Vector2d(power * attackDir, -power));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_1), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.40f);
				channel->setPitch(0.75f);
			}
			if ( playerAttackStack == 1)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack2));
				playerAnimTime = std::chrono::steady_clock::now();
				playerAttackAnim = true;
				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, 0, -Eigen::Vector2d(power * 1.5 * attackDir, power / 0.8f));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_2), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.6f);
				channel->setPitch(0.75f);
			}
			if ( playerAttackStack == 2)
			{
				sr->SetSprite(Resources::GetSprite(sprite_player_Attack3));
				playerAnimTime = std::chrono::steady_clock::now();
				playerAttackAnim = true;
				GameManager::mainCamera->gameObject.lock()->
					GetComponent<CameraControl>()->Shock(0, attackDir, Eigen::Vector2d(0, -power * 2));
				FMOD::Channel* channel;
				GameManager::soundSystem->playSound(Resources::GetSound(Sound_Attack_3), 0, false, &channel);
				channel->setVolume(GameManager::masterSound * GameManager::SFXSound * 0.6f);
				channel->setPitch(0.75f);
			}
			playerAttackStack++;
			playerAttackStack = playerAttackStack % 3;
			sr->animationLoop = false;
			sr->flip.x() = (attackDir == 1) ? 0 : 1;
			sr->Reset();
			sr->Play();
		}
		else if (attackDir == 2)
		{
			 playerAttackStack = 0;

			sr->SetSprite(Resources::GetSprite(sprite_player_idle2));
			playerAnimTime = std::chrono::steady_clock::now();
			playerAttackAnim = true;
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
			//*ConsoleDebug::console << nodeSystem->GetMusicTime() << "\n";

			for (int i = 0; i < EECList.size(); i++)
			{
				if (!EECList[i].expired())
				{
					double dTime = (keyDeltaTime - EECList[i].lock()->nodeData.time);
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
								//*ConsoleDebug::console << dTime << "\n\n";
								//totaldTime += dTime;
								//*ConsoleDebug::console << totaldTime / 32.0 << "\n\n";
								EECList[i].lock()->Hit(hitPerfact);

								FMOD::Channel* channel;
								GameManager::soundSystem->playSound(Resources::GetSound(Sound_Hit_1), 0, false, &channel);
								channel->setVolume(GameManager::masterSound* GameManager::SFXSound * 0.75f);
								combo++;
								comboObj.lock()->GetComponent<EffectComponent>()->Execute(0);
								GameManager::mainCamera->gameObject.lock()->
									GetComponent<CameraControl>()->HolyEffect(14.0, 0.15f, 0);
								score += scoreHitAdd * (0.5 + 0.5 * ((2.0 - hitPerfact) / 2.0));

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

	if (combo > maxCombo)
		maxCombo = combo;
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

		}
		else
		{
			EECList.erase(EECList.begin() + i);
			i--;
		}
	}
	/*
	if (InputManager::GetKeyDown('T'))
	{
		StartNode(&project->songDataList[1]);
	}
	if (InputManager::GetKeyDown('G'))
	{
		StartNode(&project->songDataList[2]);
	}
	if (InputManager::GetKeyDown('H'))
	{
		StartNode(&project->songDataList[3]);
	}
	if (InputManager::GetKeyDown('B'))
	{
		syncMode = true;
		syncTotalTime = 0.0;
		
		StartNode(&project->songDataList[0]);
	}*/
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
		menuMode = true;
		syncMode = false;
		syncTotalTime = 0.0;

		FMOD::Channel* channel;
		GameManager::soundSystem->playSound(Resources::GetSound(Sound_UI_Start_1), 0, false, &channel);
		GameManager::mainCamera->gameObject.lock()->
			GetComponent<CameraControl>()->HolyEffect(20.0, 1.0f, 0);
	}
	if (InputManager::GetKeyDown('L'))
	{
		project->autoAttack = !project->autoAttack;
	}


	if (playerAttackAnim)
	{
		if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - playerAnimTime).count() / 1000000.0 >= 0.51)
		{
			playerAttackAnim = false;
			playerAttackStack = 0;
			sr->SetSprite(Resources::GetSprite(sprite_player_idle));
			sr->animationLoop = true;
			sr->Reset();
			sr->Play();
		}
	}

	if (InputManager::GetKeyDown(MouseL))
	{
		for (int i = 0; i < 10; i++)
			velocityAvgs[i] = 0;
		menuMoveVelocity = 0;
		if (InputManager::mousePos.x() >= GameManager::RenderSize.x() - 200, InputManager::mousePos.y() <= 50)
		{
			if (!menuMode && nodeSystem->isStart)
				if (nodeSystem->isPause)
					nodeSystem->DisPause();
				else
					nodeSystem->Pause();
		}
	}
	if (InputManager::GetKey(MouseL))
	{
		for (int i = 10-1; i > 0; i--)
			velocityAvgs[i] = velocityAvgs[i - 1];
		velocityAvgs[0] = (InputManager::mousePos.y() - InputManager::mousePrevPos.y()) * 0.05;
		if (velocityAvgs[1] == 0)
			for (int i = 1; i < 10; i++)
				velocityAvgs[i] = velocityAvgs[0];
		menuMoveVelocity = -(InputManager::mousePos.y() - InputManager::mousePrevPos.y()) * 0.05;

	}
	if (InputManager::GetKeyUp(MouseL))
	{
		float velocityAvg = 0;
		for (int i = 0; i < 10; i++)
			velocityAvg += velocityAvgs[i];
		velocityAvg /= 10.0;

		menuMoveVelocity = -velocityAvg;

		auto startTrans = menuSongStart.lock()->transform;
		auto worldMousePos = GameManager::mainCamera->ScreenToWorld(InputManager::mousePos);
		if ((startTrans->position.x() - 240) < worldMousePos.x() && (startTrans->position.x() + 240) > worldMousePos.x())
		{
			if ((startTrans->position.y() - 48) < worldMousePos.y() && (startTrans->position.y() + 48) > worldMousePos.y())
			{
				if (selectMenuIndex != -1 && !project->songDataList[selectMenuIndex].isLock)
				{
					FMOD::Channel* channel;
					GameManager::soundSystem->playSound(Resources::GetSound(Sound_UI_Start_1), 0, false, &channel);
					GameManager::mainCamera->gameObject.lock()->
						GetComponent<CameraControl>()->HolyEffect(20.0, 1.0f, 0);

					StartNode(&project->songDataList[selectMenuIndex]);
				}
			}
		}
	}
	//*ConsoleDebug::console << nodeSystem->GetMusicTime() << "\n" << "\n";

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
	float hitDistance = project->inGame->playerHitDistance;
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
	if (this->type == 11)
	{
		trans->position += shockPower * (GameManager::deltaTime * 60);
		shockPower.x() = (abs(shockPower.x()) + ((0 - abs(shockPower.x())) / 28.0) * (GameManager::deltaTime * 60)) * this->dir;
		shockPower.y() += 0.381 * (GameManager::deltaTime * 60);
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