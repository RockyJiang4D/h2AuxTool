#include "ProcessH2testw.h"
#include "winioctl.h" 


CProcessH2testw::CProcessH2testw(void)
{
	m_nRunh2Count = 0;
	m_hwnd = NULL;
}

CProcessH2testw::~CProcessH2testw()
{

}

void CProcessH2testw::PerformH2RW(DWORD dwMBytes, TCHAR driveLetter)
{
	CString szTotalCapacity;
	DWORD dwRwCapacity;
	CString szFileSystem;

	if (IsValidUsbDrive(driveLetter))
	{
		dwRwCapacity = CalcuteRwCapacity(driveLetter, dwMBytes, szTotalCapacity);
		szFileSystem = GetFileSystem(driveLetter);
		StartH2testw(driveLetter, szFileSystem, szTotalCapacity, dwRwCapacity);
		m_nRunh2Count++;
	}
}

void CProcessH2testw::PerformH2RW(DWORD dwMBytes)
{
	CString szTotalCapacity;
	DWORD dwRwCapacity;
	CString szFileSystem;
	TCHAR driveLetter;

	DWORD dwDrives = GetLogicalDrives();
	DWORD dwMask = 1;

	for (int i = 0;i < 26;i++)
	{
		if (dwDrives & dwMask)
		{
			driveLetter = 'A' + i;

			if (IsValidUsbDrive(driveLetter))
			{
				dwRwCapacity = CalcuteRwCapacity(driveLetter, dwMBytes, szTotalCapacity);
				szFileSystem = GetFileSystem(driveLetter);
				StartH2testw(driveLetter, szFileSystem, szTotalCapacity, dwRwCapacity);
				m_nRunh2Count++;
			}
		}
		dwMask <<= 1;
	}
}

CString CProcessH2testw::GetFileSystem(TCHAR driveLetter)
{
	WCHAR lpVolumeNameBuffer[_MAX_FNAME];
	DWORD nVolumeNameSize = _MAX_FNAME;
	DWORD nMaximumComponentLength;
	DWORD nFileSystemFlags;
	DWORD nFileSystemNameSize = 20;
	WCHAR lpFileSystemNameBuffer[20];
	DWORD nVolumeSerialNumber = 0;//便于驱动器无效时做判断
	CString strRootPathName;

	strRootPathName.Format(_T("%c:\\"), driveLetter);
	LPCTSTR lpRootPathName = strRootPathName;

	if (!GetVolumeInformation(
		lpRootPathName,
		lpVolumeNameBuffer,
		nVolumeNameSize,
		&nVolumeSerialNumber,
		&nMaximumComponentLength,
		&nFileSystemFlags,
		lpFileSystemNameBuffer,
		nFileSystemNameSize))
	{
		//AfxMessageBox(L"GetVolumeInformation error");
		return L"";
	}
	//AfxMessageBox(lpFileSystemNameBuffer);

	CString szFileSystem = lpFileSystemNameBuffer;
	return szFileSystem;
}

DWORD CProcessH2testw::CalcuteRwCapacity(TCHAR driveLetter, DWORD dwMBytes, CString &szTotalCapacity)
{
	_ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;

	DWORD	dwFreeSizeMBytes;
	__int64 iVolumeSize = 0, iVolumeFreeSize = 0;
	CString strRootPathName;

	strRootPathName.Format(_T("%c:\\"), driveLetter);

	if (!GetDiskFreeSpaceEx(strRootPathName.Mid(0, 2), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
	{
		return 0;
	}
	iVolumeSize = (TotalNumberOfBytes.QuadPart >> 20);/// 1024 / 1024;
	iVolumeFreeSize = (FreeBytesAvailable.QuadPart >> 20);// / 1024 / 1024;
	if (iVolumeSize >= (1 << 20)) // > 1TB
	{
		szTotalCapacity.Format(_T(" - %.2fTB"), (float)((float)iVolumeSize / 1024 / 1024));
	}
	else if (iVolumeSize >= (1 << 10)) // > 1GB
	{
		szTotalCapacity.Format(_T(" - %.2fGB"), (float)((float)iVolumeSize / 1024));
	}
	else
	{
		szTotalCapacity.Format(_T(" - %.2fMB"), (float)((float)iVolumeSize));
	}

	dwFreeSizeMBytes = (DWORD)(FreeBytesAvailable.QuadPart >> 20);// / 1024 / 1024;

	if ((MAX_NUM_OF_MBYTES != dwMBytes) && (dwMBytes > dwFreeSizeMBytes))
	{
		dwMBytes = dwFreeSizeMBytes;
	}

	return dwMBytes;
}

void CProcessH2testw::CloseH2testw()
{
	EnumWindows((WNDENUMPROC)EnumWindowsCB_CloseH2, 0);
}

HWND	CProcessH2testw::FindH2WindowEx(CString szTitleEn, CString szTitleCn)
{
	ENUM_FIND_WIN_PARAM enumFindWinParam;

	enumFindWinParam.pThis = this;
	enumFindWinParam.szTitleCn = szTitleCn;
	enumFindWinParam.szTitleEn = szTitleEn;

	m_hwnd = NULL;
	if (!EnumWindows((WNDENUMPROC)EnumWindowsCB_FindWin, (LPARAM)&enumFindWinParam))
	{

	}
	return m_hwnd;
}

HWND	CProcessH2testw::FindH2Window(CString szTitleEn, CString szTitleCn)
{
	HWND	hWnd = NULL;
	int	i;

	for (i = 0; i < MAX_SEARCH_NUM; i++)
	{

		hWnd = ::FindWindow(NULL, szTitleEn);

		if (NULL != hWnd)
		{
			return hWnd;
		}

		hWnd = ::FindWindow(NULL, szTitleCn);

		if (NULL != hWnd)
		{
			return hWnd;
		}

		Sleep(50);
	}

	return NULL;

}

HWND	CProcessH2testw::FindH2SubWindow(HWND parentHWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn)
{
	int	i;
	HWND hChildWnd = NULL;

	for (i = 0; i < MAX_SEARCH_NUM; i++)
	{
#if 0
		hChildWnd = ::FindWindowEx(parentHWnd, hChildWnd, szClassName, szCaption);
#else
		hChildWnd = FindSubWindow(parentHWnd, szClassName, szCaptionEn, szCaptionCn);
#endif
		if (NULL != hChildWnd)
		{
			return hChildWnd;
		}
		Sleep(50);
	}

	return NULL;

}

BOOL	CProcessH2testw::SendMsgToWin(HWND parentHWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn, UINT msg, WPARAM wParam, LPARAM lParam, BOOL	bPostMsg)
{
	CWnd* pChildWnd = NULL;
	HWND	hSubWnd;
	hSubWnd = FindH2SubWindow(parentHWnd, szClassName, szCaptionEn, szCaptionCn);

	if (NULL == hSubWnd)
	{
		return FALSE;
	}
	WaitWinReady(hSubWnd);
	pChildWnd = CWnd::FromHandle(hSubWnd);

	if (bPostMsg)
	{
		pChildWnd->PostMessage(msg, wParam, lParam);
	}
	else
	{
		pChildWnd->SendMessage(msg, wParam, lParam);
	}
	return TRUE;
}

HWND CProcessH2testw::FindSubWindow(HWND ParentWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn)
{
	HWND hChild = ::GetWindow(ParentWnd, GW_CHILD);

	TCHAR szWinClassName[100];
	TCHAR szWinText[100];

	while (hChild != NULL)
	{
		szWinClassName[0] = '\0';
		szWinText[0] = '\0';
		::GetClassName(hChild, szWinClassName, sizeof(szWinClassName) / sizeof(TCHAR));
		::GetWindowText(hChild, szWinText, sizeof(szWinText) / sizeof(TCHAR));
		if ((_tcscmp(szWinClassName, szClassName) == 0)
			&& ((NULL != (_tcsstr(szWinText, szCaptionEn))
				|| (NULL != (_tcsstr(szWinText, szCaptionCn))))))
		{
//			OutputDebugString(szWinText);
			return hChild;
		}
		else
		{
			hChild = ::GetWindow(hChild, GW_HWNDNEXT);
		}
	}
	return NULL;
}

void CProcessH2testw::WaitWinReady(HWND hWnd)
{
	int	i;
	for (i = 0; i < MAX_SEARCH_NUM; i++)
	{
		if (::IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
		{
			break;
		}
		Sleep(50);
	}
}


BOOL	CProcessH2testw::StartH2testw(TCHAR driveLetter, CString szFileSystem, CString szTotalCapacity, DWORD dwNumOfMBytes)
{
	CString szH2MainWinTitle;
	CString szH2BrowseWinTitle;
	HWND	hH2Wnd;
	HWND	hProgressWnd;
	HWND	hBrowseWnd;
	STARTUPINFO				si;
	PROCESS_INFORMATION		pi;
	CWnd* pChildWnd = NULL;
	TCHAR	szText[255] = L"M:";
	//	TCHAR	szTmp[255] = L"\0";


	szH2MainWinTitle.Format(L"H2testw - %c: - ", driveLetter);
	szH2MainWinTitle += szFileSystem;
	szH2MainWinTitle += szTotalCapacity;

	hH2Wnd = FindH2Window(szH2MainWinTitle, szH2MainWinTitle);

	if (NULL != hH2Wnd)
	{
		//		AfxMessageBox(L"H2 is running");
		return FALSE;
	}

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	GetStartupInfo(&si);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	CString szCmd = L".\\h2testw.exe";
	USES_CONVERSION;
	BOOL	bRet = TRUE;
	do
	{
		if (!CreateProcess(NULL, //No module name (use command line).
			(LPWSTR)(LPCWSTR)(szCmd),			//Command line.     
			NULL,				//   Process   handle   not   inheritable.     
			NULL,				//   Thread   handle   not   inheritable.     
			TRUE,				//   Set   handle   inheritance   to   TRUE.     
			0,					//   No   creation   flags.     
			NULL,				//   Use   parent's   environment   block.     
			NULL,				//   Use   parent's   starting   directory.     
			&si,				//   Pointer   to   STARTUPINFO   structure.   
			&pi)				//   Pointer   to   PROCESS_INFORMATION   structure.   
			)
		{
			OutputDebugString(L"Fail to create H2testw procress!\r\n");
			bRet = FALSE;
			break;
		}

		hH2Wnd = FindH2Window(L"H2testw", L"h2testw");

		if (NULL == hH2Wnd)
		{
			//			AfxMessageBox(L"H2 is not running");
			bRet = FALSE;
			break;
		}

		::SetForegroundWindow(hH2Wnd);
		::SetWindowText(hH2Wnd, szH2MainWinTitle);
		RECT rect = { 0 };
		GetWindowRect(hH2Wnd, &rect);
		int		iWinWidth = rect.right - rect.left;
		int		iWinHeight = rect.bottom - rect.top;
		int 	iScrWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
		int 	iScrHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率
		int		iRowCount = iScrHeight / (iWinHeight + 100);
		int 	iColCount = iScrWidth / (iWinWidth + 20);
		MoveWindow(hH2Wnd, (m_nRunh2Count % iColCount) * (iWinWidth + 20), ((m_nRunh2Count % (iColCount * iRowCount)) / iColCount) * (iWinHeight + 100), iWinWidth, iWinHeight, TRUE);
		WaitWinReady(hH2Wnd);
		//		Sleep(500);


		BlockInput(TRUE);

		SendMsgToWin(hH2Wnd, L"Button", L"English", L"简体中文", BM_CLICK);
		//		Sleep(500);

		SendMsgToWin(hH2Wnd, L"Button", L"Select target", L"选择对象", BM_CLICK);

		CString szBrowseWinTitle = L"浏览文件夹";
		hBrowseWnd = FindH2Window(szBrowseWinTitle, szBrowseWinTitle);
		if (NULL == hBrowseWnd)
		{
			bRet = FALSE;
			break;
		}

		szBrowseWinTitle.Format(L"浏览文件夹 - %c: - ", driveLetter);
		szBrowseWinTitle += szFileSystem;
		szBrowseWinTitle += szTotalCapacity;
		::SetWindowText(hBrowseWnd, szBrowseWinTitle);
		WaitWinReady(hBrowseWnd);
		//		Sleep(1000);


		szText[0] = driveLetter;

		SendMsgToWin(hBrowseWnd, L"Edit", L"", L"", WM_SETTEXT, (WPARAM)(0), (LPARAM)(LPCTSTR)szText, FALSE);

		SendMsgToWin(hBrowseWnd, L"Button", L"OK", L"确定", BM_CLICK);
		//		Sleep(1000);

		WaitWinReady(hH2Wnd);
		/*
				CString szTmp;
				szTmp.Format(L"%s", szText);

				hH2SubWnd = FindH2SubWindow(hH2Wnd, L"Static", szTmp);
				if (NULL == hH2SubWnd)
				{
					bRet = FALSE;
					break;
				}
		*/
		if (MAX_NUM_OF_MBYTES == dwNumOfMBytes)
		{
			SendMsgToWin(hH2Wnd, L"Button", L"all", L"所有可用", BM_CLICK);
		}
		else
		{
			SendMsgToWin(hH2Wnd, L"Button", L"only", L"仅", BM_CLICK);

			CString szMBytes;
			szMBytes.Format(L"%d", dwNumOfMBytes);
			//			Sleep(200);
			SendMsgToWin(hH2Wnd, L"Edit", L"", L"", WM_SETTEXT, (WPARAM)(0), (LPARAM)(LPCTSTR)szMBytes, FALSE);
		}

		SendMsgToWin(hH2Wnd, L"Button", L"Write + Verify", L"写入 + 校验", BM_CLICK);
		//		Sleep(300);

		hH2Wnd = FindH2Window(L"H2testw", L"h2testw");
		if (NULL != hH2Wnd)
		{
			WaitWinReady(hH2Wnd);
			if (NULL != FindSubWindow(hH2Wnd, L"Static", L"Attention:", L"注意:"))
			{
				SendMsgToWin(hH2Wnd, L"Button", L"OK", L"确定", BM_CLICK);
			}
		}

		CString szProgressWinTitle;// = L"H2testw | Progress";
#if 1
		hProgressWnd = FindH2Window(L"H2testw | Progress", L"H2testw | 进度      ");
#else
		hProgressWnd = FindH2WindowEx(L"H2testw | Progress", L"H2testw | 进度");
#endif
		if (NULL == hProgressWnd)
		{
			bRet = FALSE;
			break;
		}

		TCHAR szWinText[100];
		::GetWindowText(hProgressWnd, szWinText, sizeof(szWinText) / sizeof(TCHAR));
		szProgressWinTitle = szWinText;

		CString szTemp;
		szTemp.Format(L" - %c: - ", driveLetter);
		szTemp += szFileSystem;
		szProgressWinTitle += szTemp;
		szProgressWinTitle += szTotalCapacity;
		::SetWindowText(hProgressWnd, szProgressWinTitle);
		WaitWinReady(hProgressWnd);

	} while (0);
	BlockInput(FALSE);
	//	WaitForSingleObject(pi.hProcess, INFINITE);	 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//	AfxMessageBox(szH2MainWinTitle);
	return bRet;

}

BOOL CProcessH2testw::GetDriveTypeByBus(TCHAR driveLetter, WORD* type)
{
	// TODO: 在此处添加实现代码.
	HANDLE hDevice;               // handle to the drive to be examined
	BOOL result;                 // results flag
	DWORD readed;                   // discard results

	STORAGE_DESCRIPTOR_HEADER* pDevDescHeader;
	STORAGE_DEVICE_DESCRIPTOR* pDevDesc;
	DWORD devDescLength;
	STORAGE_PROPERTY_QUERY query;
	CString driveStr;

	driveStr.Format(_T("\\\\.\\%c:"), driveLetter);

	hDevice = CreateFile(
		driveStr, // drive to open
		GENERIC_READ | GENERIC_WRITE,     // access to the drive
		FILE_SHARE_READ | FILE_SHARE_WRITE, //share mode
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL            // do not copy file attribute
	);
	if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		printf("CreateFile() Error: %ld\r\n", GetLastError());
		return FALSE;
	}

	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	pDevDescHeader = (STORAGE_DESCRIPTOR_HEADER*)malloc(sizeof(STORAGE_DESCRIPTOR_HEADER));
	if (NULL == pDevDescHeader)
	{
		(void)CloseHandle(hDevice);
		return FALSE;
	}

	result = DeviceIoControl(
		hDevice,     // device to be queried
		IOCTL_STORAGE_QUERY_PROPERTY,     // operation to perform
		&query,
		sizeof query,               // no input buffer
		pDevDescHeader,
		sizeof(STORAGE_DESCRIPTOR_HEADER),     // output buffer
		&readed,                 // # bytes returned
		NULL);      // synchronous I/O
	if (!result)        //fail
	{
		printf("IOCTL_STORAGE_QUERY_PROPERTY Error: %ld", GetLastError());
		free(pDevDescHeader);
		(void)CloseHandle(hDevice);
		return FALSE;
	}

	devDescLength = pDevDescHeader->Size;
	pDevDesc = (STORAGE_DEVICE_DESCRIPTOR*)malloc(devDescLength);
	if (NULL == pDevDesc)
	{
		free(pDevDescHeader);
		(void)CloseHandle(hDevice);
		return FALSE;
	}

	result = DeviceIoControl(
		hDevice,     // device to be queried
		IOCTL_STORAGE_QUERY_PROPERTY,     // operation to perform
		&query,
		sizeof query,               // no input buffer
		pDevDesc,
		devDescLength,     // output buffer
		&readed,                 // # bytes returned
		NULL);      // synchronous I/O
	if (!result)        //fail
	{
		printf("IOCTL_STORAGE_QUERY_PROPERTY Error: %ld", GetLastError());
		free(pDevDescHeader);
		free(pDevDesc);
		(void)CloseHandle(hDevice);
		return FALSE;
	}

	//printf("%d\n", pDevDesc->BusType);
	*type = (WORD)pDevDesc->BusType;
	free(pDevDescHeader);
	free(pDevDesc);

	(void)CloseHandle(hDevice);
	return TRUE;
}


BOOL  CALLBACK CProcessH2testw::EnumWindowsCB_CloseH2(HWND hwnd, DWORD lParam)
{
	// TODO: 在此处添加实现代码.
	// 窗口是否可视
	if (!::IsWindowVisible(hwnd))
	{
		return TRUE;
	}

	// 窗口是否可激活
	if (!::IsWindowEnabled(hwnd))
	{
		return TRUE;
	}

	// Do something
	TCHAR szTitle[255];
	//setLocale(LC_ALL, L"");
	::GetWindowText(hwnd, (LPWSTR)szTitle, 255);

	CString szTemp;
	szTemp.Format(L"%s", szTitle);

	CWnd* pWnd = NULL;

	if (szTemp.Find(L"H2testw") >= 0)
	{
//		OutputDebugString(szTitle);
		TRACE("\r\n");
		pWnd = CWnd::FromHandle(hwnd);
		pWnd->SendMessage(WM_CLOSE);
		Sleep(100);
	}
	return TRUE;
}


BOOL  CALLBACK CProcessH2testw::EnumWindowsCB_FindWin(HWND hwnd, DWORD lParam)
{
	if (NULL == lParam)
	{
		return TRUE;
	}
	CString szTitleEn, szTitleCn;
	CProcessH2testw* pCProcessH2testw;
	szTitleCn = ((ENUM_FIND_WIN_PARAM*)lParam)->szTitleCn;
	szTitleEn = ((ENUM_FIND_WIN_PARAM*)lParam)->szTitleEn;
	pCProcessH2testw = ((ENUM_FIND_WIN_PARAM*)lParam)->pThis;
	
	// 窗口是否可视
	if (!::IsWindowVisible(hwnd))
	{
		return TRUE;
	}

	// 窗口是否可激活
	if (!::IsWindowEnabled(hwnd))
	{
		return TRUE;
	}

	// Do something
	TCHAR szTemp[255];
	//setLocale(LC_ALL, L"");
	::GetWindowText(hwnd, (LPWSTR)szTemp, 255);

	CString szTitle;
	szTitle.Format(L"%s", szTemp);

	szTitle = szTitle.Trim();
	szTitleCn = szTitleCn.Trim();
	szTitleEn = szTitleEn.Trim();

	OutputDebugString(L"#");
	OutputDebugString(szTitle);
	OutputDebugString(L"# ");
	OutputDebugString(L"#");
	OutputDebugString(szTitleCn);
	OutputDebugString(L"# ");
	OutputDebugString(L"#");
	OutputDebugString(szTitleEn);
	OutputDebugString(L"#");
	OutputDebugString(L"\r\n");

	if ((szTitle.Find(szTitleCn) > 0) 
		|| (szTitle.Find(szTitleEn) > 0))
	{
		pCProcessH2testw->m_hwnd = hwnd;
		return FALSE;
	}
	
	return TRUE;
}


void CProcessH2testw::DeleteAllh2Files()
{
	// TODO: 在此处添加实现代码.
	DWORD dwDrives = GetLogicalDrives();
	DWORD dwMask = 1;
	CString 	strMatch;
	CFileFind	finder;
	CFile		tempFile;
	CString		szFilePath;

	for (int i = 0;i < 26;i++)
	{
		if (dwDrives & dwMask)
		{
			TCHAR driveLetter = 'A' + i;

			if (IsValidUsbDrive(driveLetter))
			{
				strMatch.Format(L"%c:\\*.h2w", driveLetter);
				BOOL bWorking = finder.FindFile(strMatch);

				while (bWorking)
				{
					bWorking = finder.FindNextFile();
					szFilePath = finder.GetFilePath();

					tempFile.Remove(szFilePath);
				}
			}
		}
		dwMask <<= 1;
	}
}


BOOL CProcessH2testw::IsValidUsbDrive(TCHAR driveLetter)
{
	// TODO: 在此处添加实现代码.
	WCHAR lpVolumeNameBuffer[_MAX_FNAME];
	DWORD nVolumeNameSize = _MAX_FNAME;
	DWORD nMaximumComponentLength;
	DWORD nFileSystemFlags;
	DWORD nFileSystemNameSize = 20;
	WCHAR lpFileSystemNameBuffer[20];
	DWORD nVolumeSerialNumber = 0;//便于驱动器无效时做判断
	CString strRootPathName;
	WORD	busType;
	CString driveCstr;

	driveLetter &= ~0x20; // uppercase

	if ((driveLetter < 'A') || (driveLetter > 'Z'))
	{
		return FALSE;
	}
	driveCstr.Format(_T("%c:\\"), driveLetter);

	if ((GetDriveType(driveCstr) != DRIVE_REMOVABLE)
		&& (GetDriveType(driveCstr) != DRIVE_FIXED))
	{
		return FALSE;
	}

	if (!GetDriveTypeByBus(driveLetter, &busType))
	{
		return FALSE;
	}
	if (busType != 7) // usb bus drive
	{
		return FALSE;
	}

	strRootPathName.Format(_T("%c:\\"), driveLetter);

	LPCTSTR lpRootPathName = strRootPathName;

	if (!GetVolumeInformation(
		lpRootPathName,
		lpVolumeNameBuffer,
		nVolumeNameSize,
		&nVolumeSerialNumber,
		&nMaximumComponentLength,
		&nFileSystemFlags,
		lpFileSystemNameBuffer,
		nFileSystemNameSize))
	{
		//AfxMessageBox(L"GetVolumeInformation error");
		return FALSE;
	}
	return TRUE;
}


