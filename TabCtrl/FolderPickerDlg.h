#pragma once


// FolderPickerDlg

class CTabOne;

class FolderPickerDlg : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(FolderPickerDlg)

public:
	FolderPickerDlg(CTabOne* view);
	virtual ~FolderPickerDlg();

protected:
	CTabOne* m_view;
	void OnBrowse();
	DECLARE_MESSAGE_MAP()
};


 