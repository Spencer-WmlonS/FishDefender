// 一个简单的c++程序，用于在屏幕右上角显示或隐藏一个红点，并切换颜色
// 需要使用Windows API和GDI+库
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// 全局变量
HWND hWnd; // 窗口句柄
bool visible = false; // 红点是否可见

Gdiplus::Color color = Color::Red; // 红点的颜色
int ddys = 1;

// 定义快捷键
#define HOTKEY_SHOW_HIDE 0x0001 // ctrl+shift+n
#define HOTKEY_CHANGE_COLOR 0x0002 // ctrl+shift+m

// 定义窗口类名和标题
#define WINDOW_CLASS_NAME L"RedDot"
#define WINDOW_TITLE L"RedDot"

// 定义窗口大小和位置
#define WINDOW_WIDTH 20
#define WINDOW_HEIGHT 20
#define WINDOW_X GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH
#define WINDOW_Y 0

// 定义红点的半径和中心
#define DOT_RADIUS 8
#define DOT_CENTERX WINDOW_WIDTH / 2
#define DOT_CENTERY WINDOW_HEIGHT / 2

// 窗口过程函数，处理窗口消息
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: // 窗口创建时注册快捷键
        if (!RegisterHotKey(hWnd, HOTKEY_SHOW_HIDE, MOD_CONTROL | MOD_SHIFT, 'N'))
        {
            MessageBox(NULL, L"注册快捷键失败！", L"错误", MB_OK | MB_ICONERROR);
            return -1;
        }
        if (!RegisterHotKey(hWnd, HOTKEY_CHANGE_COLOR, MOD_CONTROL | MOD_SHIFT, 'M'))
        {
            MessageBox(NULL, L"注册快捷键失败！", L"错误", MB_OK | MB_ICONERROR);
            return -1;
        }
        break;
    case WM_DESTROY: // 窗口销毁时注销快捷键和退出程序
        UnregisterHotKey(hWnd, HOTKEY_SHOW_HIDE);
        UnregisterHotKey(hWnd, HOTKEY_CHANGE_COLOR);
        PostQuitMessage(0);
        break;
    case WM_HOTKEY: // 处理快捷键消息
        switch (wParam)
        {
        case HOTKEY_SHOW_HIDE: // 显示或隐藏红点
            visible = !visible;
            InvalidateRect(hWnd, NULL, TRUE); // 重绘窗口
            break;
        case HOTKEY_CHANGE_COLOR: // 切换红点颜色

            if (ddys == 1) {
                color = Color::Green;
                ddys = 2;
            }

            else if (ddys == 2) {
                color = Color::Blue;
                ddys = 3;
            }

            else if (ddys == 3) {
                color = Color::Red;
                ddys = 1;
            }

            InvalidateRect(hWnd, NULL, TRUE); // 重绘窗口
            break;
        }
        break;
    case WM_PAINT: // 绘制窗口内容
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps); // 获取设备上下文
        Graphics graphics(hdc); // 创建GDI+图形对象
        graphics.SetSmoothingMode(SmoothingModeAntiAlias); // 设置抗锯齿模式
        if (visible) // 如果红点可见，绘制一个实心圆
        {
            SolidBrush brush(color); // 创建实心画刷，颜色为color
            graphics.FillEllipse(&brush, DOT_CENTERX - DOT_RADIUS, DOT_CENTERY - DOT_RADIUS, DOT_RADIUS * 2, DOT_RADIUS * 2); // 填充椭圆，即圆形
        }
        EndPaint(hWnd, &ps); // 结束绘制
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam); // 默认处理其他消息
    }
    return 0;
}

// 主函数，创建并显示窗口，// 进入消息循环
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 初始化GDI+库
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 注册窗口类
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, L"注册窗口类失败！", L"错误", MB_OK | MB_ICONERROR);
        return -1;
    }

    // 创建窗口
    hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_POPUP,
        WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, L"创建窗口失败！", L"错误", MB_OK | MB_ICONERROR);
        return -1;
    }

    // 设置窗口为置顶
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理GDI+库
    GdiplusShutdown(gdiplusToken);

    // 返回消息参数
    return (int)msg.wParam;
}