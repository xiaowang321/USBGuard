
// USBGuard.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUSBGuardApp: 
// �йش����ʵ�֣������ USBGuard.cpp
//

class CUSBGuardApp : public CWinApp
{
public:
	CUSBGuardApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUSBGuardApp theApp;