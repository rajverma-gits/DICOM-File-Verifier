#pragma once

#include "resource.h"

#define PATH_ERROR		   -1
#define PATH_NOT_FOUND		0
#define PATH_IS_FILE		1
#define PATH_IS_FOLDER		2


class CFExeption
{
public:
	CFExeption(DWORD dwErrCode);
	CFExeption(CString sErrText);
	CString GetErrorText() { return m_sError; }
	DWORD GetErrorCode() { return m_dwError; }

private:
	CString m_sError;
	DWORD m_dwError;
};


//*****************************************************************************************************

class CFileOperation
{
public:
	CFileOperation(); // constructor
	bool Copy(CString sSource, CString sDest, bool m_png, bool m_jpg, bool m_txt, bool m_ctm, bool m_mrm); // copy file or folder
	CString GetErrorString() { return m_sError; } // return error description
	DWORD GetErrorCode() { return m_dwError; } // return error code
	void ShowError() // show error message
	{
		MessageBox(NULL, m_sError, _T("Error"), MB_OK | MB_ICONERROR);
	}
	void SetAskIfReadOnly(bool bAsk = true) // sets behavior with readonly files(folders)
	{
		m_bAskIfReadOnly = bAsk;
	}
	bool IsAskIfReadOnly() // return current behavior with readonly files(folders)
	{
		return m_bAskIfReadOnly;
	}
	void SetOverwriteMode(bool bOverwrite = true) // sets overwrite mode on/off
	{
		m_bOverwriteMode = bOverwrite;
	}
	bool IsOverwriteMode() { return m_bOverwriteMode; } // return current overwrite mode
	int CheckPath(CString sPath);
	bool IsAborted() { return m_bAborted; }
	bool modalityCheck(CString sPath);

protected:
	void DoCopy(CString sSource, CString sDest);
	void DoFileCopy(CString sSourceFile, CString sDestFile);
	void CheckAndCopy(CString sSourceFolder, CString sDestFolder);
	void DoFolderCopy(CString sSourceFolder, CString sDestFolder);
	bool FolderExists(CString sSourceFolder);
	bool IsFileExist(CString sPathName);
	void PreparePath(CString &sPath);
	void Initialize();
	void CheckSelfRecursion(CString sSource, CString sDest);
	bool CheckSelfCopy(CString sSource, CString sDest);
	CString ChangeFileName(CString sFileName);
	CString ParseFolderName(CString sPathName);

private:
	CString m_sError;
	DWORD m_dwError;
	bool m_bAskIfReadOnly;
	bool m_bOverwriteMode;
	bool m_bAborted;
	int m_iRecursionLimit;
	bool png, jpg, txt, ctm, mrm;
};