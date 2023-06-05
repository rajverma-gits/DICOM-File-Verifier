// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h" 
#include "MyTabCtrl.h"

#include "TabOne.h"
#include "TabTwo.h"
#include "TabThree.h"
// CMyTabCtrl

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)

CMyTabCtrl::CMyTabCtrl()
{
	m_tabPages[0] = new CTabOne;
	m_tabPages[1] = new CTabTwo;

	m_nNumberOfPages = 2;
}

CMyTabCtrl::~CMyTabCtrl()
{
	for (int nCount = 0; nCount < m_nNumberOfPages; nCount++) {
		delete m_tabPages[nCount];
	}
}

//Added
/*class CTabTwo : public CDialogEx
{
	// ...
private:
	CBitmap m_dragBitmap;
};*/
//Till here

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMyTabCtrl message handlers




void CMyTabCtrl::Init()
{
	m_tabCurrent = 0;
	
	m_tabPages[0]->Create(IDD_TAB_ONE, this);
	m_tabPages[1]->Create(IDD_TAB_TWO, this);

	m_tabPages[0]->ShowWindow(SW_SHOW);
	m_tabPages[1]->ShowWindow(SW_HIDE);

//	m_tabPages[1]->DragAcceptFiles(TRUE);

	SetRectangle();
}


void CMyTabCtrl::SetRectangle() 
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX = itemRect.left;
	nY = itemRect.bottom + 1;
	nXc = tabRect.right - itemRect.left - 1;
	nYc = tabRect.bottom - nY - 1;

	m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for (int nCount = 1; nCount < m_nNumberOfPages; nCount++) {
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void CMyTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTabCtrl::OnLButtonDown(nFlags, point);
	//Counting c++ programs in a specifiedd manner
	if (m_tabCurrent != GetCurFocus()) {
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent = GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
	}
}

