#pragma once
#include "afxwin.h"
class CAboutAppDialog :
	public CDialog
{
public:
	CAboutAppDialog(UINT nIDTemplate, CWnd* pParent = nullptr);
	~CAboutAppDialog();
	//afx_msg void OnLinkClicked();
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnButtonClicked();
	afx_msg void OnLinkClicked();
	afx_msg void OnDestroy();
	CStatic m_staticLink;
	CFont m_fontLink;
private:
	// Add any private member variables as needed

	DECLARE_MESSAGE_MAP();
};

