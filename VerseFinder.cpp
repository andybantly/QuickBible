// VerseFinder.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#if !defined(LITE)
#include "Project.h"
#include "VerseFinder.h"
#include "Verse.h"
#include <vector>
using namespace std;

extern CProjectApp * g_pApp;

// CVerseFinder dialog

IMPLEMENT_DYNAMIC(CVerseFinderDlg, CDialog)
CVerseFinderDlg::CVerseFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerseFinderDlg::IDD, pParent)
	, m_csSearch(_T("")), m_csVerse(_T("")), m_csTranslation(_T("")), m_csBook(_T("")), m_csVerseHeader(_T(""))
{
	m_hIcon = g_pApp->LoadIcon(IDR_MAINFRAME);
	m_pParent = pParent;
	m_nID = CVerseFinderDlg::IDD;
}

CVerseFinderDlg::~CVerseFinderDlg()
{
}

BOOL CVerseFinderDlg::Create()
{
	return CDialog::Create(m_nID,m_pParent);
}

void CVerseFinderDlg::Destroy()
{
	DestroyWindow();
}

void CVerseFinderDlg::PostNcDestroy()
{
	delete this;
}

void CVerseFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRANSLATION, m_Translation);
	DDX_Control(pDX, IDC_COMBO_BOOK, m_Book);
	DDX_Control(pDX, IDC_COMBO_CHAPTER, m_Chapter);
	DDX_Control(pDX, IDC_EDIT_SEARCHPHRASE, m_Search);
	DDX_Text(pDX, IDC_EDIT_SEARCHPHRASE, m_csSearch);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_Results);
	DDX_Text(pDX, IDC_EDIT_VERSE, m_csVerse);
	DDX_CBString(pDX, IDC_COMBO_TRANSLATION, m_csTranslation);
	DDX_CBString(pDX, IDC_COMBO_BOOK, m_csBook);
	DDX_Text(pDX, IDC_EDIT_VERSEHEADER, m_csVerseHeader);
}

BOOL CVerseFinderDlg::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	// Set the pointer to the main window
	m_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;

	// Set the window text
	SetWindowText(g_pApp->QBString(277));
	GetDlgItem(IDC_STATIC_TRANSLATION)->SetWindowText(g_pApp->QBString(219));
	GetDlgItem(IDC_STATIC_BOOK)->SetWindowText(g_pApp->QBString(220));
	GetDlgItem(IDC_STATIC_CHAPTER)->SetWindowText(g_pApp->QBString(221));
	GetDlgItem(IDC_STATIC_SEARCHPHRASE)->SetWindowText(g_pApp->QBString(282));
	GetDlgItem(IDC_STATIC_RESULTS)->SetWindowText(g_pApp->QBString(283));
	GetDlgItem(IDC_STATIC_VERSE)->SetWindowText(g_pApp->QBString(284));

	m_Ok.CreateText(this,IDOK,g_pApp->QBString(281));
	m_AddSelected.CreateText(this,IDC_ADDSELECTED,g_pApp->QBString(285));
	m_AddAll.CreateText(this,IDC_ADDALL,g_pApp->QBString(286));
	m_PreviewVerse.CreateText(this,IDC_PREVIEWVERSE,g_pApp->QBString(287));

	// Set the icon
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// Give the search box the focus (requires returning FALSE)
	m_Search.SetFocus();

	return FALSE;
}

// Setup the verse finder window
void CVerseFinderDlg::SetupVerseFinder()
{
	// Fill the translation combo box
	CString csItemText;
	int nbTrans = g_pApp->LookupNBTrans();
	for (int iTrans = 1;iTrans <= nbTrans;++iTrans)
	{
		CString csTranslationName = g_pApp->LookupTranslationName(iTrans);
		int iCurSel = m_Translation.AddString(csTranslationName);
	}

	CString csTranslationName;
	m_Translation.GetWindowText(csTranslationName);
}

BEGIN_MESSAGE_MAP(CVerseFinderDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CVerseFinderDlg::OnFind)
	ON_BN_CLICKED(IDCANCEL, &CVerseFinderDlg::OnClose)
	ON_CBN_CLOSEUP(IDC_COMBO_TRANSLATION, &CVerseFinderDlg::OnCloseupTranslation)
	ON_CBN_CLOSEUP(IDC_COMBO_BOOK, &CVerseFinderDlg::OnCloseupBook)
	ON_BN_CLICKED(IDC_ADDSELECTED, &CVerseFinderDlg::OnAddSelected)
	ON_BN_CLICKED(IDC_ADDALL, &CVerseFinderDlg::OnAddAll)
	ON_LBN_SELCHANGE(IDC_LIST_RESULTS, &CVerseFinderDlg::OnResultsSelection)
	ON_CBN_EDITCHANGE(IDC_COMBO_TRANSLATION, &CVerseFinderDlg::OnEditTranslation)
	ON_CBN_EDITCHANGE(IDC_COMBO_BOOK, &CVerseFinderDlg::OnEditBook)
	ON_BN_CLICKED(IDC_PREVIEWVERSE, &CVerseFinderDlg::OnPreviewVerse)
END_MESSAGE_MAP()

void CVerseFinderDlg::OnCloseupTranslation()
{
	CString csTranslationName;
	int iCurSelTrans = m_Translation.GetCurSel();
	if (iCurSelTrans == -1)
	{
		m_Book.ResetContent();
		m_Chapter.ResetContent();
		m_Book.EnableWindow(FALSE);
		m_Chapter.EnableWindow(FALSE);
		return;
	}
	else
		m_Book.EnableWindow();

	// Get the selection text
	m_Translation.GetLBText(iCurSelTrans,csTranslationName);

	// Reset the chapters
	m_Chapter.ResetContent();
	m_Chapter.EnableWindow(FALSE);

	// Fill in the book combo box
	int nbTrans = g_pApp->LookupNBTrans();
	for (int iTrans = 1;iTrans <= nbTrans;++iTrans)
	{
		CString csName = g_pApp->LookupTranslationName(iTrans);
		if (csTranslationName == csName)
		{
			if (!g_pApp->IsLoaded(iTrans))
			{
				CWaitCursor wait;
				if (!g_pApp->LoadTranslation(iTrans))
					return;
			}
			bool bCurSel = false;
			m_Book.ResetContent();
			int nbBooks = g_pApp->LookupNBBooks(iTrans);
			for (int iBook = 1;iBook <= nbBooks;++iBook)
			{
				CString csBook = g_pApp->LookupBookName(iTrans,iBook);
				m_Book.AddString(csBook);
			}
			break;
		}
	}
}

void CVerseFinderDlg::OnEditTranslation()
{
	UpdateData();
	bool bFound = g_pApp->MatchTranslationNames(m_csTranslation);
	if (!bFound)
	{
		m_Book.ResetContent();
		m_Chapter.ResetContent();
		m_Book.EnableWindow(FALSE);
		m_Chapter.EnableWindow(FALSE);
		return;
	}
	else
	{
		int iCurSelTrans = m_Translation.FindString(-1,m_csTranslation);
		if (iCurSelTrans != -1)
		{
			m_Translation.SetCurSel(iCurSelTrans);
			OnCloseupTranslation();
		}
	}
}

void CVerseFinderDlg::OnCloseupBook()
{
	UpdateData();
	int iCurSelTrans = m_Translation.GetCurSel();
	if (iCurSelTrans == -1)
	{
		m_Book.ResetContent();
		m_Chapter.ResetContent();
		m_Book.EnableWindow(FALSE);
		m_Chapter.EnableWindow(FALSE);
		return;
	}
	else
		m_Book.EnableWindow();

	// Fill in the chapter combo box
	int iTrans = g_pApp->LookupTranslationIndex(m_csTranslation);
	if (iTrans)
	{
		int iCurSelBook = m_Book.GetCurSel();
		if (iCurSelBook == -1)
		{
			m_Chapter.ResetContent();
			m_Chapter.EnableWindow(FALSE);
			return;
		}
		else
			m_Chapter.EnableWindow();
		CString csBookName;
		m_Book.GetLBText(iCurSelBook,csBookName);

		CString csBookNo;
		int nbBooks = g_pApp->LookupNBBooks(iTrans);
		for (int iBook = 1;iBook <= nbBooks;++iBook)
		{
			CString csName = g_pApp->LookupBookName(iTrans,iBook);
			if (csBookName == csName)
			{
				m_Chapter.ResetContent();
				int nbChap = g_pApp->LookupNBChap(iTrans,iBook);
				CString csChapNo;
				for (int iChap = 1;iChap <= nbChap;++iChap)
				{
					csChapNo.Format("%d",iChap);
					m_Chapter.AddString(csChapNo);
				}
				break;
			}
		}
	}
}

void CVerseFinderDlg::OnEditBook()
{
	UpdateData();

	// Fill in the chapter combo box
	int iTrans = g_pApp->LookupTranslationIndex(m_csTranslation);
	if (iTrans)
	{
		int iCurSelBook = g_pApp->LookupBookIndex(iTrans,m_csBook);
		if (iCurSelBook)
		{
			m_Book.SetCurSel(iCurSelBook - 1);
			OnCloseupBook();
		}
		else
		{
			m_Chapter.ResetContent();
			m_Chapter.EnableWindow(FALSE);
		}
	}
}

// CVerseFinder message handlers
void CVerseFinderDlg::OnFind()
{
	UpdateData();
	SetWindowText(g_pApp->QBString(218));

	CWaitCursor wait;

	int iTranslation = -1;
	int iBook = -1;
	int iChapter = -1;

	// Get the translation
	CString csTranslationName;
	int iCurSelTrans = m_Translation.GetCurSel();
	if (iCurSelTrans != -1)
	{
		m_Translation.GetLBText(iCurSelTrans,csTranslationName);
		iTranslation = g_pApp->LookupTranslationIndex(csTranslationName);
	}
	
	// Get the book
	CString csBookName;
	int iCurSelBook = m_Book.GetCurSel();
	if (iCurSelBook != -1)
	{
		m_Book.GetLBText(iCurSelBook,csBookName);
		iBook = g_pApp->LookupBookIndex(iTranslation,csBookName);
	}

	// Get the chapter
	int iCurSelChapter = m_Chapter.GetCurSel();
	if (iCurSelChapter != -1)
		iChapter = iCurSelChapter + 1;

	// Check the search text
	if (m_csSearch.GetLength())
	{
		// Clear the previous results
		m_Results.ResetContent();
		m_vResults.clear();

		// Search for the new results
		g_pApp->SearchVerse(iTranslation,iBook,iChapter,m_csSearch,m_vResults);

		if (m_vResults.size())
		{
			int nResults = (int)m_vResults.size();
			for (int iResult = 0;iResult < nResults;++iResult)
			{
				CVerse & Verse = m_vResults[iResult];
				CString csTranslation = g_pApp->LookupTranslationName(Verse.m_iTranslation);
				CString csBook = g_pApp->LookupBookName(Verse.m_iTranslation,Verse.m_iBook);
				CString csResult;
				csResult.Format("%s %s %d:%d-%d",csTranslation,csBook,Verse.m_iChapter,Verse.m_iVerseFrom,Verse.m_iVerseTo);
				m_Results.AddString(csResult);
			}
		}
	}
}

void CVerseFinderDlg::OnClose()
{
	ShowWindow(SW_HIDE);
}

// Add only the selected verses
void CVerseFinderDlg::OnAddSelected()
{
	int nResults = (int)m_vResults.size();
	for (int iResult = 0;iResult < nResults;++iResult)
	{
		if (m_Results.GetSel(iResult) > 0)
		{
			// Add this verse
			CVerse & Verse = m_vResults[iResult];
			m_pDlg->m_VerseList.AddExistingVerse(Verse);

			// Select the next verse
			iResult++;
			if (iResult == nResults)
				iResult = 0;
			m_Results.SetCurSel(iResult);
			OnResultsSelection();
			break;
		}
	}

	// Make sure the last verse is selected
	m_pDlg->m_VerseList.EnsureVisible(nResults - 1,FALSE);

	// Redraw the settings view
	m_pDlg->m_VerseList.UpdateSettingsView();

	// Highlight the last verse
	m_pDlg->m_VerseList.SetFocus();
	m_pDlg->HighlightLastVerse();
}

// Add all the found verses
void CVerseFinderDlg::OnAddAll()
{
	int nResults = (int)m_vResults.size();
	for (int iResult = 0;iResult < nResults;++iResult)
	{
		CVerse & Verse = m_vResults[iResult];
		m_pDlg->m_VerseList.AddExistingVerse(Verse);
	}

	// Make sure the last verse is selected
	m_pDlg->m_VerseList.EnsureVisible(nResults - 1,FALSE);

	// Redraw the settings view
	m_pDlg->m_VerseList.UpdateSettingsView();

	// Highlight the last verse
	m_pDlg->m_VerseList.SetFocus();
	m_pDlg->HighlightLastVerse();
}

// Show the selected verse in the preview box
void CVerseFinderDlg::OnResultsSelection()
{
	m_csVerse.Empty();
	int nResults = (int)m_vResults.size();
	for (int iResult = 0;iResult < nResults;++iResult)
	{
		if (m_Results.GetSel(iResult) > 0)
		{
			CVerse & Verse = m_vResults[iResult];
			CString csTranslation = g_pApp->LookupTranslationName(Verse.m_iTranslation);
			CString csBook = g_pApp->LookupBookName(Verse.m_iTranslation,Verse.m_iBook);
			CString csResult;
			m_csVerseHeader.Format("%s %s %d:%d-%d",csTranslation,csBook,Verse.m_iChapter,Verse.m_iVerseFrom,Verse.m_iVerseTo);
			m_csVerse = Verse.m_csVerse;
			break;
		}
	}
	UpdateData(FALSE);
}

// Preview the verse
void CVerseFinderDlg::OnPreviewVerse()
{
	m_csVerse.Empty();
	int nResults = (int)m_vResults.size();
	for (int iResult = 0;iResult < nResults;++iResult)
	{
		if (m_Results.GetSel(iResult) > 0)
		{
			CVerse & Verse = m_vResults[iResult];
			g_pApp->UpdateSearchPreview(Verse);
			break;
		}
	}
}
#endif