#pragma once


// DicomFileEditBox

class DicomFileEditBox : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(DicomFileEditBox)

public:
	DicomFileEditBox();
	virtual ~DicomFileEditBox();

protected:
	void OnBrowse();
	DECLARE_MESSAGE_MAP()
};


