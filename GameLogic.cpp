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
#include <Thread>

#include <Eigen/Dense>

#include "TotalLoader.h"

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

std::shared_ptr<Camera> GameManager::mainCamera = NULL;
std::shared_ptr<World> GameManager::mainWorld = NULL;


bool GameManager::targetFrameLock = true;
float GameManager::targetFrame = 60;
float GameManager::targetFrameBetween = ((float)1000) / GameManager::targetFrame;

float GameManager::deltaTime = 1 / GameManager::targetFrame;

std::chrono::steady_clock::time_point GameManager::updatePrevClock;
std::chrono::steady_clock::time_point GameManager::updateNowClock;
std::chrono::steady_clock::time_point GameManager::GameStartClock;

int GameManager::screenX = 2560;
int GameManager::screenY = 1440;

Eigen::Vector2d GameManager::RenderSize = Eigen::Vector2d(1920, 1080);

int GameManager::viewX = 1920;
int GameManager::viewY = 1080;

HWND GameManager::mainHWnd = NULL;

RECT GameManager::viewSize;
RECT GameManager::monitorSize;
int GameManager::gameDestroy = false;

ID2D1Factory* GameManager::mainFactory = NULL;
ID2D1HwndRenderTarget* GameManager::mainRT = NULL;
IWICImagingFactory* GameManager::wicFactory = NULL;
ID2D1SolidColorBrush* GameManager::pBrush = NULL;
IDWriteFactory* GameManager::pWFactory = NULL;
IDWriteTextFormat* GameManager::pWFormat = NULL;

int GameManager::RenderMode = 1;
FMOD::System* GameManager::soundSystem = nullptr;

float GameManager::masterSound = 1.0f;
float GameManager::BGMSound = 1.0f;
float GameManager::SFXSound = 1.0f;

std::map<int, ID2D1Bitmap*> Resources::resourceMap = std::map<int, ID2D1Bitmap*>();
std::map<int, FMOD::Sound*> Resources::soundMap = std::map<int, FMOD::Sound*>();
std::map<int, std::shared_ptr<Sprite>> Resources::spriteMap = std::map<int, std::shared_ptr<Sprite>>();

FMOD::ChannelGroup* GameManager::channelGroup = nullptr;
FMOD::DSP* GameManager::dsp = nullptr;
int GameManager::fftSize = 640;//256
FMOD_DSP_PARAMETER_FFT* GameManager::fftData = nullptr;

void* GameManager::extradriverdata = nullptr;
void GameManager::Init()
{
	FMOD::System_Create(&GameManager::soundSystem);
	soundSystem->init(64, FMOD_INIT_NORMAL, extradriverdata);
	//result = soundSystem->playSound(sound, 0, false, &channel);
	//channel->setVolume(0.4f);
}

void GameManager::BeforeUpdate()
{
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('Q'))
	{
		//DestroyWindow(hWnd);
		GameManager::gameDestroy = true;
		DestroyWindow(GameManager::mainHWnd);
	}
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('D'))
	{
		if (ConsoleDebug::console->consoleActive)
			ConsoleDebug::console->Close();
		else
			ConsoleDebug::console->Start();

		ConsoleDebug::console->consoleActive = !ConsoleDebug::console->consoleActive;
	}
	if (InputManager::GetKeyDown('S'))
	{
		//viewX = monitorSize.right;
		//viewY = monitorSize.bottom;
		//SetWindowPos(hWnd, HWND_TOP, screenX / 2 - viewX / 2, screenY / 2 - viewY / 2,
		//	viewX, viewY, SWP_FRAMECHANGED);
	}
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('1'))
		GameManager::targetFrame = 30;
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('2'))
		GameManager::targetFrame = 60;
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('3'))
		GameManager::targetFrame = 144;
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('4'))
		GameManager::targetFrame = 244;
	if (InputManager::GetKey(VK_CONTROL) && InputManager::GetKeyDown('0'))
		GameManager::targetFrameLock = !GameManager::targetFrameLock;
	if (InputManager::GetKeyDown(MouseL))
	{
		if (InputManager::mousePos.x() > (GameManager::viewSize.right - 40) && InputManager::mousePos.y() < 40)
			DestroyWindow(GameManager::mainHWnd);
	}
	if(GameManager::soundSystem != nullptr)
		GameManager::soundSystem->update();
}

void GameManager::GameDestroy()
{
	/*
	for (int i = 0; i < mainWorld->objectList.size(); i++)
		if (mainWorld->objectList[i] != NULL)
			for (int j = 0; j < mainWorld->objectList[i]->componentList.size(); j++)
				if (std::dynamic_pointer_cast<SpriteRenderer>(mainWorld->objectList[i]->componentList[j]) != NULL)
					std::dynamic_pointer_cast<SpriteRenderer>(mainWorld->objectList[i]->componentList[j])->sprite = NULL;
	*/
	mainWorld->objectList.clear();
	mainCamera = NULL;
	mainWorld = NULL;
}

ID2D1Bitmap* Resources::ImageLoading(int id, LPCTSTR path)
{
	resourceMap[id] = NULL;
	ID2D1Bitmap* bitmap = NULL;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICFormatConverter* pConverter = NULL;

	GameManager::wicFactory->CreateDecoderFromFilename(
		path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
	pDecoder->GetFrame(0, &pSource);

	GameManager::wicFactory->CreateFormatConverter(&pConverter);
	pConverter->Initialize(
		pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
		NULL, 0.0f, WICBitmapPaletteTypeCustom);
	GameManager::mainRT->CreateBitmapFromWicBitmap(pConverter, NULL, &bitmap);
	pConverter->Release();
	pDecoder->Release();
	pSource->Release();

	resourceMap[id] = bitmap;
	return bitmap;
}

ID2D1Bitmap* Resources::GetImage(int id)
{
	if (resourceMap.find(id) == resourceMap.end())
		return NULL;
	return resourceMap[id];
}

FMOD::Sound* Resources::SoundLoading(int id, LPCSTR path)
{
	FMOD::Sound* result;
	GameManager::soundSystem->createSound(path, FMOD_LOOP_OFF, 0, &result);
	soundMap[id] = result;
	return result;
}

FMOD::Sound* Resources::GetSound(int id)
{
	if (soundMap.find(id) == soundMap.end())
		return NULL;
	return soundMap[id];
}

std::weak_ptr<Sprite> Resources::PushSprite(int id, std::shared_ptr<Sprite> sprite)
{
	Resources::spriteMap[id] = sprite;
	return sprite;
}

std::weak_ptr<Sprite> Resources::GetSprite(int id)
{
	return Resources::spriteMap[id];
}

//World Init
std::shared_ptr<GameObject> World::CreateGameObject()
{
	auto obj = std::make_shared<GameObject>();
	this->objectList.push_back(obj);
	return obj;
}
void World::Init()
{
	std::shared_ptr<GameObject> mainCameraObject = this->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	mainCameraObject->transform->position = Eigen::Vector2d(960, 540);
}
void World::Update()
{
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if ((nowGameObject->isActive != nowGameObject->isPrevActive) && nowGameObject->isActive)
		{
			nowGameObject->isPrevActive = nowGameObject->isActive;
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->Enable();
			}
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isActive)
		{
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				if (nowComponent->isFirst)
				{
					nowComponent->Start();
					nowComponent->isFirst = false;
				}
			}
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isActive)
		{
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->Update();
			}
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isActive)
		{
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->LateUpdate();
			}
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isDestroy)
			nowGameObject->isActive = false;
		if ((nowGameObject->isActive != nowGameObject->isPrevActive) && (!nowGameObject->isActive))
		{
			nowGameObject->isPrevActive = nowGameObject->isActive;
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->Disable();
			}
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isDestroy)
		{
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->Destroy();
			}
			nowGameObject->Clear();
			objectList.erase(objectList.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < this->objectList.size(); i++)
	{
		auto nowGameObject = this->objectList[i];
		if (nowGameObject->isActive)
		{
			for (int j = 0; j < nowGameObject->componentList.size(); j++)
			{
				auto nowComponent = nowGameObject->componentList[j];
				nowComponent->BeforeRender();
			}
		}
	}

}


GameObject::GameObject()
{ 
	this->transform = this->AddComponent(std::make_shared<Transform>());
}
GameObject::~GameObject()
{
	this->componentList.clear();
}

void GameObject::Destroy()
{
	this->isDestroy = true;
}
void GameObject::Clear()
{
	this->isDestroy = true;
	componentList.clear();
	transform = nullptr;
}
/*
template std::shared_ptr<EnemyEntityComponent> GameObject::AddComponent<EnemyEntityComponent>(std::shared_ptr<EnemyEntityComponent> element);
template std::shared_ptr<Transform> GameObject::AddComponent<Transform>(std::shared_ptr<Transform> element);
template std::shared_ptr<Camera> GameObject::AddComponent<Camera>(std::shared_ptr<Camera> element);
template std::shared_ptr<SpriteRenderer> GameObject::AddComponent<SpriteRenderer>(std::shared_ptr<SpriteRenderer> element);
template std::shared_ptr<TestComponent> GameObject::AddComponent<TestComponent>(std::shared_ptr<TestComponent> element);
*/

//Trnasform Init
Eigen::Matrix3d Transform::GetL2WMat()
{
	Eigen::Matrix3d posM;
	Eigen::Matrix3d scaleM;
	Eigen::Matrix3d rotateM;
	posM <<
		1, 0, this->position.x(),
		0, 1, this->position.y(),
		0, 0, 1;
	scaleM <<
		this->localScale.x(), 0, 0,
		0, this->localScale.y(), 0,
		0, 0, 1;
	rotateM <<
		cos(this->rotationZ), -sin(this->rotationZ), 0,
		sin(this->rotationZ), cos(this->rotationZ), 0,
		0, 0, 1;
	return posM * rotateM * scaleM;
}

Eigen::Matrix3d Transform::GetW2LMat()
{
	return this->GetL2WMat().inverse();
}

//Trnasform Init
Eigen::Matrix3d Transform::GetL2WMatScale()
{
	Eigen::Matrix3d scaleM;
	scaleM <<
		this->localScale.x(), 0, 0,
		0, this->localScale.y(), 0,
		0, 0, 1;
	return scaleM;
}
Eigen::Matrix3d Transform::GetW2LMatScale()
{
	return this->GetL2WMatScale().inverse();
}
Eigen::Matrix3d Transform::GetL2WMatRotate()
{
	Eigen::Matrix3d rotateM;
	rotateM <<
		cos(this->rotationZ), -sin(this->rotationZ), 0,
		sin(this->rotationZ), cos(this->rotationZ), 0,
		0, 0, 1;
	return rotateM;
}
Eigen::Matrix3d Transform::GetW2LMatRotate()
{
	return this->GetL2WMatRotate().inverse();
}
Eigen::Matrix3d Transform::GetL2WMatPosition()
{
	Eigen::Matrix3d posM;
	posM <<
		1, 0, this->position.x(),
		0, 1, this->position.y(),
		0, 0, 1;
	return posM;
}
Eigen::Matrix3d Transform::GetW2LMatPosition()
{
	return this->GetL2WMatPosition().inverse();
}

Eigen::Matrix3d Camera::ScreenTranslateMatrix()
{
	
	Eigen::Matrix3d m;
	m << 1, 0, GameManager::RenderSize.x() / 2,
		 0, 1, GameManager::RenderSize.y() / 2,
		 0, 0, 1;

	return m;
}

//Camera Init

Eigen::Vector2d Camera::ScreenToWorld(Eigen::Vector2d pos)
{
	Eigen::Matrix3d m = GameManager::mainCamera->ScreenTranslateMatrix()
		* GameManager::mainCamera->gameObject.lock()->transform->GetW2LMat();
	return (m.inverse() * Eigen::Vector3d(pos.x(), pos.y(), 1)).head(2);
}
Eigen::Vector2d Camera::WorldToScreen(Eigen::Vector2d pos)
{
	Eigen::Matrix3d m = GameManager::mainCamera->ScreenTranslateMatrix()
		* GameManager::mainCamera->gameObject.lock()->transform->GetW2LMat();
	return (m * Eigen::Vector3d(pos.x(), pos.y(), 1)).head(2);
}

void Camera::Render(HDC hdc)
{
	sort(rendererList.begin(), rendererList.end(), [](auto a, auto b)
	{
		return a->zIndex < b->zIndex;
	});
	for (int i = 0; i < rendererList.size(); i++)
	{
		rendererList[i]->Render(hdc, this);
	}
	rendererList.clear();
}


void Camera::Update()
{
	Component::Update();

}



void SpriteRenderer::Enable()
{
	Component::Enable();
}

void SpriteRenderer::Disable()
{
	Component::Disable();
}

void SpriteRenderer::Update()
{
	Component::Update();
	if (IsPlay())
	{
		animationTime += GameManager::deltaTime * animationSpeedScale;
		auto sprite = this->sprite.lock();
		if (animationTime > sprite->time)
		{
			if (animationLoop)
				animationTime -= sprite->time;
			else
				Stop();
		}
	}
}
void SpriteRenderer::LateUpdate()
{
	Component::LateUpdate();
	auto sprite = this->sprite.lock();
	if (IsPlay())
	{
		animationIndex = animationTime / (sprite->time / sprite->imageList.size());
		animationIndex = min(animationIndex, sprite->imageList.size() - 1);
	}
}
void SpriteRenderer::Start()
{
	Component::Start();
}
void SpriteRenderer::Destroy()
{
	Component::Destroy();
}

void Renderer::BeforeRender()
{
	Component::BeforeRender();
	GameManager::mainCamera->PushRenderer(this);
}

void SpriteRenderer::BeforeRender()
{
	if (alphaValue != 0)
		Renderer::BeforeRender();

}
void Renderer::Render(HDC hdc, Camera* camera)
{

}

SpriteRenderer::~SpriteRenderer()
{
	
}

bool SpriteRenderer::IsPlay()
{
	return isAnimation == true;
}

void SpriteRenderer::Play()
{
	isAnimation = true;
}
void SpriteRenderer::Stop()
{
	isAnimation = false;
	animationTime = 0;
}

void SpriteRenderer::Pause()
{
	isAnimation = false;
}

void SpriteRenderer::Reset()
{
	animationIndex = 0;
	animationTime = 0;
}

void SpriteRenderer::SetSprite(std::weak_ptr<Sprite> sprite)
{
	this->sprite = sprite;
}

void SpriteRenderer::Render(HDC hDC, Camera* camera)
{
	if (GameManager::RenderMode == 0)
	{
		CImage* img = this->sprite2;
		Eigen::Vector2d pivot = Eigen::Vector2d(0, 0);
		Eigen::Vector2d imageOffset = Eigen::Vector2d(0, 0);
		Eigen::Vector2d imageScale = Eigen::Vector2d(1, 1);

		Eigen::Matrix3d m = this->gameObject.lock()->transform->GetL2WMat();
		m = GameManager::mainCamera->ScreenTranslateMatrix()
			* GameManager::mainCamera->gameObject.lock()->transform->GetW2LMat();

		float angle = this->gameObject.lock()->transform->rotationZ;
		float x = this->gameObject.lock()->transform->position.x();
		float y = this->gameObject.lock()->transform->position.y();
		float z = 1;

		Eigen::Vector3d ScreenPos = Eigen::Vector3d(x, y, 1);
		ScreenPos = (m * ScreenPos);
		auto t = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatRotate()
			* Eigen::Vector3d(cos(angle), sin(angle), 0);
		angle = atan2(t.y(), t.x()) * R2D;
		//*ConsoleDebug::console << ScreenPos.x() << ", " << ScreenPos.y() << "\n";

		float imageW = img->GetWidth();
		float imageH = img->GetHeight();
		int flipx = flip.x();
		int flipy = flip.y();
		float sizex = renderSize.x() * this->gameObject.lock()->transform->localScale.x();
		float sizey = renderSize.y() * this->gameObject.lock()->transform->localScale.y();
		float pivotx = pivot.x();
		float pivoty = pivot.y();
		float imageOffsetx = imageOffset.x();
		float imageOffsety = imageOffset.y();
		float imageScalex = imageScale.x();
		float imageScaley = imageScale.y();

		Eigen::Vector3d stretch = Eigen::Vector3d(sizex, sizey, 0);
		stretch = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatScale() * stretch;

		float filterX = imageW * imageOffsetx;
		float filterY = imageH * imageOffsety;
		float filterW = imageW * imageScalex;
		float filterH = imageH * imageScaley;
		BOOL bResult;
		float bmp3Size = sqrt(stretch.x() * stretch.x() + stretch.y() * stretch.y());
		//img2.m_r
	
		HDC m_hDC = img->GetDC();
		HDC m_hDC2 = CreateCompatibleDC(m_hDC);
		HDC m_hDC3 = CreateCompatibleDC(m_hDC);

		auto bmp = CreateCompatibleBitmap(m_hDC, stretch.x(), stretch.y());
		auto bmp3 = CreateCompatibleBitmap(m_hDC, bmp3Size * 2, bmp3Size * 2);

		SelectObject(m_hDC2, bmp);
		StretchBlt(m_hDC2, 0, 0, stretch.x(), stretch.y(), m_hDC,
			filterX + (flipx == 1 ? 0 : filterW - 1),
			filterY + (flipy == 1 ? 0 : filterH - 1),
			(flipx == 1 ? 1 : -1) * filterW, (flipy == 1 ? 1 : -1) * filterH, SRCCOPY);

		SelectObject(m_hDC3, bmp3);

		Eigen::Matrix3d rotateM;
		rotateM <<
			cos(angle * D2R), -sin(angle * D2R), 0,
			sin(angle * D2R), cos(angle * D2R), 0,
			0, 0, 1;
		Eigen::Vector3d v1, v2, v3;
		v1 << stretch.x(), 0, 1;
		v2 << 0, stretch.y(), 1;
		v3 << -stretch.x() * pivotx, -stretch.y() * pivoty, 1;
		v1 = rotateM * v1;
		v2 = rotateM * v2;
		v3 = rotateM * v3;

		POINT a[3] = {
			{bmp3Size + v3.x(), bmp3Size + v3.y()},
			{bmp3Size + v3.x() + v1.x(), bmp3Size + v3.y() + v1.y()},
			{bmp3Size + v3.x() + v2.x(), bmp3Size + v3.y() + v2.y()},
		};
		if (alpha == 1)
		{
			if (round(angle * 1000) != 0)
			{
				::PlgBlt(m_hDC3, a, m_hDC2,
					0, 0,
					stretch.x(), stretch.y(),
					NULL, NULL, NULL);
			}
			else
			{
				::BitBlt(m_hDC3, bmp3Size, bmp3Size, bmp3Size, bmp3Size, m_hDC2, 0, 0, SRCCOPY);
			}
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bResult = ::AlphaBlend(hDC,
				ScreenPos.x() - bmp3Size, ScreenPos.y() - bmp3Size,
				bmp3Size * 2, bmp3Size * 2,
				m_hDC3, 0, 0,
				bmp3Size * 2, bmp3Size * 2, bf);
		}
		else
		{
			if (round(angle * 1000) != 0)
			{
				a[0].x += ScreenPos.x() - bmp3Size;
				a[0].y += ScreenPos.y() - bmp3Size;
				a[1].x += ScreenPos.x() - bmp3Size;
				a[1].y += ScreenPos.y() - bmp3Size;
				a[2].x += ScreenPos.x() - bmp3Size;
				a[2].y += ScreenPos.y() - bmp3Size;
				::PlgBlt(hDC, a, m_hDC2,
					0, 0,
					stretch.x(), stretch.y(),
					NULL, NULL, NULL);
			}
			else
			{
				::BitBlt(hDC, ScreenPos.x(), ScreenPos.y(), bmp3Size, bmp3Size, m_hDC2, 0, 0, SRCCOPY);
			}
		}
		img->ReleaseDC();
		DeleteObject(m_hDC2);
		DeleteObject(bmp);

		DeleteObject(m_hDC3);
		DeleteObject(bmp3);
	}
	else
	{
		std::shared_ptr<Sprite> sprite = this->sprite.lock();
		Eigen::Matrix3d m = this->gameObject.lock()->transform->GetL2WMat();
		m = GameManager::mainCamera->ScreenTranslateMatrix()
			* GameManager::mainCamera->gameObject.lock()->transform->GetW2LMat();

		float angle = this->gameObject.lock()->transform->rotationZ;
		float x = this->gameObject.lock()->transform->position.x();
		float y = this->gameObject.lock()->transform->position.y();
		float z = 1;

		Eigen::Vector3d ScreenPos = Eigen::Vector3d(x, y, 1);
		ScreenPos = (m * ScreenPos);
		auto t = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatRotate()
			* Eigen::Vector3d(cos(angle), sin(angle), 0);
		angle = atan2(t.y(), t.x()) * R2D;

		float imageOffsetx = sprite->spriteOffset.x();
		float imageOffsety = sprite->spriteOffset.y();
		float imageScalex = sprite->spriteTiled.x();
		float imageScaley = sprite->spriteTiled.y();

		float imageW = sprite->spriteSize.x();
		float imageH = sprite->spriteSize.y();
		float sizex = renderSize.x() * this->gameObject.lock()->transform->localScale.x();
		float sizey = renderSize.y() * this->gameObject.lock()->transform->localScale.y();

		Eigen::Vector3d stretch = Eigen::Vector3d(sizex, sizey, 0);
		stretch = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatScale() * stretch;

		float filterX = imageW * imageOffsetx;
		float filterY = imageH * imageOffsety;
		float filterW = imageW * imageScalex;
		float filterH = imageH * imageScaley;

		D2D1_RECT_F RectF = D2D1::RectF(ScreenPos.x() - stretch.x() * sprite->pivot.x(), ScreenPos.y() - stretch.y() * sprite->pivot.y(), ScreenPos.x() + stretch.x() * (1 - sprite->pivot.x()), ScreenPos.y() + stretch.y() * (1 - sprite->pivot.y()));
		D2D1_RECT_F RectF2 = D2D1::RectF(filterX, filterY, filterX + filterW, filterY + filterH);

		GameManager::mainRT->SetTransform(
			D2D1::Matrix3x2F(flip.x() == 0 ? 1 : -1, 0, 0, flip.y() == 0 ? 1 : -1, 0, 0)
			* D2D1::Matrix3x2F::Translation((flip.x() == 0 ? 0 : 1) * (ScreenPos.x() * 2), (flip.y() == 0 ? 0 : 1) * (ScreenPos.y() * 2))
			* D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(ScreenPos.x(), ScreenPos.y())));
		ID2D1Bitmap* bitmap = sprite->imageList[animationIndex];
		GameManager::mainRT->DrawBitmap(bitmap, RectF, alphaValue, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, // 이 값이 보간 모드
			RectF2);
	}
}

void OverlayRenderer::Render(HDC hdc, Camera* camera)
{
	auto spriteVigntte = this->sprite.lock()->imageList[animationIndex];
	D2D1_RECT_F RectF = D2D1::RectF(0, 0, GameManager::viewSize.right, GameManager::viewSize.bottom);
	D2D1_RECT_F RectF2 = D2D1::RectF(0, 0, spriteVigntte->GetSize().width, spriteVigntte->GetSize().height);

	GameManager::mainRT->SetTransform(D2D1::Matrix3x2F(1, 0, 0, 1, 0, 0));
	GameManager::mainRT->DrawBitmap(spriteVigntte, RectF, this->alphaValue, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, RectF2);
}

void OverlayRenderer::LateUpdate()
{
	Component::LateUpdate();
	auto sprite = this->sprite.lock();
	if (IsPlay())
	{
		animationIndex = animationTime / (sprite->time / sprite->imageList.size());
		animationIndex = min(animationIndex, sprite->imageList.size() - 1);
	}
}

void TextComponent::Render(HDC hdc, Camera* camera)
{
	Eigen::Matrix3d m = this->gameObject.lock()->transform->GetL2WMat();
	m = GameManager::mainCamera->ScreenTranslateMatrix()
		* GameManager::mainCamera->gameObject.lock()->transform->GetW2LMat();

	float angle = this->gameObject.lock()->transform->rotationZ;
	float x = this->gameObject.lock()->transform->position.x();
	float y = this->gameObject.lock()->transform->position.y();
	float z = 1;

	Eigen::Vector2d pivot = Eigen::Vector2d(0.5, 0.5);

	Eigen::Vector3d ScreenPos = Eigen::Vector3d(x, y, 1);
	ScreenPos = (m * ScreenPos);
	auto t = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatRotate()
		* Eigen::Vector3d(cos(angle), sin(angle), 0);
	angle = atan2(t.y(), t.x()) * R2D;

	float sizex = 1000 * this->gameObject.lock()->transform->localScale.x();
	float sizey = 100 * this->gameObject.lock()->transform->localScale.y();
	Eigen::Vector3d stretch = Eigen::Vector3d(sizex, sizey, 0);

	D2D1_RECT_F RectF = D2D1::RectF(ScreenPos.x() - stretch.x() * pivot.x(), ScreenPos.y() - stretch.y() * pivot.y(), ScreenPos.x() + stretch.x() * (1 - pivot.x()), ScreenPos.y() + stretch.y() * (1 - pivot.y()));

	GameManager::mainRT->SetTransform(
		D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(ScreenPos.x(), ScreenPos.y())));
	//---------------------------여기작업중ㅇ이였음-------------------------

	//TCHAR tempConsole[100]; int indexY = 0;
	//_stprintf_s(tempConsole, L"Cytus2 : Urgency-SIHanatsuka");
	//GameManager::mainRT->DrawTextW(tempConsole, wcslen(tempConsole), font, RectF, GameManager::pBrush);
	GameManager::mainRT->DrawTextW(text, wcslen(text), font, RectF, brush);

}

unsigned int InputManager::KeyMaskData[KeyDataSize];
KeyData InputManager::KeyData[KeyDataSize];

Eigen::Vector2d InputManager::mousePos = Eigen::Vector2d(0, 0);
Eigen::Vector2d InputManager::mousePrevPos = Eigen::Vector2d(0, 0);

void InputManager::SetKeyState(int keyID, unsigned int bitMask)
{
	KeyMaskData[keyID] = bitMask;
}
void InputManager::SetKeyDownTime(int keyID, std::chrono::steady_clock::time_point time)
{
	KeyData[keyID].KeyDown = time;
}
void InputManager::SetKeyUpTime(int keyID, std::chrono::steady_clock::time_point time)
{
	KeyData[keyID].KeyUp = time;
}

std::chrono::steady_clock::time_point InputManager::GetKeyDownTime(int keyID)
{
	return KeyData[keyID].KeyDown;
}
std::chrono::steady_clock::time_point InputManager::GetKeyUpTime(int keyID)
{
	return KeyData[keyID].KeyUp;
}

unsigned int InputManager::GetKeyState(int keyID)
{
	return KeyMaskData[keyID];
}

bool InputManager::GetKeyDown(int keyID)
{
	return (KeyMaskData[keyID] & KeyDownBit) != 0;
}
bool InputManager::GetKey(int keyID)
{
	return (KeyMaskData[keyID] & KeyStayBit) != 0;
}
bool InputManager::GetKeyUp(int keyID)
{
	return (KeyMaskData[keyID] & KeyUpBit) != 0;
}

void InputManager::BeforeUpdate()
{
	for (int keyIndex = 0; keyIndex < KeyDataSize; keyIndex++)
	{
		if (InputManager::GetKeyUp(keyIndex))
			InputManager::SetKeyState(keyIndex, InputManager::GetKeyState(keyIndex) & (~KeyStayBit));
		if (InputManager::GetKeyDown(keyIndex))
			InputManager::SetKeyState(keyIndex, InputManager::GetKeyState(keyIndex) | KeyStayBit);
	}
}

void InputManager::AfterUpdate()
{
	InputManager::mousePrevPos = InputManager::mousePos;
	for (int keyIndex = 0; keyIndex < KeyDataSize; keyIndex++)
	{
		if (InputManager::GetKeyDown(keyIndex))
			InputManager::SetKeyState(keyIndex, InputManager::GetKeyState(keyIndex) & (~KeyDownBit));
		if (InputManager::GetKeyUp(keyIndex))
		{
			InputManager::SetKeyState(keyIndex, InputManager::GetKeyState(keyIndex) & (~KeyStayBit));
			InputManager::SetKeyState(keyIndex, InputManager::GetKeyState(keyIndex) & (~KeyUpBit));
		}
	}
}


void NodeSystem::Start(FMOD::Sound* sound)
{
	if (!isStart)
	{
		isStart = true;
		isPause = false;
		delayTotalTime = 0;
		unsigned int tempMosicMS;
		sound->getLength(&tempMosicMS, FMOD_TIMEUNIT_MS);
		musicTotalSecond = tempMosicMS / 1000.0;
		GameManager::soundSystem->playSound(sound, GameManager::channelGroup, false, &this->musicChannel);
		this->startClock = std::chrono::steady_clock::now();
	}
	//auto betweenClock = std::chrono::duration_cast<std::chrono::microseconds>();
	//sound.play
}
void NodeSystem::Pause()
{
	if (!isPause && isStart)
	{
		isPause = true;

		bool playing = false;
		this->musicChannel->isPlaying(&playing);
		if (playing)
			this->musicChannel->setPaused(true);
		this->pauseClock = std::chrono::steady_clock::now();
	}
}
void NodeSystem::DisPause()
{
	if (isPause && isStart)
	{
		isPause = false;

		bool channelPause = false;
		this->musicChannel->getPaused(&channelPause);
		if (channelPause)
			this->musicChannel->setPaused(false);
		delayTotalTime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - this->pauseClock).count() / 1000000.0;
	}
}

double NodeSystem::GetMusicTime()
{
	if (isStart)
	{
		double startToNowTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - this->startClock).count() / 1000000.0;
		if (isPause)
			startToNowTime = std::chrono::duration_cast<std::chrono::microseconds>(this->pauseClock - this->startClock).count() / 1000000.0;
		double musicTime = startToNowTime - delayTotalTime;
		return musicTime;
	}
	return 0.0;
}
double NodeSystem::GetDeltaTime(std::chrono::steady_clock::time_point time)
{
	if (isStart)
	{
		double startToNowTime = std::chrono::duration_cast<std::chrono::microseconds>(time - this->startClock).count() / 1000000.0;
		if (isPause)
			startToNowTime = std::chrono::duration_cast<std::chrono::microseconds>(this->pauseClock - this->startClock).count() / 1000000.0;
		double musicTime = startToNowTime - delayTotalTime;
		return musicTime;
	}
	return -1.0;
}

double NodeSystem::GetMusicTotalTime()
{
	return musicTotalSecond;
}

void NodeSystem::Stop()
{
	if (isStart)
	{
		isStart = false;
		isPause = false;
		delayTotalTime = 0;
		this->musicChannel->stop();
		this->musicChannel = nullptr;
	}
}


void JsonReader::Write(std::string path, nlohmann::json json)
{
	std::ofstream json_file;
	json_file.open(path);
	json_file << json;
	json_file.close();
}


nlohmann::json JsonReader::Read(std::string path)
{
	nlohmann::json json;
	std::string s, total;
	std::ifstream json_file;
	json_file.open(path);

	while (!json_file.eof())
	{
		json_file >> s;
		total += s;
	}
	json = json.parse(total);
	json_file.close();
	*ConsoleDebug::console << total << "\n";
	return json;
}

void replaceAll(std::string& name, const char* findStr, const char* to)
{
	int pos;
	while ((pos = name.find(findStr)) != std::string::npos)
		name.replace(pos, strlen(findStr), to);
}

std::string JsonReader::stringFormat(std::string s)
{
	replaceAll(s, "<space>", " ");
	return s;
}
