#pragma once
#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
#include "afxcmn.h"
#include "ContentList.h"
#include "ImageButton.h"

// CManageContentDlg dialog

class CManageContentDlg : public CDialog
{
	DECLARE_DYNAMIC(CManageContentDlg)

public:
	CManageContentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManageContentDlg();

	// Methods for creating and destroying the modeless dialog
	BOOL Create();
	void Destroy();

// Dialog Data
	enum { IDD = IDD_CREATECONTENT };

protected:
	HICON m_hIcon;
	CWnd * m_pParent;
	int m_nID;
	bool m_bInit;
	CProjectDlg * m_pDlg;
	CContentList m_Books;
	CContentList m_Chapters;
	CContentList m_Verses;
	CImageButton m_AddBook;
	CImageButton m_EditBook;
	CImageButton m_DeleteBook;
	CImageButton m_AddChapter;
	CImageButton m_EditChapter;
	CImageButton m_DeleteChapter;
	CImageButton m_AddVerse;
	CImageButton m_EditVerse;
	CImageButton m_DeleteVerse;
	CImageButton m_UpdateVerseText;
	CEdit m_VerseText;
	CString m_csVerseText;
	CImageButton m_Save;

protected:
	int m_iTranslation;

protected:
	// Initialization
	void Init();
	void SelectItems(int iBook = -1,int iChapter = -1,int iVerse = -1);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	// Events
	afx_msg void OnClose();
	afx_msg void OnAddBook();
	afx_msg void OnEditBook();
	afx_msg void OnDeleteBook();
	afx_msg void OnAddChapter();
	afx_msg void OnEditChapter();
	afx_msg void OnDeleteChapter();
	afx_msg void OnAddVerse();
	afx_msg void OnEditVerse();
	afx_msg void OnDeleteVerse();
	afx_msg void OnChangeEditVerseText();
	afx_msg void OnUpdateEditVerseText();
	afx_msg void OnClickedSave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickBooks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickChapters(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickVerses(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif