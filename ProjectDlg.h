// ProjectDlg.h : header file
// Copyright (C) 2007 Andrew S. Bantly
//

#pragma once

#include "FullWnd.h"
#include "VerseList.h"
#include "SettingsList.h"
#include "Gripper.h"
#include "ImageButton.h"
#include "StaticLink.h"
#include "SplashWnd.h"
#include "GridEdit.h"
#include "QuickEdit.h"
#include "MessageBox.h"
#include "afxwin.h"
#include "afxcmn.h"

int CALLBACK FontProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
);
bool __stdcall RegisterCallback();

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg dialog

class CProjectDlg : public CDialog
{
	DECLARE_SERIAL(CProjectDlg)

	friend class CPreviewDlg;
	friend class CSettingsList;
	friend class CFullWnd;
	friend class CVerseList;
	friend class CCaptureThread;

// Construction
public:
	CProjectDlg(int iDD = IDD_PROJECT_DIALOG,CWnd* pParent = NULL);	// standard constructor
	~CProjectDlg();

// Dialog Data
	//{{AFX_DATA(CProjectDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void Serialize(CArchive & ar);
	//}}AFX_VIRTUAL

protected:
	int m_iDD;
	CRect m_DialogRect;
	CPoint m_ptMinTrack;
	CGripper m_Gripper;
	CPoint m_ptMaxTrack;
	bool m_bInitDialog;
	bool m_bPreviewOn;
	int m_nbRegisterAttempts;

protected:
	CSplashWnd * m_pSplashWnd;
	CBitmap m_SplashBitmap;

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hFSIcon;
#if !defined(PERSONAL) && !defined(LITE)
	CFullWnd m_FullWnd;
#endif
	bool m_bFullScreen;
	bool m_bMinimized;
	CRect m_VerseRect;
	CString m_csBkgImage;

public:
	enum {VERSEVIEW,SETTINGSVIEW};
	CTabCtrl m_Mode;
	CVerseList m_VerseList;
	CSettingsList m_SettingsList;
	CString m_csMinPointSizeBible;
	bool m_bAskCheckUpdatePointSize;

public:
	void ProcessClick(CListCtrl * pList,int iItem,int iMode);

public:
	BOOL m_bFirstVerse;
	int m_iCtrl,m_iShift;
	int m_iFirstItem,m_iLastItem;

protected:
	CString m_csFont;
	CStringArray m_csaFont;
	CEdit m_PointSize;
	UINT m_iPointSize;
	int m_iPointSizeMin;
	int m_iPointSizeMax;
	CEdit m_MaxPointSize;
	UINT m_iMaxPointSize;
	CComboBox m_MonitorList;
	BOOL m_bShowTranslation;
	BOOL m_bShowPassage;
	BOOL m_bAutoSizeFont;
	BOOL m_bBkgOn;
	BOOL m_bBkgMix;
	BOOL m_bBkgSolid;
	COLORREF m_crBkgSolid;
	UINT m_iMixAmt;
	COLORREF m_crTxt;
	COLORREF m_crBkg;
	CImageButton m_TxtColor;
	CImageButton m_BkgColor;
	CImageButton m_FileLoad;
	CImageButton m_FileSave;
	CImageButton m_Screen;
	CImageButton m_Preview;
	CImageButton m_Help;
	CImageButton m_Search;
	CImageButton m_FontSelect;
	BOOL m_bCenterJustify;
	BOOL m_bTransition;
	int m_nTransStep;
	CStaticLink m_HomePage;
	CStaticLink m_Register;

	// Background video
	BOOL m_bBkgVid;
	CString m_csBkgVid;

	// Background video capture
	BOOL m_bBkgCapture;

	// Text only owner draw
	CImageButton m_AddVerse;
	CImageButton m_AddNextVerse;
	CImageButton m_RemVerse;
	CImageButton m_RemAllVerse;
	CImageButton m_Update;
	CImageButton m_NextVerse;
	CImageButton m_PreviewVerse;
	CImageButton m_BkgImage;
	CImageButton m_AddQuickVerse;
	CImageButton m_ManageContent;

	// Quick Verse Entry
	CQuickEdit m_QuickTranslation;
	CQuickEdit m_QuickBook;
	CQuickEdit m_QuickChapter;
	CQuickEdit m_QuickFromVerse;
	CQuickEdit m_QuickToVerse;
	int m_iLastTranslation,m_iLastBook;
	CString m_csLastTranslationText,m_csLastBookText,m_csLastChapter;
	CStringArray m_csaLastBookList,m_csaLastChapterList,m_csaLastVerseList;

public:
	void HighlightLastVerse();
	void SetPointSize(int iPointSize);
	int GetPointSize();
	int GetMaxPointSize();
	void CheckAllFontSize();
	void CheckUpdatePointSize(int iMinPointSize);
	int DetectFontSize(CVerse * pVerse);
	int DetectAllFontSize();
	CString GetCurrentFontName();
	void GetLanguageEdition(CString & csLanguage,CString & csEdition);
	bool Register();
	void OpenSettings(CString & csPathFile);

protected:
	void MoveVerseSelection(int iDirection);
	void OnNextVersePreview();
	void OnLastVersePreview();
	void GetQuickText(int & iTranslation,int & iVerse,CString & csTranslationText,CStringArray & csaBookList,CString & csBookText,CStringArray & csaChapterList,CString & csChapter,CStringArray & csaVerseList,CString & csFromVerse,CString & csToVerse);

	// Generated message map functions
	//{{AFX_MSG(CProjectDlg)
	virtual void OnSize(UINT nType,int cx,int cy);
	virtual void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnHelpInfo(HELPINFO * lpHelpInfo);
	virtual void OnCancel();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	afx_msg void OnAddVerse();
	afx_msg void OnAddNextVerse();
	afx_msg void OnRemoveVerse();
	afx_msg void OnRemoveAllVerses();
	afx_msg void OnUpdate();
	afx_msg void OnPreview();
	afx_msg void OnNextVerse();
	afx_msg void OnPreviousVerse();
	afx_msg void OnProject();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnTxtColor();
	afx_msg void OnBkgColor();
	afx_msg void OnBkgImage();
	afx_msg void OnAutoSizeFont();
	afx_msg void OnKeyDownVerseList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnQBHelp();
	afx_msg void OnMonitor();
	afx_msg void OnShowHidePreview();
	afx_msg void OnSearch();
	afx_msg void OnMode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFontSelect();
	afx_msg void OnKillFocusQuickVerse();
	afx_msg void OnAddQuickVerse();
	afx_msg void OnManageContent();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.