#pragma once

#include "resource.h"

__declspec(selectany) HICON g_hIcon;
__declspec(selectany) HWND g_hwnd;
__declspec(selectany) bool g_bCaptured = false;
__declspec(selectany) void (*MsgRoutine)(const HACCEL& hAccelTable);

//Hot Key
enum{
	//Screen Capture Framework
	HK_SCREENSHOT,
	HK_OPEN_DIRECTORY,
	HK_LEFT_DIRECTORY,
	HK_RIGHT_DIRECTORY,

	//Sound Device Manager
	HK_LEFT_SOUND_DEVICE,
	HK_RIGHT_SOUND_DEVICE,

	HK_QUIT,
};

//Task Bar Menu
enum{
	TB_EXIT,
	TB_INFO,
	TB_CAPTURE,
	TB_SET_DIRECTORY,
	TB_SET_LINE_DRAW,
	TB_SET_LINE_COLOR,
	TB_OPEN_FOLDER,
	TB_DEFAULT,
};

void SetOption(bool bPeek, bool bWindowShow);