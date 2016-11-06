#pragma once

#include "setupapi.h"
#include "dbt.h"

#pragma comment (lib,"setupapi.lib")

class  DeviceController
{
public:
	DeviceController(){};
	virtual ~DeviceController(){};
	virtual int DeviceState() = 0;
	virtual BOOL DeviceRegister(HWND hWnd) = 0;
	virtual int DeviceDetect(UINT nEventType, DWORD_PTR dwData) = 0;
	virtual BOOL DeviceEnable(BOOL state) = 0;
	virtual BOOL DeviceIdentify(HDEVINFO& hDevInfo, SP_DEVINFO_DATA& spDevInfoData) = 0;
};

