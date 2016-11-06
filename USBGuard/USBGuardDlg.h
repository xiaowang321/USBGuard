
// USBGuardDlg.h : 头文件
//

#pragma once

#include "DeviceController.h"
#include "afxwin.h"


// CUSBGuardDlg 对话框
class CUSBGuardDlg : public CDialogEx
{

private:
	DeviceController *device;
	CString sys_password;

// 构造
public:
	CUSBGuardDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBGUARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg BOOL OnDeviceChange(UINT nEventType,DWORD_PTR dwData);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConfirm();
	afx_msg void Refresh();
};
