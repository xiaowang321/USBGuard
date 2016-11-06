
// USBGuardDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CUSBGuardDlg 对话框



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


// CUSBGuardDlg 消息处理程序

BOOL CUSBGuardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	device = new USBController();

	if (!device->DeviceRegister(this->m_hWnd))
	{
		MessageBox(_T("Fatal Error:Device Notifcation Register Falied"));
	}

	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
#ifdef _DEBUG
	CString output;
	output.Format(_T("预设口令为：%s"),sys_password);
	MessageBox(output, _T("USB端口保护程序（演示用）"), MB_OK | MB_ICONWARNING);
#endif
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBGuardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	switch (state)//检测设备状态
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
	if (user_password.IsEmpty()){//口令输入为空时，退出
		MessageBox(_T("口令不得为空！"),_T("USB端口保护程序"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (user_password.Compare(sys_password)!=0){//口令输入错误时，退出
		MessageBox(_T("口令不正确！请重新输入！"), _T("USB端口保护程序"), MB_OK | MB_ICONWARNING);
		GetDlgItem(IDC_PASSWORD)->SetWindowText(_T(""));
		return;
	}
	//口令正确时，启用USB设备。
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
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("离线"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
		Sleep(1000);
		break;
	case USBState::USB_DISABLE:
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("禁用"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(TRUE);
		Sleep(1000);
		break;
	case USBState::USB_ENABLE:
		GetDlgItem(IDC_TIPS)->SetWindowText(_T("启用"));
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
		Sleep(1000);
		break;
	default:break;
	}
	return;
}
