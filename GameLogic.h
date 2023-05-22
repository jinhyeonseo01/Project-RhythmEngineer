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
#include <wincodec.h>

#define D2R 0.0174532f
#define R2D 57.295779f
#define PI 3.141592f

class GameObject;
class World;
class Component;
class Transform;
class Camera;
class Renderer;
class SpriteRenderer;
class GameManager;


class Component
{
public:
	bool isActive = true;
	bool idFirst = true;
	bool isDestroy = false;

	//ID2D1Bitmap

	std::shared_ptr<GameObject> gameObject = NULL;

	virtual void Update(){};
	virtual void LateUpdate() {};
	virtual void BeforeRender() {};
	virtual void Start() {};
	virtual void Enable () {};
	virtual void Disable() {};
	virtual void Destroy() {};
};
class Renderer;

class Transform : public Component
{
public:
	Eigen::Vector2d position = Eigen::Vector2d(0, 0);
	Eigen::Vector2d localScale = Eigen::Vector2d(1, 1);
	float rotationZ = 0;
	Eigen::Matrix3d GetL2WMat();
	Eigen::Matrix3d GetW2LMat();
	Eigen::Matrix3d GetL2WMatScale();
	Eigen::Matrix3d GetW2LMatScale();
	Eigen::Matrix3d GetW2LMatRotate();
	Eigen::Matrix3d GetL2WMatRotate();

	Eigen::Matrix3d GetL2WMatPosition();
	Eigen::Matrix3d GetW2LMatPosition();

	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void BeforeRender() {};
	virtual void Start() {};
	virtual void Enable() {};
	virtual void Disable() {};
	virtual void Destroy() {};
};

class Camera : public Component
{
public:
	Eigen::Vector2d cameraScale = Eigen::Vector2d(1, 1);
	std::vector<Renderer*> rendererList;
	
	Eigen::Matrix3d ScreenTranslateMatrix();
	virtual void Render(HDC hdc);
	template <typename T> void PushRenderer(T* element);
};

class Renderer : public Component
{
public:
	int zIndex = 0;
	virtual void Render(HDC hdc, Camera* camera);

	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void BeforeRender();
	virtual void Start() {};
	virtual void Enable() {};
	virtual void Disable() {};
	virtual void Destroy() {};
};

class SpriteRenderer : public Renderer
{
public:
	CImage* sprite = NULL;
	ID2D1Bitmap* sprite2 = NULL;

	Eigen::Vector2d renderSize = Eigen::Vector2d(100, 100);
	Eigen::Vector2d pivot = Eigen::Vector2d(0, 0);
	Eigen::Vector2d flip = Eigen::Vector2d(0, 0);
	Eigen::Vector2d imageOffset = Eigen::Vector2d(0, 0);
	Eigen::Vector2d imageScale = Eigen::Vector2d(1, 1);
	bool alpha = true;

	virtual void SetSprite(CImage* sprite);
	virtual void SetSprite2(ID2D1Bitmap* sprite);
	virtual void Render(HDC hdc, Camera* camera);

	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void BeforeRender();
	virtual void Start() {};
	virtual void Enable() {};
	virtual void Disable() {};
	virtual void Destroy() {};
	~SpriteRenderer();
};

class GameObject
{
public:
	bool isActive = true;
	bool idFirst = true;
	bool isDestroy = false;

	std::vector<std::shared_ptr<Component>> componentList;
	std::shared_ptr<Transform> transform = NULL;
	template <typename T> std::shared_ptr<T> AddComponent(std::shared_ptr<T> element);
	template <typename T> std::shared_ptr<T> GetComponent();
	template <typename T> std::vector<std::shared_ptr<T>> GetComponents();
	GameObject();
	~GameObject();
	void Destroy();
};



class World
{
public:
	std::vector<std::shared_ptr<GameObject>> objectList;
	std::shared_ptr<GameObject> CreateGameObject();

	virtual void Init();
	virtual void Update();
};

class GameManager
{
public:
	static std::shared_ptr<Camera> mainCamera;
	static std::shared_ptr<World> mainWorld;

	static bool targetFrameLock;
	static float targetFrame;
	static float targetFrameBetween;

	static float deltaTime;

	static std::chrono::steady_clock::time_point updatePrevClock;
	static std::chrono::steady_clock::time_point updateNowClock;

	static int screenX;
	static int screenY;

	static int viewX;
	static int viewY;

	static RECT viewSize;
	static RECT monitorSize;
	static Eigen::Vector2d RenderSize;

	static ID2D1Factory* mainFactory;
	static ID2D1HwndRenderTarget* mainRT;
	static IWICImagingFactory* wicFactory;
	static ID2D1SolidColorBrush* pBrush;
	static IDWriteFactory* pWFactory;
	static IDWriteTextFormat* pWFormat;

	static int RenderMode;

	static int gameDestroy;

	static void Init();
	static void GameDestroy();
};

class Resources
{
public:
	static std::map<int, ID2D1Bitmap*> resourceMap;
	static ID2D1Bitmap* ImageLoading(int id, LPCTSTR path);
	static ID2D1Bitmap* GetImage(int id);
};