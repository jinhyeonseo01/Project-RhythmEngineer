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

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "TotalLoader.h"

void ProjectI::Init()
{
	Resources::ImageLoading(0, L".\\Resources\\Image\\Back.png");
	Resources::ImageLoading(1, L".\\Resources\\Image\\Back4.png");
}