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

#define D2R 0.0174532f
#define R2D 57.295779f

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

	virtual void Render(HDC hdc);
	template <typename T> void PushRenderer(T element);
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
	virtual void Render(HDC hdc, Camera* camera);

	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void BeforeRender();
	virtual void Start() {};
	virtual void Enable() {};
	virtual void Disable() {};
	virtual void Destroy() {};
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
};

class GameManager
{
public:
	static std::shared_ptr<Camera> mainCamera;
	static std::shared_ptr<World> mainWorld;


	static float targetFrame;
	static float targetFrameBetween;


	static std::chrono::steady_clock::time_point updatePrevClock;
	static std::chrono::steady_clock::time_point updateNowClock;

	static int screenX;
	static int screenY;

	static int viewX;
	static int viewY;

	static RECT viewSize;
	static RECT monitorSize;

	static void Init();
};