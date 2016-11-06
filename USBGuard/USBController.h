#pragma once
#include "DeviceController.h"

typedef enum USBState{
	USB_OFFLINE,//����״̬
	USB_DISABLE,//����״̬
	USB_ENABLE,//����״̬
};//����USB�˿ڵ�����״̬

class USBController :
	public DeviceController
{
public:
	USBController();
	~USBController();

	//ʵ�ֳ������еĳ����Ա
	virtual int DeviceState(){ return current_state; }
	virtual BOOL DeviceRegister(HWND hWnd);
	virtual int DeviceDetect(UINT nEventType, DWORD_PTR dwData);
	virtual BOOL DeviceEnable(BOOL state);
	virtual BOOL DeviceIdentify(HDEVINFO& hDevInfo, SP_DEVINFO_DATA& spDevInfoData);
	
	//�������ж�������г�Ա
private:
	int current_state;//USB�˿ڵ�ǰ��״̬
	CString m_pszDevId[10];
	BOOL EnableUSB(HDEVINFO& hDevInfo, CString& strDevId, BOOL state);
	BOOL FindUSB(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData);
};

