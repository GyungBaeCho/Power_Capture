#pragma once

#include "Component.h"

class CObject
{
public:
	CObject() {	}
	~CObject() {}
	void Release();

	void Update(float fElapsedTime);
	void Render(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat);
//	void OperateComponent(ECompType eCompType);

	void AddComponent(IComponent* pComponent);
	IComponent* GetComponent(ECompType eCompType);

	void SetPosition(const VECTOR2& vec);
	void SetPosition(float x, float y);
	const VECTOR2& GetPosition();

	void SetSize(const VECTOR2& vec);
	void SetSize(float x, float y);
	const VECTOR2& GetSize();

	void SetDelete() {
		m_bDelete = true;
	}
	bool IsDeleted();
private:
	std::wstring						m_wName;
	std::map<ECompType, IComponent*>	m_mComponent;
	VECTOR2		m_vPosition;
	VECTOR2		m_vSize;
	bool		m_bDelete = false;
};
