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

void GameInit();
void GameUpdate(HWND wnd);

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

	wnd = CreateWindow(className, titleName,
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



void GameInit()
{
	debug(L"- Console Test Start -\n");
	GameManager::mainWorld = std::make_shared<World>();

	std::shared_ptr<GameObject> mainCameraObject = GameManager::mainWorld->CreateGameObject();
	GameManager::mainCamera = mainCameraObject->AddComponent<Camera>(std::make_shared<Camera>());

	GameManager::updateNowClock = std::chrono::steady_clock::now();
	GameManager::updatePrevClock = GameManager::updateNowClock;
}

float deltatime = 0;
float totalTime = 0;
float totalPerTime = 0;

static float angle = 30;

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

		InvalidateRect(wnd, NULL, FALSE);
		totalPerTime = totalTime;
		totalTime -= GameManager::targetFrameBetween;
		angle += 1;
	}
}

CImage img;
CImage img2;

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
			img.Load(L".\\Resources\\Image\\Back.png");
			img2.Load(L".\\Resources\\Image\\Back2.jpg");
			break;
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
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (LOWORD(l) > (GameManager::viewSize.right - 40) && HIWORD(l) < 40) // 게임종료
				DestroyWindow(hWnd);
			
			break;
		}
		case WM_PAINT:
		{
			hDC2 = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &GameManager::viewSize);

			//std::shared_ptr<GameObject> obj = GameManager::mainWorld->CreateGameObject();
			//auto sprite = obj->AddComponent<SpriteRenderer>(std::make_shared<SpriteRenderer>());
			auto BitmapWidth = GameManager::viewSize.right;
			auto BitmapHeight = GameManager::viewSize.bottom;
			auto hDC = CreateCompatibleDC(hDC2);
			auto hBitmapBuffer = CreateCompatibleBitmap(hDC2, BitmapWidth, BitmapHeight);
			//hMemoryDC = CreateCompatibleDC(hDC2);

			SelectObject(hDC, hBitmapBuffer);
			img2.Draw(hDC, 0, 0, img2.GetWidth(), img2.GetHeight());
			
			float x = 300;
			float y = 300;

			float imageW = img.GetWidth();
			float imageH = img.GetHeight();
			int flipx = 1;
			int flipy = 1;
			float sizex = 100;
			float sizey = 100;
			float pivotx = 0.5f;
			float pivoty = 0.5f;
			float imageOffsetx = 0.34;
			float imageOffsety = 0.34;
			float imageScalex = 0.36;
			float imageScaley = 0.36;
			BOOL bResult;

			img2.Draw(hDC, 285, 285, 30, 30);
			//img2.m_r

			HDC m_hDC = img.GetDC();

			float stretchW = sizex;
			float stretchH = sizey;
			HDC m_hDC2 = CreateCompatibleDC(m_hDC);
			HBITMAP bmp = CreateCompatibleBitmap(m_hDC, stretchW, stretchH);
			SelectObject(m_hDC2, bmp);
			float filterX = imageW * imageOffsetx;
			float filterY = imageH * imageOffsety;
			float filterW = imageW * imageScalex;
			float filterH = imageH * imageScaley;
			StretchBlt(m_hDC2, 0, 0, stretchW, stretchH, m_hDC,
				filterX + (flipx == 1 ? 0 : filterW - 1),
				filterY + (flipy == 1 ? 0 : filterH - 1),
				(flipx == 1 ? 1 : -1) * filterW, (flipy == 1 ? 1 : -1) * filterH, SRCCOPY);


			float bmp3Size = sqrt(stretchW * stretchW + stretchH * stretchH);
			HDC m_hDC3 = CreateCompatibleDC(m_hDC);
			HBITMAP bmp3 = CreateCompatibleBitmap(m_hDC, bmp3Size * 2, bmp3Size * 2);
			SelectObject(m_hDC3, bmp3);

			Eigen::Matrix3d rotateM;
			rotateM <<
				cos(angle * D2R), -sin(angle * D2R), 0,
				sin(angle * D2R), cos(angle * D2R), 0,
				0, 0, 1;
			Eigen::Vector3d v1, v2, v3;
			v1 << stretchW, 0, 1;
			v2 << 0, stretchH, 1;
			v3 << -stretchW * pivotx, -stretchH * pivoty, 1;
			v1 = rotateM * v1;
			v2 = rotateM * v2;
			v3 = rotateM * v3;

			POINT a[3] = {
				{bmp3Size + v3.x(), bmp3Size + v3.y()},
				{bmp3Size + v3.x() + v1.x(), bmp3Size + v3.y() + v1.y()},
				{bmp3Size + v3.x() + v2.x(), bmp3Size + v3.y() + v2.y()},
			};
			::PlgBlt(m_hDC3, a, m_hDC2,
				0, 0,
				stretchW, stretchH,
				NULL, NULL, NULL);

			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xff;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bResult = ::AlphaBlend(hDC,
				x - bmp3Size, y - bmp3Size,
				bmp3Size * 2, bmp3Size * 2,
				m_hDC3, 0, 0,
				bmp3Size * 2, bmp3Size * 2, bf);

			DeleteObject(m_hDC2);
			DeleteObject(bmp);

			DeleteObject(m_hDC3);
			DeleteObject(bmp3);

			img.ReleaseDC();
			
			TCHAR tempConsole[100];
			_stprintf_s(tempConsole, L"%f", 1);
			TextOut(hDC, 700, 0, tempConsole, lstrlenW(tempConsole));

			BitBlt(hDC2, 0, 0, BitmapWidth, BitmapHeight, hDC, 0, 0, SRCCOPY);
			DeleteDC(hDC);
			DeleteObject(hBitmapBuffer);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			//ConsoleDebug::console->Close();
			//ConsoleDebug::console->consoleActive = false;

			//UnregisterClass(className, hInstMain);
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hWnd, msgUD, w, l);
			break;
	}
	return 0;
}
