
// h2AuxToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "h2AuxTool.h"
#include "h2AuxToolDlg.h"
#include "afxdialogex.h"
#include "ProcessH2testw.h"
#include "Dbt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ch2AuxToolDlg 对话框

Ch2AuxToolDlg::Ch2AuxToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_H2AUXTOOL_DIALOG, pParent)
	, m_szValueUsbDrives(_T(""))
	, m_nCapacity(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ch2AuxToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LABEL_USB_DRIVES, m_szValueUsbDrives);
	DDX_Text(pDX, IDC_EDIT_CAPACITY, m_nCapacity);
	DDV_MinMaxInt(pDX, m_nCapacity, 0, INT_MAX);
	DDX_Control(pDX, IDC_EDIT_CAPACITY, m_ctrlCapacityEdit);
	DDX_Control(pDX, IDC_CHECK_TOP_SHOW, m_ctrlTopShow);
	DDX_Control(pDX, IDC_RADIO_ALL_CAPACITY, m_ctrlAllCapacity);
	DDX_Control(pDX, IDC_RADIO_PARTIAL_CAPACITY, m_ctrlPartialCapacity);
	DDX_Control(pDX, IDC_LIST_USB_DRIVES, m_ctrlUsbDrives);
}

BEGIN_MESSAGE_MAP(Ch2AuxToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BUTTON_RUN_H2, &Ch2AuxToolDlg::OnBnClickedButtonRunH2)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_H2, &Ch2AuxToolDlg::OnBnClickedButtonCloseH2)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_H2_FILE, &Ch2AuxToolDlg::OnBnClickedButtonDeleteH2File)
	ON_BN_CLICKED(IDC_CHECK_TOP_SHOW, &Ch2AuxToolDlg::OnBnClickedCheckTopShow)
	ON_EN_SETFOCUS(IDC_EDIT_CAPACITY, &Ch2AuxToolDlg::OnSetfocusEditCapacity)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_USB_DRIVES, &Ch2AuxToolDlg::OnClickListUsbDrives)
END_MESSAGE_MAP()


// Ch2AuxToolDlg 消息处理程序

BOOL Ch2AuxToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

//	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	m_nCapacity = 1024;
	UpdateData(FALSE);

	m_ctrlAllCapacity.SetCheck(TRUE);
	m_ctrlPartialCapacity.SetCheck(FALSE);

	m_imgList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 0);//第一个‘1’代表加载图标的个数，加载多个图标也可
	m_imgList.Add(AfxGetApp()->LoadIconW(IDI_ICON_REMOVABLE_DRIVE));

	DWORD dwStyle = m_ctrlUsbDrives.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	m_ctrlUsbDrives.SetExtendedStyle(dwStyle);
	//m_ctrlUsbDrives.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP  | LVS_EX_CHECKBOXES  | LVS_EX_SUBITEMIMAGES);
	m_ctrlUsbDrives.SetImageList(&m_imgList, LVSIL_SMALL);

	m_ctrlTopShow.SetCheck(TRUE);

	this->OnBnClickedCheckTopShow();

	ProcessDevChange();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ch2AuxToolDlg::OnPaint()
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
HCURSOR Ch2AuxToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

TCHAR Ch2AuxToolDlg::FirstDriveFromMask(ULONG unitmask)
{
	TCHAR i;

	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask >>= 1;
	}

	return (i + 'A');
}


void Ch2AuxToolDlg::ProcessDevChange()
{
	m_szValueUsbDrives = L"";
	DWORD dwDrives = GetLogicalDrives();
	DWORD dwMask = 1;

	m_ctrlUsbDrives.DeleteAllItems();

	for (int i = 0; i < 26; i++)
	{
		if (dwDrives & dwMask)
		{
			TCHAR driveLetter = 'A' + i;
			if (m_cProcessH2.IsValidUsbDrive(driveLetter))
			{
				CString szTmp;
				szTmp.Format(L"  %c:", driveLetter);
				m_szValueUsbDrives += szTmp;
				m_szValueUsbDrives += L"\\  ";
				UpdateData(FALSE);

				m_ctrlUsbDrives.InsertItem(m_ctrlUsbDrives.GetItemCount(), szTmp, 0);
			}
		}
		dwMask <<= 1;
	}
	
	for (int i = 0; i < m_ctrlUsbDrives.GetItemCount(); i++)
	{
		m_ctrlUsbDrives.SetCheck(i, TRUE);
	}
}

BOOL Ch2AuxToolDlg::OnDeviceChange(UINT wParam, DWORD_PTR lParam)
{
	//    DEV_BROADCAST_DEVICEINTERFACE * dbd = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;
	TCHAR driveLetter;
	PDEV_BROADCAST_HDR pHdr;
	PDEV_BROADCAST_VOLUME pDevVolume;
	CString szTotalCap, cstrFreeCap, szVolumeLabel, szFileSys;
	CString szVid, szPid, szSerial, szUsbVer, szUsbHubId, szUsbDeviceId;

	//这里进行信息匹配,比如guid等
	switch (wParam)
	{
	case DBT_DEVICEARRIVAL:
		//			PRINT_CONSOLE("@@@ arrival");
		pHdr = (PDEV_BROADCAST_HDR)lParam;
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			pDevVolume = (PDEV_BROADCAST_VOLUME)lParam;
			if (pDevVolume->dbcv_flags == 0x0)
			{
				driveLetter = (TCHAR)FirstDriveFromMask(pDevVolume->dbcv_unitmask);
				TRACE1("INSERT %c\r\n", driveLetter);
				ProcessDevChange();
			}
		}
		return TRUE;

	case DBT_DEVICEREMOVECOMPLETE:

		pHdr = (PDEV_BROADCAST_HDR)lParam;
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			pDevVolume = (PDEV_BROADCAST_VOLUME)lParam;
			if (pDevVolume->dbcv_flags == 0x0)
			{
				driveLetter = (TCHAR)FirstDriveFromMask(pDevVolume->dbcv_unitmask);
				TRACE1("REMOVE %c\r\n", driveLetter);
				ProcessDevChange();
			}
		}
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

void Ch2AuxToolDlg::OnBnClickedButtonRunH2()
{
	// TODO: 在此添加控件通知处理程序代码
	
	DWORD	dwMBytes;

	CString szH2Path = _T(".\\h2testw.exe");
	DWORD dwAttrib = GetFileAttributes(szH2Path);
	if (!((INVALID_FILE_ATTRIBUTES != dwAttrib)
		&& 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))) // NOT EXIST
	{
		AfxMessageBox(L"H2testw.exe does not exist");
		return;
	}

	((CButton*)GetDlgItem(IDC_BUTTON_RUN_H2))->EnableWindow(FALSE);
	m_cProcessH2.DeleteAllh2Files();

	if (m_ctrlPartialCapacity.GetCheck())
	{
		UpdateData(TRUE);
		dwMBytes = m_nCapacity;
	}
	else
	{
		dwMBytes = CProcessH2testw::MAX_NUM_OF_MBYTES;
	}

	for (int i = 0; i < m_ctrlUsbDrives.GetItemCount(); i++)
	{
		if (m_ctrlUsbDrives.GetCheck(i))
		{
			CString szDriveStr = m_ctrlUsbDrives.GetItemText(i, 0);//(行，列)
			szDriveStr = szDriveStr.Trim();
			m_cProcessH2.PerformH2RW(dwMBytes, szDriveStr.GetAt(0));
		}
	}

	((CButton*)GetDlgItem(IDC_BUTTON_RUN_H2))->EnableWindow(TRUE);
}


void Ch2AuxToolDlg::OnBnClickedButtonCloseH2()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_BUTTON_CLOSE_H2))->EnableWindow(FALSE);
	m_cProcessH2.DeleteAllh2Files();
	m_cProcessH2.CloseH2testw();
	((CButton*)GetDlgItem(IDC_BUTTON_CLOSE_H2))->EnableWindow(TRUE);
}


void Ch2AuxToolDlg::OnBnClickedButtonDeleteH2File()
{
	// TODO: 在此添加控件通知处理程序代码

	((CButton*)GetDlgItem(IDC_BUTTON_DELETE_H2_FILE))->EnableWindow(FALSE);
	m_cProcessH2.DeleteAllh2Files();
	((CButton*)GetDlgItem(IDC_BUTTON_DELETE_H2_FILE))->EnableWindow(TRUE);
}


void Ch2AuxToolDlg::OnBnClickedCheckTopShow()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_ctrlTopShow.GetCheck())
	{
		::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		::SetWindowPos(this->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}



void Ch2AuxToolDlg::OnSetfocusEditCapacity()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlPartialCapacity.SetCheck(TRUE);
	m_ctrlAllCapacity.SetCheck(FALSE);

	((CEdit*)GetDlgItem(IDC_EDIT_CAPACITY))->SetSel(0, -1); //EDIT控件中的内容全部选中
}


void Ch2AuxToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	m_imgList.Detach();
}


void Ch2AuxToolDlg::OnClickListUsbDrives(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;
	int nItemNo = pNMListView->iItem;
	if (-1 != nItemNo)
	{
		BOOL bChecked = m_ctrlUsbDrives.GetCheck(nItemNo);
		m_ctrlUsbDrives.SetCheck(nItemNo, !bChecked);
	}
}
