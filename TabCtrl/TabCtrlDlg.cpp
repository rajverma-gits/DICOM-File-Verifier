
// TabCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "TabCtrlDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "AboutAppDialog.h"
#include "mergecom.h"
#include "diction.h"
#include "mergecom.h"
#include "mc3media.h"
#include "mcstatus.h"
#include "mc3msg.h"
#include "mc3services.h"
#include "mc3items.h"
#include "pcd.h"
#include "pic.h"
#include "stdtypes.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <winbase.h>
#include <tchar.h>
#include <windows.h>
#include <string> 
#include "atlconv.h"
//#include "afxstr.h"
#include "atlstr.h"
#include "TabOne.h"
//#include <error.h>
#include <string.h>
#include <vector>
#include <locale>
#include <minwinbase.h>

#include <time.h>
#ifdef _WIN32
#include <fcntl.h>
#endif

/* DICOM VR Lengths */
#define AE_LENGTH 16
#define DA_LENGTH 10 
#define UI_LENGTH 64
#define SVC_LENGTH 130
#define STR_LENGTH 100
#define WORK_SIZE (64*1024)

#define TIME_OUT 30


#if defined(_WIN32)
#define BINARY_READ "rb"
#define BINARY_WRITE "wb"
#define BINARY_APPEND "rb+"
#define BINARY_READ_APPEND "a+b"
#define BINARY_CREATE "w+b"
#define TEXT_READ "r"
#define TEXT_WRITE "w"
#else
#define BINARY_READ "r"
#define BINARY_WRITE "w"
#define BINARY_APPEND "r+"
#define BINARY_READ_APPEND "a+"
#define BINARY_CREATE "w+"
#define TEXT_READ "r"
#define TEXT_WRITE "w"
#endif

using namespace std;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CTabCtrlDlg dialog



CTabCtrlDlg::CTabCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTabCtrlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTabCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabCtrlDlg)
	DDX_Control(pDX, IDC_TAB, m_tabMyTabCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTabCtrlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_HELP_MERGECOMTOOLKIT, &CTabCtrlDlg::OnHelpMergecomtoolkit)
	ON_COMMAND(ID_HELP_ABOUT, &CTabCtrlDlg::OnHelpAbout)
END_MESSAGE_MAP()

// CTabCtrlDlg message handlers

BOOL CTabCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);	   // Set small icon

	// TODO: Add extra initialization here
	m_tabMyTabCtrl.InsertItem(0, _T("Test Data Preparation"));
	m_tabMyTabCtrl.InsertItem(1, _T("DICOM Tags Verification"));

	m_tabMyTabCtrl.Init();

	// Enable drag and drop
	m_tabMyTabCtrl.ModifyStyleEx(0, WS_EX_ACCEPTFILES); // ABHI ADDED 
	DragAcceptFiles(); // ABHI ADDED
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


/*void CTabCtrlDlg::OnDropFiles(HDROP hDropInfo)
{
	// Get the number of dropped files
	UINT nFiles = DragQueryFile(hDropInfo, -1, NULL, 0);

	// Loop through each dropped file
	for (UINT i = 0; i < nFiles; i++)
	{
		// Get the path of the dropped file
		TCHAR szFilePath[MAX_PATH];
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		// TODO: Handle the dropped file
	}

	// Call the base class function
	CDialog::OnDropFiles(hDropInfo);
}*/



void CTabCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTabCtrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTabCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CTabCtrlDlg::OnHelpMergecomtoolkit()
{
	// TODO: Add your command handler code here
	ShellExecute(NULL, _T("open"), _T("https://www.ibm.com/watson-health/about/merge-healthcare"), NULL, NULL, SW_SHOWNORMAL);
}


void CTabCtrlDlg::OnHelpAbout()
{
	// TODO: Add your command handler code here
	CAboutAppDialog dlg(IDD_ABOUT, this);
	dlg.DoModal();
}
