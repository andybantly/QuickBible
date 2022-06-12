// Project.h : main header file for the QuickBible application
// Copyright (C) 2007 Andrew S. Bantly
//

#if !defined(AFX_PROJECT_H__D70C9F65_EF99_40E0_BA13_B9D8388ECA4C__INCLUDED_)
#define AFX_PROJECT_H__D70C9F65_EF99_40E0_BA13_B9D8388ECA4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "DBFormat.h"
#include "Preview.h"
#include "VerseFinder.h"
#include "Verse.h"
#include "ManageContentDlg.h"
#include <functional>
#include <intrin.h>
#include <memory>
#include "resource.h"		// main symbols

BOOL CALLBACK EnumDisplayMonitorsProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);

class CBible
{
public:
	CBible() : m_iMaxTranslation(0), m_iMaxBook(0), m_iMaxChapter(0), m_iMaxVerse(0), m_nMaxVerseLength(0) {}
	CBible(const char * pDescription,const char * pTranslation) : m_iMaxTranslation(0), m_iMaxBook(0), m_iMaxChapter(0), m_iMaxVerse(0), m_nMaxVerseLength(0)
	{
		m_csDescription = pDescription;
		m_csTranslation = pTranslation;
		CString csDBPath;
		char * pDBPath = csDBPath.GetBufferSetLength(_MAX_PATHNAME);
		if (GetModuleFileName(0,pDBPath,_MAX_PATHNAME) != 0)
		{
			CString csDrive,csDir,csFname,csExt;
			_splitpath(pDBPath,csDrive.GetBufferSetLength(_MAX_DRIVE),csDir.GetBufferSetLength(_MAX_DIR),csFname.GetBufferSetLength(_MAX_FNAME),csExt.GetBufferSetLength(_MAX_EXT));
			csDBPath.ReleaseBuffer();
			csDrive.ReleaseBuffer();
			csDir.ReleaseBuffer();
			csFname.ReleaseBuffer();
			csExt.ReleaseBuffer();
/*#if defined(_DEBUG) || defined(_RELEASE)
			csDir = "\\Users\\Andy\\Documents\\BDB\\";
#endif*/
			m_csTransFile.Format("%s%sEC%s.bdb",csDrive,csDir,m_csTranslation);
		}
		m_bLoaded = false;
	}
	CBible(const CBible & rhs) {*this = rhs;}
	CBible & operator = (const CBible & rhs)
	{
		if (this != &rhs)
		{
			m_csDescription = rhs.m_csDescription;
			m_csTranslation = rhs.m_csTranslation;
			m_csTransFile = rhs.m_csTransFile;
			m_bLoaded = rhs.m_bLoaded;
			m_nMaxVerseLength = rhs.m_nMaxVerseLength;
			m_iMaxTranslation = rhs.m_iMaxTranslation;
			m_iMaxBook = rhs.m_iMaxBook;
			m_iMaxChapter = rhs.m_iMaxChapter;
			m_iMaxVerse = rhs.m_iMaxVerse;
		}
		return *this;
	}
	CString m_csDescription;
	CString m_csTranslation;
	CString m_csTransFile;
	int m_nMaxVerseLength;
	int m_iMaxTranslation;
	int m_iMaxBook;
	int m_iMaxChapter;
	int m_iMaxVerse;
	bool m_bLoaded;
};

/////////////////////////////////////////////////////////////////////////////
// CProjectApp:
// See Project.cpp for the implementation of this class
//

class CProjectApp : public CWinApp
{
public:
	CProjectApp();
	~CProjectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	std::vector<CBible> m_vBibles;
	std::vector<CRect> m_vMonRect;
	bool Initialize();
	bool IsInstalled();
	CString GetInstallString();
	bool IsInstalled(CString & csGUID);
	bool IsRegistered();
	DWORD VolSN();
	DWORD CPUid();
	CString GetUID();

	// Preview Window
	void UpdateLastFrame(bool bPreview);
	void UpdateSearchPreview(CVerse & Verse);

	// Looking up verses
	bool IsLoaded(int iTranslation);
	BOOL LoadTranslation(int iTranslation);
	BOOL SaveTranslation(CBible & Bible);
	BOOL GetBible(int iTranslation,CDBBible & DBBible);
	CString LookupDB(int iTranslation,int iBook,int iChapter,int iVerseFrom,int iVerseTo);
	int LookupNBTrans();
	CString LookupTranslationName(int iTranslation);
	CString LookupTranslationAbbr(int iTranslation);
	bool SearchTranslationNames(CString & csText,CString & csTranslationName);
	bool MatchTranslationNames(CString & csText);
	int LookupNBBooks(int iTranslation);
	CString LookupBookName(int iTranslation,int iBook);
	CString LookupChapterName(int iTranslation,int iBook,int iChapter);
	CString LookupVerseName(int iTranslation,int iBook,int iChapter,int iVerse);
	bool SearchBookNames(int iTranslation,CString & csPartialBookName,CString & csBookName);
	int LookupNBChap(int iTranslation,int iBook);
	int LookupNBVerses(int iTranslation,int iBook,int iChapter);
	int LookupTranslationIndex(CString & csSelection);
	int LookupBookIndex(int iTranslation,CString & csBookName);

	BOOL AddBookName(int iTranslation,CString & csBook);
	BOOL RenameBookIndex(int iTranslation,int iBook,CString & csBook);
	BOOL DeleteBookIndex(int iTranslation,int iBook);
	BOOL AddChapterName(int iTranslation,int iBook,CString & csChapter);
	BOOL RenameChapterIndex(int iTranslation,int iBook,int iChapter,CString & csChapter);
	BOOL DeleteChapterIndex(int iTranslation, int iBook, int iChapter);
	BOOL AddVerseName(int iTranslation,int iBook,int iChapter,CString & csVerse);
	BOOL DeleteVerseIndex(int iTranslation, int iBook, int iChapter, int iVerse);
	BOOL RenameVerseIndex(int iTranslation,int iBook,int iChapter,int iVerse,CString & csVerse);
	BOOL UpdateVerseIndex(int iTranslation,int iBook,int iChapter,int iVerse,CString & csVerseText);

	CVerse LookupLongestVerse(int iTranslation);
	bool SearchVerse(int iTranslation,int iBook,int iChapter,CString csSearch,std::vector<CVerse> & Results);
	void GetList(CStringArray & csaList,CString csTranslationName = CString(),CString csBookName = CString(),CString csChapter = CString());
	void GetShellCommand();

	// Load a resource string
	CString QBString(int iId);

	// Lookup version and registration information
	CString GetVersionNumber();
	void GetLanguageEdition(CString & csLanguage,CString & csEdition);
	CString GetRegistrationPath();

	// Get paths and temporary file names
	CString GetSpecialFolderPath(int nFolder);
	CString GetTempFileName(const char * pName = NULL,const char * pExt = NULL);

	//{{AFX_MSG(CProjectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CDBTranslation m_DBTrans;

public:
	CPreviewDlg * m_pPreviewDlg;
#if !defined(LITE)
	CVerseFinderDlg * m_pVerseFinderDlg;
#endif
#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
	CManageContentDlg * m_pManageContentDlg;
#endif
	// The list of verses
	std::vector<CVerse> m_vecVerses;

public:
	POSITION GetFirstSelectedItemPosition();
	int GetNextSelectedItem(POSITION & Pos);
	BOOL SetItemState(int iVerse,int iSelected);

private:
	HMODULE m_hResDLL;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECT_H__D70C9F65_EF99_40E0_BA13_B9D8388ECA4C__INCLUDED_)
