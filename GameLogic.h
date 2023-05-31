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
#include "fmod.hpp"
#include "fmod_errors.h"

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
class Sprite;
class InputManager;
class GameManager;

class Component
{
public:
	bool isFirst = true;

	std::weak_ptr<GameObject> gameObject;

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
	Eigen::Vector2d ScreenToWorld(Eigen::Vector2d pos);
	Eigen::Vector2d WorldToScreen(Eigen::Vector2d pos);
	template <typename T> void PushRenderer(T* element);
	virtual void Render(HDC hdc);
	virtual void Update();
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
	CImage* sprite2 = NULL;
	std::weak_ptr<Sprite> sprite;
	Eigen::Vector2d flip = Eigen::Vector2d(0, 0);
	Eigen::Vector2d renderSize = Eigen::Vector2d(100, 100);

	int animationIndex = 0;
	bool animationLoop = true;
	bool isAnimation = true;
	float animationTime = 0;
	float animationSpeedScale = 1.0f;

	bool alpha = true;

	virtual void SetSprite(std::weak_ptr<Sprite> sprite);

	void Play();
	void Stop();
	void Pause();
	void Reset();
	bool IsPlay();

	virtual void Render(HDC hdc, Camera* camera);

	virtual void Update();
	virtual void LateUpdate();
	virtual void BeforeRender();
	virtual void Start();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
	~SpriteRenderer();
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	bool isPrevActive = false;
	bool isActive = true;
	bool isCreated = true;
	bool isDestroy = false;

	std::vector<std::shared_ptr<Component>> componentList;
	std::shared_ptr<Transform> transform;
	template <typename T> std::shared_ptr<T> AddComponent(std::shared_ptr<T> element);
	template <typename T> std::shared_ptr<T> GetComponent();
	template <typename T> std::vector<std::shared_ptr<T>> GetComponents();
	GameObject();
	~GameObject();
	void Destroy();
	void Clear();
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
	static std::chrono::steady_clock::time_point GameStartClock;

	static int screenX;
	static int screenY;

	static int viewX;
	static int viewY;

	static HWND mainHWnd;

	static RECT viewSize;
	static RECT monitorSize;
	static Eigen::Vector2d RenderSize;

	static ID2D1Factory* mainFactory;
	static ID2D1HwndRenderTarget* mainRT;
	static IWICImagingFactory* wicFactory;
	static ID2D1SolidColorBrush* pBrush;
	static IDWriteFactory* pWFactory;
	static IDWriteTextFormat* pWFormat;

	static FMOD::System* soundSystem;
	static void* extradriverdata;

	static int RenderMode;

	static int gameDestroy;

	static void Init();
	static void BeforeUpdate();
	static void GameDestroy();
};

#define KeyDataSize 512

#define MouseL 0x100
#define MouseR 0x101
#define MouseM 0x102

#define KeyDownBit 0x01
#define KeyStayBit 0x02
#define KeyUpBit 0x04


class InputManager
{
public:
	static unsigned int KeyMaskData[KeyDataSize];
	static Eigen::Vector2d mousePos;
	static Eigen::Vector2d mousePrevPos;
	static void SetKeyState(int keyID, unsigned int bitMask);
	static unsigned int GetKeyState(int keyID);
	static bool GetKeyDown(int keyID);
	static bool GetKey(int keyID);
	static bool GetKeyUp(int keyID);
	static bool GetMouseDown(int keyID, int type);
	static bool GetMouse(int keyID, int type);
	static bool GetMouseUp(int keyID, int type);
	static void BeforeUpdate();
	static void AfterUpdate();
};

class Resources
{
public:
	static std::map<int, ID2D1Bitmap*> resourceMap;
	static std::map<int, FMOD::Sound*> soundMap;
	static std::map<int, std::shared_ptr<Sprite>> spriteMap;

	static ID2D1Bitmap* ImageLoading(int id, LPCTSTR path);
	static ID2D1Bitmap* GetImage(int id);

	static FMOD::Sound* SoundLoading(int id, LPCSTR path);
	static FMOD::Sound* GetSound(int id);

	static std::weak_ptr<Sprite> PushSprite(int id, std::shared_ptr<Sprite> sprite);
	static std::weak_ptr<Sprite> GetSprite(int id);
};


class Sprite
{
public:
	std::vector<ID2D1Bitmap*> imageList;
	Eigen::Vector2i spriteSize = Eigen::Vector2i(100, 100);
	Eigen::Vector2d pivot = Eigen::Vector2d(0, 0);
	Eigen::Vector2d spriteOffset = Eigen::Vector2d(0, 0);
	Eigen::Vector2d spriteTiled = Eigen::Vector2d(1, 1);
	float time = 1.0f;
	void PushImage(ID2D1Bitmap* image)
	{
		imageList.push_back(image);
	}
};

std::weak_ptr<Sprite> SpriteGroupLoad(const TCHAR* mainPath, const TCHAR* SubPath, const TCHAR* FName, int startIndex, const TCHAR* LName, int count, int resourceIndex, int spriteIndex);
