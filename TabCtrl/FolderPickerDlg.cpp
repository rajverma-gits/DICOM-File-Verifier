// FolderPickerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "FolderPickerDlg.h"
#include "TabOne.h"


// FolderPickerDlg

IMPLEMENT_DYNAMIC(FolderPickerDlg, CMFCEditBrowseCtrl)

FolderPickerDlg::FolderPickerDlg(CTabOne* view) : m_view{ view }
{

}

FolderPickerDlg::~FolderPickerDlg()
{
}


BEGIN_MESSAGE_MAP(FolderPickerDlg, CMFCEditBrowseCtrl)
END_MESSAGE_MAP()

void FolderPickerDlg::OnBrowse()
{
	UpdateData();
	CFolderPickerDialog folder;
	if (folder.DoModal() == IDOK)
		this->SetWindowTextW(folder.GetFolderPath());
	UpdateData(false);
}


// FolderPickerDlg message handlers


