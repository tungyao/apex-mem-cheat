#include "d3dguix.h"

#include <mutex>
#include <random>

bool InitD3D() {
    /*
    D3D这玩意比较复杂，如果单纯是想搞点辅助什么的，复制粘贴我的足够了，
    如果想深入学习，可能得另找资料了，下面的这些基本是固定的，想知道是用来干啥的
    可以自行百度，我这个人比较懒。。。
    */

    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // 创建D3D设备
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WindowHandler, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                             &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    if (pLine == NULL)
        D3DXCreateLine(g_pd3dDevice, &pLine);

    //创建D3D字体
    D3DXCreateFontW(g_pd3dDevice, 16, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, FF_DONTCARE, L"Vernada", &Font);

    return true;
}

void CreateTransparentWindows(HWND GameWindowHandel, Draw DrawFunc) {
    cout << GameWindowHandel << endl;

    if (DrawFunc == NULL || GameWindowHandel == 0) return;

    GameHwnd = GameWindowHandel;
    Render = DrawFunc;

    //初始化窗口类
    wClass.cbClsExtra = NULL;
    wClass.cbSize = sizeof(WNDCLASSEX);
    wClass.cbWndExtra = NULL;
    wClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(0, 0, 0));
    wClass.hCursor = LoadCursor(0, IDC_ARROW);
    wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
    wClass.hInstance = GetModuleHandle(NULL);
    wClass.lpfnWndProc = (WNDPROC) WinProc;
    wClass.lpszClassName = LPCSTR(L" ");
    wClass.lpszMenuName = LPCSTR(L" ");
    wClass.style = CS_VREDRAW | CS_HREDRAW;

    //Registration window
    if (RegisterClassEx(&wClass) == 0) {
        MessageBox(NULL, LPCSTR(L"Error creating window"), LPCSTR(L"hit"), 0);
        exit(1);
    }

    //Creating a window
    GetWindowRect(GameHwnd, &WindowsRect);
    Width = WindowsRect.right - WindowsRect.left;
    Height = WindowsRect.bottom - WindowsRect.top;
    WindowHandler = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, LPCSTR(L" "), LPCSTR(L" "),
                                   WS_POPUP, 1, 1, Width, Height, 0, 0, 0, 0);

    //display window
    SetLayeredWindowAttributes(WindowHandler, 0, RGB(0, 0, 0), LWA_COLORKEY);
    ShowWindow(WindowHandler, SW_SHOW);
    UpdateWindow(WindowHandler);
    SetTimer(WindowHandler, 1, 1000, NULL);

    InitD3D();
}

void MsgLoop() {
    while (1) {
        //Make the auxiliary window always cover the game window
        if (GameHwnd) {
            GetWindowRect(GameHwnd, &WindowsRect);
            Width = WindowsRect.right - WindowsRect.left;
            Height = WindowsRect.bottom - WindowsRect.top;
            DWORD dwStyle = GetWindowLong(GameHwnd, GWL_STYLE);
            if (dwStyle & WS_BORDER) {
                WindowsRect.top += 23;
                Height -= 23;
            }
            MoveWindow(WindowHandler, WindowsRect.left, WindowsRect.top, Width, Height, true);
        }

        //Handling window messages
        MSG Message;
        ZeroMemory(&Message, sizeof(Message));
        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&Message);
            TranslateMessage(&Message);
        }

        Sleep(1);
    }


    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
    CloseWindow(WindowHandler);

    ::UnregisterClass(wClass.lpszClassName, wClass.hInstance);
}

LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
        case WM_PAINT:
            if (g_pd3dDevice) Render(); //That's where we call our frame drawing and stuff like that
            break;

        case WM_CREATE:
            DwmExtendFrameIntoClientArea(hWnd, &Margin);
            break;

        case WM_DESTROY: {
            g_pD3D->Release();
            g_pd3dDevice->Release();
            exit(1);
            return 0;
        }
        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
            break;
    }
    return 0;
}

void Line(D3DCOLOR Color, float X1, float Y1, float X2, float Y2, float Width) {
    D3DXVECTOR2 Vertex[2] = {{X1, Y1}, {X2, Y2}};
    pLine->SetWidth(Width);
    pLine->Draw(Vertex, 2, Color);
}

void Text(float X, float Y, const char *Str, D3DCOLOR Color) {
    RECT Rect = {(LONG) X, (LONG) Y};
    Font->DrawTextA(NULL, Str, -1, &Rect, DT_CALCRECT, Color);
    Font->DrawTextA(NULL, Str, -1, &Rect, DT_LEFT, Color);
}

void Box(float X, float Y, float W, float H, float Width, D3DCOLOR Color) {
    D3DXVECTOR2 Vertex[5] = {{X, Y}, {X + W, Y}, {X + W, Y + H}, {X, Y + H}, {X, Y}};
    pLine->SetWidth(Width);
    pLine->Draw(Vertex, 5, Color);
}

void DrawStart() {
    g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pd3dDevice->BeginScene();
}

void DrawEnd() {
    g_pd3dDevice->EndScene();
    g_pd3dDevice->Present(0, 0, 0, 0);
}

int hickness = 2;
D3DCOLOR red = D3DCOLOR_XRGB(255, 0, 0);
D3DCOLOR green = D3DCOLOR_XRGB(0, 255, 0);
D3DCOLOR bule = D3DCOLOR_XRGB(0, 0, 255);

int x = 0;
int revers = 0;

void DrawFunc() {
    if (revers) {
        x++;
    } else {
        x--;
    }
    if (x >= Width) {
        revers = 0;
    }
    if (x <= 0) {
        revers = 1;
    }
    DrawStart();
    Line(green, 20, 20, 66, 66, hickness);
    Box(x, 100, 50, 100, hickness, red);
    Text(200, 200, "777", bule);
    DrawEnd();
}

HWND GameWindow = (HWND) 0x50A00;
std::mutex mtx;

int generateRandomNumbers(int lowerBound, int upperBound) {
    // 创建随机数生成器
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(lowerBound, upperBound);

    // 使用锁保护输出
    lock_guard<std::mutex> lock(mtx);
    int randomNum = distribution(generator);
    std::cout << "Generated number: " << randomNum << std::endl;
    // 可以在这里添加延时来模拟计算过程
    return randomNum;
}

void DrawSomthing() {
}

void Start() {
    GameWindow = FindWindow(LPCSTR("YodaoMainWndClass"), nullptr);
    CreateTransparentWindows(GameWindow, DrawFunc);
    MsgLoop();
}
