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
//Lixound

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

	GameManager::mainHWnd = wnd = CreateWindow(className, titleName,
		WS_POPUP | WS_VISIBLE,
		GameManager::screenX / 2 - GameManager::viewX / 2, GameManager::screenY / 2 - GameManager::viewY / 2,
		GameManager::viewX, GameManager::viewY,
		wnd, NULL,
		hInst, 0);

	CreateDirect(wnd);

	projectI.Init();

	FrameInit(8);

	GameManager::mainWorld->Init();

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
			D2D1::ColorF::White
		), &GameManager::pBrush);
	GameManager::pWFactory->CreateTextFormat(
		L"PF스타더스트S Bold",
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

//--------Frame--------

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
				frameTotal = (frameTotal + frame[i]) / 2.0f;
				frameCheck[i] = now;
				frame[i] = 0;
			}
			frame[i]++;
		}
	}
}

//--------Game--------
void GameInit()
{
	debug(L"- Console Test Start -\n");
	timeBeginPeriod(1);
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	GameManager::updateNowClock = std::chrono::steady_clock::now();
	GameManager::updatePrevClock = GameManager::updateNowClock;
	GameManager::GameStartClock = std::chrono::steady_clock::now();

	for (int KeyIndex = 0; KeyIndex < KeyDataSize; KeyIndex++)
		InputManager::KeyMaskData[KeyIndex] = 0;
}

void GameUpdate(HWND wnd)
{
	GameManager::targetFrameBetween = ((float)1000) / GameManager::targetFrame;

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
		InputManager::BeforeUpdate();
		GameManager::BeforeUpdate();
		GameManager::mainWorld->Update();
		InputManager::AfterUpdate();

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
	GetClientRect(GameManager::mainHWnd, &GameManager::viewSize);
	HDC mainHDC = NULL;
	HDC hDC = NULL;
	HBITMAP hBitmapBuffer = NULL;
	if (GameManager::RenderMode == 0)
	{
		mainHDC = GetDC(GameManager::mainHWnd);
		hDC = CreateCompatibleDC(mainHDC);
		hBitmapBuffer = CreateCompatibleBitmap(mainHDC, GameManager::RenderSize.x(), GameManager::RenderSize.y());
		SelectObject(hDC, hBitmapBuffer);
	}

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
		ReleaseDC(GameManager::mainHWnd, mainHDC);
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
			if (!InputManager::GetKey(w))
			{
				InputManager::SetKeyState(w, InputManager::GetKeyState(w) | KeyDownBit);
				InputManager::SetKeyDownTime(w, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_KEYUP:
		{
			if (InputManager::GetKeyDown(w) || InputManager::GetKey(w))
			{
				InputManager::SetKeyState(w, InputManager::GetKeyState(w) | KeyUpBit);
				InputManager::SetKeyUpTime(w, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			InputManager::mousePos = Eigen::Vector2d(LOWORD(l), HIWORD(l));
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (!InputManager::GetKey(MouseL))
			{
				InputManager::SetKeyState(MouseL, InputManager::GetKeyState(MouseL) | KeyDownBit);
				InputManager::SetKeyDownTime(MouseL, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (InputManager::GetKeyDown(MouseL) || InputManager::GetKey(MouseL))
			{
				InputManager::SetKeyState(MouseL, InputManager::GetKeyState(MouseL) | KeyUpBit);
				InputManager::SetKeyUpTime(MouseL, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if (!InputManager::GetKey(MouseR))
			{
				InputManager::SetKeyState(MouseR, InputManager::GetKeyState(MouseR) | KeyDownBit);
				InputManager::SetKeyDownTime(MouseR, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			if (InputManager::GetKeyDown(MouseR) || InputManager::GetKey(MouseR))
			{
				InputManager::SetKeyState(MouseR, InputManager::GetKeyState(MouseR) | KeyUpBit);
				InputManager::SetKeyUpTime(MouseR, std::chrono::steady_clock::now());
			}
			break;
		}
		case WM_PAINT:
		{
			HDC mainHDC = BeginPaint(GameManager::mainHWnd, &ps);
			//GameRender();

			EndPaint(GameManager::mainHWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			ConsoleDebug::console->Close();
			ConsoleDebug::console->consoleActive = false;

			//UnregisterClass(className, hInstMain);
			GameManager::GameDestroy();
			GameManager::gameDestroy = true;
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hWnd, msgUD, w, l);
			break;
	}
	return 0;
}
