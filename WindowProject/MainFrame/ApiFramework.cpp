// WindowProject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ApiFramework.h"
#include "ScreenCaptureFramework.h"
#include "SoundDeviceManager.h"
#include "time.h"
#include "Utility.h"
#include "Resource.h"

#include "D2DFramework.h"

#define MAX_LOADSTRING 100

// Global Variables:                           // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void				Release();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void GetMsgRoutine(const HACCEL& hAccelTable);
void PeekMsgRoutine(const HACCEL& hAccelTable);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWPROJECT));

	MsgRoutine = PeekMsgRoutine;
	MsgRoutine = GetMsgRoutine;

    // Main message loop:
    while (g_bLoop)
    {
		MsgRoutine(hAccelTable);
    }

	Release();

	return NULL;
}

void GetMsgRoutine(const HACCEL& hAccelTable) {
	static MSG msg;
	GetMessage(&msg, nullptr, 0, 0);
	DispatchMessage(&msg);
}

void PeekMsgRoutine(const HACCEL& hAccelTable) {
	static MSG msg;
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		DispatchMessage(&msg);
	}
	else {
		Sleep(FRAME_PER_MICRO_SECOND);
		ScreenCaptureFramework->Update();
		ScreenCaptureFramework->Render();
	}
}

void SetOption(bool bPeek, bool bWindowShow) {
	if (bPeek) {
		MsgRoutine = PeekMsgRoutine;
	//	ScreenCaptureFramework->Render();
	}
	else {
		MsgRoutine = GetMsgRoutine;
	}
	ShowWindow(g_hwnd, bWindowShow);
}


//  FUNCTION: MyRegisterClass()
//  PURPOSE: Registers the window class.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(WNDCLASSEX));
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground	= (HBRUSH)CreateSolidBrush(RGB(0, 0, 0, 1));
	wcx.hInstance		= hInstance;
	wcx.lpfnWndProc		= WndProc;
	wcx.lpszClassName	= L"MainWindow";
//	wcx.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wcx.hCursor			= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wcx.style			= CS_HREDRAW | CS_VREDRAW;
	return RegisterClassEx(&wcx);
}

struct ENUM_DISP_ARG
{
	TCHAR msg[500];
	int monId;
};
BOOL CALLBACK EnumDispProc(HMONITOR hMon, HDC dcMon, RECT* pRcMon, LPARAM lParam)
{
	ENUM_DISP_ARG* pArg = reinterpret_cast<ENUM_DISP_ARG*>(lParam);

	if (FRAME_BUFFER_WIDTH < pRcMon->right) {
		FRAME_BUFFER_WIDTH = pRcMon->right;
	}
	if (FRAME_BUFFER_HEIGHT < pRcMon->bottom) {
		FRAME_BUFFER_HEIGHT = pRcMon->bottom;
	}

	DISPLAY_DEVICE dd;
	DEVMODE dm;
	memset(&dd, 0, sizeof DISPLAY_DEVICE);
	memset(&dm, 0, sizeof(DEVMODE));
	dd.cb = sizeof(DISPLAY_DEVICE);
	EnumDisplayDevices(NULL, pArg->monId-1, &dd, 0);
	EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
	SYSTEM_RESOL_X += dm.dmPelsWidth;
	if (SYSTEM_RESOL_Y < dm.dmPelsHeight) {
		SYSTEM_RESOL_Y = dm.dmPelsHeight;
	}
	pArg->monId++;
	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // Store instance handle in our global variable
	ENUM_DISP_ARG arg = { 0 };
	arg.monId = 1;
	EnumDisplayMonitors(0, 0, EnumDispProc, reinterpret_cast<LPARAM>(&arg));
	RECT rect = { FRAME_BUFFER_POS_X, FRAME_BUFFER_POS_Y, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
 
	int nMonitor = GetSystemMetrics(SM_CMONITORS);
	DISPLAY_DEVICE dd;
	DEVMODE dm;
	for (int i = 0; i<nMonitor ; ++i) {
		memset(&dd, 0, sizeof DISPLAY_DEVICE);
		memset(&dm, 0, sizeof(DEVMODE));
		dd.cb = sizeof(DISPLAY_DEVICE);
		EnumDisplayDevices(NULL, i, &dd, 0);
		EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
	}

//	DWORD dwStyle = WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	DWORD dwStyle = WS_POPUP;
	DWORD dwExStyle = WS_EX_LAYERED|WS_EX_TOPMOST;
//	DWORD dwExStyle = WS_EX_LAYERED;
//	DWORD dwExStyle = NULL;
	g_hwnd = CreateWindowEx(
		dwExStyle,
		L"MainWindow",
		L"Power_Capture",
		dwStyle,
		rect.left, rect.top, 
		rect.right - rect.left, rect.bottom - rect.top, 
		NULL, NULL, hInstance, NULL
	);
	SetLayeredWindowAttributes(g_hwnd, RGB(255,0,255), 240, ULW_COLORKEY|LWA_ALPHA);

	if (!g_hwnd) return -1;
	ScreenCaptureFramework->CreateInstance(g_hwnd);

	//Create Instance & Initializing
	D2DFramework;

	//Create Instance & Initializing
	SoundDeviceManager;

	g_hIcon = CreateSmallIcon(g_hwnd);
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = g_hwnd;
	nid.uID = 100;
	nid.hIcon = g_hIcon;
	nid.uCallbackMessage = WM_TASKBAR;
	wsprintf(nid.szTip,L"Power Capture\nby Gyung Bae Cho");
	nid.hBalloonIcon;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD,&nid);

	ShowWindow(g_hwnd, true);
	ShowWindow(g_hwnd, false);
	return TRUE;
}

void Release() {
	ScreenCaptureFramework->Release();

	//Release Resources
	NOTIFYICONDATA nid;						//System Tray 제거
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = g_hwnd;
	nid.uID = 100;
	Shell_NotifyIcon(NIM_DELETE,&nid);
}

bool HotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case HK_QUIT:
		g_bLoop = false;
		return true;
	}
	return false;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	//	SetTimer(hWnd, 1, FRAME_PER_MICRO_SECOND, NULL);
		break;
    case WM_COMMAND:
	{
		ScreenCaptureFramework->MenuSelect(hWnd, message, wParam, lParam);
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_TIMER:
	//	KillTimer(hWnd, wParam);
		break;
	case WM_PAINT:
		cout << "PAINT" << endl;
		ValidateRect(hWnd, NULL);
		break;
	case WM_HOTKEY:
		if (ScreenCaptureFramework->HotKey(hWnd, message, wParam, lParam));		//Screen Capture Framework Hotkey
		else if (SoundDeviceManager->HotKey(hWnd, message, wParam, lParam));	//Sound Device Manager Hotkey
		else HotKey(hWnd, message, wParam, lParam);								//Api Main Framework Hotkey	
		break;
	case WM_TASKBAR:
		ScreenCaptureFramework->TaskBar(hWnd, message, wParam, lParam);
		break;
	case WM_DEVICECHANGE:
		SoundDeviceManager->PrepareDeviceList();
		break;
	case WM_MOUSEMOVE:case WM_LBUTTONDOWN:case WM_LBUTTONUP:case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:case WM_RBUTTONDBLCLK:case WM_MBUTTONDOWN:case WM_MBUTTONUP:         
	case WM_MOUSEWHEEL:
		ScreenCaptureFramework->MouseInput(hWnd, message,wParam, lParam);
		//if( (SHORT)HIWORD(wParam) > 0 )
		//	cout << HIWORD(wParam) << endl;
		////	cout << "마우스 휠 위로" << endl;
		//else
		//	cout << HIWORD(wParam) << endl;
		////	cout << "마우스 휠 아래로" << endl;
		break;
	case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP:
		ScreenCaptureFramework->KeyInput(hWnd, message, wParam, lParam);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
