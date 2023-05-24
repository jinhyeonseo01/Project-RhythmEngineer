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

std::map<int, ID2D1Bitmap*> Resources::resourceMap = std::map<int, ID2D1Bitmap*>();


void GameManager::Init()
{
	
}

void GameManager::GameDestroy()
{
	for (int i = 0; i < mainWorld->objectList.size(); i++)
		if (mainWorld->objectList[i] != NULL)
			for (int j = 0; j < mainWorld->objectList[i]->componentList.size(); j++)
				if (std::dynamic_pointer_cast<SpriteRenderer>(mainWorld->objectList[i]->componentList[j]) != NULL)
					std::dynamic_pointer_cast<SpriteRenderer>(mainWorld->objectList[i]->componentList[j])->sprite = NULL;
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
	if(resourceMap.find(id) != resourceMap.end())
		return resourceMap[id];
	return NULL;
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
	//for()
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
	for (int i = 0; i < this->componentList.size(); i++)
	{
		componentList[i]->isDestroy = true;
	}
}

template std::shared_ptr<Transform> GameObject::AddComponent<Transform>(std::shared_ptr<Transform> element);
template std::shared_ptr<Camera> GameObject::AddComponent<Camera>(std::shared_ptr<Camera> element);
template std::shared_ptr<SpriteRenderer> GameObject::AddComponent<SpriteRenderer>(std::shared_ptr<SpriteRenderer> element);
template <typename T> std::shared_ptr<T> GameObject::AddComponent(std::shared_ptr<T> element)
{
	for (int i = 0; i < this->componentList.size(); i++)
	{
		if (this->componentList[i] == element)
			return NULL;
	}
	std::shared_ptr<Component> component = std::dynamic_pointer_cast<Component>(element);
	if (component != NULL)
		this->componentList.push_back(component);
	else
		return NULL;

	component->gameObject = weak_from_this();
	return element;
}

template <typename T> std::shared_ptr<T> GameObject::GetComponent()
{
	std::shared_ptr<T> component = NULL;
	for (int i = 0; i < this->componentList.size(); i++)
	{
		if ((component = std::dynamic_pointer_cast<std::shared_ptr<T>>(this->componentList[i])) != NULL)
			return component;
	}
	return NULL;
}

template <typename T> std::vector<std::shared_ptr<T>> GameObject::GetComponents()
{
	std::vector<std::shared_ptr<T>> components;
	std::shared_ptr<T> component = NULL;
	for (int i = 0; i < this->componentList.size(); i++)
	{
		if ((component = std::dynamic_pointer_cast<std::shared_ptr<T>>(this->componentList[i])) != NULL)
			components.push_back(component);
	}
	return components;
}

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
template void Camera::PushRenderer<SpriteRenderer>(SpriteRenderer* element);
template <typename T> void Camera::PushRenderer(T* element)
{
	Renderer* renderer = dynamic_cast<Renderer*>(element);
	if (renderer != NULL)
	{
		rendererList.push_back(renderer);
	}
}


void Renderer::BeforeRender()
{
	Component::BeforeRender();
	GameManager::mainCamera->PushRenderer(this);
}

void SpriteRenderer::BeforeRender()
{
	Renderer::BeforeRender();

}
void Renderer::Render(HDC hdc, Camera* camera)
{

}

SpriteRenderer::~SpriteRenderer()
{
	this->sprite = NULL;
}

void SpriteRenderer::SetSprite(CImage* sprite)
{
	this->sprite = sprite;
}
void SpriteRenderer::SetSprite2(ID2D1Bitmap* sprite2)
{
	this->sprite2 = sprite2;
}

void SpriteRenderer::Render(HDC hDC, Camera* camera)
{
	if (GameManager::RenderMode == 0)
	{
		CImage* img = this->sprite;
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

		float imageOffsetx = imageOffset.x();
		float imageOffsety = imageOffset.y();
		float imageScalex = imageScale.x();
		float imageScaley = imageScale.y();

		float imageW = sprite2->GetSize().width;
		float imageH = sprite2->GetSize().height;
		float sizex = renderSize.x() * this->gameObject.lock()->transform->localScale.x();
		float sizey = renderSize.y() * this->gameObject.lock()->transform->localScale.y();

		Eigen::Vector3d stretch = Eigen::Vector3d(sizex, sizey, 0);
		stretch = GameManager::mainCamera->gameObject.lock()->transform->GetW2LMatScale() * stretch;

		float filterX = imageW * imageOffsetx;
		float filterY = imageH * imageOffsety;
		float filterW = imageW * imageScalex;
		float filterH = imageH * imageScaley;

		D2D1_RECT_F RectF = D2D1::RectF(ScreenPos.x() - stretch.x() * pivot.x(), ScreenPos.y() - stretch.y() * pivot.y(), ScreenPos.x() + stretch.x() * (1 - pivot.x()), ScreenPos.y() + stretch.y() * (1 - pivot.y()));
		D2D1_RECT_F RectF2 = D2D1::RectF(filterX, filterY, filterX + filterW, filterY + filterH);

		GameManager::mainRT->SetTransform(
			D2D1::Matrix3x2F(flip.x() == 0 ? 1 : -1, 0, 0, flip.y() == 0 ? 1 : -1, 0, 0)
			* D2D1::Matrix3x2F::Translation((flip.x() == 0 ? 0 : 1) * (ScreenPos.x() * 2), (flip.y() == 0 ? 0 : 1) * (ScreenPos.y() * 2))
			* D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(ScreenPos.x(), ScreenPos.y())));
		ID2D1Bitmap* bitmap = this->sprite2;
		GameManager::mainRT->DrawBitmap(bitmap, RectF, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, // 이 값이 보간 모드
			RectF2);
	}
}