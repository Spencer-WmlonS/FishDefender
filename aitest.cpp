#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
// ȫ�ֱ���
HWND hWnd; // ���ھ��
bool visible = false; // ����Ƿ�ɼ�
//typedef BOOL(*UPDATELAYEREDWINDOWFUNCTION)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
Gdiplus::Color color = Color::Red; // ������ɫ
int ddys = 1; // �����ݼ�
#define HOTKEY_SHOW_HIDE 0x0001 // ctrl+shift+n
#define HOTKEY_CHANGE_COLOR 0x0002 // ctrl+shift+m
// ���崰�������ͱ���
#define WINDOW_CLASS_NAME L"RedDot"
#define WINDOW_TITLE L"RedDot"
// ���崰�ڴ�С��λ��
#define WINDOW_WIDTH 20
#define WINDOW_HEIGHT 20
#define WINDOW_X GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH
#define WINDOW_Y 0
// ������İ뾶������
#define DOT_RADIUS 8
#define DOT_CENTERX WINDOW_WIDTH / 2
#define DOT_CENTERY WINDOW_HEIGHT / 2

BOOL OnForceShow(HWND hWnd) {
    HWND hForeWnd = NULL;
    DWORD dwForeID = 0;
    DWORD dwCurID = 0;
    hForeWnd = ::GetForegroundWindow();
    dwCurID = ::GetCurrentThreadId();
    dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
    ::AttachThreadInput(dwCurID, dwForeID, TRUE);
    ::ShowWindow(hWnd, SW_SHOWNORMAL);
    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ::SetForegroundWindow(hWnd); // ��ǰ̨�����߳���������ǰ�̣߳�Ҳ���ǳ���A�еĵ����̣߳�
    ::AttachThreadInput(dwCurID, dwForeID, FALSE);
    return TRUE;
}

// ���ڹ��̺���������������Ϣ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// ����������������ʾ���ڣ�// ������Ϣѭ��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //UPDATELAYEREDWINDOWFUNCTION UpdateLayeredWindow;

    // ��ʼ��GDI+��
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // ע�ᴰ����
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
    /*HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
    wcex.hbrBackground = hBrush;*/

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, L"ע�ᴰ����ʧ�ܣ�", L"����", MB_OK | MB_ICONERROR);
        return -1;
    }

    // ��������
    /*hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_POPUP,
        WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);*/

    hWnd = CreateWindowEx(/*WS_EX_LAYERED |*/ WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WINDOW_CLASS_NAME, WINDOW_TITLE, WS_POPUP,
        WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, L"��������ʧ�ܣ�", L"����", MB_OK | MB_ICONERROR);
        return -1;
    }

    // ���ô���Ϊ�ö�
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    /*HWND hWnd = ::GetForegroundWindow();
    if (OnForceShow(hWnd))
        return TRUE;

    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);*/

    // ��ʾ����
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ����GDI+��
    GdiplusShutdown(gdiplusToken);

    // ������Ϣ����
    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: // ���ڴ���ʱע���ݼ�
        if (!RegisterHotKey(hWnd, HOTKEY_SHOW_HIDE, MOD_CONTROL | MOD_SHIFT, 'N')) {
            MessageBox(NULL, L"ע���ݼ�ʧ�ܣ�", L"����", MB_OK | MB_ICONERROR);
            return -1;
        }
        if (!RegisterHotKey(hWnd, HOTKEY_CHANGE_COLOR, MOD_CONTROL | MOD_SHIFT, 'M')) {
            MessageBox(NULL, L"ע���ݼ�ʧ�ܣ�", L"����", MB_OK | MB_ICONERROR);
            return -1;
        }
        break;
    case WM_DESTROY: // ��������ʱע����ݼ����˳�����
        UnregisterHotKey(hWnd, HOTKEY_SHOW_HIDE);
        UnregisterHotKey(hWnd, HOTKEY_CHANGE_COLOR);
        PostQuitMessage(0);
        break;
    case WM_HOTKEY: // ������ݼ���Ϣ
        switch (wParam)
        {
        case HOTKEY_SHOW_HIDE: // ��ʾ�����غ��
            visible = !visible;
            InvalidateRect(hWnd, NULL, TRUE); // �ػ洰��
            break;
        case HOTKEY_CHANGE_COLOR: // �л������ɫ

            if (ddys == 1) {
                color = Color::ForestGreen;
                ddys = 2;
            }

            else if (ddys == 2) {
                color = Color::Yellow;
                ddys = 3;
            }

            else if (ddys == 3) {
                color = Color::OrangeRed;
                ddys = 1;
            }

            InvalidateRect(hWnd, NULL, TRUE); // �ػ洰��
            break;
        }
        break;
    case WM_ERASEBKGND: // ��������������TRUE��ʾ�Ѿ���������ֹ��˸
        return TRUE;
    case WM_PAINT: // ���ƴ�������
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps); // ��ȡ�豸������
        Graphics graphics(hdc); // ����GDI+ͼ�ζ���
        graphics.SetSmoothingMode(SmoothingModeAntiAlias); // ���ÿ����ģʽ
        if (visible) // ������ɼ�������һ��ʵ��Բ
        {
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
            SolidBrush brush(color); // ����ʵ�Ļ�ˢ����ɫΪcolor
            graphics.FillEllipse(&brush, DOT_CENTERX - DOT_RADIUS, DOT_CENTERY - DOT_RADIUS, DOT_RADIUS * 2, DOT_RADIUS * 2); // �����Բ����Բ��
            UpdateLayeredWindow(hWnd, NULL, NULL, NULL, hdc, NULL, RGB(255, 255, 255), NULL, ULW_COLORKEY); // ʹ��UpdateLayeredWindow����ʵ��͸������
        }
        else {
            SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
        }
        EndPaint(hWnd, &ps); // ��������
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam); // Ĭ�ϴ���������Ϣ
    }
    return 0;
}