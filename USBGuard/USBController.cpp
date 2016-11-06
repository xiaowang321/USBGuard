#include "stdafx.h"
#include "USBController.h"


USBController::USBController()
{
	current_state = USBState::USB_OFFLINE;//初始化为离线状态
}
USBController::~USBController()
{
}
/*注册USB设备通知信息*/
BOOL USBController::DeviceRegister(HWND hWnd){
	BOOL flag = TRUE;
	static const GUID GUID_DEVINTERFACE =
	{
		0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED }
	};//USB的GUID
	//注册USB插拔事件
	HDEVNOTIFY hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
	ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE;
	hDevNotify = RegisterDeviceNotification(hWnd, &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (!hDevNotify)
		flag &= FALSE;
	else
		flag &= TRUE;
	return flag;
}

BOOL USBController::DeviceIdentify(HDEVINFO& hDevInfo, SP_DEVINFO_DATA& spDevInfoData){
	int m_nStrID = 0;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) {
		DWORD nSize = 0;
		TCHAR buf[MAX_PATH];
		CString strDevIdTemp;

		if (!SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize)) {
			int ErrorCode = GetLastError();
			CString ErrorMessage;
			ErrorMessage.Format(_T("%s%s(%x)"), _T("SetupDiGetDeviceInstanceId(): "), _com_error(ErrorCode).ErrorMessage(), ErrorCode);
			AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION);
			return FALSE;
		}

		strDevIdTemp = buf;
		strDevIdTemp = strDevIdTemp.Left(26);
		if (0 == (strDevIdTemp.Compare(_T("STORAGE\\VOLUME\\_??_USBSTOR"))))
		{
			m_pszDevId[m_nStrID] = buf;
			m_nStrID++;
		}
	}
	if (!m_pszDevId[0].IsEmpty()){
		for (int i = 1; i < 10; i++)
		{
			if (m_pszDevId[i].Compare(m_pszDevId[0]) == 0)
				m_pszDevId[i].SetString(_T(""));
		}

		return TRUE;
	}
	return FALSE;
}

/*发现新的USB设备，返回当前USB端口状态的判断*/
int USBController::DeviceDetect(UINT nEventType, DWORD_PTR dwData){
	PDEV_BROADCAST_HDR pdb;
	CString strVolume = _T("");
	pdb = (PDEV_BROADCAST_HDR)dwData;
	switch (current_state){
	case USBState::USB_OFFLINE://当前为离线状态
		switch (nEventType){
		case DBT_DEVICEARRIVAL://监测到设备上线
			current_state = USB_DISABLE;//默认设备遭到禁用
			break;
		default:break;
		}
		break;
	case USBState::USB_DISABLE://当前为禁用状态
		switch (nEventType){
		case DBT_DEVICEREMOVECOMPLETE://监测到设备离线，但是由于设备禁用也会触发该消息，因此不改变状态
			current_state = USB_OFFLINE;
			break;
		default:break;
		}
		break;
	case USBState::USB_ENABLE://当前为启用状态
		switch (nEventType){
		case DBT_DEVICEREMOVECOMPLETE://监测到设备离线，则将状态重置为离线状态。
			current_state = USBState::USB_OFFLINE;
			break;
		default:break;
		}
		break;
	}
	return current_state;
}

/*通过state参数来使能USB端口，TRUE表示启用，FALSE表示禁用*/
BOOL USBController::DeviceEnable(BOOL state){
	BOOL flag = TRUE;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);
	if (INVALID_HANDLE_VALUE == hDevInfo) {
		int ErrorCode = GetLastError();
		CString ErrorMessage;
		ErrorMessage.Format(_T("%s%s(%x)"), _T("SetupDiGetClassDevs(): "), _com_error(ErrorCode).ErrorMessage(), ErrorCode);
		AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION);
		return FALSE;
	}
	SP_DEVINFO_DATA spDevInfoData;
	if (DeviceIdentify(hDevInfo, spDevInfoData))
	{
		for (int j = 0; !m_pszDevId[j].IsEmpty(); j++)
			flag &= EnableUSB(hDevInfo, m_pszDevId[j], state);
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	if (flag == FALSE){
		current_state = USBState::USB_OFFLINE;
	}
	else if (state == TRUE)
		current_state = USBState::USB_ENABLE;
	return flag;
}

BOOL USBController::EnableUSB(HDEVINFO& hDevInfo, CString& strDevId, BOOL state){
	SP_PROPCHANGE_PARAMS spPropChangeParams;
	spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
	if (TRUE == state){
		spPropChangeParams.StateChange = DICS_ENABLE;
	}
	else if (FALSE == state){
		spPropChangeParams.StateChange = DICS_DISABLE;
	}
	spPropChangeParams.HwProfile = 0;

	SP_DEVINFO_DATA spDevInfoData;
	BOOL res = FindUSB(hDevInfo, strDevId, spDevInfoData);
	if (res) {
		if (!SetupDiSetClassInstallParams(hDevInfo, &spDevInfoData,(SP_CLASSINSTALL_HEADER*)&spPropChangeParams, sizeof(spPropChangeParams))) {
			int ErrorCode = GetLastError();
			CString ErrorMessage;
			ErrorMessage.Format(_T("%s%s(%x)"), _T("SetupDiSetClassInstallParams(): "), _com_error(ErrorCode).ErrorMessage(), ErrorCode);
			AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION);
			return FALSE;
		}
		else if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &spDevInfoData)) {
			int ErrorCode = GetLastError();
			if ((!(state == FALSE && ErrorCode == 0xE000020B)) && (ErrorCode != 0xD)){
				CString ErrorMessage;
				ErrorMessage.Format(_T("%s%s(%x)"), _T("SetupDiClassInstaller(): "), _com_error(ErrorCode).ErrorMessage(), ErrorCode);
				AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION);
				return FALSE;
			}
			else
				return TRUE;
		}
		else
			return TRUE;
	}
	return FALSE;
}

BOOL USBController::FindUSB(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData){
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) {
		DWORD nSize = 0;
		TCHAR buf[MAX_PATH];
		CString strDevIdTemp;
		if (!SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize)) {
			int ErrorCode = GetLastError();
			CString ErrorMessage;
			ErrorMessage.Format(_T("%s%s(%x)"), _T("SetupDiGetDeviceInstanceId(): "), _com_error(ErrorCode).ErrorMessage(), ErrorCode);
			AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION);
			return FALSE;
		}
		if (szDevId.Compare(buf) == 0){
			return TRUE;
		}
	}
	return FALSE;
}
