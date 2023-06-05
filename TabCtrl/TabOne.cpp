// TabOne.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "TabOne.h"
#include "afxdialogex.h"
#include "FileOperation.h"


// CTabOne dialog

IMPLEMENT_DYNAMIC(CTabOne, CDialog)

CTabOne::CTabOne(CWnd* pParent /*=NULL*/)
	: CDialog(CTabOne::IDD, pParent)
	, m_folder1(_T(""))
	, m_folder2(_T(""))
	, m_folderPicker1{ this }
	, m_folderPicker2{ this }
	, m_png(FALSE)
	, m_jpg(FALSE)
	, m_txt(FALSE)
	, m_ctm(FALSE)
	, m_mrm(FALSE)
{

}



CTabOne::~CTabOne()
{
}

void CTabOne::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDER1, m_folderPicker1);
	DDX_Control(pDX, IDC_FOLDER2, m_folderPicker2);
	DDX_Text(pDX, IDC_FOLDER1, m_folder1);
	DDX_Text(pDX, IDC_FOLDER2, m_folder2);
	DDX_Check(pDX, IDC_CHECK1, m_png);
	DDX_Check(pDX, IDC_CHECK2, m_jpg);
	DDX_Check(pDX, IDC_CHECK3, m_txt);
	DDX_Check(pDX, IDC_CHECK4, m_ctm);
	DDX_Check(pDX, IDC_CHECK5, m_mrm);
}

 

BEGIN_MESSAGE_MAP(CTabOne, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CTabOne::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK3, &CTabOne::OnBnClickedCheck3)
END_MESSAGE_MAP()


// CTabOne message handlers


void CTabOne::OnBnClickedButton1()
{
	UpdateData();
	CFileOperation fo;
	fo.SetOverwriteMode(false);  //TRUE: Doesn't copy again, FALSE: Copy as copy of, copy (1) of...
	if (!fo.Copy(m_folder1, m_folder2, m_png, m_jpg, m_txt, m_ctm, m_mrm))
	{
		fo.ShowError();
	}
}


void CTabOne::OnBnClickedCheck3()
{
	// TODO: Add your control notification handler code here
}
