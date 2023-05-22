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

void GameDebugRender(HDC hDC, ID2D1HwndRenderTarget* dPT);

void GameInit();
void GameUpdate(HWND wnd);
void GameRender();

void CreateDirect(HWND hWnd);

ProjectI projectI;

CImage img;
CImage img2;

std::shared_ptr<SpriteRenderer> sr;
std::shared_ptr<SpriteRenderer> sr2;

void FrameInit(int count);
void FrameUpdate();

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

	CreateDirect(wnd);

	projectI.Init();

	img.Load(L".\\Resources\\Image\\Back_2.png");
	img2.Load(L".\\Resources\\Image\\Back4_2.png");
	std::shared_ptr<GameObject> obj = GameManager::mainWorld->CreateGameObject();
	sr = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr->SetSprite2(Resources::GetImage(0));
	sr->SetSprite(&img);
	sr->pivot = Eigen::Vector2d(0.5f, 0.5f);
	obj->transform->position = Eigen::Vector2d(100, 100);

	std::shared_ptr<GameObject> obj2 = GameManager::mainWorld->CreateGameObject();
	sr2 = obj2->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
	sr2->renderSize = Eigen::Vector2d(1920, 1080);
	sr2->SetSprite(&img2);
	sr2->SetSprite2(Resources::GetImage(1));
	sr2->pivot = Eigen::Vector2d(0, 0);
	sr2->alpha = 0;
	obj2->transform->position = Eigen::Vector2d(0, 0);

	FrameInit(8);

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
		if(!GameManager::gameDestroy)
			GameUpdate(wnd);
	}
	//delete ConsoleDebug::console;
	//DestroyWindow(wnd);
	//UnregisterClass(className, hInst);
	//return message.wParam;
	//m_pRenderTarget->Resize
	return 0;
}



void CreateDirect(HWND hWnd)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &(GameManager::mainFactory));
	
	D2D1_SIZE_U size = D2D1::SizeU(GameManager::viewX, GameManager::viewY);
	GameManager::mainFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), &GameManager::mainRT);
	
	WICBitmapAlphaChannelOption opt = WICBitmapUseAlpha;
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&GameManager::wicFactory));

	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&GameManager::pWFactory));
	GameManager::mainRT->CreateSolidColorBrush(
		D2D1::ColorF(
			D2D1::ColorF::Black
		), &GameManager::pBrush);
	GameManager::pWFactory->CreateTextFormat(
		L"맑은 고딕",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15.0f,
		L"ko-kr",
		&GameManager::pWFormat);
	//pWFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//pWFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void GameInit()
{
	debug(L"- Console Test Start -\n");
	GameManager::mainWorld = std::make_shared<World>();

	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());
	mainCameraObject->transform->position = Eigen::Vector2d(960, 540);

	GameManager::updateNowClock = std::chrono::steady_clock::now();
	GameManager::updatePrevClock = GameManager::updateNowClock;

	GameManager::GameStartClock = std::chrono::steady_clock::now();
	//----World Load로 옮길것.
}

float deltatime = 0;
float totalTime = 0;

int frameCount = 0;
float frameTotal = 0;
int frame[100];
std::chrono::steady_clock::time_point frameStartCheck;
std::chrono::steady_clock::time_point frameCheck[100];
void FrameInit(int count)
{
	frameCount = count;
	for (int i = 0; i < frameCount; i++)
	{
		frame[i] = -1;
		frameCheck[i] = std::chrono::steady_clock::now();
	}
	frameStartCheck = std::chrono::steady_clock::now();
}
void FrameUpdate()
{
	for (int i = 0; i < frameCount; i++)
	{
		if (frame[i] != -1 || (frame[i] == -1 && std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frameStartCheck).count() >= ((1000000.0f / frameCount) * i)))
		{
			auto now = std::chrono::steady_clock::now();
			if (frame[i] == -1)
				frameCheck[i] = now;
			if (std::chrono::duration_cast<std::chrono::microseconds>(now - frameCheck[i]).count() >= 1000000)
			{
				frameTotal = (frameTotal * (((float)frameCount - 1.0f) / frameCount) + frame[i] * (1.0f / frameCount));
				frameCheck[i] = now;
				frame[i] = 0;
			}
			frame[i]++;
		}
	}
}

void GameUpdate(HWND wnd)
{
	GameManager::updateNowClock = std::chrono::steady_clock::now();
	auto betweenClock = std::chrono::duration_cast<std::chrono::microseconds>(GameManager::updateNowClock - GameManager::updatePrevClock);
	GameManager::updatePrevClock = GameManager::updateNowClock;
	deltatime = (((float)betweenClock.count()) / 1000);
	totalTime += deltatime;
	//(*ConsoleDebug::console).Clear();
	if (totalTime >= GameManager::targetFrameBetween || (!GameManager::targetFrameLock))
	{
		//(*ConsoleDebug::console) << totalTime << "\n";
		GameManager::deltaTime = totalTime / 1000.0f;
		FrameUpdate();
		GameRender();

		if (GameManager::targetFrameLock)
			while (totalTime >= GameManager::targetFrameBetween)
				totalTime -= GameManager::targetFrameBetween;
		else
			totalTime = 0;
	}
	else
		std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void GameRender()
{
	GetClientRect(mainHWnd, &GameManager::viewSize);
	HDC mainHDC = NULL;
	HDC hDC = NULL;
	HBITMAP hBitmapBuffer = NULL;
	if (GameManager::RenderMode == 0)
	{
		mainHDC = GetDC(mainHWnd);
		hDC = CreateCompatibleDC(mainHDC);
		hBitmapBuffer = CreateCompatibleBitmap(mainHDC, GameManager::RenderSize.x(), GameManager::RenderSize.y());
		SelectObject(hDC, hBitmapBuffer);
	}
	
	//sr->gameObject->transform->rotationZ += 15.0f * GameManager::deltaTime;
	GameManager::mainCamera->PushRenderer(sr2.get());
	GameManager::mainCamera->PushRenderer(sr.get());

	GameManager::mainRT->BeginDraw();
	GameManager::mainRT->SetTransform(D2D1::Matrix3x2F::Identity());
	GameManager::mainRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));

	GameManager::mainCamera->Render(hDC);

	GameDebugRender(hDC, GameManager::mainRT);
	if (GameManager::RenderMode == 0)
	{
		StretchBlt(mainHDC, 0, 0, GameManager::viewSize.right, GameManager::viewSize.bottom, hDC, 0, 0, GameManager::RenderSize.x(), GameManager::RenderSize.y(), SRCCOPY);
		DeleteDC(hDC);
		DeleteObject(hBitmapBuffer);
		ReleaseDC(mainHWnd, mainHDC);
	}
	GameManager::mainRT->EndDraw();
}

void GameDebugRender(HDC hDC, ID2D1HwndRenderTarget* dPT)
{
	TCHAR tempConsole[100]; int indexY = 0;
	if (GameManager::RenderMode == 0)
	{
		_stprintf_s(tempConsole, L"TargetFrameLock:%s", GameManager::targetFrameLock ? L"True" : L"False");
		TextOut(hDC, 10, 10 + (20 * (indexY++)), tempConsole, lstrlenW(tempConsole));
		_stprintf_s(tempConsole, L"TargetFrame:%.0f", GameManager::targetFrame);
		TextOut(hDC, 10, 10 + (20 * (indexY++)), tempConsole, lstrlenW(tempConsole));
		_stprintf_s(tempConsole, L"Frame:%d", (int)frameTotal);
		TextOut(hDC, 10, 10 + (20 * (indexY++)), tempConsole, lstrlenW(tempConsole));
	}
	else
	{
		dPT->SetTransform(D2D1::Matrix3x2F::Identity());
		D2D1_RECT_F layoutRect = D2D1::RectF(10, 10 + (20 * (indexY++)), 500, 10 + (20 * (indexY+1)));
		_stprintf_s(tempConsole, L"TargetFrameLock:%s", GameManager::targetFrameLock ? L"True" : L"False");
		dPT->DrawTextW(tempConsole, wcslen(tempConsole), GameManager::pWFormat, layoutRect, GameManager::pBrush);

		layoutRect = D2D1::RectF(10, 10 + (20 * (indexY++)), 500, 10 + (20 * (indexY + 1)));
		_stprintf_s(tempConsole, L"TargetFrame:%.0f", GameManager::targetFrame);
		dPT->DrawTextW(tempConsole, wcslen(tempConsole), GameManager::pWFormat, layoutRect, GameManager::pBrush);

		layoutRect = D2D1::RectF(10, 10 + (20 * (indexY++)), 500, 10 + (20 * (indexY + 1)));
		_stprintf_s(tempConsole, L"Frame:%d", (int)frameTotal);
		dPT->DrawTextW(tempConsole, wcslen(tempConsole), GameManager::pWFormat, layoutRect, GameManager::pBrush);
	}
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
			{
				//DestroyWindow(hWnd);
				GameManager::gameDestroy = true;
				DestroyWindow(hWnd);
				DestroyWindow(GetConsoleWindow());
			}
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
			/*
			if (w == 'R')
				pivotX -= 0.1f;
			if (w == 'T')
				pivotX += 0.1f;
			if (w == 'G')
				flipX = flipX ? 0 : 1;
			if (w == 'H')
				flipY = flipY ? 0 : 1;
				*/
			if (w == VK_RIGHT)
			{
				GameManager::mainCamera->gameObject.lock()->transform->rotationZ += 0.1f;
			}
			if (w == VK_LEFT)
			{
				GameManager::mainCamera->gameObject.lock()->transform->rotationZ -= 0.1f;
			}
			if (w == VK_UP)
			{
				GameManager::mainCamera->gameObject.lock()->transform->localScale += Eigen::Vector2d(0.1f, 0.1f);
			}
			if (w == VK_DOWN)
			{
				GameManager::mainCamera->gameObject.lock()->transform->localScale -= Eigen::Vector2d(0.1f, 0.1f);
			}
			if (w == 'W')
				GameManager::mainCamera->gameObject.lock()->transform->position += (GameManager::mainCamera->gameObject.lock()->transform->GetL2WMat() * Eigen::Vector3d(0, -10.0f, 0)).head<2>();
			if (w == 'S')
				GameManager::mainCamera->gameObject.lock()->transform->position += (GameManager::mainCamera->gameObject.lock()->transform->GetL2WMat() * Eigen::Vector3d(0, 10.0f, 0)).head<2>();
			if (w == 'A')
				GameManager::mainCamera->gameObject.lock()->transform->position += (GameManager::mainCamera->gameObject.lock()->transform->GetL2WMat() * Eigen::Vector3d(-10.0f, 0, 0)).head<2>();
			if (w == 'D')
				GameManager::mainCamera->gameObject.lock()->transform->position += (GameManager::mainCamera->gameObject.lock()->transform->GetL2WMat() * Eigen::Vector3d(+10.0f, 0, 0)).head<2>();
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
			HDC mainHDC = BeginPaint(mainHWnd, &ps);
			//GameRender();

			EndPaint(mainHWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			ConsoleDebug::console->Close();
			ConsoleDebug::console->consoleActive = false;

			//UnregisterClass(className, hInstMain);
			GameManager::GameDestroy();
			GameManager::gameDestroy = true;
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
