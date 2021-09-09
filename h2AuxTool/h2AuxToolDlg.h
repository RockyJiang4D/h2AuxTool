
// h2AuxToolDlg.h: 头文件
//

#pragma once

#include "ProcessH2testw.h"

// Ch2AuxToolDlg 对话框
class Ch2AuxToolDlg : public CDialogEx
{
// 构造
public:
	Ch2AuxToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H2AUXTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnDeviceChange(UINT wParam, DWORD_PTR lParam);
	afx_msg void OnBnClickedButtonRunH2();
	afx_msg void OnBnClickedButtonCloseH2();
	afx_msg void OnBnClickedButtonDeleteH2File();
	afx_msg void OnBnClickedCheckTopShow();
	CString m_szValueUsbDrives;
	int m_nCapacity;
	CButton m_ctrlTopShow;
public:
	CButton m_ctrlAllCapacity;
	CButton m_ctrlPartialCapacity;
	CEdit	m_ctrlCapacityEdit;
private:
	CProcessH2testw  m_cProcessH2;
	TCHAR FirstDriveFromMask(ULONG unitmask);
	

	void ProcessDevChange();
public:
	afx_msg void OnSetfocusEditCapacity();
	CListCtrl m_ctrlUsbDrives;
	CImageList m_imgList;
	afx_msg void OnDestroy();
	afx_msg void OnClickListUsbDrives(NMHDR* pNMHDR, LRESULT* pResult);
};
