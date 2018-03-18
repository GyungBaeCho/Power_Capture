#pragma once

//Returns Date/Time into wstring
std::wstring GetTimeW(time_t date);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

HICON CreateSmallIcon(HWND hWnd);
