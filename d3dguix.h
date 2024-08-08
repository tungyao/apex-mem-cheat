#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <iostream>
#include <mutex>
#include <vector>
#include<Windows.h>

using namespace std;

static MARGINS Margin;
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static ID3DXLine *pLine = 0;
static ID3DXFont *Font;
// WindowHandler
static HWND WindowHandler, GameHwnd;
static RECT WindowsRect; // WindowsRect
static int Width, Height;

//注册窗口需要用到的窗口类
static WNDCLASSEX wClass;


//画矩形，文字之类的单独放在这个函数里
typedef void (*Draw)();

static Draw Render;


//窗口消息处理函数
LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

bool InitD3D();

void CreateTransparentWindows(HWND GameWindowHandel, Draw DrawFunc);

void MsgLoop();

void Line(D3DCOLOR Color, float X1, float Y1, float X2, float Y2, float Width);

void Text(float X, float Y, const char *Str, D3DCOLOR Color);

void Box(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

void DrawStart();

void DrawEnd();

void Start();

struct Box_T {
    int data[20][4];
};

inline vector<Box_T> vec;
void pushBox(Box_T box);
