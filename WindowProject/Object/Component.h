#pragma once

class CObject;

enum class ECompType : char {
	Render,
	Update,
	MouseInput,
	KeyboardInput,
};

class IComponent
{
public:
	IComponent() {}
	virtual ~IComponent() {}
	virtual void Release() {}

	void SetCompType(ECompType eCompType) {
		m_eCompType = eCompType;
	}
	ECompType GetCompType() {
		return m_eCompType;
	}

	void SetOwner(CObject *pOwner) {
		m_pOwner = pOwner;
	}
	CObject* GetOwner() {
		return m_pOwner;
	}

protected:
	CObject		*m_pOwner;

private:
	ECompType	m_eCompType;
};

//Update Component---------------------------------------------------------------------------------------------
class IUpdateComponent : public IComponent {
public:
	IUpdateComponent(CObject *pOwner);
	~IUpdateComponent() {}

	virtual void Update(float fElapsedTime);

private:

};

class ISlidePopUpdateComponent : public IUpdateComponent {
public:
	ISlidePopUpdateComponent(CObject *pOwner);
	~ISlidePopUpdateComponent() {}

	virtual void Update(float fElapsedTime);

private:
	float	m_fAcceleration = 1000;
	int		m_nMinus = 1;
	float	m_fTimeCount = 0;
};

//Render Component---------------------------------------------------------------------------------------------
class IRenderComponent : public IComponent {
public:
	IRenderComponent(CObject *pOwner);
	~IRenderComponent() {}

	virtual void Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat);

	void SetStringData(std::wstring str) {
		m_wstrData = str;
	}

protected:
	std::wstring m_wstrData;
	
private:
};

class ITextRenderComponent : public IRenderComponent {
public:
	ITextRenderComponent(CObject * pOwner) : IRenderComponent(pOwner) {}
	~ITextRenderComponent() {}

	virtual void Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat);

private:

};

class ICapturedRenderComponent : public IRenderComponent {
public:
	ICapturedRenderComponent(CObject * pOwner) : IRenderComponent(pOwner) {}
	~ICapturedRenderComponent() {}
	void Release() {
		if (m_pd2dBitmap) {
			m_pd2dBitmap->Release();
			m_pd2dBitmap = nullptr;
		}
	}

	virtual void Render(ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush *pBrush, IDWriteTextFormat *pTextFormat);

	void SetBitmap(ID2D1Bitmap	*pd2dBitmap) {
		m_pd2dBitmap = pd2dBitmap;
	}
private:
	ID2D1Bitmap	*m_pd2dBitmap = nullptr;
};

//Key Input Component------------------------------------------------------------------------------------------