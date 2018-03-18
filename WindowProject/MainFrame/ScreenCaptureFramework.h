#pragma once

#define ScreenCaptureFramework CScreenCaptureFramework::GetInstance()

class CScreenCaptureFramework
{
public:	
	static void CreateInstance(HWND hWnd);
	static CScreenCaptureFramework * GetInstance();
	void Initialize(HWND hWnd);
	void Release();

	void Update();
	void Render();

	//Capture
	void FullCapture();
	void SelectCapture();
	void RegionCapture();

	void DrawOutLine();
	void LoadToClipBoard();
	void SaveCaputre();

	bool OperationExecute(const std::wstring& wsOperation);
	void LoadOption();

	void SaveOption();

	void MouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void KeyInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool HotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void TaskBar(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	void MenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	CScreenCaptureFramework() {}
	~CScreenCaptureFramework() {}

private:
	static CScreenCaptureFramework* m_pInstance;

	HWND	m_hWnd;
	//모니터 픽셀 사이즈
	int		m_monitorX;
	int		m_monitorY;
	//해상도 옵션 사이즈
	int		m_systemX;
	int		m_systemY;
	
	bool	m_bFirstExecution = false;
	bool	m_bLeftMouseButton = false;

	//Mouse Points
	POINT	m_preMousePos;
	POINT	m_nowMousePos;

	HDC		m_hCapturedDC;
	HBITMAP m_hCapturedBitmap;
	HDC		m_hSelectedDC;
	HBITMAP m_hSelectedBitmap;
};

_declspec(selectany) CScreenCaptureFramework* CScreenCaptureFramework::m_pInstance = nullptr;

