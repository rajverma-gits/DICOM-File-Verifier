// TabTwo.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrl.h"
#include "TabTwo.h"
#include "afxdialogex.h"
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
#include "FileOperation.h"

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


// CTabTwo dialog

IMPLEMENT_DYNAMIC(CTabTwo, CDialog)


CTabTwo::CTabTwo(CWnd* pParent /*=NULL*/)
	: CDialog(CTabTwo::IDD, pParent)
	, m_dicom(_T(""))
	, m_ListTextCtrl(_T(""))
{
	AfxOleInit();
}

CTabTwo::~CTabTwo()
{
}


void CTabTwo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_dicomPicker);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, m_dicom);

	DDX_Control(pDX, IDC_LIST1, m_control);
	DDX_LBString(pDX, IDC_LIST1, m_ListTextCtrl);
	DDX_Control(pDX, IDC_LIST2, m_Lcontrol2);
}


BEGIN_MESSAGE_MAP(CTabTwo, CDialog)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CTabTwo::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CTabTwo::OnEnChangeMfceditbrowse1)
	ON_LBN_SELCHANGE(IDC_LIST1, &CTabTwo::OnLbnSelchangeList1)
END_MESSAGE_MAP()



// CTabTwo message handlers
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
	char     pat_wht[AE_LENGTH + 1];
	char     mod[UI_LENGTH + 1];

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

QR_STATUS GetValue(int A_messageid, unsigned long A_tag, char *A_value, int A_size, char *A_default)
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


MC_STATUS NOEXP_FUNC MediaToFileObj(char*     A_filename,
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

void changeDate(CString &date)
{
	if (date.GetLength() != 8)
		return;
	CString year = date.Left(4);
	CString month = date.Mid(4,2);
	CString day = date.Right(2);
	date = day + "-" + month + "-" + year;
}

void ListFilesInFolder(const CString &folderPath, vector<wstring> &fileNames)
{
	wstring wstr((LPCTSTR)folderPath);
	wstring searchPath = folderPath + L"\\*.*";
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(searchPath.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				wstring filename = fd.cFileName;
				wcout.imbue(locale(""));
				fileNames.push_back(filename);
			}
		} while (FindNextFileW(hFind, &fd));
	}

	FindClose(hFind);

}

bool isFileFormat(const wstring &fileName, const wstring &desiredFormat1, const wstring &desiredFormat2)
{
	size_t dotPos = fileName.find_last_of(L".");
	if (dotPos != wstring::npos)
	{
		wstring extension = fileName.substr(dotPos + 1);
		wstring lowerCaseExtension;
		locale loc;
		for (wchar_t c : extension)
			lowerCaseExtension += tolower(c, loc);
		return lowerCaseExtension == desiredFormat1 || lowerCaseExtension == desiredFormat2;
	}
	return false;
}

void CTabTwo::OnBnClickedButton1()
{
	UpdateData();
	m_control.ResetContent();
	//MessageBox(L"You selected " + m_dicom);
	vector<wstring> fileNames;
	ListFilesInFolder(m_dicom, fileNames);
	wstring desiredFormat1 = L"dcm";
	wstring desiredFormat2 = L"ima";

	for (const auto&fileName : fileNames)
	{
		if (isFileFormat(fileName, desiredFormat1, desiredFormat2))
		{
			CString result(fileName.c_str());
			m_control.AddString(result);
		}
	}
	// TODO: Add your control notification handler code here
}


void CTabTwo::OnEnChangeMfceditbrowse1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

bool ContainsSpecialCharacters(const char* charArray)
{
	const char* specialChars = "!@#$%^&*()+{}[]<>,.?/\\|~`";

	for (int i = 0; charArray[i] != '\0'; i++)
	{
		if (strchr(specialChars, charArray[i]) != nullptr)
		{
			return true;  
		}
	}

	return false; 
}


void CTabTwo::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	CListBox* lbox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (lbox == NULL)
	{
		MessageBox(L"Please select ");
	}
	int curser = lbox->GetCurSel();
	CString seltext;
	lbox->GetText(curser, seltext);
	CString finalFilePath = m_dicom + L"\\" + seltext;
	/*CString theData;
	UINT uiSelection = m_control.GetCurSel();

	if (uiSelection == LB_ERR) return;


	m_control.GetText(uiSelection, theData);*/
	//MessageBox(finalFilePath);
	UpdateData();
	m_Lcontrol2.ResetContent();
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
	}

	amergeStatus = MC_Library_Initialization(NULL, NULL, NULL);

	if (amergeStatus != MC_NORMAL_COMPLETION
		&& amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return;
	}

	if (theApp.initialised == false)
	{
		amergeStatus = MC_Register_Application(&theApp.MyAppID, "MergeComApp");
		if (amergeStatus != MC_NORMAL_COMPLETION && amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
		{
			printf("\t%s\n", MC_Error_Message(amergeStatus));
			return;
		}
		theApp.initialised = true;
	}

	CBinfo callbackInfo = { 0 };

	int fileID;
	CStringA myStringA(finalFilePath);
	amergeStatus = MC_Create_File(&fileID, myStringA, "STANDARD_MR", C_STORE_RQ);
	if (amergeStatus != MC_NORMAL_COMPLETION && amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		printf("Unable to create file object", amergeStatus);
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return;
		//exit(EXIT_FAILURE);
	}

	 
	amergeStatus = MC_Open_File(theApp.MyAppID, fileID, &callbackInfo, MediaToFileObj);
	if (amergeStatus != MC_NORMAL_COMPLETION && amergeStatus != MC_LIBRARY_ALREADY_INITIALIZED)
	{
		printf("Unable to read file from media", amergeStatus);
		printf("\t%s\n", MC_Error_Message(amergeStatus));
		return;
	}

	StudyNode study;

	bool empty = false;
	bool invalid = false;
	CString empMsg = L"Empty Tags : ";
	CString inMsg = L"\nInvalid Tags : ";
	CString pid = L"", pname = L"", pbday = L"", psex = L"", modality = L"";
	if (GetValue(fileID, MC_ATT_PATIENT_ID, study.pat_id, sizeof(study.pat_id), "") == QR_FAILURE)
		return;
	else
	{
		if (strlen(study.pat_id) == 0)
		{
			empty = true;
			empMsg += "PATIENT ID ";
		}
		if (ContainsSpecialCharacters(study.pat_id))
		{
			invalid = true;
			inMsg += " PATIENT ID ";
		}
		pid = L"Patient ID: ";
		pid += study.pat_id;
	}
	if (GetValue(fileID, MC_ATT_PATIENTS_NAME, study.pat_name, sizeof(study.pat_name), "") == QR_FAILURE)
		return;
	else
	{
		if (strlen(study.pat_name) == 0)
		{
			empty = true;
			empMsg += " PATIENT NAME ";
		}
		if (ContainsSpecialCharacters(study.pat_name))
		{
			invalid = true;
			inMsg += " PATIENT NAME ";
		}
		pname = L"Patient Name: ";
		pname += study.pat_name;
	}

	if (GetValue(fileID, MC_ATT_PATIENTS_BIRTH_DATE, study.pat_bday, sizeof(study.pat_bday), "") == QR_FAILURE)
		return;
	else
	{
		if (strlen(study.pat_bday) == 0)
		{
			empty = true;
			empMsg += " PATIENT BIRTHDAY ";
		}
		CString x(study.pat_bday);
		changeDate(x);
		pbday = L"Patient Birthday: ";
		pbday += x;
	}
	if (GetValue(fileID, MC_ATT_PATIENTS_SEX, study.pat_sex, sizeof(study.pat_sex), "") == QR_FAILURE)
		return;
	else
	{
		if (strlen(study.pat_sex) == 0)
		{
			empty = true;
			empMsg += " PATIENT SEX ";
		}
		psex = L"Patient Sex: ";
		psex += study.pat_sex;
	}
	if (GetValue(fileID, MC_ATT_MODALITY, study.mod, sizeof(study.mod), "") == QR_FAILURE)
		return;
	else
	{
		if (strlen(study.mod) == 0)
		{
			empty = true;
			empMsg += " MODALITY ";
		}
		modality = L"Modlaity: ";
		modality += study.mod;
	}

	if (empty == true && invalid == true)
	{
		//CString tags = L"\ntags are empty";
		CString pMsg = empMsg + inMsg;
		MessageBox(pMsg);
	}
	else if (empty == true)
		MessageBox(empMsg);
	else if (invalid == true)
		MessageBox(inMsg);

	m_Lcontrol2.AddString(modality);
	m_Lcontrol2.AddString(pid);
	m_Lcontrol2.AddString(pname);
	m_Lcontrol2.AddString(pbday);
	m_Lcontrol2.AddString(psex);
}
