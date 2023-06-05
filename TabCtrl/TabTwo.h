#pragma once
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "DicomFileEditBox.h"
#include "ListBoxDlg.h"
#include "afxcmn.h"


// CTabTwo dialog

class CTabTwo : public CDialog
{
	DECLARE_DYNAMIC(CTabTwo)

public:
	CTabTwo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabTwo();
	//virtual BOOL PreTranslateMessage(MSG* pMsg); //ABHI ADDED

// Dialog Data
	enum { IDD = IDD_TAB_TWO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	DicomFileEditBox m_dicomPicker;
	CString m_dicom;
	CListBox  m_control;
	CString m_ListTextCtrl;
	afx_msg void OnBnClickedButton2();
	CListBox m_Lcontrol2;
	afx_msg void OnEnChangeMfceditbrowse1();
};
