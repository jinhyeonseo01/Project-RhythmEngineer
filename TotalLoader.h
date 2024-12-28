#pragma once
#include "ConsoleDebug.h"
#include "GameLogic.h"
#include "Project.h"
#include <d2d1.h>
//#include <d2d1_1.h>
//#include <d2d1effects.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "fmod_errors.h"

#include "lib/nlohmann/json.hpp"
#include "lib/nlohmann/json_fwd.hpp"

#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1.lib")
//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
    // 디버그 빌드 시
#pragma comment(lib, "fmodL_vc.lib")
#pragma comment(lib, "fmodstudioL_vc.lib")
#else
    // 릴리스 빌드 시
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#endif
