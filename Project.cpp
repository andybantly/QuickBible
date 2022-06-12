// Project.cpp : Defines the class behaviors for the application.
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "AC.h"
#include "Crypt.h"
#include "atlenc.h"
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProjectApp * g_pApp = NULL;
HWND g_hWndCapture = NULL;
int g_iDriver = 0;

// Enumerate the monitors in the display
BOOL CALLBACK EnumDisplayMonitorsProc(
	HMONITOR hMonitor,  // handle to display monitor
	HDC hdcMonitor,     // handle to monitor-appropriate device context
	LPRECT lprcMonitor, // pointer to monitor intersection rectangle
	LPARAM dwData       // data passed from EnumDisplayMonitors
)
{
	MONITORINFOEX mi;
	mi.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor,&mi);
	CString csDeviceName = mi.szDevice;
	CRect Rect = *lprcMonitor;
	vector<CRect> * pvMonRect = (vector<CRect> *)dwData;
	pvMonRect->push_back(Rect);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectApp

BEGIN_MESSAGE_MAP(CProjectApp, CWinApp)
	//{{AFX_MSG_MAP(CProjectApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectApp construction

CProjectApp::CProjectApp() : m_hResDLL(NULL)
{
	// Initialize COM
	OleInitialize(NULL);

	// Load the resource DLL for the primary language (or English if it is not supported)
	m_hResDLL = LoadLibrary("QBRes.dll");

	// Detect the primary language
	bool bSupportedLanguage = false;
	WORD wLangID = PRIMARYLANGID(GetUserDefaultLCID());
	switch (wLangID)
	{
	case LANG_ENGLISH:
	case LANG_FRENCH:
	case LANG_GERMAN:
	case LANG_ITALIAN:
	case LANG_SPANISH:
		bSupportedLanguage = true;
		break;
	default:;
	}

	// Initialize Common Controls
	InitCommonControls();

	// Enable HTML help
	EnableHtmlHelp();
}

CProjectApp::~CProjectApp()
{
	// Free the resource dll
	if (m_hResDLL)
	{
		FreeLibrary(m_hResDLL);
		m_hResDLL = NULL;
	}

	// Uninitialize COM
	OleUninitialize();
}

// Load up the bibles
bool CProjectApp::Initialize()
{
#if defined(ENGLISH)

#if defined(DEMO)
	// DEMO King James Version
	m_vBibles.push_back(CBible("DEMO King James Version","DEMO"));
#endif

#if defined(PERSONAL)
	// King James Version
	m_vBibles.push_back(CBible(QBString(115),QBString(154)));
#endif

#if defined(LITE)
	// King James Version
	m_vBibles.push_back(CBible(QBString(115),QBString(154)));
#endif

#if defined(BASIC)
	// King James Version + Apocrypha
	m_vBibles.push_back(CBible(QBString(115),QBString(154)));
	m_vBibles.push_back(CBible(QBString(116),QBString(155)));
#endif

#if defined(FULL)
	// King James Version + Apocrypha
	m_vBibles.push_back(CBible(QBString(115),QBString(154)));
	m_vBibles.push_back(CBible(QBString(116),QBString(155)));
	// American Standard Version
	m_vBibles.push_back(CBible(QBString(121),QBString(160)));
	// Youngs Literal Translation
	m_vBibles.push_back(CBible(QBString(122),QBString(161)));
	// Darby Translation
	m_vBibles.push_back(CBible(QBString(123),QBString(162)));
	// Douay-Rheims
	m_vBibles.push_back(CBible(QBString(130),QBString(169)));
#endif

#if defined(_DEBUG) || defined(_RELEASE)
	int iId = 109,iIdTrans = 148;
	for (;iId < 148;++iId,++iIdTrans)
		m_vBibles.push_back(CBible(QBString(iId),QBString(iIdTrans)));
	m_vBibles.push_back(CBible("Custom","CUST"));
#endif

#endif // ENGLISH

#if defined(FRENCH)

#if defined(PERSONAL)
	// Louis Segond
	m_vBibles.push_back(CBible(QBString(135),QBString(174)));
#endif

#if defined(LITE)
	// Louis Segond
	m_vBibles.push_back(CBible(QBString(135),QBString(174)));
#endif

#if defined(BASIC)
	// Louis Segond
	m_vBibles.push_back(CBible(QBString(135),QBString(174)));
#endif

#endif // FRENCH

#if defined(GERMAN)

#if defined(PERSONAL)
	// Luther Bibel 1545
	m_vBibles.push_back(CBible(QBString(133),QBString(172)));
#endif

#if defined(LITE)
	// Luther Bibel 1545
	m_vBibles.push_back(CBible(QBString(133),QBString(172)));
#endif

#if defined(BASIC)
	// Luther Bibel 1545
	m_vBibles.push_back(CBible(QBString(133),QBString(172)));
#endif

#if defined(FULL)
	// Elberfelder
	m_vBibles.push_back(CBible(QBString(131),QBString(170)));
	// Luther Bibel 1545
	m_vBibles.push_back(CBible(QBString(133),QBString(172)));
#endif

#endif // GERMAN

#if defined(SPANISH)

#if defined(PERSONAL)
	// Reina-Valera Antigua
	m_vBibles.push_back(CBible(QBString(143),QBString(182)));
#endif

#if defined(LITE)
	// Reina-Valera Antigua
	m_vBibles.push_back(CBible(QBString(143),QBString(182)));
#endif

#if defined(BASIC)
	// Reina-Valera Antigua
	m_vBibles.push_back(CBible(QBString(143),QBString(182)));
#endif

#endif // SPANISH

#if defined(ITALIAN)

#if defined(PERSONAL)
	// Conferenza Episcopale Italiana
	m_vBibles.push_back(CBible(QBString(144),QBString(183)));
#endif

#if defined(LITE)
	// Conferenza Episcopale Italiana
	m_vBibles.push_back(CBible(QBString(144),QBString(183)));
#endif

#if defined(BASIC)
	// Conferenza Episcopale Italiana
	m_vBibles.push_back(CBible(QBString(144),QBString(183)));
#endif

#endif // ITALIAN

#if defined(LATIN)

#if defined(PERSONAL)
	// Biblia Sacra Vulgata
	m_vBibles.push_back(CBible(QBString(147),QBString(186)));
#endif

#if defined(LITE)
	// Biblia Sacra Vulgata
	m_vBibles.push_back(CBible(QBString(147),QBString(186)));
#endif

#if defined(BASIC)
	// Biblia Sacra Vulgata
	m_vBibles.push_back(CBible(QBString(147),QBString(186)));
#endif

#endif // LATIN

	// Size the database for all possible translations
	int nTrans = (int)m_vBibles.size();
	m_DBTrans.m_vBible.resize(nTrans);

#if !defined(LITE)
	// Set up the verse finder
	if (m_pVerseFinderDlg)
		m_pVerseFinderDlg->SetupVerseFinder();
#endif

	// Enumerate the monitors for the display coordinates
	EnumDisplayMonitors(NULL,NULL,EnumDisplayMonitorsProc,(LPARAM)&m_vMonRect);

	// Enumerate the capture devices
	BOOL bDesc = TRUE;
	for (WORD iIndex = 0;bDesc && (iIndex < 10);iIndex++)
	{
		CString csName,csVersion;
		LPSTR pName = csName.GetBufferSetLength(255);
		LPSTR pVersion = csVersion.GetBufferSetLength(255);
		bDesc = capGetDriverDescription(iIndex,pName,csName.GetLength(),pVersion,csVersion.GetLength());
		csName.ReleaseBuffer();
		csVersion.ReleaseBuffer();
	}

	return true;
}

// Parse command line for standard shell commands, DDE, file open
void CProjectApp::GetShellCommand()
{
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Look for the open shell command
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		CString csFileName = cmdInfo.m_strFileName;
		if (csFileName.GetLength() > 4)
		{
			// Get the file name that is trying to be opened
			CString csDrive,csDir,csFname,csExt;
			_splitpath(csFileName,csDrive.GetBufferSetLength(_MAX_DRIVE),csDir.GetBufferSetLength(_MAX_DIR),csFname.GetBufferSetLength(_MAX_FNAME),csExt.GetBufferSetLength(_MAX_EXT));
			csDrive.ReleaseBuffer();
			csDir.ReleaseBuffer();
			csFname.ReleaseBuffer();
			csExt.ReleaseBuffer();

			// Look for the well known file types
			if (csExt.CompareNoCase(".bbl") == 0)
			{
				// Look for bible settings
				CProjectDlg * pDlg = ((CProjectDlg *)m_pMainWnd);
				pDlg->OpenSettings(csFileName);
			}
			else if (csExt.CompareNoCase(".bdb") == 0)
			{
				// Look for bible databases
				int nbTrans = LookupNBTrans();
				for (int iTranslation = 1;iTranslation <= nbTrans;++iTranslation)
				{
					CBible & Bible = m_vBibles[iTranslation - 1];

					// Get the bible database file name
					CString csTransDrive,csTransDir,csTransFname,csTransExt;
					_splitpath(Bible.m_csTransFile,csTransDrive.GetBufferSetLength(_MAX_DRIVE),csTransDir.GetBufferSetLength(_MAX_DIR),csTransFname.GetBufferSetLength(_MAX_FNAME),csTransExt.GetBufferSetLength(_MAX_EXT));
					csTransDrive.ReleaseBuffer();
					csTransDir.ReleaseBuffer();
					csTransFname.ReleaseBuffer();
					csTransExt.ReleaseBuffer();

					// Look for a match
					if (csTransFname.CompareNoCase(csFname) == 0)
					{
						// Load this translation
						LoadTranslation(iTranslation);
						break;
					}
				}
			}
		}
	}
}

// Check for proper installation
bool CProjectApp::IsInstalled()
{
	CString csInstallString = GetInstallString();
	return IsInstalled(csInstallString);
}

CString CProjectApp::GetInstallString()
{
	CString csInstallString;
#if defined(ENGLISH)

#if defined(DEMO)
	csInstallString = "{52DE9DB6-550C-4BA1-B38E-661CA8152323}";
#endif

#if defined(LITE)
	csInstallString = "{D2D32AAF-4548-4DBD-B882-4728BBB63638}";
#endif

#if defined(PERSONAL)
	csInstallString = "{174ABA3A-1027-4862-86F6-04CA3C7288EC}";
#endif

#if defined(BASIC)
	csInstallString = "{BA7FD58F-56F2-4483-A2A7-B5E5BF56B398}";
#endif

#if defined(FULL)
	csInstallString = "{A2C262F8-A6F6-4EE8-B24A-41BA6424E9C5}";
#endif

#endif // ENGLISH

#if defined(FRENCH)

#if defined(LITE)
	csInstallString = "{09B65642-64EB-4241-9A95-A4A7A70EDA8F}";
#endif

#if defined(PERSONAL)
	csInstallString = "{7F1BCD20-2559-4936-AFE1-7A4C64BD3BA6}";
#endif

#if defined(BASIC)
	csInstallString = "{C08CE8FB-4CC8-4827-93DD-9543BBFB019B}";
#endif

#endif // FRENCH

#if defined(GERMAN)

#if defined(LITE)
	csInstallString = "{CA621D05-1942-4721-B20E-81E5C0A2C5B9}";
#endif

#if defined(PERSONAL)
	csInstallString = "{1EC19146-26CB-495D-9368-821FD36D1588}";
#endif

#if defined(BASIC)
	csInstallString = "{87D7AC6E-222A-4044-B40B-3F7CA0C870D1}";
#endif

#if defined(FULL)
	csInstallString = "{B4BF2C22-5CB9-4885-8A13-A298ACDABB50}";
#endif

#endif // GERMAN

#if defined(SPANISH)

#if defined(LITE)
	csInstallString = "{0703FBEB-92B8-4FC2-B888-CE23053E2A69}";
#endif

#if defined(PERSONAL)
	csInstallString = "{9E8042AD-EE37-4BFD-A56F-9C53927F4269}";
#endif

#if defined(BASIC)
	csInstallString = "{8932E1A5-9EE3-4478-9E90-8788BBA5D91D}";
#endif

#endif // SPANISH

#if defined(ITALIAN)

#if defined(LITE)
	csInstallString = "{E46EEF9C-D899-4DA6-BB72-1338CF5C9252}";
#endif

#if defined(PERSONAL)
	csInstallString = "{F47AC81F-4200-490A-A331-AB0BB5E4DEB0}";
#endif

#if defined(BASIC)
	csInstallString = "{B405FA0A-5133-49EC-9A62-FB8E3D83E273}";
#endif

#endif // ITALIAN

#if defined(LATIN)

#if defined(LITE)
	csInstallString = "{2C54DE96-2F9F-475F-968F-7CF483D7AAC1}";
#endif

#if defined(PERSONAL)
	csInstallString = "{E9368848-F6AD-466E-A9DB-AE72E0E9E098}";
#endif

#if defined(BASIC)
	csInstallString = "{AEA72E39-FDF2-4AB2-A5D6-92D9769D5738}";
#endif

#endif // LATIN

#if defined(_RELEASE) || defined(_DEBUG)
	csInstallString = "{00000000-0000-0000-0000-000000000000}";
#endif
	return csInstallString;
}

// Check if we are installed
bool CProjectApp::IsInstalled(CString & csGUID)
{
#if !defined(_RELEASE) && !defined(_DEBUG)
	HKEY hKey = (HKEY)INVALID_HANDLE_VALUE;
	CString csKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
	csKey += csGUID;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,csKey,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS)
		return false;
	csKey = "DisplayVersion";
	DWORD dwType = 0,dwCount = 0;
    if (RegQueryValueEx(hKey,csKey,NULL,&dwType,NULL,&dwCount) == ERROR_SUCCESS)
    {
        if (dwType == REG_SZ && dwCount > 0)
        {
			CString csDisplayVersion;
			char * pDisplayVersion = csDisplayVersion.GetBufferSetLength(dwCount);
			if (RegQueryValueEx(hKey,csKey,NULL,NULL,(BYTE*)pDisplayVersion,&dwCount) == ERROR_SUCCESS)
			{
				if (csDisplayVersion == GetVersionNumber())
				{
					RegCloseKey(hKey);
					return true;
				}
			}
		}
	}
	if (hKey != (HKEY)INVALID_HANDLE_VALUE)
		RegCloseKey(hKey);
	return false;
#else
	return true;
#endif
}

// Licensing
// GUID (use installer product id) s/b the private key, volume s/b public
bool CProjectApp::IsRegistered()
{
	return true;
/*
#if !defined(DEMO)
	CString csSN = GetUID();
	CString csInstallString = GetInstallString();
	int nLen = csInstallString.GetLength();
	if (nLen == 0)
		return false;
	CCrypt SN((BYTE*)csInstallString.GetBuffer(),nLen);
	std::vector<BYTE> vecSN;
	SN.Encrypt(csSN,vecSN);

	// Base64 encode
	int nLen64 = Base64EncodeGetRequiredLength((int)vecSN.size(),ATL_BASE64_FLAG_NOPAD);
	std::vector<char> vec64(nLen64);
	Base64Encode(&vecSN[0],(int)vecSN.size(),&vec64[0],&nLen64,ATL_BASE64_FLAG_NOPAD);
	CString cs64(&vec64[0],nLen64);

	// Read the license file
	CStdioFile LicenseFile;
	CFileException Error;
	CString csRegistrationPath = GetRegistrationPath();
	CString csLicenseFile;
	csLicenseFile.Format("%s\\qb.dat",csRegistrationPath);
	if (!LicenseFile.Open(csLicenseFile,CFile::modeRead,&Error))
	{
		CString csMessage;
		Error.GetErrorMessage(csMessage.GetBufferSetLength(255),255);
		csMessage.ReleaseBuffer();
		return false;
	}
	CString csBase64Encoded;
	LicenseFile.ReadString(csBase64Encoded);
	LicenseFile.Close();

	// Validate
	return cs64 == csBase64Encoded;
#else
	return true;
#endif
*/
}

// Get the volume serial number
DWORD CProjectApp::VolSN()
{
	CString csSN,csDrive;
	CString csPath = GetSpecialFolderPath(CSIDL_WINDOWS);
	char * pPath = csPath.GetBuffer();
	char * pDrive = csDrive.GetBufferSetLength(_MAX_DRIVE);
	_splitpath(pPath,pDrive,NULL,NULL,NULL);
	csDrive.ReleaseBuffer();
	csDrive += "\\";
	DWORD dwSN = 0;
	if (csDrive.GetAt(1) == ':')
		GetVolumeInformation(csDrive,NULL,0,&dwSN,NULL,NULL,NULL,0);
	int nLen = csPath.GetLength();
	for (int i = 0;i < nLen;++i)
		dwSN = 1664525L * dwSN + 1013904223L;
	return dwSN;
}

DWORD CProjectApp::CPUid()
{
	DWORD dwID = 0;
	int CPUInfo[4] = {0,0,0,0};
	memset(&CPUInfo,0,sizeof(CPUInfo));
	unsigned nIds, nExIds, i, iInfo;

	// __cpuid with an InfoType argument of 0 returns the number of
	// valid Ids in CPUInfo[0] and the CPU identification string in
	// the other three array elements. The CPU identification string is
	// not in linear order. The code below arranges the information 
	// in a human readable form.
	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];

	// Get the information associated with each valid Id
	for (i = 0;i <= nIds;++i)
	{
		memset(&CPUInfo,0,sizeof(CPUInfo));
		__cpuid(CPUInfo, i);
		if (i == 1)
			CPUInfo[1] &= 0xFFFF;

		// XOR the information together
		for (iInfo = 0;iInfo < 4;++iInfo)
			dwID ^= CPUInfo[iInfo];
	}

	// Calling __cpuid with 0x80000000 as the InfoType argument
	// gets the number of valid extended IDs.
	memset(&CPUInfo,0,sizeof(CPUInfo));
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];

	// Get the information associated with each extended ID.
	for (i = 0x80000000; i <= nExIds;++i)
	{
		memset(&CPUInfo,0,sizeof(CPUInfo));
		__cpuid(CPUInfo, i);

		// XOR the information together
		for (iInfo = 0;iInfo < 4;++iInfo)
			dwID ^= CPUInfo[iInfo];
	}

	// Return the unique identifier
	for (i = 0;i < nIds;++i)
		dwID = 1664525L * dwID + 1013904223L;
	return dwID;
}

// Get the Unique Identifier
CString CProjectApp::GetUID()
{
	CString csUID;
	csUID.Format("%0X%0X",CPUid(),VolSN());
	return csUID;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CProjectApp object

CProjectApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CProjectApp initialization

int CProjectApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

BOOL CProjectApp::InitInstance()
{
	InitCommonControls();

	AfxEnableControlContainer();

	// Store the application pointer
	g_pApp = (CProjectApp *)AfxGetApp();

	// Show the main dialog which depends on the screen resolution
	int cxFullScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int iDD = IDD_PROJECT_DIALOG;
	if (cxFullScreen < 960)
		iDD = IDD_PROJECT_DIALOG_SMALL;
	CProjectDlg dlg(iDD);
	m_pMainWnd = &dlg;

	// Create the preview modeless dialog
	m_pPreviewDlg = new CPreviewDlg(m_pMainWnd);
	if (m_pPreviewDlg->Create() != TRUE)
	{
		delete m_pPreviewDlg;
		m_pPreviewDlg = NULL;
	}
	else
	{
		CRect Rect;
		m_pPreviewDlg->GetWindowRect(&Rect);
		m_pPreviewDlg->MoveWindow(SM_CXICON,SM_CYICON,Rect.Width(),Rect.Height(),FALSE);
	}

#if !defined(LITE)
	// Create the search modeless dialog
	m_pVerseFinderDlg = new CVerseFinderDlg(m_pMainWnd);
	if (m_pVerseFinderDlg->Create() != TRUE)
	{
		delete m_pVerseFinderDlg;
		m_pVerseFinderDlg = NULL;
	}
#endif

#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
	// Create the content management modeless dialog
	m_pManageContentDlg = new CManageContentDlg(m_pMainWnd);
	if (m_pManageContentDlg->Create() != TRUE)
	{
		delete m_pManageContentDlg;
		m_pManageContentDlg = NULL;
	}
#endif

	// Launch the main workspace dialog
	INT_PTR nResponse = dlg.DoModal();

	// Destroy the preview dialog
	if (m_pPreviewDlg)
		m_pPreviewDlg->Destroy();

#if !defined(LITE)
	// Destroy the verse finder dialog
	if (m_pVerseFinderDlg)
		m_pVerseFinderDlg->Destroy();
#endif

#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
	// Destroy the content management dialog
	if (m_pManageContentDlg)
		m_pManageContentDlg->Destroy();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CProjectApp::UpdateLastFrame(bool bPreview)
{
	if (bPreview)
	{
		m_pPreviewDlg->m_bSearchPreview = FALSE;
		m_pPreviewDlg->RedrawWindow();
	}
	else
		m_pPreviewDlg->Render();
}

void CProjectApp::UpdateSearchPreview(CVerse & Verse)
{
	m_pPreviewDlg->m_SearchVerse = Verse;
	m_pPreviewDlg->m_bSearchPreview = TRUE;
	m_pPreviewDlg->RedrawWindow();
}

bool CProjectApp::IsLoaded(int iTranslation)
{
	CBible & Bible = m_vBibles[iTranslation - 1];
	return Bible.m_bLoaded;
}

BOOL CProjectApp::LoadTranslation(int iTranslation)
{
	CWaitCursor Wait;
	CProjectDlg * pDlg = ((CProjectDlg *)m_pMainWnd);
	CBible & Bible = m_vBibles[iTranslation - 1];
	CFile File;
	if (!Bible.m_bLoaded && File.Open(Bible.m_csTransFile,CFile::modeRead))
	{
		// The decoder
		CArithmeticEncoder AC;

		// Read the compressed file into the buffer
		DWORD dwEncode = (DWORD)File.GetLength();
		char * pEncode = new char[dwEncode];
		File.Read(pEncode,dwEncode);
		File.Close();

		// Decode the file to another buffer
		char * pDecode = NULL;
		DWORD dwDecode = 0;
		AC.DecodeBuffer(pEncode,dwEncode,&pDecode,&dwDecode);
		delete [] pEncode;
		pEncode = 0;

		// Attach the buffer to a memory based file
		CMemFile MemFile;
		MemFile.Attach((BYTE *)pDecode,dwDecode);

		// Serialize using a memory based file
		CArchive arFile(&MemFile,CArchive::load);

		// Add the decompressed bible
		CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
		DBBible.Serialize(arFile);

		// Clean up
		MemFile.Detach();
		delete [] pDecode;

		// Success
		Bible.m_bLoaded = true;
		CVerse Verse = LookupLongestVerse(iTranslation);
		if (Verse)
		{
			int iMinPointSize = pDlg->DetectFontSize(&Verse);
			if (iMinPointSize)
			{
				int iPointSize = pDlg->GetPointSize();
				if (iMinPointSize < iPointSize)
				{
					pDlg->m_csMinPointSizeBible = LookupTranslationName(iTranslation);
					pDlg->CheckUpdatePointSize(iMinPointSize);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CProjectApp::SaveTranslation(CBible & Bible)
{
	int iTranslation = g_pApp->LookupTranslationIndex(Bible.m_csDescription);
	LoadTranslation(iTranslation);

	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CCrypt InBook((BYTE*)Bible.m_csTranslation.GetBuffer(),Bible.m_csTranslation.GetLength());
		CCrypt InVerse((BYTE*)Bible.m_csDescription.GetBuffer(),Bible.m_csDescription.GetLength());
		Bible.m_csTranslation = "CUST";
		Bible.m_csTransFile = "ECCUST.BDB";
		Bible.m_csDescription = "Custom";
		CCrypt OutBook((BYTE*)Bible.m_csTranslation.GetBuffer(),Bible.m_csTranslation.GetLength());
		CCrypt OutVerse((BYTE*)Bible.m_csDescription.GetBuffer(),Bible.m_csDescription.GetLength());

		CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
		std::vector<CDBBook> & vBook = DBBible.m_vBook;
		for (int iBook = 0;iBook < (int)vBook.size();++iBook)
		{
			CDBBook & DBBook = vBook[iBook];
			CString csBook;
			InBook.Decrypt(DBBook.m_vecCryptName,csBook);
			DBBook.m_vecCryptName.clear();
			OutBook.Encrypt(csBook,DBBook.m_vecCryptName);
			for (int nChap = 0;nChap < (int)DBBook.m_vChap.size();++nChap)
			{
				CDBChap & DBChap = DBBook.m_vChap[nChap];
				for (int nVerse = 0;nVerse < (int)DBChap.m_vVerse.size();++nVerse)
				{
					CDBVerse & DBVerse = DBChap.m_vVerse[nVerse];
					CString csVerse;
					InVerse.Decrypt(DBVerse.m_vecCryptVerse,csVerse);
					DBVerse.m_vecCryptVerse.clear();
					OutVerse.Encrypt(csVerse,DBVerse.m_vecCryptVerse);
				}
			}
		}
	}

	try
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		if (Bible.m_bLoaded)
		{
			// Serialize to disk
			{
				CFile TempFile;
				TempFile.Open("Temp.bdb",CFile::modeCreate|CFile::modeWrite);
				CArchive arBibleFile(&TempFile,CArchive::store);
				CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
				DBBible.Serialize(arBibleFile);
			}

			// Encode the data
			CFile TempBibleFile;
			TempBibleFile.Open("Temp.bdb",CFile::modeRead);
			DWORD dwLength = (DWORD)TempBibleFile.GetLength();
			char * pBufferIn = new char[dwLength];
			TempBibleFile.Read(pBufferIn,dwLength);
			TempBibleFile.Close();
			DWORD dwDestLength = 0;
			char * pBufferOut = NULL;
			CArithmeticEncoder AC;
			AC.EncodeBuffer((char *)pBufferIn,dwLength,&pBufferOut,&dwDestLength);
			delete [] pBufferIn;

			// Write the final DB for the bible
			CFile CompFile;
			CString csCompFile;
			csCompFile.Format("EC%s.bdb",Bible.m_csTranslation);
			CompFile.Open(csCompFile,CFile::modeCreate|CFile::modeWrite);
			CompFile.Write(pBufferOut,dwDestLength);
			delete [] pBufferOut;
		}

		// Delete the temporary file
		CFile::Remove("Temp.bdb");
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

// Get the bible for the translation
BOOL CProjectApp::GetBible(int iTranslation,CDBBible & DBBible)
{
	BOOL bRet = FALSE;
	int nBibles = (int)m_DBTrans.m_vBible.size();
	if (iTranslation <= nBibles)
	{
		DBBible = m_DBTrans.m_vBible[iTranslation - 1];
		bRet = TRUE;
	}
	return bRet;
}

CString CProjectApp::LookupDB(int iTranslation,int iBook,int iChapter,int iVerseFrom,int iVerseTo)
{
	CProjectDlg * pDlg = ((CProjectDlg *)m_pMainWnd);
	CString csVerse;
	csVerse.Format("%s 0x%02d%02d%02d%02d%02d",QBString(225),iTranslation,iBook,iChapter,iVerseFrom,iVerseTo);
	if (iTranslation < 1 || iTranslation > (int)m_vBibles.size())
		return csVerse;
	CBible & Bible = m_vBibles[iTranslation - 1];
	if (!Bible.m_bLoaded)
	{
		CWaitCursor wait;
		if (!LoadTranslation(iTranslation))
			return csVerse;
		pDlg->m_VerseList.ResizeColumns();
		pDlg->m_VerseList.RedrawWindow();
	}
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	int nBooks = (int)DBBible.m_vBook.size();
	if (iBook < 1 || iBook > nBooks)
		return csVerse;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return csVerse;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (iVerseFrom < 1 || iVerseFrom > (int)DBChap.m_vVerse.size())
		return csVerse;
	if (iVerseTo < 1 || iVerseTo < iVerseFrom || iVerseTo > (int)DBChap.m_vVerse.size())
		return csVerse;
	csVerse.Empty();
	CCrypt Crypt((BYTE*)Bible.m_csDescription.GetBuffer(),Bible.m_csDescription.GetLength());
	int c;
	for (int iVerse = iVerseFrom;iVerse <= iVerseTo;++iVerse)
	{
		CString cs;
		CDBVerse & DBVerse = DBChap.m_vVerse[iVerse - 1];
		Crypt.Decrypt(DBVerse.m_vecCryptVerse,cs);
		c = cs.GetAt(0);
		if (c >= 0 && c < 256)
		{
			if (isalpha(c) && islower(c))
			{
				c = toupper(c);
				cs.SetAt(0,c);
			}
		}
		cs.Trim();
		if (csVerse.GetLength())
			csVerse += " ";
		csVerse += cs;
	}
	while (csVerse.Find("  ") != -1)
		csVerse.Replace("  "," ");
	return csVerse;
}

int CProjectApp::LookupNBTrans()
{
	return (int)m_vBibles.size();
}

CString CProjectApp::LookupTranslationName(int iTranslation)
{
	CBible & Bible = m_vBibles[iTranslation - 1];
	return Bible.m_csDescription;
}

CString CProjectApp::LookupTranslationAbbr(int iTranslation)
{
	CBible & Bible = m_vBibles[iTranslation - 1];
	return Bible.m_csTranslation;
}

bool CProjectApp::SearchTranslationNames(CString & csText,CString & csTranslationName)
{
	bool bRet = false;
	int nTranslations = (int)m_vBibles.size();
	for (int iTranslation = 1;iTranslation <= nTranslations;++iTranslation)
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CString csSearch = csText;
		csSearch.MakeLower();
		CString csBibleTranslationName = Bible.m_csDescription;
		csBibleTranslationName.MakeLower();
		if (csBibleTranslationName.Find(csSearch) == 0)
		{
			csTranslationName = Bible.m_csDescription;
			bRet = true;
			break;
		}
	}
	return bRet;
}

bool CProjectApp::MatchTranslationNames(CString & csText)
{
	int nTranslations = (int)m_vBibles.size();
	for (int iTranslation = 1;iTranslation <= nTranslations;++iTranslation)
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CString csSearch = csText;
		csSearch.MakeLower();
		CString csBibleTranslationName = Bible.m_csDescription;
		csBibleTranslationName.MakeLower();
		if (csBibleTranslationName == csSearch)
			return true;
	}
	return false;
}

int CProjectApp::LookupNBBooks(int iTranslation)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return 0;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	return (int)DBBible.m_vBook.size();
}

CString CProjectApp::LookupBookName(int iTranslation,int iBook)
{
	CString csRet;
	csRet.Format("Book %d",iBook);
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return csRet;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return csRet;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (DBBook.m_vecCryptName.size())
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
		Crypt.Decrypt(DBBook.m_vecCryptName,csRet);
	}
	return csRet;
}

CString CProjectApp::LookupChapterName(int iTranslation,int iBook,int iChapter)
{
	CString csRet;
	csRet.Format("Chapter %d",iChapter);
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return csRet;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return csRet;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return csRet;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (DBChap.m_vecCryptName.size())
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
		Crypt.Decrypt(DBChap.m_vecCryptName,csRet);
	}
	return csRet;
}

CString CProjectApp::LookupVerseName(int iTranslation,int iBook,int iChapter,int iVerse)
{
	CString csRet;
	csRet.Format("Verse %d",iVerse);
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return csRet;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return csRet;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return csRet;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (iVerse < 1 || iVerse > (int)DBChap.m_vVerse.size())
		return csRet;
	CDBVerse & DBVerse = DBChap.m_vVerse[iVerse - 1];
	if (DBVerse.m_vecCryptName.size())
	{
		CBible & Bible = m_vBibles[iTranslation - 1];
		CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
		Crypt.Decrypt(DBVerse.m_vecCryptName,csRet);
	}
	return csRet;
}

bool CProjectApp::SearchBookNames(int iTranslation,CString & csText,CString & csBookName)
{
	bool bRet = false;
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return bRet;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	CString csSearch = csText;
	csSearch.MakeLower();
	int nBooks = (int)DBBible.m_vBook.size();
	for (int iBook = 1;iBook <= nBooks;++iBook)
	{
		CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
		CString csBook;
		Crypt.Decrypt(DBBook.m_vecCryptName,csBook);
		CString csBibleBookName = csBook;
		csBibleBookName.MakeLower();
		if (csBibleBookName.Find(csSearch) == 0)
		{
			csBookName = csBook;
			bRet = true;
			break;
		}
	}
	return bRet;
}

int CProjectApp::LookupNBChap(int iTranslation,int iBook)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return 0;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return 0;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	return (int)DBBook.m_vChap.size();
}

int CProjectApp::LookupNBVerses(int iTranslation,int iBook,int iChapter)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return 0;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return 0;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return 0;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	return (int)DBChap.m_vVerse.size();
}

// The the index of the translation
int CProjectApp::LookupTranslationIndex(CString & csSelection)
{
	int iIndex = 1;
	int nTranslations = (int)m_vBibles.size();
	for (int iTranslation = 1;iTranslation <= nTranslations;++iTranslation)
	{
		CString csTranslationName = LookupTranslationName(iTranslation);
		if (csTranslationName == csSelection)
			return iTranslation;
	}
	return 0;
}

// Lookup a book index by book name
int CProjectApp::LookupBookIndex(int iTranslation,CString & csBookName)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return 0;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	int nBooks = (int)DBBible.m_vBook.size();
	for (int iBook = 1;iBook <= nBooks;++iBook)
	{
		CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
		CString csBook;
		Crypt.Decrypt(DBBook.m_vecCryptName,csBook);
		if (csBook.CompareNoCase(csBookName) == 0)
			return iBook;
	}
	return 0;
}

// Add a new book, by name, to a translation
BOOL CProjectApp::AddBookName(int iTranslation,CString & csBook)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	CDBBook DBBook;
	Crypt.Encrypt(csBook,DBBook.m_vecCryptName);
	DBBible.m_vBook.push_back(DBBook);
	return TRUE;
}

// Rename a book, by index, to a new book name
BOOL CProjectApp::RenameBookIndex(int iTranslation,int iBook,CString & csBook)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	Crypt.Encrypt(csBook,DBBook.m_vecCryptName);
	return TRUE;
}

// Delete a book, by index, from a translation
BOOL CProjectApp::DeleteBookIndex(int iTranslation,int iBook)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	std::vector<CDBBook> & vBook = DBBible.m_vBook;
	vBook.erase(vBook.begin() + iBook - 1);
	return TRUE;
}

// Add a chapter, by book index and chapter name
BOOL CProjectApp::AddChapterName(int iTranslation,int iBook,CString & csChapter)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	CDBChap DBChap;
	Crypt.Encrypt(csChapter,DBChap.m_vecCryptName);
	DBBook.m_vChap.push_back(DBChap);
	return TRUE;
}

// Rename a chapter, by book and chapter index
BOOL CProjectApp::RenameChapterIndex(int iTranslation,int iBook,int iChapter,CString & csChapter)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	Crypt.Encrypt(csChapter,DBChap.m_vecCryptName);
	return TRUE;
}

// Delete a chapter, by index, from a book and translation
BOOL CProjectApp::DeleteChapterIndex(int iTranslation, int iBook, int iChapter)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	std::vector<CDBChap> & vChap = DBBook.m_vChap;
	vChap.erase(vChap.begin() + iChapter - 1);
	return TRUE;
}

// Add a verse, by book and chapter index
BOOL CProjectApp::AddVerseName(int iTranslation,int iBook,int iChapter,CString & csVerse)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	CDBVerse DBVerse;
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	Crypt.Encrypt(csVerse,DBVerse.m_vecCryptName);
	DBChap.m_vVerse.push_back(DBVerse);
	return TRUE;
}

// Add a verse, by book and chapter index
BOOL CProjectApp::RenameVerseIndex(int iTranslation,int iBook,int iChapter,int iVerse,CString & csVerse)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (iVerse < 1 || iVerse > (int)DBChap.m_vVerse.size())
		return FALSE;
	CDBVerse & DBVerse = DBChap.m_vVerse[iVerse - 1];
	CCrypt Crypt((BYTE *)Bible.m_csTranslation.GetBuffer(),(DWORD)Bible.m_csTranslation.GetLength());
	Crypt.Encrypt(csVerse,DBVerse.m_vecCryptName);
	return TRUE;
}

// Delete a verse, by index, from a chapter, book and translation
BOOL CProjectApp::DeleteVerseIndex(int iTranslation, int iBook, int iChapter, int iVerse)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (iVerse < 1 || iVerse > (int)DBChap.m_vVerse.size())
		return FALSE;
	std::vector<CDBVerse> & vVerse = DBChap.m_vVerse;
	vVerse.erase(vVerse.begin() + iVerse - 1);
	return TRUE;
}

// Update a verse, by verse, book, and chapter index
BOOL CProjectApp::UpdateVerseIndex(int iTranslation,int iBook,int iChapter,int iVerse,CString & csVerseText)
{
	if (iTranslation < 1 || iTranslation > (int)m_DBTrans.m_vBible.size())
		return FALSE;
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	if (iBook < 1 || iBook > (int)DBBible.m_vBook.size())
		return FALSE;
	CDBBook & DBBook = DBBible.m_vBook[iBook - 1];
	if (iChapter < 1 || iChapter > (int)DBBook.m_vChap.size())
		return FALSE;
	CDBChap & DBChap = DBBook.m_vChap[iChapter - 1];
	if (iVerse < 1 || iVerse > (int)DBChap.m_vVerse.size())
		return FALSE;
	CDBVerse & DBVerse = DBChap.m_vVerse[iVerse - 1];
	CCrypt Crypt((BYTE *)Bible.m_csDescription.GetBuffer(),Bible.m_csDescription.GetLength());
	Crypt.Encrypt(csVerseText,DBVerse.m_vecCryptVerse);
	return TRUE;
}

// Lookup the longest verse to help set an intelligent default for the text size
CVerse CProjectApp::LookupLongestVerse(int iTranslation)
{
	CBible & Bible = m_vBibles[iTranslation - 1];
	CDBBible & DBBible = m_DBTrans.m_vBible[iTranslation - 1];
	for (int nBook = 0;nBook < (int)DBBible.m_vBook.size();++nBook)
	{
		CDBBook & DBBook = DBBible.m_vBook[nBook];
		for (int nChap = 0;nChap < (int)DBBook.m_vChap.size();++nChap)
		{
			CDBChap & DBChap = DBBook.m_vChap[nChap];
			for (int nVerse = 0;nVerse < (int)DBChap.m_vVerse.size();++nVerse)
			{
				CDBVerse & DBVerse = DBChap.m_vVerse[nVerse];
				int nLength = (int)DBVerse.m_vecCryptVerse.size();
				if (nLength > Bible.m_nMaxVerseLength)
				{
					// Store the longest verse for the translation
					Bible.m_iMaxTranslation = iTranslation;
					Bible.m_iMaxBook = nBook + 1;
					Bible.m_iMaxChapter = nChap + 1;
					Bible.m_iMaxVerse = nVerse + 1;
					Bible.m_nMaxVerseLength = nLength + 1;
				}
			}
		}
	}
	
	// Lookup the verse
	CVerse Verse;
	Verse.m_iTranslation = Bible.m_iMaxTranslation;
	Verse.m_iBook = Bible.m_iMaxBook;
	Verse.m_iChapter = Bible.m_iMaxChapter;
	Verse.m_iVerseFrom = Bible.m_iMaxVerse;
	Verse.m_iVerseTo = Bible.m_iMaxVerse;
	Verse.LookupVerse();

	// Return the longest verse
	return Verse;
}

bool CProjectApp::SearchVerse(int iTranslation,int iBook,int iChapter,CString csSearch,vector<CVerse> & Results)
{
	bool bFound = false;
#if !defined(LITE)
	CProjectDlg * pDlg = ((CProjectDlg *)m_pMainWnd);
	csSearch.MakeLower();

	// Set up the translations to search
	int nBegTrans = iTranslation,nEndTrans = iTranslation;
	if (iTranslation < 1)
	{
		nBegTrans = 1;
		nEndTrans = (int)m_DBTrans.m_vBible.size();
	}

	// Iterate over the translations in the search
	for (int jTranslation = nBegTrans;jTranslation <= nEndTrans;++jTranslation)
	{
		// Load a translation
		CBible & Bible = m_vBibles[jTranslation - 1];
		if (!Bible.m_bLoaded)
		{
			CWaitCursor wait;
			if (!LoadTranslation(jTranslation))
				return bFound;
			m_pVerseFinderDlg->BringWindowToTop();
			m_pVerseFinderDlg->RedrawWindow();
			pDlg->m_VerseList.ResizeColumns();
			pDlg->m_VerseList.RedrawWindow();
		}

		// Initialize the decrypter
		CCrypt Crypt((BYTE*)Bible.m_csDescription.GetBuffer(),Bible.m_csDescription.GetLength());

		// Setup the books to search
		int nBegBook = iBook,nEndBook = iBook;
		if (iBook < 1)
		{
			CDBBible & DBBible = m_DBTrans.m_vBible[jTranslation - 1];
			nBegBook = 1;
			nEndBook = (int)DBBible.m_vBook.size();
		}

		// Iterate over the books in the search
		for (int jBook = nBegBook;jBook <= nEndBook;++jBook)
		{
			// Setup the chapters to search
			int nBegChap = iChapter,nEndChap = iChapter;
			if (iChapter < 1)
			{
				CDBBible & DBBible = m_DBTrans.m_vBible[jTranslation - 1];
				CDBBook & DBBook = DBBible.m_vBook[jBook - 1];
				nBegChap = 1;
				nEndChap = (int)DBBook.m_vChap.size();
			}

			// Iterate over the chapters in the search
			for (int jChapter = nBegChap;jChapter <= nEndChap;++jChapter)
			{
				// Set up the verse to search
				CDBBible & DBBible = m_DBTrans.m_vBible[jTranslation - 1];
				CDBBook & DBBook = DBBible.m_vBook[jBook - 1];
				CDBChap & DBChap = DBBook.m_vChap[jChapter - 1];
				int nBegVerse = 1,nEndVerse = (int)DBChap.m_vVerse.size();
				
				// Initialize the verse string length vector
				vector<int> vPos(nEndVerse + 1);
				vPos[0] = 0;

				// Iterate over the verse to search
				CString csVerse;
				for (int jVerse = nBegVerse;jVerse <= nEndVerse;++jVerse)
				{
					CString cs;
					CDBVerse & DBVerse = DBChap.m_vVerse[jVerse - 1];
					Crypt.Decrypt(DBVerse.m_vecCryptVerse,cs);
					csVerse += cs;
					
					// Update the verse string length vector
					int nLastLength = vPos[jVerse - 1];
					int nThisLength = cs.GetLength();
					vPos[jVerse] = nLastLength + nThisLength;
				}

				// Search for the phrase
				csVerse.MakeLower();

				int iPos = csVerse.Find(csSearch);
				if (iPos != -1)
				{
					for (int jVerse = nBegVerse;jVerse <= nEndVerse;++jVerse)
					{
						int nThisPos = jVerse - 1;
						int nThisLength = vPos[nThisPos];
						int nNextPos = jVerse;
						int nNextLength = vPos[nNextPos];
						if (iPos >= nThisLength && iPos < nNextLength)
						{
							CVerse Verse;
							Verse.m_iTranslation = jTranslation;
							Verse.m_iBook = jBook;
							Verse.m_iChapter = jChapter;
							Verse.m_iVerseFrom = jVerse;
							Verse.m_iVerseTo = jVerse;
							Verse.m_csVerse = Verse.LookupVerse();
							Results.push_back(Verse);
							
							CString csSearchResults;
							csSearchResults.Format(QBString(189),(int)Results.size());
							m_pVerseFinderDlg->SetWindowText(csSearchResults);
						}
					}
					bFound = true;
				}
			}
		}
	}
#endif
	return bFound;
}

// Get the list based on the information
void CProjectApp::GetList(CStringArray & csaList,CString csTranslationName,CString csBookName,CString csChapter)
{
	if (csTranslationName.IsEmpty())
	{
		int nbTrans = g_pApp->LookupNBTrans();
		for (int iTrans = 1;iTrans <= nbTrans;++iTrans)
		{
			CString csTranslationName = g_pApp->LookupTranslationName(iTrans);
			csaList.Add(csTranslationName);
		}
	}
	else if (csBookName.IsEmpty())
	{
		int iTrans = g_pApp->LookupTranslationIndex(csTranslationName);
		if (iTrans)
		{
			int nbBooks = g_pApp->LookupNBBooks(iTrans);
			for (int iBook = 1;iBook <= nbBooks;++iBook)
			{
				CString csBook = g_pApp->LookupBookName(iTrans,iBook);
				csaList.Add(csBook);
			}
		}
	}
	else if (csChapter.IsEmpty())
	{
		int iTrans = g_pApp->LookupTranslationIndex(csTranslationName);
		if (iTrans)
		{
			int iBook = g_pApp->LookupBookIndex(iTrans,csBookName);
			if (iBook)
			{
				int nbChap = g_pApp->LookupNBChap(iTrans,iBook);
				CString csChapNo;
				for (int iChap = 1;iChap <= nbChap;++iChap)
				{
					csChapNo.Format("%d",iChap);
					csaList.Add(csChapNo);
				}
			}
		}
	}
	else // Verse list
	{
		int iTrans = g_pApp->LookupTranslationIndex(csTranslationName);
		if (iTrans)
		{
			int iBook = g_pApp->LookupBookIndex(iTrans,csBookName);
			if (iBook)
			{
				int iChapter = atoi(csChapter);
				int nbVerses = g_pApp->LookupNBVerses(iTrans,iBook,iChapter);
				CString csVerseNo;
				for (int iVerse = 1;iVerse <= nbVerses;++iVerse)
				{
					csVerseNo.Format("%d",iVerse);
					csaList.Add(csVerseNo);
				}
			}
		}
	}
}

// Load a resource string
CString CProjectApp::QBString(int iId)
{
	CString cs;
	if (m_hResDLL)
	{
		HRSRC hResInfo = NULL;
		hResInfo = FindResource(m_hResDLL,MAKEINTRESOURCE((iId >> 4) + 1),RT_STRING);
		if (hResInfo)
		{
			// Load the resource
			HGLOBAL hg = LoadResource(m_hResDLL,hResInfo);
			wchar_t * pRes = (wchar_t *)LockResource(hg);
			if (pRes)
			{
				// Calculate the offset into string section
				UINT nBaseResID = iId & 0xfffffff0;
				UINT nOffset = iId - nBaseResID;
				for (UINT i = 0; i < nOffset; ++i)
					pRes += *pRes + 1;

				// Calculate the string length
				int len = *pRes;
				++pRes;

				// Get the string
				wchar_t * psz = new wchar_t[len + 1];
				wcsncpy(psz, pRes, len);
				psz[len] = 0;

				// Assign the string
				cs = psz;

				// Cleanup the string
				delete [] psz;

				// Cleanup the resource
				UnlockResource(hg);
				FreeResource(hg);
			}
		}
	}
	return cs;
}

CString CProjectApp::GetVersionNumber()
{
	CString csVersion;
	VS_FIXEDFILEINFO FFI;
	memset(&FFI,NULL,sizeof(VS_FIXEDFILEINFO));

	auto_ptr<char> ModuleFileName(new char[MAX_PATH + 1]);
	char * pModuleFileName = ModuleFileName.get();
    GetModuleFileName(0,pModuleFileName,MAX_PATH);
	HMODULE hModule = ::GetModuleHandle(pModuleFileName);
	GetModuleFileName(hModule,pModuleFileName,MAX_PATH);
	DWORD dw = 0,dwLen = 0;
	dwLen = GetFileVersionInfoSize(pModuleFileName,&dw);

	auto_ptr<BYTE> VersionInfo(new BYTE[dwLen]);
	BYTE * pVersionInfo = VersionInfo.get();
	GetFileVersionInfo(pModuleFileName,0,dwLen,pVersionInfo);

	// Get translation info
	struct TRANSLATION
	{
		WORD wLangID;
		WORD wCodePage;
	} Translation;

	void * lpvi = NULL;
	UINT uiLen = 0;
	VerQueryValue(pVersionInfo,"\\VarFileInfo\\Translation",&lpvi,&uiLen);
	Translation = *(TRANSLATION*)lpvi;

	// Get the product version
	CString csProductVersionQuery,csProductVersion;
	csProductVersionQuery.Format(_T("\\StringFileInfo\\%04x%04x\\ProductVersion"),Translation.wLangID,Translation.wCodePage);

	LPCTSTR pVal;
	if (VerQueryValue(pVersionInfo,(LPTSTR)(LPCTSTR)csProductVersionQuery,(void**)&pVal,&uiLen))
		csVersion = pVal;
	return csVersion;
}

void CProjectApp::GetLanguageEdition(CString & csLanguage,CString & csEdition)
{
#if defined(ENGLISH)
	csLanguage = g_pApp->QBString(293);
#endif

#if defined(FRENCH)
	csLanguage = g_pApp->QBString(294);
#endif

#if defined(GERMAN)
	csLanguage = g_pApp->QBString(295);
#endif

#if defined(SPANISH)
	csLanguage = g_pApp->QBString(296);
#endif

#if defined(ITALIAN)
	csLanguage = g_pApp->QBString(297);
#endif

#if defined(LATIN)
	csLanguage = g_pApp->QBString(298);
#endif

#if defined(FULL)
	csEdition = g_pApp->QBString(288);
#endif

#if defined(BASIC)
	csEdition = g_pApp->QBString(289);
#endif

#if defined(PERSONAL)
	csEdition = g_pApp->QBString(290);
#endif

#if defined(LITE)
	csEdition = g_pApp->QBString(291);
#endif

#if defined(DEMO)
	csEdition = g_pApp->QBString(292);
#endif

#if defined(_DEBUG) || defined(_RELEASE)
	csEdition = g_pApp->QBString(259);
#endif
}

// Get the registration path
CString CProjectApp::GetRegistrationPath()
{
	CString csRegistrationPath;
	char * pRegistrationPath = csRegistrationPath.GetBufferSetLength(_MAX_PATHNAME);
	GetModuleFileName(0,pRegistrationPath,_MAX_PATHNAME);
	csRegistrationPath.ReleaseBuffer();
	int iPos = csRegistrationPath.ReverseFind('\\');
	if (iPos != -1)
		csRegistrationPath = csRegistrationPath.Mid(0,iPos);
	return csRegistrationPath;
}

CString CProjectApp::GetSpecialFolderPath(int nFolder)
{
	CString csPath;
	char * pPath = csPath.GetBufferSetLength(_MAX_PATHNAME);
	SHGetSpecialFolderPath(NULL,pPath,nFolder,FALSE);
	csPath.ReleaseBuffer();
	return csPath;
}

// Generate a temporary file name
CString CProjectApp::GetTempFileName(const char * pName,const char * pExt)
{
	// Get the users application data directory
	CString csPath = GetSpecialFolderPath(CSIDL_LOCAL_APPDATA);

	// Get the short name version
	CString csShortPath;
	GetShortPathName(csPath,csShortPath.GetBufferSetLength(_MAX_PATHNAME),_MAX_PATHNAME);
	csShortPath.ReleaseBuffer();

	// File name
	if (pName == NULL)
	{
		int fs = _MAX_PATHNAME;
		std::auto_ptr<char> Buffer(new char[fs + 1]);
		char * pBuffer = Buffer.get();
		tmpnam(pBuffer);
		csShortPath += pBuffer;
	}
	else
	{
		csShortPath += "\\";
		csShortPath += pName;
		csShortPath += ".";
	}

	// File extension
	if (pExt == NULL)
		csShortPath += "tmp";
	else
		csShortPath += pExt;

	return csShortPath;
}

// Get the position of the first selected verse
POSITION CProjectApp::GetFirstSelectedItemPosition()
{
	POSITION Pos = NULL;
	for (int iPos = 0;iPos < (int)m_vecVerses.size();++iPos)
	{
		if (m_vecVerses[iPos].m_bSelected)
		{
			// Set the first "previously" found selection
			Pos = (POSITION)(INT_PTR)(iPos + 1);
			break;
		}
	}
	return Pos;
}

// Get the position of the next selected verse
int CProjectApp::GetNextSelectedItem(POSITION & Pos)
{
	// Get the "previously" found selection 
	int iVerse = (int)(INT_PTR)(Pos - 1);

	// Update the position
	Pos = NULL;
	for (int iPos = iVerse + 1;iPos < (int)m_vecVerses.size();++iPos)
	{
		if (m_vecVerses[iPos].m_bSelected)
		{
			// Set the "previously" found selection (1-based)
			Pos = (POSITION)(INT_PTR)(iPos + 1);
			break;
		}
	}
	return iVerse;
}

// Set the selection property
BOOL CProjectApp::SetItemState(int iVerse,int iSelected)
{
	// Boundary condition
	if (iVerse < (int)m_vecVerses.size())
	{
		// Set the selection property
		CVerse & Verse = m_vecVerses[iVerse];
		Verse.m_bSelected = iSelected ? TRUE : FALSE;
	}
	return TRUE;
}