#pragma once

#include "targetver.h"
#include "afxwin.h"

class CProcessH2testw
{
public:
	enum
	{
		MAX_SEARCH_NUM = 50,
		MAX_NUM_OF_MBYTES = 0xFFFFFFFF,
	};

	CProcessH2testw(void);
	~CProcessH2testw();
	void DeleteAllh2Files();
	void PerformH2RW(DWORD dwMBytes);
	void CloseH2testw();
	BOOL IsValidUsbDrive(TCHAR driveLetter);
protected:
	static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, DWORD lParam);
private:
	UINT	m_nRunh2Count;

	DWORD CalcuteRwCapacity(TCHAR driveLetter, DWORD dwMBytes, CString& szTotalCapacity);
	CString GetFileSystem(TCHAR driveLetter);
	BOOL StartH2testw(TCHAR driveLetter, CString szFileSystem, CString szTotalCap, DWORD dwNumOfMBytes);
	BOOL GetDriveTypeByBus(TCHAR driveLetter, WORD* type);
	
	void WaitWinReady(HWND hWnd);
	HWND FindH2Window(CString szTitleEn, CString szTitleCn);
	HWND FindH2SubWindow(HWND parentHWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn);
	HWND FindSubWindow(HWND ParentWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn);
	BOOL SendMsgToWin(HWND parentHWnd, CString szClassName, CString szCaptionEn, CString szCaptionCn, UINT msg, WPARAM wParam = NULL, LPARAM lParam = NULL, BOOL	bPostMsg = TRUE);
};



