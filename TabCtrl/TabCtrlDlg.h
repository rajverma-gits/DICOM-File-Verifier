
// TabCtrlDlg.h : header file
//

#pragma once
#include "MyTabCtrl.h"

// CTabCtrlDlg dialog
class CTabCtrlDlg : public CDialogEx
{
// Construction
public:
	CTabCtrlDlg(CWnd* pParent = NULL);	// standard constructor
	CMyTabCtrl m_tabMyTabCtrl;
// Dialog Data
	enum { IDD = IDD_TABCTRL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
public:
	afx_msg void OnHelpMergecomtoolkit();
	CMenu menu;
	afx_msg void OnHelpAbout();
};
