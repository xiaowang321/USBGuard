
// USBGuardDlg.h : ͷ�ļ�
//

#pragma once

#include "DeviceController.h"
#include "afxwin.h"


// CUSBGuardDlg �Ի���
class CUSBGuardDlg : public CDialogEx
{

private:
	DeviceController *device;
	CString sys_password;

// ����
public:
	CUSBGuardDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_USBGUARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
