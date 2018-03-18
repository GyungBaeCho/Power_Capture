#include "stdafx.h"
#include "Component.h"
#include "Object.h"

//Update Component---------------------------------------------------------------------------------------------
IUpdateComponent::IUpdateComponent(CObject * pOwner){
	SetCompType(ECompType::Update);
	SetOwner(pOwner);
	pOwner->AddComponent(this);
}

void IUpdateComponent::Update(float fElapsedTime) {
	VECTOR2 vPosition = m_pOwner->GetPosition();
	vPosition += VECTOR2(-10*fElapsedTime, 0);
	m_pOwner->SetPosition(vPosition);
}

ISlidePopUpdateComponent::ISlidePopUpdateComponent(CObject * pOwner) : IUpdateComponent(pOwner){

}

void ISlidePopUpdateComponent::Update(float fElapsedTime)
{
	VECTOR2 vPosition = m_pOwner->GetPosition();
	VECTOR2 vSize = m_pOwner->GetSize();

	vPosition += VECTOR2(m_fAcceleration * m_nMinus * fElapsedTime, 0);
	if (vPosition.x > 0) {
		vPosition.x = 0;
		m_fAcceleration = 0;
		m_nMinus = -1;
	}

	if (m_nMinus == -1) {
		if (m_fTimeCount < 0.6) {
			m_fTimeCount += fElapsedTime;
			return;
		}
	}

	m_pOwner->SetPosition(vPosition);
	m_fAcceleration += (m_fAcceleration+400)*fElapsedTime;

	if (vPosition.x+vSize.x < 0) {
		m_pOwner->SetDelete();
	}
}

//Render Component---------------------------------------------------------------------------------------------
IRenderComponent::IRenderComponent(CObject * pOwner) {
	SetCompType(ECompType::Render);
	SetOwner(pOwner);
	pOwner->AddComponent(this);
}

void IRenderComponent::Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush * pBrush, IDWriteTextFormat *pTextFormat) {

}

void ITextRenderComponent::Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush * pBrush, IDWriteTextFormat * pTextFormat){
	VECTOR2 vPosition = m_pOwner->GetPosition();
	VECTOR2 vSize = m_pOwner->GetSize();

	pBrush->SetColor(D2D1::ColorF(0, 0.5, 1, 1));
	pRenderTarget->FillRectangle(D2D1::RectF(vPosition.x + 0, vPosition.y, vPosition.x+vSize.x, vPosition.y+vSize.y), pBrush);

	pBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
	pRenderTarget->DrawTextW(m_wstrData.c_str(), m_wstrData.length(), pTextFormat, D2D1::RectF(vPosition.x+30, vPosition.y+10, vPosition.x+vSize.x, vPosition.y+vSize.y), pBrush);
}

void ICapturedRenderComponent::Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat) {
	VECTOR2 vPosition = m_pOwner->GetPosition();
	VECTOR2 vSize = m_pOwner->GetSize();

	pBrush->SetColor(D2D1::ColorF(0, 0.5, 1, 1));
	pRenderTarget->FillRectangle(D2D1::RectF(vPosition.x + 0, vPosition.y, vPosition.x+vSize.x, vPosition.y+vSize.y), pBrush);

	pBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
	pRenderTarget->DrawTextW(m_wstrData.c_str(), m_wstrData.length(), pTextFormat, D2D1::RectF(vPosition.x+10, vPosition.y+(vSize.y/2)-10, vPosition.x+vSize.x, vPosition.y+vSize.y), pBrush);

	if (m_pd2dBitmap) {
		D2D1_SIZE_F bitmapSize = m_pd2dBitmap->GetSize();
		
		float fBorder = 4;
		VECTOR2 vBegin = VECTOR2(vPosition.x+150+fBorder, vPosition.y+fBorder);
		VECTOR2 vEnd = VECTOR2(vPosition.x+vSize.x-fBorder, vPosition.y+vSize.y-fBorder);

		float fSizeX = vEnd.x - vBegin.x;
		float fSizeY = vEnd.y - vBegin.y;

		float fRatio = fSizeX / bitmapSize.width;
		if (bitmapSize.height * fRatio > fSizeY) {
			fRatio = fSizeY / bitmapSize.height;
		}

		float fCenterX = vBegin.x + (fSizeX/2);
		float fCenterY = vBegin.y + (fSizeY/2);

		fSizeX = bitmapSize.width * fRatio / 2;
		fSizeY = bitmapSize.height * fRatio / 2;
		
		pRenderTarget->DrawBitmap(m_pd2dBitmap, D2D1::RectF(fCenterX-fSizeX, fCenterY-fSizeY, fCenterX+fSizeX, fCenterY+fSizeY));
	}
}