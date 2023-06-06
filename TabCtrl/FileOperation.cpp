#include "stdafx.h"
#include "resource.h" 
#include "FileOperation.h"
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

typedef struct study_node
{
	char     pat_id[UI_LENGTH + 1];
	char     pat_name[UI_LENGTH + 1];
	char     pat_bday[DA_LENGTH + 1];
	char     pat_sex[AE_LENGTH + 1];

	char     stu_inst[UI_LENGTH + 1];
	char     stu_date[DA_LENGTH + 1];
	char     stu_time[AE_LENGTH + 1];
	char     stu_anum[AE_LENGTH + 1];
	char     stu_id[AE_LENGTH + 1];
	char     stu_desc[UI_LENGTH + 1];
	char     stu_physname[UI_LENGTH + 1];

} StudyNode;

typedef enum
{
	QR_FAILURE = 0,
	QR_SUCCESS,
	QR_OFFLINE,
	QR_NOTAG
} QR_STATUS;

QR_STATUS GetValue1(int A_messageid, unsigned long A_tag, char *A_value, int A_size, char *A_default)
{
	MC_STATUS      mcStatus;
	static char    S_prefix[] = "GetValue";

	mcStatus = MC_Get_Value_To_String(A_messageid, A_tag, A_size, A_value);
	if (mcStatus == MC_NULL_VALUE || mcStatus == MC_EMPTY_VALUE || mcStatus == MC_INVALID_TAG)
	{
		if (!A_default)
		{
			A_value[0] = '\0';
			return(QR_NOTAG);
		}
		strncpy(A_value, A_default, A_size);
		A_value[A_size - 1] = '\0';
	}
	else if (mcStatus != MC_NORMAL_COMPLETION)
	{
		printf("\t%s\n", MC_Error_Message(mcStatus));
		printf("%s: ***          Tag:  %lX\n", S_prefix, A_tag);
		return(QR_FAILURE);
	}
	return(QR_SUCCESS);
} /* GetValue() */


MC_STATUS NOEXP_FUNC MediaToFileObj1(char*     A_filename,
	void*     A_userInfo,
	int*      A_dataSize,
	void**    A_dataBuffer,
	int       A_isFirst,
	int*      A_isLast)
{

	CBinfo*         callbackInfo = (CBinfo*)A_userInfo;
	size_t          bytes_read;
	int             retStatus;

	if (!A_userInfo)
		return MC_CANNOT_COMPLY;

	if (A_isFirst)
	{
		callbackInfo->bytesRead = 0;
		callbackInfo->fp = fopen(A_filename, BINARY_READ);

		retStatus = setvbuf(callbackInfo->fp, (char *)NULL, _IOFBF, 32768);
		if (retStatus != 0)
		{
			printf("WARNING:  Unable to set IO buffering on input file.\n");
		}

		if (callbackInfo->bufferLength == 0)
		{
			MC_STATUS mcStatus = MC_NORMAL_COMPLETION;
			int length = 0;

			mcStatus = MC_Get_Int_Config_Value(WORK_BUFFER_SIZE, &length);
			if (mcStatus != MC_NORMAL_COMPLETION)
			{
				length = 64 * 1024;
			}
			callbackInfo->bufferLength = length;
		}

		callbackInfo->buffer = (char*)malloc(callbackInfo->bufferLength);
		if (callbackInfo->buffer == NULL)
		{
			printf("Error: failed to allocate file read buffer [%d] kb", (int)callbackInfo->bufferLength);
			return MC_CANNOT_COMPLY;
		}
	}

	if (!callbackInfo->fp)
		return MC_CANNOT_COMPLY;

	bytes_read = fread(callbackInfo->buffer, 1, callbackInfo->bufferLength, callbackInfo->fp);
	if (ferror(callbackInfo->fp))
	{
		free(callbackInfo->buffer);
		callbackInfo->buffer = NULL;

		return MC_CANNOT_COMPLY;
	}

	if (feof(callbackInfo->fp))
	{
		*A_isLast = 1;

		fclose(callbackInfo->fp);
		callbackInfo->fp = NULL;
	}
	else
		*A_isLast = 0;

	*A_dataBuffer = callbackInfo->buffer;
	*A_dataSize = (int)bytes_read;
	callbackInfo->bytesRead += bytes_read;

	return MC_NORMAL_COMPLETION;

};

CFExeption::CFExeption(DWORD dwErrCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	m_sError = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
	m_dwError = dwErrCode;
}

CFExeption::CFExeption(CString sErrText)
{
	m_sError = sErrText;
	m_dwError = 0;
}

//************************************************************************************************************//

CFileOperation::CFileOperation()
{
	Initialize();
}


void CFileOperation::Initialize()
{
	m_sError = _T("No error");
	m_dwError = 0;
	m_bAskIfReadOnly = true;
	m_bOverwriteMode = true;
	m_bAborted = false;
	m_iRecursionLimit = -1;
	png = false, jpg = false, txt = false;
}

void CFileOperation::DoFolderCopy(CString sSourceFolder, CString sDestFolder)
{
		CFileFind ff;
		if (!CreateDirectory(sDestFolder, NULL))
		{
			DWORD dwErr = GetLastError();
			if (dwErr != 183)
			{
				ff.Close();
				throw new CFExeption(dwErr);
			}
		}
		//CFileFind ff;
		CString sPathSource = sSourceFolder;
		BOOL bRes = ff.FindFile(sPathSource);
		while (bRes)
		{
			bRes = ff.FindNextFile();
			if (ff.IsDots()) continue;
			if (ff.IsDirectory()) // source is a folder
			{
				if (m_iRecursionLimit == 0) continue;
				sPathSource = ff.GetFilePath() + CString("\\") + CString("*.*");
				CString sPathDest = sDestFolder + ff.GetFileName() + CString("\\");
				if (CheckPath(sPathDest) == PATH_NOT_FOUND)
				{
					if (!CreateDirectory(sPathDest, NULL))
					{
						ff.Close();
						throw new CFExeption(GetLastError());
					}
				}
				if (m_iRecursionLimit > 0) m_iRecursionLimit--;
				DoFolderCopy(sPathSource, sPathDest);
			}
			else // source is a file
			{
				CString sNewFileName = sDestFolder + ff.GetFileName();
				DoFileCopy(ff.GetFilePath(), sNewFileName);
			}
		}
		ff.Close();
		AfxMessageBox(_T("COPIED SUCCESSFULLY"));
}

bool CFileOperation::FolderExists(CString strFolderName)
{
	return GetFileAttributes(strFolderName) != INVALID_FILE_ATTRIBUTES;
}

void CFileOperation::CheckAndCopy(CString sSourceFolder, CString sDestFolder)
{
	if (FolderExists(sDestFolder) == true)
	{
		const int result = MessageBox(NULL, L"Folder already exist. Do you still want to copy with same name?", L"WARNING !!", MB_YESNOCANCEL);

		switch (result)
		{
		case IDYES:
		{
			m_bOverwriteMode = true; // COPY FILES with as it is, if false: copy of ... 
			DoFolderCopy(sSourceFolder, sDestFolder);
		}
			break;
		case IDNO:
		{
			sDestFolder.Delete(sDestFolder.GetLength() - 1);
			BOOL bOvrwriteFails = FALSE;
			if (!m_bOverwriteMode)
			{
				while (FolderExists(sDestFolder)) // Every time Dest folder name changes as copy, copy (1) ... and if it doesn't exist in directory it will create one.
				{
					sDestFolder = ChangeFileName(sDestFolder);
				}
				bOvrwriteFails = TRUE;
			}
			PreparePath(sDestFolder);
			DoFolderCopy(sSourceFolder, sDestFolder);
		}
			break;
		case IDCANCEL:
			// Do nothing
			break;
		}
		//AfxMessageBox(_T("Folder Already Exist"));

	}
	else
		DoFolderCopy(sSourceFolder, sDestFolder);
}

void CFileOperation::DoCopy(CString sSource, CString sDest)
{
	CheckSelfRecursion(sSource, sDest);
	// source & Dest not found
	if (CheckPath(sSource) == PATH_NOT_FOUND && CheckPath(sDest) == PATH_NOT_FOUND)
	{
		CString sError = sSource + CString(" path not found");
		throw new CFExeption(sError);
	}
	// source not found
	if (CheckPath(sSource) == PATH_NOT_FOUND)
	{
		CString sError = sSource + CString(" Source path not found");
		throw new CFExeption(sError);
	}
	// dest not found
	if (CheckPath(sDest) == PATH_NOT_FOUND)
	{
		CString sError = sDest + CString(" Destination path not found");
		throw new CFExeption(sError);
	}
	if (png == false && jpg == false && txt == false && ctm == false && mrm == false)
		MessageBox(NULL, L"Please select the type of file you wanted to copy", _T("Error"), MB_OK | MB_ICONERROR);
	else
	{
		// folder to file
		if (CheckPath(sSource) == PATH_IS_FOLDER && CheckPath(sDest) == PATH_IS_FILE)
		{
			throw new CFExeption(_T("Wrong operation"));
		}
		// folder to folder
		if (CheckPath(sSource) == PATH_IS_FOLDER && CheckPath(sDest) == PATH_IS_FOLDER)
		{
			CFileFind ff;
			CString sError = sSource + CString(" not found");
			PreparePath(sSource);
			PreparePath(sDest);
			sSource += "*.*";
			if (!ff.FindFile(sSource))
			{
				ff.Close();
				throw new CFExeption(sError);
			}
			if (!ff.FindNextFile())
			{
				ff.Close();
				throw new CFExeption(sError);
			}
			CString sFolderName = ParseFolderName(sSource);
			if (!sFolderName.IsEmpty()) // the source is not drive
			{
				sDest += sFolderName;
				PreparePath(sDest);
			}
			ff.Close();
			CheckAndCopy(sSource, sDest);
		}
		// file to file
		if (CheckPath(sSource) == PATH_IS_FILE && CheckPath(sDest) == PATH_IS_FILE)
		{
			DoFileCopy(sSource, sDest);
		}
		// file to folder
		if (CheckPath(sSource) == PATH_IS_FILE && CheckPath(sDest) == PATH_IS_FOLDER)
		{
			PreparePath(sDest);
			wchar_t drive[MAX_PATH], dir[MAX_PATH], name[MAX_PATH], ext[MAX_PATH];
			_wsplitpath_s(sSource, drive, dir, name, ext);
			sDest = sDest + CString(name) + CString(ext);
			DoFileCopy(sSource, sDest);
		}
	}
}

bool CFileOperation::modalityCheck(CString sSourceFile)
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
		return false;
	}

	free(mergeIniPath);

	if (amergeStatus != MC_NORMAL_COMPLETION
		&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		// write error into log file
		printf("Merge initialisation failed");
		return false;
	}

	amergeStatus = MC_Library_Initialization(NULL, NULL, NULL);

	if (amergeStatus != MC_NORMAL_COMPLETION
		&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return false;
	}

	if (initialised == false)
	{
		amergeStatus = MC_Register_Application(&MyAppID, "MergeComApp");
		if (amergeStatus != MC_NORMAL_COMPLETION
			&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
		{
			printf("\t%s\n", MC_Error_Message(amergeStatus));
			return false;
		}
		initialised = true;
	}

	CBinfo callbackInfo = { 0 };

	int fileID;
	CStringA myStringA(sSourceFile);
	amergeStatus = MC_Create_File(&fileID, myStringA, "STANDARD_MR", C_STORE_RQ);
	if (amergeStatus != MC_NORMAL_COMPLETION)
	{
		printf("Unable to create file object", amergeStatus);
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return false;
		//exit(EXIT_FAILURE);
	}

	amergeStatus = MC_Open_File(MyAppID, fileID, &callbackInfo, MediaToFileObj1);
	if (amergeStatus != MC_NORMAL_COMPLETION)
	{
		printf("Unable to read file from media", amergeStatus);
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return false;
	}

	char mod[UI_LENGTH + 1];
	if (GetValue1(fileID, MC_ATT_MODALITY, mod, sizeof(mod), "") == QR_FAILURE)
		return false;
	CString m1(mod);
	if (m1 == "MR" && mrm == true)
		return true;
	else if (m1 == "CT" && ctm == true)
		return true;
	else
		return false;
}

void CFileOperation::DoFileCopy(CString sSourceFile, CString sDestFile)
{
	BOOL bOvrwriteFails = FALSE;
	if (!m_bOverwriteMode)
	{
		while (IsFileExist(sDestFile))
		{
			sDestFile = ChangeFileName(sDestFile);
		}
		bOvrwriteFails = TRUE;
	}
	CString sExt;
	wchar_t drive[MAX_PATH];
	wchar_t dir[MAX_PATH];
	wchar_t name[MAX_PATH];
	wchar_t ext[MAX_PATH];
	_wsplitpath_s((LPCTSTR)sDestFile, drive, dir, name, ext);
	sExt = ext;
	int length = sExt.GetLength();

	for (int i = 0; i < length; i++) {
		sExt.SetAt(i, std::tolower(sExt[i]));
	}
		if (sExt == L".png" && png == true)
			if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
		if (sExt == L".jpg" && jpg == true)
			if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
		if (sExt == L".dcm" || sExt == L".ima"  && txt == true)
			if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
		if (sExt == L".dcm" || sExt == L".ima"&& mrm == true)
		{
			if (CFileOperation::modalityCheck(sSourceFile))
				if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
		}
		if (sExt == L".dcm" || sExt == L".ima"&& ctm == true)
		{
			if (CFileOperation::modalityCheck(sSourceFile))
				if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
		}
}


bool CFileOperation::Copy(CString sSource, CString sDest, bool m_png, bool m_jpg, bool m_txt, bool m_ctm, bool m_mrm)
{
	png = m_png, jpg = m_jpg, txt = m_txt, ctm = m_ctm, mrm = m_mrm;
	if (CheckSelfCopy(sSource, sDest)) return true;
	bool bRes;
	try
	{
		DoCopy(sSource, sDest);
		bRes = true;
	}
	catch (CFExeption* e)
	{
		m_sError = e->GetErrorText();
		m_dwError = e->GetErrorCode();
		delete e;
		if (m_dwError == 0) bRes = true;
		bRes = false;
	}
	m_iRecursionLimit = -1;
	return bRes;
}

CString CFileOperation::ChangeFileName(CString sFileName)
{
	CString sName, sNewName, sResult;
	wchar_t drive[MAX_PATH];
	wchar_t dir[MAX_PATH];
	wchar_t name[MAX_PATH];
	wchar_t ext[MAX_PATH];
	_wsplitpath_s((LPCTSTR)sFileName, drive, dir, name, ext);
	sName = name;

	int pos = sName.Find(_T("Copy "));
	if (pos == -1)
	{
		sNewName = CString("Copy of ") + sName + CString(ext);
	}
	else
	{
		int pos1 = sName.Find('(');
		if (pos1 == -1)
		{
			sNewName = sName;
			sNewName.Delete(0, 8);
			sNewName = CString("Copy (1) of ") + sNewName + CString(ext);
		}
		else
		{
			CString sCount;
			int pos2 = sName.Find(')');
			if (pos2 == -1)
			{
				sNewName = CString("Copy of ") + sNewName + CString(ext);
			}
			else
			{
				sCount = sName.Mid(pos1 + 1, pos2 - pos1 - 1);
				sName.Delete(0, pos2 + 5);
				CT2A ascii(sCount);
				int iCount = atoi(ascii.m_psz);
				iCount++;
				sNewName = CString("Copy (") + (CString)(std::to_wstring(iCount).c_str()) + CString(") of ") + (LPCTSTR)sName + CString(ext);
			}
		}
	}

	sResult = CString(drive) + CString(dir) + sNewName;
	return sResult;
}


bool CFileOperation::IsFileExist(CString sPathName)
{
	HANDLE hFile;
	hFile = CreateFile(sPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	CloseHandle(hFile);
	return true;
}


int CFileOperation::CheckPath(CString sPath)
{
	DWORD dwAttr = GetFileAttributes(sPath);
	if (dwAttr == 0xffffffff)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
			return PATH_NOT_FOUND;
		return PATH_ERROR;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) return PATH_IS_FOLDER;
	return PATH_IS_FILE;
}


void CFileOperation::PreparePath(CString &sPath)
{
	if (sPath.Right(1) != "\\") sPath += "\\";
}

CString CFileOperation::ParseFolderName(CString sPathName)
{
	CString sFolderName = sPathName;
	int pos = sFolderName.ReverseFind('\\');
	if (pos != -1) sFolderName.Delete(pos, sFolderName.GetLength() - pos);
	pos = sFolderName.ReverseFind('\\');
	if (pos != -1) sFolderName = sFolderName.Right(sFolderName.GetLength() - pos - 1);
	else sFolderName.Empty();
	return sFolderName;
}


void CFileOperation::CheckSelfRecursion(CString sSource, CString sDest)
{
	if (sDest.Find(sSource) != -1)
	{
		int i = 0, count1 = 0, count2 = 0;
		for (i = 0; i < sSource.GetLength(); i++)	if (sSource[i] == '\\') count1++;
		for (i = 0; i < sDest.GetLength(); i++)	if (sDest[i] == '\\') count2++;
		if (count2 >= count1) m_iRecursionLimit = count2 - count1;
	}
}


bool CFileOperation::CheckSelfCopy(CString sSource, CString sDest)
{
	bool bRes = false;
	if (CheckPath(sSource) == PATH_IS_FOLDER)
	{
		CString sTmp = sSource;
		int pos = sTmp.ReverseFind('\\');
		if (pos != -1)
		{
			sTmp.Delete(pos, sTmp.GetLength() - pos);
			if (sTmp.CompareNoCase(sDest) == 0) bRes = true;
		}
	}
	return bRes;
}