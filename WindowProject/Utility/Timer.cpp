#include "stdafx.h"
#include "Timer.h"

void CTimer::Initialize(int nLockFPS) {
	__int64 countsPereSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPereSec);
	m_dSecondsPerCount = 1.0 / (double)countsPereSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_nPrevTime);
	m_nCurrTime = 0;

	m_dLockTime = 1.0f / nLockFPS;

	lstrcpy(m_wName, _T("Normal"));
	ZeroMemory(m_wcTimeInfo, sizeof(WCHAR) * 50);
	ZeroMemory(m_wcAdvanceInfo, sizeof(WCHAR) * 50);
}

float CTimer::Update() {
	//Real Elapsed Time °è»ê
	QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrTime);
	m_dAdvanceTime = (m_nCurrTime - m_nPrevTime) * m_dSecondsPerCount;

	//FPS Time Lock
	__int64	nPrevTime;
	__int64	nCurrTime;
	m_dDeltaTime = m_dAdvanceTime;

	QueryPerformanceCounter((LARGE_INTEGER*)&nPrevTime);
	while (true) {
		QueryPerformanceCounter((LARGE_INTEGER*)&nCurrTime);
		m_dDeltaTime += (nCurrTime - nPrevTime) * m_dSecondsPerCount;
		if (m_dDeltaTime >= m_dLockTime) {
			m_dWaitTime = m_dDeltaTime - m_dAdvanceTime;
			if (m_dDeltaTime < 0.0) m_dDeltaTime = 0.0;
			break;
		}
		nPrevTime = nCurrTime;
	}

	m_nFrameCounter++;
	m_dSumElapsedTime += m_dDeltaTime;
	m_dSumAdvanceTime += m_dAdvanceTime;

	m_dSecondCounter += m_dDeltaTime;
	if (m_dSecondCounter > 1.0) {
		m_nAvgFPS = m_nFrameCounter;
		m_dAvgElapsedTime = m_dSumElapsedTime / m_nFrameCounter;
		m_dAvgAdvanceTime = m_dSumAdvanceTime / m_nFrameCounter;
		m_nFrameCounter = 0;
		m_dSecondCounter = 0;
		m_dSumElapsedTime = 0;
		m_dSumAdvanceTime = 0;

		swprintf(m_wcTimeInfo, 50, L"%s : %04dFPS | %f", m_wName, m_nAvgFPS, static_cast<float>(m_dAvgElapsedTime));
		swprintf(m_wcAdvanceInfo, 50, L"Advance :             %f", static_cast<float>(m_dAvgAdvanceTime));
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&m_nPrevTime);
	return (float)m_dDeltaTime;
}