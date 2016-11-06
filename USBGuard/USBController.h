#pragma once
#include "DeviceController.h"

typedef enum USBState{
	USB_OFFLINE,//离线状态
	USB_DISABLE,//禁用状态
	USB_ENABLE,//启用状态
};//定义USB端口的三种状态

class USBController :
	public DeviceController
{
public:
	USBController();
	~USBController();

	//实现抽象类中的抽象成员
	virtual int DeviceState(){ return current_state; }
	virtual BOOL DeviceRegister(HWND hWnd);
	virtual int DeviceDetect(UINT nEventType, DWORD_PTR dwData);
	virtual BOOL DeviceEnable(BOOL state);
	virtual BOOL DeviceIdentify(HDEVINFO& hDevInfo, SP_DEVINFO_DATA& spDevInfoData);
	
	//具体类中定义的特有成员
private:
	int current_state;//USB端口当前的状态
	CString m_pszDevId[10];
	BOOL EnableUSB(HDEVINFO& hDevInfo, CString& strDevId, BOOL state);
	BOOL FindUSB(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData);
};

