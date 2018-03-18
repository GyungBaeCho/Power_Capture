#pragma once

#include <mmdeviceapi.h>
#include "PolicyConfig.h"


#define SoundDeviceManager		CSoundDeviceManager::GetInstance()
class CSoundDeviceManager {
public:
	CSoundDeviceManager() {}
	~CSoundDeviceManager() {}

	static CSoundDeviceManager* GetInstance() {
		if (m_pInstance) {
			return m_pInstance;
		}
		else {
			m_pInstance = new CSoundDeviceManager;
			m_pInstance->Initialize();
			return m_pInstance;
		}
	}

	void Initialize();

	//Sound Output Device를 List화 합니다.
	void PrepareDeviceList();

	std::wstring GetNowDeviceName();

	HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);

	bool HotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static CSoundDeviceManager*	m_pInstance;
	std::vector<WCHAR*>			m_vwDeviceID;
	std::vector<WCHAR*>			m_vwDeviceName;
	int							m_pvtID=0;
	UINT						m_nCount = 0;
};

__declspec(selectany) CSoundDeviceManager* CSoundDeviceManager::m_pInstance = nullptr;