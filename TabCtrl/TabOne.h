#pragma once
#include "afxeditbrowsectrl.h"
#include "FolderPickerDlg.h" 

// CTabOne dialog

class CTabOne : public CDialog
{
	DECLARE_DYNAMIC(CTabOne)

public:
	CTabOne(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabOne();

// Dialog Data
	enum { IDD = IDD_TAB_ONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	FolderPickerDlg m_folderPicker1;
	FolderPickerDlg m_folderPicker2;
	CString m_folder1;
	CString m_folder2;
	afx_msg void OnBnClickedButton1();
	BOOL m_png;
	BOOL m_jpg;
	BOOL m_txt;
	afx_msg void OnBnClickedCheck3();
	BOOL m_ctm;
	BOOL m_mrm;
};
