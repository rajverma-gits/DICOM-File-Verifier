#include "stdafx.h"
#include "AboutAppDialog.h"
#include "Resource.h"


CAboutAppDialog::CAboutAppDialog(UINT nIDTemplate, CWnd* pParent /*=nullptr*/) : CDialog(nIDTemplate, pParent)
{
}


CAboutAppDialog::~CAboutAppDialog()
{
}

BOOL CAboutAppDialog::OnInitDialog()
{
	if (!CDialog::OnInitDialog())
		return FALSE;

	// Perform any initialization tasks for the dialog
	m_staticLink.SetWindowText(_T("GitHub."));
	CFont* pFont = m_staticLink.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfUnderline = TRUE;  // Underline the text
	lf.lfWeight = FW_BOLD;  // Set font weight to bold
	m_fontLink.CreateFontIndirect(&lf);
	m_staticLink.SetFont(&m_fontLink);
	
	m_staticLink.ModifyStyle(0, SS_NOTIFY);  // Enable mouse click notifications
	m_staticLink.SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	return TRUE;
}

void CAboutAppDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// Clean up the font resource
	m_fontLink.DeleteObject();
}

void CAboutAppDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// ...
	DDX_Control(pDX, IDC_STATIC4, m_staticLink);
}

BEGIN_MESSAGE_MAP(CAboutAppDialog, CDialog)
	//ON_BN_CLICKED(IDC_BUTTON, OnButtonClicked)
	// Add message map entries for any additional controls
	ON_STN_CLICKED(IDC_STATIC4, OnLinkClicked)
END_MESSAGE_MAP()

void CAboutAppDialog::OnLinkClicked()
{
	ShellExecute(NULL, _T("open"), _T("https://www.google.com/"), NULL, NULL, SW_SHOWNORMAL);
}