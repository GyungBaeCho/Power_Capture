// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

//Direct2D
#include <d2d1_2.h>
#include <dwrite.h>
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

//Process Memory Info
#include <psapi.h>

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <vector>
#include <unordered_map>
#include <map>

#include <iostream>
#include <fstream>
using std::cout;
using std::endl;

//Windows Image Formats
#include <GdiPlus.h>
using namespace Gdiplus;    
#pragma comment(lib, "gdiplus.lib") 

//IWICImagingFactory
#include <Wincodec.h>

//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")	

//CImage
#include <atlimage.h>

//폴더 탐색기 지원
#include <Shlobj.h>

#include <shellapi.h>

#define WM_TASKBAR	(WM_USER + 1)	

#define VK_0									0X30
#define VK_1									0X31
#define VK_2									0X32
#define VK_3									0X33
#define VK_4									0X34
#define VK_5									0X35
#define VK_6									0X36
#define VK_7									0X37
#define VK_8									0X38
#define VK_9									0X39

#define VK2_0									0X60
#define VK2_1									0X61
#define VK2_2									0X62
#define VK2_3									0X63
#define VK2_4									0X64
#define VK2_5									0X65
#define VK2_6									0X66
#define VK2_7									0X67
#define VK2_8									0X68
#define VK2_9									0X69

#define VK_A									0x41
#define VK_B									0x42
#define VK_C									0x43
#define VK_D									0x44
#define VK_E									0x45
#define VK_F									0x46
#define VK_G									0x47
#define VK_H									0x48
#define VK_I									0x49
#define VK_J									0x4A
#define VK_K									0x4B
#define VK_L									0x4C
#define VK_M									0x4D
#define VK_N									0x4E
#define VK_O									0x4F
#define VK_P									0x50
#define VK_Q									0x51
#define VK_R									0x52
#define VK_S									0x53
#define VK_T									0x54
#define VK_U									0x55
#define VK_V									0x56
#define VK_W									0x57
#define VK_X									0x58
#define VK_Y									0x59
#define VK_Z									0x5A

// TODO: reference additional headers your program requires here

#define STR_SWITCH_BEGIN(key)								\
{															\
	static std::unordered_map< std::wstring, int > s_hm;	\
	static bool s_bInit = false;						\
	bool bLoop = true;									\
	while(bLoop)										\
	{													\
		int nKey = -1;									\
		if(s_bInit) { nKey=s_hm[key]; bLoop = false; }	\
		switch(nKey){									\
			case -1: {

#define CASE(token)							\
			}								\
		case __LINE__:						\
			if(!s_bInit)					\
				s_hm[token] = __LINE__;		\
			else							\
			{

#define DEFAULT()			\
			}				\
			case 0:			\
			default:		\
				if(s_bInit)	\
				{

#define STR_SWITCH_END()			\
			}						\
		}							\
		if(!s_bInit) s_bInit=true;	\
	}								\
}

struct VECTOR2 {
	float x;
	float y;

	VECTOR2() : x(0), y(0) {}
	VECTOR2(float x, float y) : x(x), y(y) {}

};

VECTOR2 operator+(const VECTOR2& vec1, const VECTOR2& vec2);
VECTOR2 operator-(const VECTOR2& vec1, const VECTOR2& vec2);
VECTOR2 operator*(const VECTOR2& vec1, const VECTOR2& vec2);
VECTOR2 operator/(const VECTOR2& vec1, const VECTOR2& vec2);
VECTOR2 operator*(const VECTOR2& vec1, const float& fData);
VECTOR2 operator/(const VECTOR2& vec1, const float& fData);
void operator+=(VECTOR2& vec1, const VECTOR2& vec2);
void operator-=(VECTOR2& vec1, const VECTOR2& vec2);
void operator*=(VECTOR2& vec1, const int& nData);
void operator*=(VECTOR2& vec1, const float& fData);

__declspec(selectany) int FRAME_BUFFER_POS_X	= 0;
__declspec(selectany) int FRAME_BUFFER_POS_Y	= 0;
__declspec(selectany) int FRAME_BUFFER_HEIGHT	= 0;
__declspec(selectany) int FRAME_BUFFER_WIDTH	= 0;
__declspec(selectany) int SYSTEM_RESOL_X		= 0;
__declspec(selectany) int SYSTEM_RESOL_Y		= 0;

__declspec(selectany) VECTOR2 gWindowSize = VECTOR2( FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT );
__declspec(selectany) bool g_bLoop = true;
__declspec(selectany) HINSTANCE g_hInst;     
__declspec(selectany) HMENU hMenu;
__declspec(selectany) HMENU hSubMenu;

const int FRAME_PER_MICRO_SECOND = 31;

//Directory
__declspec(selectany) std::vector<std::wstring> g_vwsSaveDirectory;
__declspec(selectany) int g_pvtSaveDirectory = 0;