#pragma once


// CMyTabCtrl

class CMyTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl)

public:
	CMyTabCtrl();
	virtual ~CMyTabCtrl();
	CDialog *m_tabPages[3];
	int m_tabCurrent;
	int m_nNumberOfPages;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void Init();
	void SetRectangle();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void CMyTabCtrl::OnDropFiles(HDROP hDropInfo);
};

