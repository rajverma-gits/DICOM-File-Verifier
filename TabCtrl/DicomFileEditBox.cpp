// DicomFileEditBox.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "DicomFileEditBox.h"
#include "TabTwo.h"


// DicomFileEditBox

IMPLEMENT_DYNAMIC(DicomFileEditBox, CMFCEditBrowseCtrl)

DicomFileEditBox::DicomFileEditBox()
{

}

DicomFileEditBox::~DicomFileEditBox()
{
}


BEGIN_MESSAGE_MAP(DicomFileEditBox, CMFCEditBrowseCtrl)
END_MESSAGE_MAP()

void DicomFileEditBox::OnBrowse()
{
	UpdateData();
	CFolderPickerDialog folder;
	if (folder.DoModal() == IDOK)
		this->SetWindowTextW(folder.GetFolderPath());
	UpdateData(false);
}


// DicomFileEditBox message handlers