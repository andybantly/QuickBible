// VerseFinder.h : Verse search facility prototype
// Copyright (C) 2007 Andrew S. Bantly

#pragma once
#if !defined(LITE)
#include "ProjectDlg.h"
#include "Verse.h"
#include <vector>
#include "afxwin.h"
#include "ImageButton.h"
#include "resource.h"

// CVerseFinderDlg dialog

class CVerseFinderDlg : public CDialog
{
	DECLARE_DYNAMIC(CVerseFinderDlg)

public:
	CVerseFinderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVerseFinderDlg();

	// Methods for creating and destroying the modeless dialog
	BOOL Create();
	void Destroy();
	void SetupVerseFinder();

// Dialog Data
	enum { IDD = IDD_VERSEFINDER };

protected:
	virtual void DoDataExchange(CDataExchange * pDX);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;
	CWnd * m_pParent;
	int m_nID;
	std::vector<CVerse> m_vResults;
	CProjectDlg * m_pDlg;

	CImageButton m_Ok;
	CImageButton m_AddSelected;
	CImageButton m_AddAll;
	CImageButton m_PreviewVerse;

public:
	CComboBox m_Translation;
	CComboBox m_Book;
	CComboBox m_Chapter;
	CEdit m_Search;
	CString m_csSearch;
	CListBox m_Results;
	CString m_csVerseHeader;
	CString m_csVerse;
	CString m_csTranslation;
	CString m_csBook;

public:
	afx_msg void OnFind();
	afx_msg void OnClose();
	afx_msg void OnCloseupTranslation();
	afx_msg void OnCloseupBook();
	afx_msg void OnAddSelected();
	afx_msg void OnAddAll();
	afx_msg void OnResultsSelection();
	afx_msg void OnEditTranslation();
	afx_msg void OnEditBook();
	afx_msg void OnPreviewVerse();
};
#endif