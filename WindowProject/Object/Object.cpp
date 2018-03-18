#include "stdafx.h"
#include "Object.h"
#include "Component.h"

void CObject::Release() {
	for (auto& pComp : m_mComponent) {
		pComp.second->Release();
		delete pComp.second;
	}
}

void CObject::Update(float fElapsedTime) {
	IComponent* pComponent = GetComponent(ECompType::Update);
	if (!pComponent) {
		return;
	}
	IUpdateComponent* pOpComp = static_cast<IUpdateComponent*>(pComponent);

	pOpComp->Update(fElapsedTime);
}

void CObject::Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat) {
	IComponent* pComponent = GetComponent(ECompType::Render);
	if (!pComponent) {
		return;
	}
	IRenderComponent* pOpComp = static_cast<IRenderComponent*>(pComponent);
	pOpComp->Render(pRenderTarget, pBrush, pTextFormat);
}

//void CObject::OperateComponent(ECompType eCompType)
//{
//	IComponent* pComponent = GetComponent(eCompType);
//	if (!pComponent) {
//		return;
//	}
//
//	switch (eCompType) {
//	case ECompType::MouseInput:
//		break;
//	case ECompType::KeyboardInput:
//		break;
//	}
//}

void CObject::AddComponent(IComponent * pComponent) {
	if (m_mComponent[pComponent->GetCompType()]) {
		m_mComponent[pComponent->GetCompType()]->Release();
		delete m_mComponent[pComponent->GetCompType()];
	}
	m_mComponent[pComponent->GetCompType()] = pComponent;
}

IComponent * CObject::GetComponent(ECompType eCompType) {
	try {
		return m_mComponent[eCompType];
	}
	catch(std::out_of_range){
		return nullptr;
	}
}

void CObject::SetPosition(const VECTOR2 & vec) {
	m_vPosition = vec;
}

void CObject::SetPosition(float x, float y) {
	m_vPosition = VECTOR2(x, y);
}

const VECTOR2 & CObject::GetPosition() {
	return m_vPosition;
}

void CObject::SetSize(const VECTOR2 & vec) {
	m_vSize = vec;
}

void CObject::SetSize(float x, float y) {
	m_vSize = VECTOR2(x, y);
}

const VECTOR2 & CObject::GetSize() {
	return m_vSize;
}

bool CObject::IsDeleted() {
	return m_bDelete;
}
