# Project : Iaido
2023 University(한국공학대학교) Win32 Project (2.1 Grade)
2D Pipeline Process Develop Project

* * *
## Workers
 - Jin Hyeon Seo (Clrain) : Main Develop & Art
 - Jung Jin Bong : Design & Sub Develop
  
* * *
## Install & Execution 방법
Eigen 라이브러리 연결 (두 방법중 하나 선택)
1. 프로젝트 속성 -> VC++ 디렉토리 -> 포함 디렉토리 -> ./lib/egien 폴더 선택 후 추가
2. 프로젝트 탭 -> NuGen 패키지 관리 -> 찾아보기 "Eigen3" -> 죄신버전 설치
   
* * *
# Develop Log
## Day 1(23.5.15) Log
### Logic Pipeline & Render Pipeline & Debug System Develop

![Alt text](https://user-images.githubusercontent.com/45618159/238410773-956ea9fa-16f8-4216-9e79-1993f3dae090.png)

>1. Debug 창과 MicroSecond 단위로 측정되는 deltaTime log.
>2. Update PipeLine이 개발된 상태.
>3. Camera와 공간상 좌표계를 표현하고 활용하기 위해
>   벡터와 행렬의 연산을 도와줄 Eigen 라이브러리를 기용.
* * *
## Day 2
### Sprite Rendering pipeline 계산 절차 설계
![Alt text](https://user-images.githubusercontent.com/45618159/238851939-b670c12c-aa06-4527-8d6b-01236a237708.png)

3단계 프로세스에 걸쳐서 Rendering 과정을 수립.
논리적 사고 흐름을 정리.
>1. 원시데이터를 스케일링, 컷팅, 플립을 계산하여 StretchBlt 수행
>>![Alt text](https://user-images.githubusercontent.com/45618159/238854895-c18e07d8-379c-48be-84c6-fd322dd96e02.gif)
>2. 행렬 매트릭스로 3개의 벡터를 곱.
>    [ cos(angle), -sin(angle)]
>    [ sin(angle),  cos(angle)]
> v1 : (-stretch * pivot) 뒤로 당겨주는 벡터.
> v2 : (stretchW, 0)
> v3 : (0, stretchH)
>> - R*V로 구해진 회전벡터에 |stretch| stretch의 대각 길이를 더해준다.
>> - |stretch| stretch의 대각 길이 * 2의 BitMap을 준비하는 것으로 pivot 0~1사이에서 자유롭게 회전했을때 모두 비트맵에 담을 수 있도록 크기정해준다.
PigBit 수행
>>![Alt text](https://user-images.githubusercontent.com/45618159/238852550-706ff44a-358f-4929-a894-c2cd0935447a.gif)
>>![Alt text](https://user-images.githubusercontent.com/45618159/238852335-7ec4da00-d8a4-4827-9925-fe5e1b888b34.gif)
>3. 마지막으로 Alpha값이 적용될 수 있도록 Position - |stretch|위치에 렌더링한다.
> AlphaBlend 수행
>![Alt text](https://user-images.githubusercontent.com/45618159/238857866-2cf6a137-52e5-4201-894c-751719640683.gif)
<details>
<summary>렌더링 소스코드 (접기/펼치기)</summary>

```cpp
HDC m_hDC = img.GetDC();

float stretchW = sizex;
float stretchH = sizey;
// -- 1단계 --
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
	(flipx == 1 ? 1 : -1)* filterW, (flipy == 1 ? 1 : -1)* filterH, SRCCOPY);

// -- 2단계 --
float bmp3Size = sqrt(stretchW * stretchW + stretchH * stretchH);
HDC m_hDC3 = CreateCompatibleDC(m_hDC);
HBITMAP bmp3 = CreateCompatibleBitmap(m_hDC, bmp3Size * 2, bmp3Size * 2);
SelectObject(m_hDC3, bmp3);

Eigen::Matrix3d rotateM;
rotateM <<
cos(angle * D2R), -sin(angle * D2R), 0,
sin(angle* D2R), cos(angle* D2R), 0,
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

// -- 3단계 --
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
```
</details>