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



HINSTANCE hInstMain;
HWND mainHWnd;
WNDCLASSEX wndClass;

LPCTSTR className = L"Iaido";
LPCTSTR titleName = L"Iaido";

LRESULT CALLBACK WinCallBack(HWND hWnd, UINT u, WPARAM w, LPARAM l);

void GameInit();
void GameUpdate(HWND wnd);
void GameRender();

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmd, int comCount)
{
	hInstMain = hInst;
	MSG message;
	HWND wnd = NULL;
	GetWindowRect(GetDesktopWindow(), &GameManager::monitorSize); // 모니터 크기
	ConsoleDebug::consoleActive = true;
	GameManager::screenX = GameManager::monitorSize.right;
	GameManager::screenY = GameManager::monitorSize.bottom;
	ConsoleDebug::Init();
	GameManager::Init();
	GameInit();

	wndClass.cbSize = sizeof(wndClass);
	wndClass.lpfnWndProc = WinCallBack;
	wndClass.lpszClassName = (LPCWCHAR)titleName;
	wndClass.lpszMenuName = NULL;
	wndClass.hInstance = hInst;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(hInst, IDI_APPLICATION);
	wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
	wndClass.cbWndExtra = 0;
	wndClass.cbClsExtra = 0;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wndClass);

	mainHWnd = wnd = CreateWindow(className, titleName,
		WS_POPUP | WS_VISIBLE,
		GameManager::screenX / 2 - GameManager::viewX / 2, GameManager::screenY / 2 - GameManager::viewY / 2,
		GameManager::viewX, GameManager::viewY,
		wnd, NULL,
		hInst, 0);
	
	ShowWindow(wnd, comCount);
	UpdateWindow(wnd);

	BOOL bDone = FALSE;
	while (FALSE == bDone)
	{
		if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) {
				bDone = TRUE;
				break;
			}
		}
		
		GameUpdate(wnd);
	}
	//delete ConsoleDebug::console;
	//DestroyWindow(wnd);
	//UnregisterClass(className, hInst);
	//return message.wParam;
	return 0;
}

CImage img;
CImage img2;

void GameInit()
{
	debug(L"- Console Test Start -\n");
	GameManager::mainWorld = std::make_shared<World>();

	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	mainCameraObject->transform->position = Eigen::Vector2d(0, 0);

	GameManager::updateNowClock = std::chrono::steady_clock::now();
	GameManager::updatePrevClock = GameManager::updateNowClock;

	img.Load(L".\\Resources\\Image\\Back.png");
	img2.Load(L".\\Resources\\Image\\Back3.png");
}

float deltatime = 0;
float totalTime = 0;
float totalPerTime = 0;

void GameUpdate(HWND wnd)
{
	GameManager::updateNowClock = std::chrono::steady_clock::now();
	auto betweenClock = std::chrono::duration_cast<std::chrono::microseconds>(GameManager::updateNowClock - GameManager::updatePrevClock);
	deltatime = (((float)betweenClock.count()) / 1000);
	totalTime += deltatime;
	GameManager::updatePrevClock = GameManager::updateNowClock;
	//(*ConsoleDebug::console).Clear();
	while (totalTime > GameManager::targetFrameBetween)
	{
		//(*ConsoleDebug::console) << totalTime << "\n";

		//InvalidateRect(wnd, NULL, FALSE);
		GameRender();
		totalPerTime = totalTime;
		totalTime -= GameManager::targetFrameBetween;
	}
}
void GameRender()
{
	GetClientRect(mainHWnd, &GameManager::viewSize);
	HDC mainHDC = GetDC(mainHWnd);

	std::shared_ptr<GameObject> obj = GameManager::mainWorld->CreateGameObject();
	auto sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr->SetSprite(&img);
	obj->transform->position = Eigen::Vector2d(300, 300);
	GameManager::mainCamera->PushRenderer(sr.get());

	auto BitmapWidth = GameManager::viewSize.right;
	auto BitmapHeight = GameManager::viewSize.bottom;
	auto hDC = CreateCompatibleDC(mainHDC);
	auto hBitmapBuffer = CreateCompatibleBitmap(mainHDC, BitmapWidth, BitmapHeight);
	//hMemoryDC = CreateCompatibleDC(hDC2);

	SelectObject(hDC, hBitmapBuffer);

	img2.Draw(hDC, 0, 0, 400, 400);

	GameManager::mainCamera->Render(hDC);

	TCHAR tempConsole[100];
	_stprintf_s(tempConsole, L"%f", 1);
	TextOut(hDC, 700, 0, tempConsole, lstrlenW(tempConsole));

	BitBlt(mainHDC, 0, 0, BitmapWidth, BitmapHeight, hDC, 0, 0, SRCCOPY);
	DeleteDC(hDC);
	DeleteObject(hBitmapBuffer);
	ReleaseDC(mainHWnd, mainHDC);
}

LRESULT CALLBACK WinCallBack(HWND hWnd, UINT msgUD, WPARAM w, LPARAM l)
{
	PAINTSTRUCT ps;
	HDC hDC2;
	//--- 메시지 처리하기
	switch (msgUD)
	{
		case WM_CREATE:
		{
			GetClientRect(hWnd, &GameManager::viewSize);
			//SetTimer(hWnd, 1, 1, NULL);
		}
		case WM_TIMER:
		{
			
			break;
		}
		case WM_KEYDOWN:
		{
			if (w == 'Q')
				DestroyWindow(hWnd);
			if (w == 'E')
			{
				ConsoleDebug::console->Close();
				ConsoleDebug::console->consoleActive = false;
			}
			if (w == 'S')
			{
				//viewX = monitorSize.right;
				//viewY = monitorSize.bottom;
				//SetWindowPos(hWnd, HWND_TOP, screenX / 2 - viewX / 2, screenY / 2 - viewY / 2,
				//	viewX, viewY, SWP_FRAMECHANGED);
			}
			if (w == VK_RIGHT)
			{
				GameManager::mainCamera->gameObject->transform->rotationZ += 0.1f;
			}
			if (w == VK_LEFT)
			{
				GameManager::mainCamera->gameObject->transform->rotationZ -= 0.1f;
			}
			if (w == VK_UP)
			{
				GameManager::mainCamera->gameObject->transform->localScale += Eigen::Vector2d(0.1f, 0.1f);
			}
			if (w == VK_DOWN)
			{
				GameManager::mainCamera->gameObject->transform->localScale -= Eigen::Vector2d(0.1f, 0.1f);
			}
			if (w == 'W')
				GameManager::mainCamera->gameObject->transform->position += (GameManager::mainCamera->gameObject->transform->GetL2WMat() * Eigen::Vector3d(0, -10.0f, 0)).head<2>();
			if (w == 'S')
				GameManager::mainCamera->gameObject->transform->position += (GameManager::mainCamera->gameObject->transform->GetL2WMat() * Eigen::Vector3d(0, 10.0f, 0)).head<2>();
			if (w == 'A')
				GameManager::mainCamera->gameObject->transform->position += (GameManager::mainCamera->gameObject->transform->GetL2WMat() * Eigen::Vector3d(-10.0f, 0, 0)).head<2>();
			if (w == 'D')
				GameManager::mainCamera->gameObject->transform->position += (GameManager::mainCamera->gameObject->transform->GetL2WMat() * Eigen::Vector3d(+10.0f, 0, 0)).head<2>();
			break;
		}
		case WM_LBUTTONDOWN:
		{
			//if (LOWORD(l) > (GameManager::viewSize.right - 40) && HIWORD(l) < 40) // 게임종료
			//	DestroyWindow(hWnd);
			
			break;
		}
		case WM_PAINT:
		{
			hDC2 = BeginPaint(hWnd, &ps);
			
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			ConsoleDebug::console->Close();
			ConsoleDebug::console->consoleActive = false;

			//UnregisterClass(className, hInstMain);
			GameManager::GameDestroy();
			img.Destroy();
			img2.Destroy();
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hWnd, msgUD, w, l);
			break;
	}
	return 0;
}
