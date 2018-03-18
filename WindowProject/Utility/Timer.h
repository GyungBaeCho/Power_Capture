#pragma once

#define TimerEngine CTimer::GetInstance()
#define TimeElapsed CTimer::GetInstance()->GetElapsedTime()

class CTimer {
private:
	static CTimer* m_pInstance;

	// 실제 Advanced Time
	double		m_dAdvanceTime		= 0;
	// Time Lock이 유지된 시간
	double		m_dWaitTime			= 0;
	// Wait Time + Advance Time (Scene Update에 사용되는 최종 Elapsed Time)
	double		m_dDeltaTime		= -1;

	double		m_dSecondsPerCount	= 0;
	double		m_dSecondCounter	= 0;

	// Time Lock 시간
	double		m_dLockTime			= 1/70.0;

	int			m_nFrameCounter		= 0;

	double		m_dSumElapsedTime	= 0;
	double		m_dSumAdvanceTime	= 0;

	int			m_nAvgFPS			= 0;
	double		m_dAvgElapsedTime	= 0;
	double		m_dAvgAdvanceTime	= 0;

	__int64		m_nPrevTime			= 0;
	__int64		m_nCurrTime			= 0;

	WCHAR		m_wName[50];
	WCHAR		m_wcTimeInfo[50];
	WCHAR		m_wcAdvanceInfo[50];

private:
	CTimer() {}
	~CTimer() {
		Release();
	}

public:
	static void CreateInstance() { if (m_pInstance == nullptr) m_pInstance = new CTimer; }
	static CTimer* GetInstance() { return m_pInstance; }

	void CTimer::Initialize(int nLockFPS = 70);
	void Release() {}

	float Update();

	void SetName(WCHAR* pwName) {
		lstrcpy(m_wName, pwName);
	}
	float GetElapsedTime() {
		return (float)m_dDeltaTime;
	}
	float GetAvgWaitTime() {
		return static_cast<float>(m_dAvgAdvanceTime);
	}
	float GetAvgElapsedTime() {
		return (float)m_dAvgElapsedTime;
	}
	int GetAvgFPS() {
		return m_nAvgFPS;
	}
	WCHAR* GetText() {
		return m_wcTimeInfo;
	}
	WCHAR* GetAdvanceInfo() {
		return m_wcAdvanceInfo;
	}
};

__declspec(selectany) CTimer* CTimer::m_pInstance = nullptr;

class CStopWatch {
private:
	double		m_dSecondsPerCount;
	double		m_dDeltaTime;
	double		m_dSecondCounter;

	int			m_nFrameCounter;

	int			m_nAvgFPS;
	double		m_dSumElapsedTime;
	double		m_dAvgElapsedTime;

	__int64		m_nBaseTime;
	__int64		m_nPrevTime;
	__int64		m_nCurrTime;

	WCHAR		m_wcName[50];
	WCHAR		m_wcTimeInfo[50];

public:
	CStopWatch(WCHAR* pwName) {
		__int64 countsPereSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPereSec);
		m_dSecondsPerCount = 1.0 / (double)countsPereSec;
		m_dDeltaTime = -1;
		m_dSecondCounter = 0;
		m_nFrameCounter = 0;
		m_nAvgFPS = 0;
		m_dSumElapsedTime = 0;
		m_dAvgElapsedTime = 0;
		m_nBaseTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nPrevTime);
		m_nCurrTime = 0;
		ZeroMemory(m_wcTimeInfo, sizeof(m_wcTimeInfo));
		lstrcpy(m_wcName, pwName);
		m_wcTimeInfo[0] = L'\0';
	}
	virtual ~CStopWatch() {}

	void BeginTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nPrevTime);
	}

	float EndTimer(float fElapsedTime) {
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrTime);
		m_dDeltaTime = (m_nCurrTime - m_nPrevTime) * m_dSecondsPerCount;
		if (m_dDeltaTime < 0.0) m_dDeltaTime = 0.0;

		m_nFrameCounter++;
		m_dSumElapsedTime += m_dDeltaTime;

		m_dSecondCounter += fElapsedTime;
		if (m_dSecondCounter > 1.0) {
			m_nAvgFPS = 1/m_dDeltaTime;
			m_dAvgElapsedTime = (double)m_dSumElapsedTime / m_nFrameCounter;
			m_nFrameCounter = 0;
			m_dSecondCounter = 0;
			m_dSumElapsedTime = 0;
			swprintf(m_wcTimeInfo, 50, L"%s : %04dFPS | %f", m_wcName, m_nAvgFPS, m_dDeltaTime);
		}
		return (float)m_dDeltaTime;
	}

	float GetElapsedTime() {
		return static_cast<float>(m_dDeltaTime);
	}

	WCHAR* GetText() {
		return m_wcTimeInfo;
	}
};