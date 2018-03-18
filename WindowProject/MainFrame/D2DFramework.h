#pragma once
#include "Object.h"
#include "Timer.h"

_declspec(selectany) float gfElapsedTime = 0;

#if defined(CLSID_WICImagingFactory)
#undef CLSID_WICImagingFactory
#endif

enum class EMessage {
	ScreenCaptured,
	SoundDeviceChanged,
	SaveDirectoryChanged,
};

#define D2DFramework	CD2DFramework::GetInstance()
class CD2DFramework{
public:
	static CD2DFramework * GetInstance();

	void Initialize(HWND hWnd);
	void Release();

	void Update();
	void ClearScreen();
	void Render(POINT preMousePos, POINT nowMousePos);
	void ScreenShotEffect(POINT preMousePos, POINT newMousePos);

	ID2D1Bitmap* CreateD2DBitmap(const HBITMAP & hBitmap);

	ID2D1Bitmap* GetCapturedBitmap() {
		return m_pd2dBitmap;
	}

	void Message(EMessage msg);
	bool KeyInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	CD2DFramework() {}
	~CD2DFramework() {}

private:
	static CD2DFramework *		m_pInstance;
	HWND						m_hWnd;
	ID2D1Factory				*m_pD2DFactory = nullptr;
	ID2D1HwndRenderTarget		*m_pRenderTarget = nullptr;
	ID2D1SolidColorBrush		*m_pBrush = nullptr;
	ID2D1StrokeStyle			*m_pStrokeStyle = nullptr;

	IDWriteFactory				*m_pWirteFactory = nullptr;
	IDWriteTextFormat			*m_pTextFormat = nullptr;

	IWICImagingFactory			*m_pWicFactory = nullptr;
	ID2D1Bitmap					*m_pd2dBitmap = nullptr;

	std::list<CObject*>			m_lpUIObject;
};

_declspec(selectany) CD2DFramework * CD2DFramework::m_pInstance = nullptr;
