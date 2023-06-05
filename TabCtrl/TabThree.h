#pragma once
#include "afxcmn.h"


// CTabThree dialog

class CTabThree : public CDialog
{
	DECLARE_DYNAMIC(CTabThree)

public:
	CTabThree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabThree();

// Dialog Data
	enum { IDD = IDD_TAB_THREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
