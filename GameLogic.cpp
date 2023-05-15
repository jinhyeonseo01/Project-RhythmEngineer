
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
#include <algorithm>

#include <Eigen/Dense>

#include "TotalLoader.h"

std::shared_ptr<Camera> GameManager::mainCamera = NULL;
std::shared_ptr<World> GameManager::mainWorld = NULL;


float GameManager::targetFrame = 60;
float GameManager::targetFrameBetween = ((float)1000) / GameManager::targetFrame;


std::chrono::steady_clock::time_point GameManager::updatePrevClock;
std::chrono::steady_clock::time_point GameManager::updateNowClock;

int GameManager::screenX = 2560;
int GameManager::screenY = 1440;

int GameManager::viewX = 1920;
int GameManager::viewY = 1080;

RECT GameManager::viewSize;
RECT GameManager::monitorSize;


void GameManager::Init()
{

}

//World Init
std::shared_ptr<GameObject> World::CreateGameObject()
{
	auto obj = std::make_shared<GameObject>();
	this->objectList.push_back(obj);
	return obj;
}


//GameObject Init

GameObject::GameObject()
{ 
	this->transform = std::make_shared<Transform>();
	this->AddComponent(this->transform);
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
	auto component = std::dynamic_pointer_cast<Component>(element);
	if (component != NULL)
		this->componentList.push_back(component);
	else
		return NULL;

	component->gameObject = std::shared_ptr<GameObject>(this);
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

template <typename T> void Camera::PushRenderer(T element)
{
	if (element != NULL)
	{
		rendererList.push_back(element);
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
void SpriteRenderer::Render(HDC hdc, Camera* camera)
{

}