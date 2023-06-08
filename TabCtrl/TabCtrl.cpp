
// TabCtrl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "TabCtrlDlg.h"
#include <winbase.h>
#include "afxdialogex.h"
#include <string>
#include <cctype>
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
#include "TabCtrlDlg.h"

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
typedef struct CALLBACKINFO
{
	FILE*   fp;
	char    fileName[512];
	/*
	* Note! The size of this buffer impacts toolkit performance.
	*       Higher values in general should result in increased performance of reading files
	*/
	size_t  bytesRead;
	size_t  bufferLength;

	char*   buffer;
} CBinfo;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTabCtrlApp

BEGIN_MESSAGE_MAP(CTabCtrlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTabCtrlApp construction

/*void CTabCtrlApp::initialiseMerge()
{
	MC_STATUS amergeStatus;
	CString a_csMergeFilePath = _T("C:\\Users\\z004r45v\\Documents\\RajVerma_Repository\\mc3_w32_5160_008-91204\\mc3apps\\merge.ini");


	CT2CA a_tempcsMergeIniFilePath(a_csMergeFilePath);
	amergeStatus = MC_Set_MergeINI(a_tempcsMergeIniFilePath.m_psz);

	char* mergeIniPath;
	size_t requiredSize;
	errno_t err = _dupenv_s(&mergeIniPath, &requiredSize, "MERGEINI");
	if (err != 0 || mergeIniPath == NULL) {
		printf("Error: Could not");
		return;
	}

	free(mergeIniPath);

	if (amergeStatus != MC_NORMAL_COMPLETION
		&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		// write error into log file
		printf("Merge initialisation failed");
		return;
	}

	amergeStatus = MC_Library_Initialization(NULL, NULL, NULL);

	if (amergeStatus != MC_NORMAL_COMPLETION
		&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return;
	}

	
		amergeStatus = MC_Register_Application(&MyAPPID, "MergeComApp");
		if (amergeStatus != MC_NORMAL_COMPLETION && amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
		{
			printf("\t%s\n", MC_Error_Message(amergeStatus));
			return;
		}
		
}*/

CTabCtrlApp::CTabCtrlApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTabCtrlApp object

CTabCtrlApp theApp;


// CTabCtrlApp initialization

BOOL CTabCtrlApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//initialiseMerge(); //SET OF MERGECOM 

	CTabCtrlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
