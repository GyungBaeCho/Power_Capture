#include "stdafx.h"
#include "Utility.h"

std::wstring GetTimeW(time_t date)
{
	// time_t to tm
	struct tm temp = { 0 };
	localtime_s(&temp, &date);

	std::wstring stringDate;

	int year = temp.tm_year + 1900;
	int month = temp.tm_mon + 1;
	int day = temp.tm_mday;
	int hour = temp.tm_hour;
	int min = temp.tm_min;
	int sec = temp.tm_sec;

	stringDate += std::to_wstring(year);
	stringDate += L"-";

	if (std::to_wstring(month).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(month);
	stringDate += L"-";

	if (std::to_wstring(day).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(day);
	stringDate += L"[";

	if (std::to_wstring(hour).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(hour);
	stringDate += L".";

	if (std::to_wstring(min).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(min);
	stringDate += L".";

	if (std::to_wstring(sec).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(sec);
	stringDate += L"]";

	return stringDate;
}

int GetEncoderClsid(const WCHAR * format, CLSID * pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return 0;
}

HICON CreateSmallIcon(HWND hWnd)
{
	static TCHAR *szText = TEXT ("100");
	HDC hdc, hdcMem;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitMap = NULL;
	HBITMAP hBitmapMask = NULL;
	ICONINFO iconInfo;
	HFONT hFont;
	HICON hIcon;

	hdc = GetDC (hWnd);
	hdcMem = CreateCompatibleDC (hdc);
	hBitmap = CreateCompatibleBitmap (hdc, 16, 16);
	hBitmapMask = CreateCompatibleBitmap (hdc, 16, 16);
	ReleaseDC (hWnd, hdc);
	hOldBitMap = (HBITMAP)SelectObject (hdcMem, hBitmap);
	PatBlt (hdcMem, 0, 0, 16, 16, WHITENESS);

	// Draw percentage
	hFont = CreateFont (12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT ("Arial"));
	hFont = (HFONT)SelectObject (hdcMem, hFont);
	TextOut (hdcMem, 0, 0, szText, lstrlen (szText));

	SelectObject (hdc, hOldBitMap);
	hOldBitMap = NULL;

	iconInfo.fIcon = TRUE;
	iconInfo.xHotspot = 0;
	iconInfo.yHotspot = 0;
	iconInfo.hbmMask = hBitmapMask;
	iconInfo.hbmColor = hBitmap;

	hIcon = CreateIconIndirect (&iconInfo);

	DeleteObject (SelectObject (hdcMem, hFont));
	DeleteDC (hdcMem);
	DeleteDC (hdc);
	DeleteObject (hBitmap);
	DeleteObject (hBitmapMask);

	FILE* iconFile;
	fopen_s(&iconFile, "Resource/small.ico", "rb");
	fseek(iconFile, 0, SEEK_END);
	long fsize = ftell(iconFile);
	fseek(iconFile, 0, SEEK_SET);
	char* data = (char*)malloc(fsize + 1);
	fread(data, fsize, 1, iconFile);
	fclose(iconFile);

	static const int icon_size = 48;
	int offset = LookupIconIdFromDirectoryEx((PBYTE)data, TRUE, icon_size, icon_size, LR_DEFAULTCOLOR);
	if (offset != 0) {
		hIcon = CreateIconFromResourceEx((PBYTE)data + offset, 0, TRUE, 0x30000, icon_size, icon_size, LR_DEFAULTCOLOR);
		if (hIcon != NULL) {
			return hIcon;
		}
	}

	//	notifyIcon.hIcon  = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO));

	return hIcon;
}
