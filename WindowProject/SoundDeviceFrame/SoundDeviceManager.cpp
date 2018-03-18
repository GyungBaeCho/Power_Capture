#include "stdafx.h"
#include "SoundDeviceManager.h"
#include "ApiFramework.h"
#include "D2DFramework.h"

#define _CRT_SECURE_NO_WARNINGS 1

void CSoundDeviceManager::Initialize() 
{
	PrepareDeviceList();

	//HotKey Register
	RegisterHotKey(g_hwnd, HK_LEFT_SOUND_DEVICE, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 0X26);
	RegisterHotKey(g_hwnd, HK_RIGHT_SOUND_DEVICE, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 0x28);
}

void CSoundDeviceManager::PrepareDeviceList()
{
	IMMDeviceEnumerator *pEnum = nullptr;      // Audio device enumerator.
	IMMDeviceCollection *pDevices = nullptr;   // Audio device collection.
	IMMDevice *pDevice = nullptr;              // An audio device.
	IPropertyStore *pProps = nullptr;

	CoInitialize(nullptr);

	if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum)))
	{
		return;
	}

	if (FAILED(pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices)))
	{
		return;
	}

	m_vwDeviceID.clear();
	m_vwDeviceName.clear();

	cout << "=================================" << endl;
	cout << "PrepareDeviceList" << endl;
	pDevices->GetCount(&m_nCount);
	cout << "Num of Device : " << m_nCount << endl;

	for (int i = 0; i < m_nCount; ++i) 
	{
		pDevices->Item (i, &pDevice);

		m_vwDeviceID.push_back(NULL);
		pDevice->GetId(&m_vwDeviceID[i]);

		pDevice->OpenPropertyStore(STGM_READ, &pProps);
        PROPVARIANT varName;
        PropVariantInit(&varName);
        pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		m_vwDeviceName.push_back(varName.pwszVal);
		std::wcout << "[" << i << "] : " << varName.pwszVal << endl;
	
		pProps->Release();
	}

	pEnum->Release();
	pDevices->Release();
}

std::wstring CSoundDeviceManager::GetNowDeviceName() {
	return m_vwDeviceName[m_pvtID];
}

HRESULT CSoundDeviceManager::SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
	IPolicyConfig *pPolicyConfig;
	ERole reserved = eConsole;

	HRESULT hr = CoCreateInstance(
		__uuidof(CPolicyConfigClient),
		NULL,
		CLSCTX_ALL,
		__uuidof(IPolicyConfig),
		(LPVOID *)&pPolicyConfig);

	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}

	return hr;
}

bool CSoundDeviceManager::HotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case HK_LEFT_SOUND_DEVICE:
		m_pvtID--;

		if (m_pvtID < 0 || m_pvtID == m_vwDeviceID.size())
		{
			m_pvtID = m_vwDeviceID.size() - 1;
		}

		SetDefaultAudioPlaybackDevice(m_vwDeviceID[m_pvtID]);
		D2DFramework->Message(EMessage::SoundDeviceChanged);
		return true;

	case HK_RIGHT_SOUND_DEVICE:
		m_pvtID++;
		if (m_pvtID < 0 || m_pvtID == m_vwDeviceID.size())
		{
			m_pvtID = 0;
		}
		SetDefaultAudioPlaybackDevice(m_vwDeviceID[m_pvtID]);
		D2DFramework->Message(EMessage::SoundDeviceChanged);
		return true;
	}
	return false;
}
