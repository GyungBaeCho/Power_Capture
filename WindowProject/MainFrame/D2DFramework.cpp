#include "stdafx.h"
#include "D2DFramework.h"
#include "ApiFramework.h"
#include "SoundDeviceManager.h"

CD2DFramework * CD2DFramework::GetInstance() {
	if (m_pInstance) {
		return m_pInstance;
	}
	else {
		m_pInstance = new CD2DFramework;
		m_pInstance->Initialize(g_hwnd);
		return m_pInstance;
	}
}

void CD2DFramework::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	//CObject* pObject = new CObject;
	//IComponent* pUpdateComp = new IUpdateComponent(pObject);
	//IComponent* pRenderComp = new IRenderComponent(pObject);
	//pObject->OperateComponent(ECompType::Update);
	//pObject->OperateComponent(ECompType::ClearScreen);

	//Initialzing IWICImagingFactory
	CoInitialize(nullptr);
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWicFactory));

	//Direct2D ClearScreen Target
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	//D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pD2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
		);

	RECT rect;
	GetClientRect(hWnd, &rect);
	D2D1_HWND_RENDER_TARGET_PROPERTIES prop2 = D2D1::HwndRenderTargetProperties(
		hWnd,
		D2D1::SizeU(rect.right, rect.bottom),
		D2D1_PRESENT_OPTIONS_IMMEDIATELY
	);

	//Direct2D HWND Buffer
	m_pD2DFactory->CreateHwndRenderTarget(props, prop2, &m_pRenderTarget);

	//Brush
	m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &m_pBrush);

	//Stroke Style
	float dashes[] = {5.0f, 10.0f};
	m_pD2DFactory->CreateStrokeStyle
	(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			100.0f,
			D2D1_DASH_STYLE_CUSTOM,
			0.0f
		),
		dashes, ARRAYSIZE(dashes),
		&m_pStrokeStyle
	);

	//Text ClearScreen
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&m_pWirteFactory));
	m_pWirteFactory->CreateTextFormat(L"HYGothic",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f, L"", &m_pTextFormat);
}

void CD2DFramework::Release() {
	m_pD2DFactory->Release();
	m_pRenderTarget->Release();
	m_pBrush->Release();
	m_pStrokeStyle->Release();
	m_pWirteFactory->Release();
	m_pTextFormat->Release();
	m_pWicFactory->Release();
	if (m_pd2dBitmap) {
		m_pd2dBitmap->Release();
	}
	if (m_pInstance) {
		delete m_pInstance;
	}
}


void CD2DFramework::Message(EMessage msg) {
	switch (msg) {
		case EMessage::ScreenCaptured: {
			CObject* pNewObject = new CObject;
			pNewObject->SetSize(VECTOR2(300, 90));
			pNewObject->SetPosition(-pNewObject->GetSize().x, 0);
			ISlidePopUpdateComponent* pUpdateComp = new ISlidePopUpdateComponent(pNewObject);
			ICapturedRenderComponent* pRenderComp = new ICapturedRenderComponent(pNewObject);
			pRenderComp->SetBitmap(m_pd2dBitmap);
			pRenderComp->SetStringData(L"Screen Captured!");
			m_lpUIObject.push_back(pNewObject);
			SetOption(true, true);
			break;
		}
		case EMessage::SoundDeviceChanged: {
			CObject* pNewObject = new CObject;
			pNewObject->SetSize(VECTOR2(340, 70));
			pNewObject->SetPosition(-pNewObject->GetSize().x, 0);
			ISlidePopUpdateComponent* pUpdateComp = new ISlidePopUpdateComponent(pNewObject);
			ITextRenderComponent* pRenderComp = new ITextRenderComponent(pNewObject);
			std::wstring wDeviceName = SoundDeviceManager->GetNowDeviceName();
			pRenderComp->SetStringData(L"Sound Device Changed!\n" + wDeviceName);
			m_lpUIObject.push_back(pNewObject);
			SetOption(true, true);
			break;
		}
		case EMessage::SaveDirectoryChanged: {
			CObject* pNewObject = new CObject;
			pNewObject->SetSize(VECTOR2(340, 70));
			pNewObject->SetPosition(-pNewObject->GetSize().x, 0);
			ISlidePopUpdateComponent* pUpdateComp = new ISlidePopUpdateComponent(pNewObject);
			ITextRenderComponent* pRenderComp = new ITextRenderComponent(pNewObject);
			std::wstring wstrData = L"Save Directory Changed!\n" + g_vwsSaveDirectory[g_pvtSaveDirectory];
			pRenderComp->SetStringData(wstrData);
			m_lpUIObject.push_back(pNewObject);
			SetOption(true, true);
			break;
		}
	}
}

bool CD2DFramework::KeyInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYUP:
		switch (wParam) {

		}
		break;
	}
	return true;
}

void CD2DFramework::ClearScreen() {
	m_pRenderTarget->BeginDraw();
		m_pRenderTarget->Clear(D2D1::ColorF(1,0,1,1));
	m_pRenderTarget->EndDraw();
}

void CD2DFramework::Update() {
	m_lpUIObject.remove_if([](CObject* pUiObject) {
		pUiObject->Update((FRAME_PER_MICRO_SECOND / 1000.0f));
		if (pUiObject->IsDeleted()) {
			return true;
		}
		else {
			return false;
		}
	});
	if(m_lpUIObject.size()==0 && g_bCaptured==false){
		SetOption(false, false);
	}
}

void CD2DFramework::Render(POINT preMousePos, POINT nowMousePos) {
	m_pRenderTarget->BeginDraw();
		if(g_bCaptured){
			m_pRenderTarget->DrawBitmap(m_pd2dBitmap, D2D1::RectF(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
			ScreenShotEffect(preMousePos, nowMousePos);
		}
		else{
			m_pRenderTarget->Clear(D2D1::ColorF(1,0,1,1));
		}
		//UI Ã³¸®
		for (CObject* pUiObject : m_lpUIObject) {
			pUiObject->Render(m_pRenderTarget, m_pBrush, m_pTextFormat);
		}
	m_pRenderTarget->EndDraw();
}

void CD2DFramework::ScreenShotEffect(POINT preMousePos, POINT nowMousePos)
{
	POINT MousePos;
	GetCursorPos(&MousePos);

	if (nowMousePos.x < preMousePos.x) {
		std::swap(nowMousePos.x, preMousePos.x);
	}
	if (nowMousePos.y < preMousePos.y) {
		std::swap(nowMousePos.y, preMousePos.y);
	}

	m_pBrush->SetColor(D2D1::ColorF(0, 0.6, 1, 0.3));
	m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, FRAME_BUFFER_WIDTH, preMousePos.y), m_pBrush);
	m_pRenderTarget->FillRectangle(D2D1::RectF(0, preMousePos.y, preMousePos.x, nowMousePos.y), m_pBrush);
	m_pRenderTarget->FillRectangle(D2D1::RectF(nowMousePos.x, preMousePos.y, FRAME_BUFFER_WIDTH, nowMousePos.y), m_pBrush);
	m_pRenderTarget->FillRectangle(D2D1::RectF(0, nowMousePos.y, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT), m_pBrush);

	m_pBrush->SetColor(D2D1::ColorF(1,1,1,0.8));
	m_pRenderTarget->DrawLine(D2D1::Point2F(0, MousePos.y), D2D1::Point2F(FRAME_BUFFER_WIDTH, MousePos.y), m_pBrush, 0.7, 0);
	m_pRenderTarget->DrawLine(D2D1::Point2F(MousePos.x, 0), D2D1::Point2F(MousePos.x, FRAME_BUFFER_HEIGHT), m_pBrush, 0.7, 0);

	m_pBrush->SetColor(D2D1::ColorF(0,0,0,0.5));
	m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, FRAME_BUFFER_WIDTH, 30), m_pBrush);

	WCHAR wChar[20];
	swprintf_s(wChar, L"[%d X %d]", MousePos.x, MousePos.y);
	m_pBrush->SetColor(D2D1::ColorF(1,1,1,1));
	m_pRenderTarget->DrawTextW(wChar, lstrlen(wChar), m_pTextFormat, D2D1::RectF(MousePos.x+2, MousePos.y-19, MousePos.x+200, MousePos.y), m_pBrush);

	std::wstring wsDirectory = L"Save Directory : " + g_vwsSaveDirectory[g_pvtSaveDirectory];
	m_pRenderTarget->DrawTextW(wsDirectory.c_str(), wsDirectory.length(), m_pTextFormat, D2D1::RectF(0, 0, 1920, 10), m_pBrush);

	if (preMousePos.x != nowMousePos.x && preMousePos.y != nowMousePos.y) {
		m_pBrush->SetColor(D2D1::ColorF(1,1,1,1));
		m_pRenderTarget->DrawRectangle(D2D1::RectF(preMousePos.x, preMousePos.y, nowMousePos.x, nowMousePos.y), m_pBrush, 1, m_pStrokeStyle);
	}
}

ID2D1Bitmap * CD2DFramework::CreateD2DBitmap(const HBITMAP & hBitmap)
{
	//if (m_pd2dBitmap) {
	//	m_pd2dBitmap->Release();
	//	m_pd2dBitmap = nullptr;
	//}

	IWICBitmap* pDialogBitmap = nullptr;
	m_pWicFactory->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapIgnoreAlpha, &pDialogBitmap);

	IWICFormatConverter * pConvertedSourceBitmap;
	m_pWicFactory->CreateFormatConverter(&pConvertedSourceBitmap);

	if (pConvertedSourceBitmap)
	{
		pConvertedSourceBitmap->Initialize(pDialogBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		m_pRenderTarget->CreateBitmapFromWicBitmap(pConvertedSourceBitmap, 0, &m_pd2dBitmap);
	}

	pDialogBitmap->Release();
	pConvertedSourceBitmap->Release();

	return m_pd2dBitmap;
}