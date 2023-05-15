# Project : Iaido
University Win32 Project(2.1 Grade)
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
####Logic Pipeline & Render Pipeline & Debug System Develop

![Alt text](https://user-images.githubusercontent.com/45618159/238410773-956ea9fa-16f8-4216-9e79-1993f3dae090.png)
>1. Debug 창과 MicroSecond 단위로 측정되는 deltaTime log.
>2. Update PipeLine이 개발된 상태.
>3. Camera와 공간상 좌표계를 표현하고 활용하기 위해
>   벡터와 행렬의 연산을 도와줄 Eigen 라이브러리를 기용.
