#include "stdafx.h"
#include "ScreenCaptureFramework.h"
#include "ApiFramework.h"
#include "Utility.h"
#include "D2DFramework.h"
#include "Resource.h"

void CScreenCaptureFramework::CreateInstance(HWND hWnd) {
	if (!m_pInstance) {
		m_pInstance = new CScreenCaptureFramework;
		m_pInstance->Initialize(hWnd);
	}
}

CScreenCaptureFramework * CScreenCaptureFramework::GetInstance() {
	return m_pInstance;
}

void CScreenCaptureFramework::Initialize(HWND hWnd) {
	m_hWnd = hWnd;

	LoadOption();

	hMenu = CreatePopupMenu();
	hSubMenu = CreatePopupMenu();
	int index = 0;
	InsertMenu(hMenu,index++,MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)hSubMenu, L"Option");
	InsertMenu(hMenu,index++,MF_BYPOSITION | MF_STRING,TB_OPEN_FOLDER,L"Open_Folder");
	InsertMenu(hMenu,index++,MF_BYPOSITION | MF_STRING,TB_CAPTURE,L"Capture");
	InsertMenu(hMenu,index++,MF_BYPOSITION | MF_STRING,TB_INFO,L"About");
	InsertMenu(hMenu,index++,MF_BYPOSITION | MF_STRING,TB_EXIT,L"Exit");

	index = 0;
	InsertMenu(hSubMenu,index++,MF_BYPOSITION | MF_STRING,TB_SET_DIRECTORY,L"Change Save Directory");
//	InsertMenu(hSubMenu,index++,MF_BYPOSITION | MF_STRING,TB_SET_LINE_DRAW,L"On/Off Line Draw");
//	InsertMenu(hSubMenu,index++,MF_BYPOSITION | MF_STRING,TB_SET_LINE_COLOR,L"Change Line Color");
//	InsertMenu(hSubMenu,index++,MF_BYPOSITION | MF_STRING,TB_DEFAULT,L"Set Default");

	//Hot key
	RegisterHotKey(hWnd, HK_SCREENSHOT, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 'S');
	RegisterHotKey(hWnd, HK_OPEN_DIRECTORY, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 'D');
	RegisterHotKey(hWnd, HK_LEFT_DIRECTORY, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 0X25);
	RegisterHotKey(hWnd, HK_RIGHT_DIRECTORY, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 0x27);
	RegisterHotKey(hWnd, HK_QUIT, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 'Q');
}

void CScreenCaptureFramework::Release() {
	D2DFramework->Release();
}

void CScreenCaptureFramework::Update() {
	D2DFramework->Update();
}

void CScreenCaptureFramework::Render() {
	D2DFramework->Render(m_preMousePos, m_nowMousePos);
}

void CScreenCaptureFramework::FullCapture() 
{
	if (g_bCaptured) {
		return;
	}
	g_bCaptured = true;

	GetCursorPos(&m_preMousePos);
	GetCursorPos(&m_nowMousePos);

	DEVMODE DevMode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
	m_systemX = SYSTEM_RESOL_X;
	m_systemY = SYSTEM_RESOL_Y;
	m_monitorX = FRAME_BUFFER_WIDTH;
	m_monitorY = FRAME_BUFFER_HEIGHT;

	DWORD nFileSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (m_systemX*m_systemY * 4));
	BYTE* pBmpFileData = (BYTE*)GlobalAlloc(0x0040, nFileSize);

	PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)pBmpFileData;
	BFileHeader->bfType = 0x4D42; // BM
	BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
	BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	PBITMAPINFOHEADER BInfoHeader = (PBITMAPINFOHEADER)&pBmpFileData[sizeof(BITMAPFILEHEADER)];
	BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BInfoHeader->biPlanes = 1;
	BInfoHeader->biBitCount = 24;
	BInfoHeader->biCompression = BI_RGB;
	BInfoHeader->biWidth = m_systemX;
	BInfoHeader->biHeight = m_systemY;

	HWND hInputWnd = GetDesktopWindow();
	HDC desctopDC = GetDC(NULL);
	m_hCapturedDC = CreateCompatibleDC(desctopDC);
	m_hCapturedBitmap = CreateCompatibleBitmap(desctopDC, m_systemX, m_systemY);
	SelectObject(m_hCapturedDC, m_hCapturedBitmap);
	BitBlt(m_hCapturedDC, 0, 0, m_systemX, m_systemY, desctopDC, 0, 0, SRCCOPY);

	RGBTRIPLE *pImage = (RGBTRIPLE*)&pBmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
	GetDIBits(m_hCapturedDC, m_hCapturedBitmap, 0, m_systemY, pImage, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

	ReleaseDC(NULL, m_hCapturedDC);
	GlobalFree(pBmpFileData);
	DeleteObject(m_hCapturedBitmap);

	D2DFramework->CreateD2DBitmap(m_hCapturedBitmap);
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_CURSOR1)));
	SetOption(true, true);
}

void CScreenCaptureFramework::SelectCapture() 
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	HMONITOR hMonitor = MonitorFromPoint(MousePos, 0);
	MONITORINFO infoMonitor;
	infoMonitor.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &infoMonitor);
	int selectedSizeX = infoMonitor.rcMonitor.right - infoMonitor.rcMonitor.left;
	int selectedSizeY = infoMonitor.rcMonitor.bottom - infoMonitor.rcMonitor.top;
	
	DWORD nFileSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (selectedSizeX * selectedSizeY * 4));
	BYTE* pBmpFileData = (BYTE*)GlobalAlloc(0x0040, nFileSize);

	PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)pBmpFileData;
	BFileHeader->bfType = 0x4D42; // BM
	BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
	BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	PBITMAPINFOHEADER BInfoHeader = (PBITMAPINFOHEADER)&pBmpFileData[sizeof(BITMAPFILEHEADER)];
	BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BInfoHeader->biPlanes = 1;
	BInfoHeader->biBitCount = 24;
	BInfoHeader->biCompression = BI_RGB;
	BInfoHeader->biWidth = selectedSizeX;
	BInfoHeader->biHeight = selectedSizeY;

	m_hSelectedDC = CreateCompatibleDC(m_hCapturedDC);
	m_hSelectedBitmap = CreateCompatibleBitmap(m_hCapturedDC, selectedSizeX, selectedSizeY);
	SelectObject(m_hSelectedDC, m_hSelectedBitmap);

	BitBlt(m_hSelectedDC, 
		0, 
		0, 
		selectedSizeX, 
		selectedSizeY,
		m_hCapturedDC, 
		infoMonitor.rcMonitor.left, 
		infoMonitor.rcMonitor.top, 
		SRCCOPY
	);

	RGBTRIPLE *pImage = (RGBTRIPLE*)&pBmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
	GetDIBits(m_hSelectedDC, m_hSelectedBitmap, 0, m_monitorY, pImage, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

	ReleaseDC(NULL, m_hSelectedDC);
	GlobalFree(pBmpFileData);
	DeleteObject(m_hSelectedBitmap);
}

void CScreenCaptureFramework::RegionCapture()
{
	float fPreRatioX = static_cast<float>(m_preMousePos.x)/m_monitorX;
	float fPreRatioY = static_cast<float>(m_preMousePos.y)/m_monitorY;
	float fNowRatioX = static_cast<float>(m_nowMousePos.x)/m_monitorX;
	float fNowRatioY = static_cast<float>(m_nowMousePos.y)/m_monitorY;

	m_preMousePos.x = fPreRatioX * m_monitorX;
	m_preMousePos.y = fPreRatioY * m_monitorY;
	m_nowMousePos.x = fNowRatioX * m_monitorX;
	m_nowMousePos.y = fNowRatioY * m_monitorY;

	if (m_nowMousePos.x < m_preMousePos.x) {
		std::swap(m_nowMousePos.x, m_preMousePos.x);
	}
	if (m_nowMousePos.y < m_preMousePos.y) {
		std::swap(m_nowMousePos.y, m_preMousePos.y);
	}
	int nWidth = abs(m_nowMousePos.x-m_preMousePos.x);
	int nHeight = abs(m_nowMousePos.y-m_preMousePos.y);

	DWORD nFileSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(sizeof(RGBTRIPLE)+1*(nWidth*nHeight*4));
	BYTE* pBmpFileData = (BYTE*)GlobalAlloc(0x0040,nFileSize);

	PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)pBmpFileData;
	BFileHeader->bfType = 0x4D42; // BM
	BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
	BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	PBITMAPINFOHEADER BInfoHeader = (PBITMAPINFOHEADER)&pBmpFileData[sizeof(BITMAPFILEHEADER)];
	BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BInfoHeader->biPlanes = 1;
	BInfoHeader->biBitCount = 24;
	BInfoHeader->biCompression = BI_RGB;
	BInfoHeader->biWidth = nWidth;
	BInfoHeader->biHeight = nHeight;

	m_hSelectedDC = CreateCompatibleDC(m_hCapturedDC);
	m_hSelectedBitmap = CreateCompatibleBitmap(m_hCapturedDC,nWidth,nHeight);
	SelectObject(m_hSelectedDC,m_hSelectedBitmap);

	SetStretchBltMode(m_hCapturedDC, HALFTONE);
	StretchBlt(m_hSelectedDC,0,0,nWidth,nHeight,m_hCapturedDC, m_preMousePos.x, m_preMousePos.y, nWidth,nHeight,SRCCOPY|CAPTUREBLT);

	RGBTRIPLE *Image = (RGBTRIPLE*)&pBmpFileData[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	GetDIBits(m_hSelectedDC,m_hSelectedBitmap,0,nHeight,Image,(LPBITMAPINFO)BInfoHeader,DIB_RGB_COLORS);

	ReleaseDC(m_hWnd, m_hCapturedDC);
	ReleaseDC(m_hWnd, m_hSelectedDC);
	GlobalFree(pBmpFileData); 
	DeleteObject(m_hSelectedBitmap);
}

void CScreenCaptureFramework::DrawOutLine() {

}

void CScreenCaptureFramework::LoadToClipBoard(){
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, m_hSelectedBitmap);
	CloseClipboard();
}

void CScreenCaptureFramework::SaveCaputre()
{
	if (CreateDirectory(g_vwsSaveDirectory[g_pvtSaveDirectory].c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError());

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap bitmap(m_hSelectedBitmap, NULL);
	CLSID clsid;
	GetEncoderClsid(L"image/png", &clsid);
	
	std::wstring fileName = g_vwsSaveDirectory[g_pvtSaveDirectory] + GetTimeW(time(NULL)) + std::wstring(L".png");
	bitmap.Save(fileName.c_str(), &clsid);
}

std::wifstream optionFile;
bool CScreenCaptureFramework::OperationExecute(const std::wstring& wsOperation)
{
	STR_SWITCH_BEGIN(wsOperation.c_str())
	{
		CASE(L"First_Execution")
			optionFile >> m_bFirstExecution;
			break;
		CASE(L"Directories")
			if (m_bFirstExecution) {	//프로그램이 처음 실행되었을 때
				WCHAR wcPath[512];
				SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, wcPath);
				lstrcat(wcPath, L"\\Captured\\");
				g_vwsSaveDirectory.push_back(wcPath);
				g_vwsSaveDirectory.push_back(wcPath);
				g_vwsSaveDirectory.push_back(wcPath);
				g_vwsSaveDirectory.push_back(wcPath);
				m_bFirstExecution = false;
			}
			else {						//이전에 프로그램을 실행했던 전력이 있을 때
				std::wstring pivot;
				while (!optionFile.eof()) {
					optionFile >> pivot;
					if (OperationExecute(pivot)) {
						return true;
					}
					g_vwsSaveDirectory.push_back(pivot);
				}
			}
			break;
		CASE(L"LineDraw")
			break;
		CASE(L"End")
			return true;
	}
	STR_SWITCH_END()

	return false;
}

void CScreenCaptureFramework::LoadOption()
{
	std::wstring pivot;
	optionFile.open("Resource/option.txt");
	if (!optionFile.is_open()) { return; }
	optionFile.imbue(std::locale("kor"));

	while (!optionFile.eof()) {
		optionFile >> pivot;
		OperationExecute(pivot);
	}
	optionFile.close();
	SaveOption();
}

void CScreenCaptureFramework::SaveOption()
{
	std::wofstream optionFile("Resource/option.txt");
	if (!optionFile.is_open()) { return; }
	optionFile.imbue(std::locale("kor"));

	//First_Execution
	optionFile << L"First_Execution" << std::endl;
	optionFile << m_bFirstExecution << std::endl;

	//Directorys
	optionFile << L"Directories" << std::endl;
	for (std::wstring wsName : g_vwsSaveDirectory) {
		optionFile << wsName << std::endl;
	}
	optionFile << L"End";

	optionFile.close();
}

void CScreenCaptureFramework::MouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case WM_RBUTTONDOWN:
		m_bLeftMouseButton = false;
		if(g_bCaptured){
			g_bCaptured = false;
//			SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
			D2DFramework->ClearScreen();
		//	ShowWindow(hWnd, false);
		}
		break;
	case WM_MOUSEMOVE:
		if (!g_bCaptured) {
			break;
		}
		if(m_bLeftMouseButton){
			GetCursorPos(&m_nowMousePos);
		}
		break;
	case WM_LBUTTONDOWN:
		if (!g_bCaptured) {
			break;
		}
		m_bLeftMouseButton = true;
		GetCursorPos(&m_preMousePos);
		GetCursorPos(&m_nowMousePos);
		break;
	case WM_LBUTTONUP:
		if (!g_bCaptured) {
			break;
		}
		GetCursorPos(&m_nowMousePos);
		//Full Screen Capture
		if(m_preMousePos.x == m_nowMousePos.x && m_preMousePos.y == m_nowMousePos.y){
			SelectCapture();
		}
		//Region Caputre
		else{
			RegionCapture();
		}
		
		DrawOutLine();
		LoadToClipBoard();
		SaveCaputre();
		D2DFramework->CreateD2DBitmap(m_hSelectedBitmap);

		D2DFramework->Message(EMessage::ScreenCaptured);
//		D2DFramework->ClearScreen();
		SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
		g_bCaptured = false;
		m_bLeftMouseButton = false;
		break;
	}
}

void CScreenCaptureFramework::KeyInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			if(g_bCaptured){
				g_bCaptured = false;
	//			SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
				D2DFramework->ClearScreen();
			//	ShowWindow(hWnd, false);
			}
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		}
		break;
	}
}

bool CScreenCaptureFramework::HotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case HK_SCREENSHOT: {
		FullCapture();	
		return true;
	}
	case HK_OPEN_DIRECTORY:
		ShellExecute(NULL, TEXT("explore"), g_vwsSaveDirectory[g_pvtSaveDirectory].c_str(), NULL, NULL, SW_SHOW);
		return true;
	case HK_LEFT_DIRECTORY:
		g_pvtSaveDirectory--;
		if (g_pvtSaveDirectory == -1) {
			g_pvtSaveDirectory = g_vwsSaveDirectory.size()-1;
		}
		GetCursorPos(&m_preMousePos);
		GetCursorPos(&m_nowMousePos);
		D2DFramework->Message(EMessage::SaveDirectoryChanged);
		return true;
	case HK_RIGHT_DIRECTORY:
		g_pvtSaveDirectory++;
		if (g_pvtSaveDirectory == g_vwsSaveDirectory.size()) {
			g_pvtSaveDirectory = 0;
		}
		GetCursorPos(&m_preMousePos);
		GetCursorPos(&m_nowMousePos);
		D2DFramework->Message(EMessage::SaveDirectoryChanged);
		return true;
	}
	return false;
}

void CScreenCaptureFramework::TaskBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT mouse;

	switch(lParam){
	case WM_LBUTTONDBLCLK:
		break;
	case WM_LBUTTONDOWN:
		TrackPopupMenu(NULL,NULL,NULL,NULL,0,hWnd,NULL);
		break;
	case WM_RBUTTONUP:
		GetCursorPos(&mouse);
		TrackPopupMenu(hMenu,TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON,mouse.x,mouse.y,0,hWnd,NULL);
		break;
	}
}

void CScreenCaptureFramework::MenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case TB_EXIT:
		g_bLoop = false;
		break;
	case TB_INFO: {
		std::wstring str;
		str += L"        For your Magnificent Data Collecting\n";
		str += L"\n";
		str += L"                 by - Patric Cho (조경배)\n";
		str += L"\n";
		str += L"             Capture : Ctrl + Alt + Shift + S\n";
		str += L"\n";
		str += L"        Directory Open : Ctrl + Alt + Shift + D\n";
		str += L"\n";
		str += L"    Directory Change : Ctrl + Alt + Shift + (←, →)\n";
		str += L"\n";
		str += L" Sound Device Change : Ctrl + Alt + Shift + (↑, ↓)\n";
		str += L"\n";
		str += L"            Bug Report : xmfhvlex@naver.com\n";
		str += L"\n";
		str += L"         Blog : http://blog.naver.com/xmfhvlex \n";
		MessageBox(hWnd, str.c_str(), L"Power Capture ver_0.02", MB_OK | MB_ICONQUESTION);
		break;
	}
	case TB_CAPTURE:
		FullCapture();
		break;
	case TB_SET_DIRECTORY: {
		WCHAR cpath[512] = _T("");
		LPITEMIDLIST pDirList;
		BROWSEINFO browseInfo;
		browseInfo.hwndOwner = hWnd;
		browseInfo.pidlRoot = NULL;
		browseInfo.lpszTitle = (L"Select The Folder for Your Captures");
		browseInfo.pszDisplayName = cpath;
		browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
		browseInfo.lpfn = NULL;
		browseInfo.lParam = 0;

		pDirList = SHBrowseForFolder(&browseInfo);
		if(pDirList == NULL) return;

		SHGetPathFromIDList(pDirList, cpath);
		g_vwsSaveDirectory[g_pvtSaveDirectory] = std::wstring(cpath) + L"\\";

		SaveOption();
		break;
	}
	case TB_SET_LINE_DRAW:
		//	m_bLineDraw = 1 - m_bLineDraw;
		SaveOption();
		break;
	case TB_SET_LINE_COLOR: {
		//CHOOSECOLOR color;
		//COLORREF ccref[16];
		//COLORREF selcolor=0x000000;
		//memset(&color,0,sizeof(color));
		//color.lStructSize=sizeof(CHOOSECOLOR);
		//color.hwndOwner=hWnd;
		//color.lpCustColors=ccref;
		//color.rgbResult=selcolor;
		//color.Flags=CC_RGBINIT;
		//ChooseColor(&color);
		//m_dColor = color.rgbResult;
		SaveOption();
		break;
	}
	case TB_OPEN_FOLDER:
		ShellExecute(NULL, TEXT("explore"), NULL, NULL, g_vwsSaveDirectory[g_pvtSaveDirectory].c_str(), SW_SHOW);
		break;
	case TB_DEFAULT:
		g_vwsSaveDirectory.clear();
		g_vwsSaveDirectory.push_back(L".\\");
		SaveOption();
		break;
	}
}
