
// TabCtrl.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTabCtrlApp:
// See TabCtrl.cpp for the implementation of this class
//

class CTabCtrlApp : public CWinApp
{
public:
	CTabCtrlApp();

// Overrides
public:
	virtual BOOL InitInstance();
	bool initialised = false;
	int MyAppID;
	//static int MyAPPID;
	//static void initialiseMerge();
// Implementation

	DECLARE_MESSAGE_MAP()
};

//int CTabCtrlApp::MyAPPID = 0;
extern CTabCtrlApp theApp;