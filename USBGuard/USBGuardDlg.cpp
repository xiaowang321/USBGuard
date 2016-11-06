
// USBGuardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "USBGuard.h"
#include "USBGuardDlg.h"
#include "DeviceController.h"
#include "USBController.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSBGuardDlg �Ի���



CUSBGuardDlg::CUSBGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUSBGuardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	device = NULL;
	sys_password = _T("123456");
}

void CUSBGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUSBGuardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONFIRM, &CUSBGuardDlg::OnBnClickedConfirm)
END_MESSAGE_MAP()


// CUSBGuardDlg ��Ϣ�������

BOOL CUSBGuardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	device = new USBController();

	if (!device->DeviceRegister(this->m_hWnd))
	{
		MessageBox(_T("Fatal Error:Device Notifcation Register Falied"));
	}

	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
#ifdef _DEBUG
	CString output;
	output.Format(_T("Ԥ�����Ϊ��%s"),sys_password);
	MessageBox(output, _T("USB�˿ڱ���������ʾ�ã�"), MB_OK | MB_ICONWARNING);
#endif
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUSBGuardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUSBGuardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUSBGuardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUSBGuardDlg::OnDestroy(){
	if (device != NULL){
		delete(device);
		device = NULL;
	}
	SendMessage(WM_CLOSE);
}

BOOL CUSBGuardDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	int state = device->DeviceDetect(nEventType, dwData);
	switch (state)//����豸״̬
	{
	case USBState::USB_OFFLINE:
		Refresh();
		break;
	case USBState::USB_DISABLE:
		device->DeviceEnable(FALSE);
		Refresh();
		break;
	case USBState::USB_ENABLE:
		Refresh(); 
		break;
	default:break;
	}
	return TRUE;
}

void CUSBGuardDlg::OnBnClickedConfirm()
{
	CString user_password;
	GetDlgItem(IDC_PASSWORD)->GetWindowText(user_password);
	if (user_password.IsEmpty()){//��������Ϊ��ʱ���˳�
		MessageBox(_T("�����Ϊ�գ�"),_T("USB�˿ڱ�������"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (user_password.Compare(sys_password)!=0){//�����������ʱ���˳�
		MessageBox(_T("�����ȷ�����������룡"), _T("USB�˿ڱ�������"), MB_OK | MB_ICONWARNING);
		GetDlgItem(IDC_PASSWORD)->SetWindowText(_T(""));
		return;
	}
	//������ȷʱ������USB�豸��
	GetDlgItem(IDC_PASSWORD)->SetWindowText(_T(""));
	device->DeviceEnable(TRUE);
	Refresh();
	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
	return;
}


void CUSBGuardDlg::Refresh()
{
	switch (device->DeviceState()){
	case USBState::USB_OFFLINE:
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("����"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
		Sleep(1000);
		break;
	case USBState::USB_DISABLE:
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("����"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(TRUE);
		Sleep(1000);
		break;
	case USBState::USB_ENABLE:
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("����"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
		Sleep(1000);
		break;
	default:break;
	}
	return;
}
