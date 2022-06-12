// ProjectDlg.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "AboutDlg.h"
#include "BBLFileDialog.h"
#include "ImageSettings.h"
#include "Crypt.h"
#include "RegisterDlg.h"
#include "ChooseFont.h"
#include <cmath>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CProjectDlg, CDialog, VERSIONABLE_SCHEMA|3)

extern CProjectApp * g_pApp;
extern UINT g_cf;

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg dialog

CProjectDlg::CProjectDlg(int iDD,CWnd * pParent /*=NULL*/)
: CDialog(iDD, pParent),
	m_bInitDialog(true), m_bFullScreen(false), 
	m_bShowTranslation(FALSE), m_bShowPassage(TRUE), m_bBkgOn(FALSE), m_bBkgCapture(FALSE), m_bBkgVid(FALSE), m_bBkgMix(TRUE), m_iMixAmt(29), m_bBkgSolid(FALSE),
	m_iPointSize(50), m_iPointSizeMin(6), m_iPointSizeMax(1000), m_iMaxPointSize(55), m_bAskCheckUpdatePointSize(true),
	m_bMinimized(false), m_bAutoSizeFont(TRUE), m_bCenterJustify(TRUE), m_bTransition(TRUE), m_nTransStep(16),
	m_bPreviewOn(false), m_iDD(iDD),
	m_nbRegisterAttempts(0), m_pSplashWnd(0),
	m_bFirstVerse(FALSE), m_iFirstItem(-1), m_iLastItem(-1)
{
	// Initialize the key states for the shift and control keys
	m_iShift = HIBYTE(GetKeyState(VK_SHIFT)) >> 7;
	m_iCtrl = HIBYTE(GetKeyState(VK_CONTROL)) >> 7;

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = g_pApp->LoadIcon(IDR_MAINFRAME);
	m_hFSIcon = g_pApp->LoadIcon(IDR_FULLSCREEN);
	
	// Initialize the dialog coordinate rectangle
	m_DialogRect.SetRectEmpty();

	// The default color scheme is white on black
	m_csBkgImage.Empty();
	m_crTxt = RGB(255,255,255);
	m_crBkg = RGB(0,0,0);
	m_crBkgSolid = RGB(0,0,128);

	// Enumerate all styles of all fonts for the ANSI character set
	LOGFONT Logfont;
	Logfont.lfFaceName[0] = '\0';
	Logfont.lfCharSet = ANSI_CHARSET;
	HDC hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	EnumFontFamiliesEx(hDC,&Logfont,(FONTENUMPROC)FontProc,(LPARAM)&m_csaFont,0);
	DeleteDC(hDC);

	// Select 'Georgia' if it is available
	int nFont = (int)m_csaFont.GetCount();
	CString csFont;
	for (int iFont = 0;iFont < nFont;++iFont)
	{
		CString csTempFont = m_csaFont.GetAt(iFont);
		if (csTempFont == _T("Georgia"))
		{
			csFont = csTempFont;
			break;
		}
	}
	if (csFont.IsEmpty() && nFont)
		csFont = m_csaFont.GetAt(0);
	if (!csFont.IsEmpty())
		m_csFont = csFont;
}

CProjectDlg::~CProjectDlg()
{
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSELIST, m_VerseList);
	DDX_Control(pDX, IDC_SETTINGSLIST, m_SettingsList);
	DDX_Control(pDX, IDC_MONITOR, m_MonitorList);
	DDX_Check(pDX, IDC_SHOWTRANSLATION, m_bShowTranslation);
	DDX_Check(pDX, IDC_SHOWPASSAGE, m_bShowPassage);
	DDX_Control(pDX, IDC_PTSIZE, m_PointSize);
	DDX_Text(pDX, IDC_PTSIZE, m_iPointSize);
	DDV_MinMaxUInt(pDX, m_iPointSize, m_iPointSizeMin, m_iPointSizeMax);
	DDX_Control(pDX, IDC_MAXPTSIZE, m_MaxPointSize);
	DDX_Text(pDX, IDC_MAXPTSIZE, m_iMaxPointSize);
	DDV_MinMaxUInt(pDX, m_iMaxPointSize, m_iPointSizeMin, m_iPointSizeMax);
	DDX_Check(pDX, IDC_AUTOSIZEFONT, m_bAutoSizeFont);
	DDX_Check(pDX, IDC_CENTERJUSTIFY, m_bCenterJustify);
	DDX_Control(pDX, IDC_MODE, m_Mode);
	DDX_Text(pDX, IDC_EDIT_FONT, m_csFont);
	DDX_Control(pDX, IDC_QUICK_TRANSLATION, m_QuickTranslation);
	DDX_Control(pDX, IDC_QUICK_BOOK, m_QuickBook);
	DDX_Control(pDX, IDC_QUICK_CHAPTER, m_QuickChapter);
	DDX_Control(pDX, IDC_QUICK_FROMVERSE, m_QuickFromVerse);
	DDX_Control(pDX, IDC_QUICK_TOVERSE, m_QuickToVerse);
}

BEGIN_MESSAGE_MAP(CProjectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_ADDVERSE, &CProjectDlg::OnAddVerse)
	ON_BN_CLICKED(IDC_ADDNEXTVERSE, &CProjectDlg::OnAddNextVerse)
	ON_BN_CLICKED(IDC_REMVERSE, &CProjectDlg::OnRemoveVerse)
	ON_BN_CLICKED(IDC_REMALLVERSE, &CProjectDlg::OnRemoveAllVerses)
	ON_BN_CLICKED(IDC_UPDATE, &CProjectDlg::OnUpdate)
	ON_BN_CLICKED(IDC_PREVIEW, &CProjectDlg::OnPreview)
	ON_BN_CLICKED(IDC_NEXTVERSE, &CProjectDlg::OnNextVerse)
	ON_BN_CLICKED(IDC_PROJECT, &CProjectDlg::OnProject)
	ON_BN_CLICKED(IDC_LOAD, &CProjectDlg::OnLoad)
	ON_BN_CLICKED(IDC_SAVE, &CProjectDlg::OnSave)
	ON_BN_CLICKED(IDC_TXTCOLOR, &CProjectDlg::OnTxtColor)
	ON_BN_CLICKED(IDC_BKGCOLOR, &CProjectDlg::OnBkgColor)
	ON_BN_CLICKED(IDC_AUTOSIZEFONT, &CProjectDlg::OnAutoSizeFont)
	ON_BN_CLICKED(IDC_BKGIMAGE, &CProjectDlg::OnBkgImage)
	ON_NOTIFY(LVN_KEYDOWN, IDC_VERSELIST, &CProjectDlg::OnKeyDownVerseList)
	ON_BN_CLICKED(IDC_QBHELP, &CProjectDlg::OnQBHelp)
	ON_CBN_CLOSEUP(IDC_MONITOR, &CProjectDlg::OnMonitor)
	ON_BN_CLICKED(IDC_SHOWHIDEPREVIEW,&CProjectDlg::OnShowHidePreview)
	ON_BN_CLICKED(IDC_SEARCH, &CProjectDlg::OnSearch)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MODE, &CProjectDlg::OnMode)
	ON_BN_CLICKED(IDC_FONTSELECT, &CProjectDlg::OnFontSelect)
	ON_EN_KILLFOCUS(IDC_QUICK_TRANSLATION, &CProjectDlg::OnKillFocusQuickVerse)
	ON_EN_KILLFOCUS(IDC_QUICK_BOOK, &CProjectDlg::OnKillFocusQuickVerse)
	ON_EN_KILLFOCUS(IDC_QUICK_CHAPTER, &CProjectDlg::OnKillFocusQuickVerse)
	ON_EN_KILLFOCUS(IDC_QUICK_FROMVERSE, &CProjectDlg::OnKillFocusQuickVerse)
	ON_EN_KILLFOCUS(IDC_QUICK_TOVERSE, &CProjectDlg::OnKillFocusQuickVerse)
	ON_BN_CLICKED(IDC_ADDQUICKVERSE, &CProjectDlg::OnAddQuickVerse)
	ON_BN_CLICKED(IDC_MANAGECONTENT, &CProjectDlg::OnManageContent)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg message handlers

// Handle the keypress of ESC from closing the application
BOOL CProjectDlg::PreTranslateMessage(MSG * pMsg)
{
	// Get the key state
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		m_iShift = HIBYTE(GetKeyState(VK_SHIFT)) >> 7;
		m_iCtrl = HIBYTE(GetKeyState(VK_CONTROL)) >> 7;
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		WPARAM WP = pMsg->wParam;
		if (WP == VK_ESCAPE || WP == VK_RETURN)
			return TRUE;
		else if (WP == VK_F4)
		{
			// Activate the search
			OnSearch();
			return TRUE;
		}
		else if (WP == VK_RIGHT)
		{
			OnNextVersePreview();
			return TRUE;
		}
		else if (WP == VK_LEFT)
		{
			OnLastVersePreview();
			return TRUE;
		}
		else if (WP == VK_UP && GetAsyncKeyState(VK_SHIFT))
		{
			OnPreviousVerse();
			return TRUE;
		}
		else if (WP == VK_DOWN && GetAsyncKeyState(VK_SHIFT))
		{
			OnNextVerse();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CProjectDlg::OnSize(UINT nType,int cx,int cy)
{
	if (!m_bInitDialog)
		return;

	// Check the gripper window handle
	if (!m_Gripper.GetSafeHwnd())
		return;

	// Turn off drawing
	SetRedraw(FALSE);

	// If the window is being minimized
	bool bRestored = false;
	if (nType == SIZE_MINIMIZED)
		m_bMinimized = true;
	else if (m_bMinimized && nType == SIZE_RESTORED)
	{
		m_bMinimized = false;
		bRestored = true;
	}

	// Get the dialog rectangle
	CRect DialogRect,Rect;
	GetWindowRect(&DialogRect);
	ScreenToClient(&DialogRect);

	// Get the dialog size delta
	CPoint ptDiff(DialogRect.right - m_DialogRect.right,DialogRect.bottom - m_DialogRect.bottom);

	// Resize the verse list
	m_VerseList.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	m_VerseList.SetWindowPos(NULL,Rect.left,Rect.top,Rect.Width() + ptDiff.x, Rect.Height() + ptDiff.y,SWP_NOZORDER|SWP_NOACTIVATE);
	m_SettingsList.SetWindowPos(NULL,Rect.left,Rect.top,Rect.Width() + ptDiff.x, Rect.Height() + ptDiff.y,SWP_NOZORDER|SWP_NOACTIVATE);

	// Expand the verses column
	int & nVerseColumnWidth = m_VerseList.m_nVerseColumnWidth;
	nVerseColumnWidth += ptDiff.x;
	m_VerseList.SetColumnWidth(m_VerseList.m_nVerseColumn,nVerseColumnWidth);

	// Resize the tab
	m_Mode.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	m_Mode.SetWindowPos(NULL,Rect.left,Rect.top,Rect.Width() + ptDiff.x, Rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	// Resize the gripper
	m_Gripper.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	m_Gripper.SetWindowPos(NULL,Rect.left + ptDiff.x,Rect.top + ptDiff.y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

	if (m_iDD == IDD_PROJECT_DIALOG)
	{
		// Large version of the dialog
		int aMoveRight[] = {IDC_LOAD,IDC_SAVE,IDC_ADDVERSE,IDC_ADDNEXTVERSE,IDC_REMVERSE,IDC_REMALLVERSE,IDC_DELETEGROUP,IDC_FILEGROUP,IDC_ADDGROUP,0};
		int nMR = sizeof(aMoveRight) / sizeof(aMoveRight[0]);
		for (int iMR = 0;iMR < nMR;++iMR)
		{
			if (!aMoveRight[iMR])
				continue;
			CWnd * pWnd = GetDlgItem(aMoveRight[iMR]);
			pWnd->GetWindowRect(&Rect);
			ScreenToClient(&Rect);
			pWnd->SetWindowPos(NULL,Rect.left + ptDiff.x,Rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}

		int aMoveRightDown[] = {0};
		int nMRD = sizeof(aMoveRightDown) / sizeof(aMoveRightDown[0]);
		for (int iMRD = 0;iMRD < nMRD;++iMRD)
		{
			if (!aMoveRightDown[iMRD])
				continue;
			CWnd * pWnd = GetDlgItem(aMoveRightDown[iMRD]);
			pWnd->GetWindowRect(&Rect);
			ScreenToClient(&Rect);
			pWnd->SetWindowPos(NULL,Rect.left + ptDiff.x,Rect.top + ptDiff.y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}

		int aMoveDown[] = {IDC_MONITOR,IDC_PROJECT,IDC_DISPLAYGROUP,IDC_SHOWTRANSLATION,IDC_SHOWPASSAGE,IDC_UPDATE,IDC_NEXTVERSE,IDC_PREVIEWGROUP,IDC_PREVIEW,IDC_SHOWHIDEPREVIEW,IDC_SLIDESETTINGSGROUP,IDC_SETTINGSGROUP,IDC_AUTOSIZEFONT,IDC_CENTERJUSTIFY,IDC_TXTCOLOR,IDC_BKGCOLOR,IDC_BKGIMAGE,IDC_STATIC_FONT,IDC_STATIC_PTSIZE,IDC_PTSIZE,IDC_STATIC_MAXPTSIZE,IDC_MAXPTSIZE,IDC_FONTSELECT,IDC_EDIT_FONT,IDC_STATICHELP,IDC_QBHELP,IDC_SEARCH,IDC_STATIC_WEBLINK,IDC_QUICKGROUP,IDC_STATIC_QUICK_TRANSLATION,IDC_STATIC_QUICK_BOOK,IDC_STATIC_QUICK_CHAPTER,IDC_STATIC_QUICK_FROMVERSE,IDC_STATIC_QUICK_TOVERSE,IDC_QUICK_TRANSLATION,IDC_QUICK_BOOK,IDC_QUICK_CHAPTER,IDC_QUICK_FROMVERSE,IDC_QUICK_TOVERSE,IDC_ADDQUICKVERSE,IDC_STATIC_MANAGECONTENT,IDC_MANAGECONTENT,IDC_STATIC_REGISTER,0};
		int nMD = sizeof(aMoveDown) / sizeof(aMoveDown[0]);
		for (int iMD = 0;iMD < nMD;++iMD)
		{
			if (!aMoveDown[iMD])
				continue;
			CWnd * pWnd = GetDlgItem(aMoveDown[iMD]);
			pWnd->GetWindowRect(&Rect);
			ScreenToClient(&Rect);
			pWnd->SetWindowPos(NULL,Rect.left,Rect.top + ptDiff.y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}
	else
	{
		// Small version of the dialog
		int aMoveDown[] = {IDC_LOAD,IDC_SAVE,IDC_ADDVERSE,IDC_ADDNEXTVERSE,IDC_REMVERSE,IDC_REMALLVERSE,IDC_DELETEGROUP,IDC_FILEGROUP,IDC_ADDGROUP,IDC_STATICHELP,IDC_QBHELP,IDC_SEARCH,IDC_STATIC_WEBLINK,IDC_MONITOR,IDC_PROJECT,IDC_DISPLAYGROUP,IDC_SHOWTRANSLATION,IDC_SHOWPASSAGE,IDC_UPDATE,IDC_NEXTVERSE,IDC_PREVIEWGROUP,IDC_PREVIEW,IDC_SHOWHIDEPREVIEW,IDC_SLIDESETTINGSGROUP,IDC_SETTINGSGROUP,IDC_AUTOSIZEFONT,IDC_CENTERJUSTIFY,IDC_TXTCOLOR,IDC_BKGCOLOR,IDC_BKGIMAGE,IDC_STATIC_FONT,IDC_STATIC_PTSIZE,IDC_PTSIZE,IDC_STATIC_MAXPTSIZE,IDC_MAXPTSIZE,IDC_EDIT_FONT,IDC_FONTSELECT,IDC_QUICKGROUP,IDC_STATIC_QUICK_TRANSLATION,IDC_STATIC_QUICK_BOOK,IDC_STATIC_QUICK_CHAPTER,IDC_STATIC_QUICK_FROMVERSE,IDC_STATIC_QUICK_TOVERSE,IDC_QUICK_TRANSLATION,IDC_QUICK_BOOK,IDC_QUICK_CHAPTER,IDC_QUICK_FROMVERSE,IDC_QUICK_TOVERSE,IDC_ADDQUICKVERSE,IDC_MANAGECONTENT,IDC_MANAGECONTENT,IDC_STATIC_REGISTER,0};
		int nMD = sizeof(aMoveDown) / sizeof(aMoveDown[0]);
		for (int iMD = 0;iMD < nMD;++iMD)
		{
			if (!aMoveDown[iMD])
				continue;
			CWnd * pWnd = GetDlgItem(aMoveDown[iMD]);
			pWnd->GetWindowRect(&Rect);
			ScreenToClient(&Rect);
			pWnd->SetWindowPos(NULL,Rect.left,Rect.top + ptDiff.y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}

	// Redraw the resized controls
	SetRedraw();
	Invalidate();
	UpdateWindow();

	// Update the dialog size
	m_DialogRect = DialogRect;

	if (bRestored)
	{
		Rect = m_VerseRect;
		m_VerseList.SetWindowPos(NULL,Rect.left,Rect.top,Rect.Width(), Rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		m_SettingsList.SetWindowPos(NULL,Rect.left,Rect.top,Rect.Width(), Rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		RedrawWindow();
	}
	else if (!m_bMinimized)
	{
		m_VerseList.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		m_VerseRect = Rect;
	}
}

void CProjectDlg::OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI)
{
	if (m_DialogRect.IsRectEmpty())
		return;
	lpMMI->ptMinTrackSize = m_ptMinTrack;
	lpMMI->ptMaxTrackSize = m_ptMaxTrack;
}

BOOL CProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Check for proper installation
	if (!g_pApp->IsInstalled())
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(299);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		EndDialog(IDOK);
		return TRUE;
	}

	// Test for the preview dialog being created
	if (!g_pApp->m_pPreviewDlg)
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(107);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		EndDialog(IDOK);
		return TRUE;
	}

#if !defined(LITE)
	// Test for the verse finder dialog being created
	if (!g_pApp->m_pVerseFinderDlg)
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(108);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		EndDialog(IDOK);
		return TRUE;
	}
#endif

#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
	// Test for the content management dialog being created
	if (!g_pApp->m_pManageContentDlg)
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(328);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		EndDialog(IDOK);
		return TRUE;
	}
#endif

	// Set the captions
	CString csQuickBible = g_pApp->QBString(259);
	CString csLanguage,csEdition,csWindowText;

	// Load the dialog strings
	g_pApp->GetLanguageEdition(csLanguage,csEdition);

#if defined(_DEBUG) || defined(_RELEASE)
	csWindowText.Format("%s (%s)",csQuickBible,csLanguage);
#else
	csWindowText.Format("%s %s (%s)",csQuickBible,csEdition,csLanguage);
#endif

	// Set the caption
	SetWindowText(csWindowText);

	// Set all the button texts
	GetDlgItem(IDC_DELETEGROUP)->SetWindowText(g_pApp->QBString(234));
	GetDlgItem(IDC_FILEGROUP)->SetWindowText(g_pApp->QBString(235));
	GetDlgItem(IDC_ADDGROUP)->SetWindowText(g_pApp->QBString(236));
	GetDlgItem(IDC_STATICHELP)->SetWindowText(g_pApp->QBString(237));
	GetDlgItem(IDC_DISPLAYGROUP)->SetWindowText(g_pApp->QBString(241));
	GetDlgItem(IDC_SHOWTRANSLATION)->SetWindowText(g_pApp->QBString(244));
	GetDlgItem(IDC_SHOWPASSAGE)->SetWindowText(g_pApp->QBString(323));
	GetDlgItem(IDC_PREVIEWGROUP)->SetWindowText(g_pApp->QBString(247));
	GetDlgItem(IDC_SLIDESETTINGSGROUP)->SetWindowText(g_pApp->QBString(324));
	GetDlgItem(IDC_SETTINGSGROUP)->SetWindowText(g_pApp->QBString(250));
	GetDlgItem(IDC_AUTOSIZEFONT)->SetWindowText(g_pApp->QBString(251));
	GetDlgItem(IDC_CENTERJUSTIFY)->SetWindowText(g_pApp->QBString(252));
	GetDlgItem(IDC_STATIC_FONT)->SetWindowText(g_pApp->QBString(256));
	GetDlgItem(IDC_STATIC_PTSIZE)->SetWindowText(g_pApp->QBString(257));
	GetDlgItem(IDC_STATIC_MAXPTSIZE)->SetWindowText(g_pApp->QBString(258));
	GetDlgItem(IDC_STATIC_WEBLINK)->SetWindowText(g_pApp->QBString(215));
	GetDlgItem(IDC_STATIC_REGISTER)->SetWindowText(g_pApp->QBString(304));
	GetDlgItem(IDC_QUICKGROUP)->SetWindowText(g_pApp->QBString(325));
	GetDlgItem(IDC_STATIC_QUICK_TRANSLATION)->SetWindowText(g_pApp->QBString(219));
	GetDlgItem(IDC_STATIC_QUICK_BOOK)->SetWindowText(g_pApp->QBString(220));
	GetDlgItem(IDC_STATIC_QUICK_CHAPTER)->SetWindowText(g_pApp->QBString(221));
	GetDlgItem(IDC_STATIC_QUICK_FROMVERSE)->SetWindowText(g_pApp->QBString(222));
	GetDlgItem(IDC_STATIC_QUICK_TOVERSE)->SetWindowText(g_pApp->QBString(223));
	if (m_iDD == IDD_PROJECT_DIALOG)
		GetDlgItem(IDC_STATIC_MANAGECONTENT)->SetWindowText(g_pApp->QBString(327));

	// Text only owner draw
	m_AddVerse.CreateText(this,IDC_ADDVERSE,g_pApp->QBString(230));
	m_AddNextVerse.CreateText(this,IDC_ADDNEXTVERSE,g_pApp->QBString(231));
	m_RemVerse.CreateText(this,IDC_REMVERSE,g_pApp->QBString(232));
	m_RemAllVerse.CreateText(this,IDC_REMALLVERSE,g_pApp->QBString(233));
	m_Update.CreateText(this,IDC_UPDATE,g_pApp->QBString(245));
	m_NextVerse.CreateText(this,IDC_NEXTVERSE,g_pApp->QBString(246));
	m_PreviewVerse.CreateText(this,IDC_PREVIEW,g_pApp->QBString(248));
	m_BkgImage.CreateText(this,IDC_BKGIMAGE,g_pApp->QBString(255));
	m_AddQuickVerse.CreateText(this,IDC_ADDQUICKVERSE,g_pApp->QBString(326));

	// The text and background color buttons have their selected colors on them
	m_TxtColor.CreateRect(this,IDC_TXTCOLOR,RGB(255,255,255),g_pApp->QBString(253));
	m_BkgColor.CreateRect(this,IDC_BKGCOLOR,RGB(0,0,0),g_pApp->QBString(254));

	// Bitmap buttons have the bitmap image on them
	m_FileLoad.CreateBtmp(this,IDB_FILELOAD,IDC_LOAD,g_pApp->QBString(228));
	m_FileSave.CreateBtmp(this,IDB_FILESAVE,IDC_SAVE,g_pApp->QBString(229));
	m_Help.CreateBtmp(this,IDB_QBHELP,IDC_QBHELP,g_pApp->QBString(238));
	m_Screen.CreateBtmp(this,IDB_OFFPROJECTOR,IDC_PROJECT,g_pApp->QBString(195));
	m_FontSelect.CreateBtmp(this,IDB_FONTSELECT,IDC_FONTSELECT);
	m_ManageContent.CreateBtmp(this,IDB_MANAGECONTENT,IDC_MANAGECONTENT,g_pApp->QBString(329));

	// Get the registration path
	CString csRegistrationPath = g_pApp->GetRegistrationPath();

	// Set the weblink
	m_HomePage.SubclassDlgItem(IDC_STATIC_WEBLINK, this, g_pApp->QBString(215));
	m_Register.SubclassDlgItem(IDC_STATIC_REGISTER, this);
	
//#if !defined(_DEBUG) && !defined(_RELEASE)
	// Show or hide the registration link depending on the status
	bool bRegistered = g_pApp->IsRegistered();
	m_Register.ShowWindow(bRegistered ? SW_HIDE : SW_SHOW);
	m_Register.SetNavigate(RegisterCallback);
//#endif

#if defined(PERSONAL) || defined(LITE)
	GetDlgItem(IDC_UPDATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_NEXTVERSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_MONITOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_PROJECT)->EnableWindow(FALSE);
	m_Screen.EnableWindow(FALSE);
#endif

	m_Preview.CreateBtmp(this,IDB_OFFMONITOR,IDC_SHOWHIDEPREVIEW,g_pApp->QBString(202));
	m_Search.CreateBtmp(this,IDB_SEARCH,IDC_SEARCH,g_pApp->QBString(239));

#if defined(LITE)
	m_Search.EnableWindow(FALSE);
#endif

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu = g_pApp->QBString(227);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Create the gripper control
	CRect Rect;
	GetClientRect(Rect);
	Rect.left = Rect.right - GetSystemMetrics(SM_CXHSCROLL);
	Rect.top  = Rect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	m_Gripper.Create(WS_CHILD|SBS_SIZEBOX|SBS_SIZEBOXBOTTOMRIGHTALIGN|SBS_SIZEGRIP|WS_VISIBLE,Rect,this,AFX_IDW_SIZE_BOX);
	m_Gripper.SetWindowPos(&wndTop,Rect.left,Rect.top,Rect.Width(),Rect.Height(),0);

	// Get the dialog dimensions
	GetWindowRect(&m_DialogRect);

	// Allow the screen to resized 100% more horizontally and 50% more vertically
	m_ptMinTrack.x = m_DialogRect.Width();
	m_ptMinTrack.y = m_DialogRect.Height();
	m_ptMaxTrack.x = min(m_ptMinTrack.x * 4,GetSystemMetrics(SM_CXSCREEN));
	m_ptMaxTrack.y = min((long)((double)m_ptMinTrack.y * 3),GetSystemMetrics(SM_CYSCREEN));

	GetDlgItem(IDC_SETTINGSGROUP);

	// Work in client coordinates
	ScreenToClient(&m_DialogRect);

	// Get the size of the verse list
	m_VerseList.GetWindowRect(&m_VerseRect);
	ScreenToClient(&m_VerseRect);

	CString csWndClass = AfxRegisterWndClass(CS_OWNDC,0,0,0);
#if !defined(PERSONAL) && !defined(LITE)
	// Create the overhead window
	if (!(m_FullWnd.CreateEx(WS_EX_LEFT,(LPCTSTR)csWndClass,"Overhead",WS_VISIBLE|WS_POPUP,0,0,0,0,NULL,NULL)))
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(190);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		EndDialog(IDOK);
		return TRUE;
	}
	m_FullWnd.SetIcon(m_hFSIcon, TRUE);			// Set big icon
	m_FullWnd.SetIcon(m_hFSIcon, FALSE);		// Set small icon
	m_FullWnd.ShowWindow(SW_HIDE);
#endif

	// Initialize
	g_pApp->Initialize();

	// Set the Quick Translation List
	CStringArray csaQuickTranslation;
	g_pApp->GetList(csaQuickTranslation);
	m_QuickTranslation.SetPredictiveText(csaQuickTranslation);

	// Add the available monitors
	int nbMonitors = (int)g_pApp->m_vMonRect.size();
	for (int iLoop = 0;iLoop < nbMonitors;++iLoop)
	{
		CString csMonitor;
		csMonitor.Format(g_pApp->QBString(191),iLoop + 1);
		m_MonitorList.AddString(csMonitor);
	}

	// Select the default monitor for projection
	int iFirstMonitor = 0;
	if (nbMonitors > 1)
		iFirstMonitor = 1;
	m_MonitorList.SetCurSel(iFirstMonitor);

	// Add the monitor coordinates to the preview window
	g_pApp->m_pPreviewDlg->m_vMonRect = g_pApp->m_vMonRect;
	g_pApp->m_pPreviewDlg->m_iMonitor = iFirstMonitor;

	// Setup the preview window frame buffer
	g_pApp->m_pPreviewDlg->SetupPreview();

	// Initialize the tabs
	TCITEM tcTab;
	CString csWorksheet = g_pApp->QBString(192);
	tcTab.pszText = csWorksheet.GetBuffer();
	tcTab.mask = TCIF_TEXT;
	m_Mode.InsertItem(VERSEVIEW,&tcTab);

#if !defined(PERSONAL) && !defined(LITE)
	CString csSettings = g_pApp->QBString(193);
	tcTab.pszText = csSettings.GetBuffer();
	tcTab.mask = TCIF_TEXT;
	m_Mode.InsertItem(SETTINGSVIEW,&tcTab);
#endif

	// Process any shell commands
	g_pApp->GetShellCommand();

	// Create the splash screen
	m_pSplashWnd = new CSplashWnd();
	m_pSplashWnd->CreateEx(WS_EX_LEFT | WS_EX_TOPMOST,(LPCTSTR)csWndClass,"QuickBible",WS_VISIBLE|WS_POPUP,m_pSplashWnd->GetXStart(),m_pSplashWnd->GetYStart(),m_pSplashWnd->GetWidth(),m_pSplashWnd->GetHeight(),NULL,NULL);

	// Flag the dialog as being initialized
	m_bInitDialog = true;

#if !defined(_DEBUG) && !defined(_RELEASE)
	// Remind the end user to register immediately
	if (!bRegistered)
		SetTimer(1,500,NULL);
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// Handle the special message to bring up the registration dialog
void CProjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		// Kill the timer
		KillTimer(1);

		// Register
		Register();

		// Set the timer again if they haven't registered
		bool bRegistered = g_pApp->IsRegistered();
		if (!bRegistered)
		{
			if (m_nbRegisterAttempts < 1)
				SetTimer(1,600000 * 3,NULL); // 30 minutes
			else if (m_nbRegisterAttempts >= 1 && m_nbRegisterAttempts < 10)
				SetTimer(1,600000,NULL); // 10 minutes
			else
				SetTimer(1,600000 / 10,NULL); // 1 minutes
		}
		else
			m_Register.ShowWindow(SW_HIDE);
		m_nbRegisterAttempts++;
	}
}

// Helper to get the registration information
bool CProjectDlg::Register()
{
	CRegisterDlg dlg;
	dlg.DoModal();
	return false;
}

// Register the product
bool __stdcall RegisterCallback()
{
	CProjectDlg * pDlg = ((CProjectDlg *)g_pApp->m_pMainWnd);
	return pDlg->Register();
}

int CALLBACK FontProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
)
{
	CStringArray * pcsaFont = (CStringArray*)lParam;
	CString csFontName;
	if (FontType != TRUETYPE_FONTTYPE)
	{
		CComBSTR ccbFontName((TCHAR*)&lpelfe->elfLogFont.lfFaceName[0]);
		csFontName = ccbFontName;
	}
	else
	{
		CComBSTR ccbFontName((TCHAR*)&lpelfe->elfFullName[0]);
		csFontName = ccbFontName;
	}
	if (csFontName.GetLength())
		pcsaFont->Add(csFontName);
	return 1;
}

void CProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProjectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProjectDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProjectDlg::OnAddVerse()
{
	// Add the new verse to the end of the list
	m_VerseList.AddNewVerse();

	// Change the view to the worksheet if it is not active
	int iMode = m_Mode.GetCurSel();
	if (iMode == SETTINGSVIEW)
	{
		m_VerseList.ShowWindow(SW_SHOW);
		m_SettingsList.ShowWindow(SW_HIDE);
		m_Mode.SetCurSel(VERSEVIEW);
	}

	// Start editing the new verse
	m_VerseList.BeginEdit(m_VerseList.GetItemCount() - 1,CVerseList::TRANSLATION,false);
}

void CProjectDlg::OnAddNextVerse()
{
	// Add the next verse
	POSITION Pos = m_VerseList.GetFirstSelectedItemPosition();
	int iVerse = m_VerseList.GetItemCount() - 1;
	if (Pos)
		iVerse = m_VerseList.GetNextSelectedItem(Pos);
	m_VerseList.AddNextVerse(iVerse);
	HighlightLastVerse();
}

void CProjectDlg::OnRemoveVerse()
{
	// Write the count of selected variables as the first item in the data stream
	vector<int> vecSelected;
	POSITION Pos = m_VerseList.GetFirstSelectedItemPosition();
	while (Pos)
	{
		int iVerse = m_VerseList.GetNextSelectedItem(Pos);
		vecSelected.push_back(iVerse);
	}
	int nSelected = (int)vecSelected.size();
	for (int i = nSelected - 1;i >= 0;--i)
		m_VerseList.RemVerse(vecSelected[i]);
	m_VerseList.SetFocus();
	HighlightLastVerse();
}

void CProjectDlg::OnRemoveAllVerses()
{
	m_VerseList.RemAll();
}

void CProjectDlg::HighlightLastVerse()
{
	int nVerses = m_VerseList.GetItemCount();
	if (nVerses)
	{
		if (g_pApp->m_vecVerses.size() > 1)
			m_VerseList.UnSelect(nVerses - 2);
		m_VerseList.Select(nVerses - 1);
		int iMode = m_Mode.GetCurSel();
		if (iMode == SETTINGSVIEW)
		{
			m_SettingsList.SetFocus();
			m_SettingsList.RedrawWindow();
		}
		else
		{
			m_VerseList.SetFocus();
			m_VerseList.RedrawWindow();
		}

		// Set the first shift-selected item
		m_bFirstVerse = TRUE;
		m_iFirstItem = nVerses - 1;
	}
}

void CProjectDlg::OnUpdate()
{
#if !defined(PERSONAL) && !defined(LITE)
	// Update the projection verse
	UpdateData();
	m_FullWnd.RedrawWindow();
	m_VerseList.SetFocus();
#endif
}

void CProjectDlg::OnPreview()
{
	UpdateData();
	g_pApp->UpdateLastFrame(true);
	m_VerseList.SetFocus();
}

void CProjectDlg::OnNextVerse()
{
	// Select the next verse
	MoveVerseSelection(1);

	// Update the projected verse
	OnUpdate();

	// Highlight the current verse
	MoveVerseSelection(0);
}

void CProjectDlg::OnPreviousVerse()
{
	// Select the next verse
	MoveVerseSelection(-1);

	// Update the projected verse
	OnUpdate();

	// Highlight the current verse
	MoveVerseSelection(0);
}

void CProjectDlg::OnNextVersePreview()
{
	// Select the next verse
	MoveVerseSelection(1);

	// Update the projected verse
	OnPreview();

	// Highlight the current verse
	MoveVerseSelection(0);
}

void CProjectDlg::OnLastVersePreview()
{
	// Select the next verse
	MoveVerseSelection(-1);

	// Update the projected verse
	OnPreview();

	// Highlight the current verse
	MoveVerseSelection(0);
}

void CProjectDlg::MoveVerseSelection(int iDirection)
{
	// Pause redrawing the selections
	m_VerseList.SetRedraw(FALSE);
	m_SettingsList.SetRedraw(FALSE);

	// If moving forward or backward then update the positions
	if (iDirection != 0)
	{
		bool bForward = iDirection == 1 ? true : false;
		std::vector<int> vVerses;

		// Clear the selections
		POSITION Pos = m_VerseList.GetFirstSelectedItemPosition();
		while (Pos)
		{
			int iVerse = m_VerseList.GetNextSelectedItem(Pos);
			vVerses.push_back(iVerse);
			m_VerseList.UnSelect(iVerse);
		}

		// Update the positions
		int nVerses = (int)vVerses.size();
		int nItems = m_VerseList.GetItemCount();
		for (int iVerse = 0;iVerse < nVerses;++iVerse)
		{
			int & jVerse = vVerses[iVerse];
			if (bForward)
				jVerse++;
			else
				jVerse--;
			if (jVerse == nItems)
				jVerse = 0;
			if (jVerse == -1)
				jVerse = nItems - 1;

			// Update the selections
			m_VerseList.Select(jVerse);
		}
	}

	// Resume redrawing the selections
	m_VerseList.SetRedraw();
	m_SettingsList.SetRedraw();

	int iMode = m_Mode.GetCurSel();
	if (iMode == SETTINGSVIEW)
	{
		m_SettingsList.SetFocus();
		m_SettingsList.RedrawWindow();
	}
	else
	{
		m_VerseList.SetFocus();
		m_VerseList.RedrawWindow();
	}
}

void CProjectDlg::OnProject()
{
#if !defined(PERSONAL) && !defined(LITE)
	CPreviewDlg * pDlg = g_pApp->m_pPreviewDlg;
	if (m_bFullScreen)
	{
		// Stop the video
		if (pDlg->m_bBkgVid)
		{
			// Kill the timer
			if (pDlg->m_bMMTimer)
			{
				timeKillEvent(pDlg->m_mmFullTimer);
				pDlg->m_mmFullTimer = 0;
				pDlg->m_bFullVideoOnly = FALSE;
			}
		}
		else if (pDlg->m_bBkgCapture)
		{
			// Kill the capture
			pDlg->m_pCaptureThread->PostThreadMessage(WM_STOPCAPTURE,0,0);
		}
		m_bFullScreen = false;

		m_Screen.SetRedraw(FALSE);
		m_Screen.UpdateBitmap(IDB_OFFPROJECTOR);
		m_Screen.SetWindowText(g_pApp->QBString(195));
		m_Screen.SetRedraw();
		m_Screen.RedrawWindow();
		m_FullWnd.MoveWindow(0,0,0,0);
		m_FullWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		m_Screen.SetRedraw(FALSE);
		m_Screen.UpdateBitmap(IDB_ONPROJECTOR);
		m_Screen.SetWindowText(g_pApp->QBString(194));
		m_Screen.SetRedraw();
		m_Screen.RedrawWindow();

		// Update the dialog choices
		UpdateData();

		// Get the rectangle for the monitor
		CRect Rect = g_pApp->m_vMonRect[m_MonitorList.GetCurSel()];

		// Set the x and y max coordinates
		m_FullWnd.m_x = Rect.Width();
		m_FullWnd.m_y = Rect.Height();

		if (pDlg->m_bBkgCapture)
		{
			// Start the capture
			pDlg->m_pCaptureThread->PostThreadMessage(WM_STARTCAPTURE,0,0);
		}

		// Resize the window
		m_FullWnd.BringWindowToTop();
		m_FullWnd.MoveWindow(&Rect);
		m_FullWnd.ShowWindow(SW_SHOW);
		m_bFullScreen = true;
	}
	m_VerseList.SetFocus();
#endif
}

// Load a bible verse project (.bbl)
void CProjectDlg::OnLoad()
{
	// Call the custom load dialog
	CBBLFileDialog dlgFile(TRUE,g_pApp->QBString(196),g_pApp->QBString(197),g_pApp->QBString(198),OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING,g_pApp->QBString(199));
	INT_PTR nRet = dlgFile.DoModal();
	if (nRet != IDCANCEL)
	{
		// Count the files returned
		int nCount = 0;
		POSITION myPos = dlgFile.GetStartPosition();
		if (myPos != NULL)
		{
			CString csPathFile = dlgFile.GetNextPathName(myPos);
			OpenSettings(csPathFile);
		}
	}
}

// Open the settings file
void CProjectDlg::OpenSettings(CString & csPathFile)
{
	CFile File;
	if (File.Open(csPathFile,CFile::modeRead))
	{
		m_VerseList.RemAll();
		CArchive arFile(&File,CArchive::load);
		Serialize(arFile);
		m_VerseList.Serialize(arFile);
		
		// Update the settings if it is the active screen
		int iMode = m_Mode.GetCurSel();
		if (iMode == SETTINGSVIEW)
			m_VerseList.UpdateSettingsView();
	}
}

// Save a bible verse project (.bbl)
void CProjectDlg::OnSave()
{
	// Call the custom saveas dialog
	CBBLFileDialog dlgFile(FALSE,g_pApp->QBString(200),g_pApp->QBString(197),g_pApp->QBString(198),OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING,g_pApp->QBString(199));
	INT_PTR nRet = dlgFile.DoModal();
	if (nRet != IDCANCEL)
	{
		// Count the files returned
		int nCount = 0;
		POSITION myPos = dlgFile.GetStartPosition();
		if (myPos != NULL)
		{
			CString csPathFile = dlgFile.GetNextPathName(myPos);
			CFile File;
			if (File.Open(csPathFile,CFile::modeCreate|CFile::modeWrite))
			{
				CArchive arFile(&File,CArchive::store);
				Serialize(arFile);
				m_VerseList.Serialize(arFile);
			}
		}
	}
}

void CProjectDlg::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CProjectDlg));
		if (ar.IsStoring())
		{
			// Commit the dialog changes
			UpdateData();

			// Save all the global settings
			ar << m_csBkgImage;
			ar << m_csFont;
			ar << m_iPointSize;
			ar << m_iMaxPointSize;
			ar << m_bShowTranslation;
			ar << m_bAutoSizeFont;
			ar << m_bBkgOn;
			ar << m_bBkgMix;
			ar << m_bBkgSolid;
			ar << m_crBkgSolid;
			ar << m_bBkgVid;
			ar << m_csBkgVid;
			ar << m_bBkgCapture;
			ar << m_iMixAmt;
			ar << m_crTxt;
			ar << m_crBkg;
			ar << m_bTransition;
			ar << m_nTransStep;
			ar << m_bShowPassage;
		}
		else
		{
			// Get the file version
			int nVersion = ar.GetObjectSchema();
			if (nVersion >= 1 && nVersion <= 3)
			{
				// Load all the global settings
				ar >> m_csBkgImage;
				ar >> m_csFont;
				ar >> m_iPointSize;
				ar >> m_iMaxPointSize;
				if (nVersion < 3)
				{
					BOOL bProjectHL;
					ar >> bProjectHL;
				}
				ar >> m_bShowTranslation;
				ar >> m_bAutoSizeFont;
				ar >> m_bBkgOn;
				ar >> m_bBkgMix;
				ar >> m_bBkgSolid;
				ar >> m_crBkgSolid;
				if (nVersion > 2)
				{
					ar >> m_bBkgVid;
					ar >> m_csBkgVid;
					ar >> m_bBkgCapture;
				}
				ar >> m_iMixAmt;
				ar >> m_crTxt;
				ar >> m_crBkg;
				ar >> m_bTransition;
				ar >> m_nTransStep;

				if (nVersion > 2)
					ar >> m_bShowPassage;

				// Initialize the dialog
				UpdateData(FALSE);

				// Update the controls
				m_TxtColor.SetFillColor(m_crTxt);
				m_BkgColor.SetFillColor(m_crBkg);
				m_PointSize.EnableWindow(!m_bAutoSizeFont);
				m_MaxPointSize.EnableWindow(m_bAutoSizeFont);
			}
			else
			{
				CString csCaption = g_pApp->QBString(259);
				CString csFmt = g_pApp->QBString(322);
				CString csMessage;
				csMessage.Format(csFmt,ar.m_strFileName);
				CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
				MsgBox.DoModal();
			}
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

void CProjectDlg::OnTxtColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_crTxt = dlg.GetColor();
		m_TxtColor.SetFillColor(m_crTxt);
	}
}

void CProjectDlg::OnBkgColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_crBkg = dlg.GetColor();
		m_BkgColor.SetFillColor(m_crBkg);
	}
}

void CProjectDlg::OnAutoSizeFont()
{
	UpdateData();
	m_PointSize.EnableWindow(!m_bAutoSizeFont);
	m_MaxPointSize.EnableWindow(m_bAutoSizeFont);
}

void CProjectDlg::OnBkgImage()
{
	// Create the image dialog for selecting background images and mix level
	CImageSettings dlg(m_bBkgCapture,m_csBkgVid,m_csBkgImage,m_bBkgMix,m_iMixAmt,m_crBkgSolid,m_bTransition,m_nTransStep);
	if (dlg.DoModal() == IDOK)
	{
		// Get the video capture setting
		m_bBkgCapture = dlg.m_bBkgCapture;

		// Get the video file
		m_bBkgVid = dlg.m_bBkgVideo;
		if (m_bBkgVid)
			m_csBkgVid = dlg.m_csBkgVideo;
		else
			m_csBkgVid.Empty();

		// Get the image file
		m_bBkgOn = dlg.m_bBkgOn;
		if (m_bBkgOn)
			m_csBkgImage = dlg.m_csBkgImage;
		else
			m_csBkgImage.Empty();

		// Get the mix amount
		m_bBkgMix = dlg.m_bBkgMix;
		if (m_bBkgMix)
			m_iMixAmt = dlg.m_iMixAmt;

		// Get the background color
		m_bBkgSolid = dlg.m_bBkgSolid;
		if (m_bBkgSolid)
			 m_crBkgSolid = dlg.m_crColor;

		// Get the transition effect
		m_bTransition = dlg.m_bTransition;
		if (m_bTransition)
			m_nTransStep = dlg.m_iStepAmt;
	}
}

void CProjectDlg::OnKeyDownVerseList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDown->wVKey == VK_DELETE)
		OnRemoveVerse();
	*pResult = 0;
}

BOOL CProjectDlg::OnHelpInfo(HELPINFO * lpHelpInfo)
{
	HtmlHelp(NULL,HH_DISPLAY_TOC);
	return TRUE;
}

void CProjectDlg::OnQBHelp()
{
	OnHelpInfo(NULL);
}

// Update the preview window to reflect the output monitor
void CProjectDlg::OnMonitor()
{
	int iFirstMonitor = m_MonitorList.GetCurSel();
	if (iFirstMonitor == -1)
		iFirstMonitor = 0;
	g_pApp->m_pPreviewDlg->m_iMonitor = iFirstMonitor;
	g_pApp->m_pPreviewDlg->SetupPreview();
	g_pApp->m_pPreviewDlg->RedrawWindow();
}

// Show or Hide the preview window
void CProjectDlg::OnShowHidePreview()
{
	CPreviewDlg * pDlg = g_pApp->m_pPreviewDlg;

	if (m_bPreviewOn)
	{
		// Stop the video
		if (pDlg->m_bBkgVid && pDlg->m_bMMTimer)
		{
			timeKillEvent(pDlg->m_mmPrevTimer);
			pDlg->m_mmPrevTimer = 0;
			pDlg->m_bPrevVideoOnly = FALSE;
		}

		m_bPreviewOn = false;
		m_Preview.SetRedraw(FALSE);
		m_Preview.UpdateBitmap(IDB_OFFMONITOR);
		m_Preview.SetWindowText(g_pApp->QBString(202));
		m_Preview.SetRedraw();
		m_Preview.RedrawWindow();
		pDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		m_bPreviewOn = true;
		m_Preview.SetRedraw(FALSE);
		m_Preview.UpdateBitmap(IDB_ONMONITOR);
		m_Preview.SetWindowText(g_pApp->QBString(201));
		m_Preview.RedrawWindow();
		m_Preview.SetRedraw();
		pDlg->ShowWindow(SW_SHOW);
		OnPreview();
	}
}

// Activate the search
void CProjectDlg::OnSearch()
{
#if !defined(LITE)
	if (g_pApp->m_pVerseFinderDlg)
	{
		g_pApp->m_pVerseFinderDlg->ShowWindow(SW_SHOW);
		g_pApp->m_pVerseFinderDlg->BringWindowToTop();
	}
#endif
}

// Toggle between the scripture and advanced settings mode
void CProjectDlg::OnMode(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iMode = m_Mode.GetCurSel();
	m_VerseList.UpdateSettingsView();
	m_VerseList.ShowWindow(iMode == VERSEVIEW ? SW_SHOW : SW_HIDE);
	m_SettingsList.ShowWindow(iMode == SETTINGSVIEW ? SW_SHOW : SW_HIDE);
	*pResult = 0;
}

void CProjectDlg::SetPointSize(int iPointSize)
{
	m_iPointSize = iPointSize;
	UpdateData(FALSE);
}

int CProjectDlg::GetPointSize()
{
	return m_iPointSize;
}

int CProjectDlg::GetMaxPointSize()
{
	return m_iMaxPointSize;
}

// Check and update the largest font size
void CProjectDlg::CheckAllFontSize()
{
	// Detect the font size for the longest verse
	int iMinPointSize = DetectAllFontSize();

	// If there was a smaller point size than the current
	if (iMinPointSize)
	{
		// Offer the detected point size as the default point size for non-autosizing
		CheckUpdatePointSize(iMinPointSize);
	}
}

// Offer the detected point size as the default point size for non-autosizing
void CProjectDlg::CheckUpdatePointSize(int iMinPointSize)
{
	CString csCaption = g_pApp->QBString(259);
	CString csFontFormat = g_pApp->QBString(249);
	CString csFontMessage;
	csFontMessage.Format(csFontFormat,m_csMinPointSizeBible,iMinPointSize);
	CMessageBox MsgBox(csCaption,csFontMessage,CMessageBox::YESNOCANCEL);
	INT_PTR iChoice = MsgBox.DoModal();
	if (iChoice == IDOK)
		SetPointSize(iMinPointSize);
	else if (iChoice == IDCANCEL)
		m_bAskCheckUpdatePointSize = false;
}

// Detect the font size for the longest verse in all the loaded bibles
int CProjectDlg::DetectAllFontSize()
{
	// Detect the longest verse
	bool bPointSize = false;
	int iMinPointSize = m_iPointSize;
	int nTranslations = g_pApp->LookupNBTrans();
	for (int iTranslation = 1;iTranslation <= nTranslations;++iTranslation)
	{
		if (g_pApp->IsLoaded(iTranslation))
		{
			CVerse Verse = g_pApp->LookupLongestVerse(iTranslation);
			int iPointSize = DetectFontSize(&Verse);
			if (iPointSize >= m_iPointSizeMin && iPointSize < iMinPointSize)
			{
				m_csMinPointSizeBible = g_pApp->LookupTranslationName(iTranslation);
				iMinPointSize = iPointSize;
				bPointSize = true;
			}
		}
	}
	return bPointSize ? iMinPointSize : 0;
}

// Detect the font size for the verse
int CProjectDlg::DetectFontSize(CVerse * pVerse)
{
	int iPointSize = 0;
	if (m_bAskCheckUpdatePointSize)
	{
		// Invisibly render the verse to detect the point size
		g_pApp->m_pPreviewDlg->m_pSizeVerse = pVerse;
		g_pApp->UpdateLastFrame(false);

		// Get the detected point size
		iPointSize = g_pApp->m_pPreviewDlg->GetPtSize();
		iPointSize /= 10;

		// Clear the frame
		g_pApp->m_pPreviewDlg->m_pSizeVerse = NULL;
	}

	// return the point size
	return iPointSize;
}

// Get the current font name
CString CProjectDlg::GetCurrentFontName()
{
	return m_csFont;
}

void CProjectDlg::OnCancel()
{
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(279);
	CMessageBox MsgBox(csCaption,csMessage);
	INT_PTR iChoice = MsgBox.DoModal();
	if (iChoice == IDOK)
	{
		if (m_bPreviewOn)
			OnShowHidePreview();
		if (m_bFullScreen)
			OnProject();
		EndDialog(IDCANCEL);
	}
}

void CProjectDlg::OnFontSelect()
{
	CChooseFont dlg(m_csaFont,m_csFont,m_iPointSize,m_iPointSizeMin,m_iPointSizeMax);
	if (dlg.DoModal() == IDOK)
	{
		m_csFont = dlg.GetFont();
		m_iPointSize = dlg.GetPointSize();
		UpdateData(FALSE);
	}
}

// Get the text of the Quick Verse controls
void CProjectDlg::GetQuickText(int & iTranslation,int & iBook,CString & csTranslationText,CStringArray & csaBookList,CString & csBookText,CStringArray & csaChapterList,CString & csChapter,CStringArray & csaVerseList,CString & csFromVerse,CString & csToVerse)
{
	// Get the translation text
	m_QuickTranslation.GetWindowText(csTranslationText);
	if (csTranslationText.IsEmpty())
		return;

	// Test for loading a new translation and/or refreshing the book list
	if (m_csLastTranslationText != csTranslationText)
	{
		// Lookup the translation number
		iTranslation = g_pApp->LookupTranslationIndex(csTranslationText);
		if (!iTranslation)
			return;

		// Load the translation if necessary
		if (!g_pApp->IsLoaded(iTranslation))
			g_pApp->LoadTranslation(iTranslation);

		// Get the book list for the translation
		g_pApp->GetList(csaBookList,csTranslationText);

		// Update the last values
		m_csaLastBookList.RemoveAll();
		m_csaLastBookList.Append(csaBookList);
		m_csLastTranslationText = csTranslationText;
		m_iLastTranslation = iTranslation;
	}
	else
	{
		iTranslation = m_iLastTranslation;
		csaBookList.Append(m_csaLastBookList);
	}

	// Get the book text
	m_QuickBook.GetWindowText(csBookText);
	if (csBookText.IsEmpty())
		return;

	if (m_csLastBookText != csBookText)
	{
		// Lookup the book number
		iBook = g_pApp->LookupBookIndex(iTranslation,csBookText);
		if (!iBook)
			return;

		// Get the chapter list for the book
		g_pApp->GetList(csaChapterList,csTranslationText,csBookText);

		// Update the last values
		m_csaLastChapterList.RemoveAll();
		m_csaLastChapterList.Append(csaChapterList);
		m_csLastBookText = csBookText;
		m_iLastBook = iBook;
	}
	else
	{
		iBook = m_iLastBook;
		csaChapterList.Append(m_csaLastChapterList);
	}

	// Get the chapter text
	m_QuickChapter.GetWindowText(csChapter);
	if (csChapter.IsEmpty())
		return;

	if (m_csLastChapter != csChapter)
	{
		// Get the verse list
		g_pApp->GetList(csaVerseList,csTranslationText,csBookText,csChapter);

		// Update the last values
		m_csaLastVerseList.RemoveAll();
		m_csaLastVerseList.Append(csaVerseList);
		m_csLastChapter = csChapter;
	}
	else
	{
		csaVerseList.Append(m_csaLastVerseList);
		csChapter = m_csLastChapter;
	}

	// Get the verse
	m_QuickFromVerse.GetWindowText(csFromVerse);
	m_QuickToVerse.GetWindowText(csToVerse);
}

// Lost the focus of the Quick Translation, process the contents for predictive text and the other Quick controls
void CProjectDlg::OnKillFocusQuickVerse()
{
	// Get the contents of the Quick Verse controls
	int iTranslation = 0,iBook = 0;
	CString csTranslationText,csBookText,csChapter,csFromVerse,csToVerse;
	CStringArray csaBookList,csaChapterList,csaVerseList;
	GetQuickText(iTranslation,iBook,csTranslationText,csaBookList,csBookText,csaChapterList,csChapter,csaVerseList,csFromVerse,csToVerse);

	// Test the translation text
	if (csTranslationText.IsEmpty())
		return;

	// Update the predictive text for the book quick edit
	if (csaBookList.GetCount())
		m_QuickBook.SetPredictiveText(csaBookList);

	// Update the predictive text for the chapter quick edit
	if (csaChapterList.GetCount())
		m_QuickChapter.SetPredictiveText(csaChapterList);

	// Update the predictive text for the verse quick edits
	if (csaVerseList.GetCount())
	{
		m_QuickFromVerse.SetPredictiveText(csaVerseList);
		m_QuickToVerse.SetPredictiveText(csaVerseList);
	}

	// Lookup the verse
	CVerse Verse;
	Verse.m_iTranslation = iTranslation;
	Verse.m_iBook = iBook;
	Verse.m_iChapter = atoi(csChapter);
	Verse.m_iVerseFrom = atoi(csFromVerse);
	Verse.m_iVerseTo = atoi(csToVerse);

	// Update the invalid fields
	CVerse Verse2 = Verse;

	// Test the translation
	Verse2.m_iTranslation = g_pApp->LookupTranslationIndex(csTranslationText);
	if (Verse2.m_iTranslation != Verse.m_iTranslation)
	{
		csTranslationText = g_pApp->LookupTranslationName(Verse2.m_iTranslation);
		m_QuickTranslation.SetWindowText(csTranslationText);
	}

	// Test the book
	int nBooks = g_pApp->LookupNBBooks(Verse2.m_iTranslation);
	if (Verse2.m_iBook > nBooks)
		Verse2.m_iBook = 1;
	if (Verse2.m_iBook != Verse.m_iBook)
	{
		csBookText = g_pApp->LookupBookName(Verse2.m_iTranslation,Verse2.m_iBook);
		m_QuickBook.SetWindowText(csBookText);
	}

	// Test the chapter
	int nChapters = g_pApp->LookupNBChap(Verse2.m_iTranslation,Verse2.m_iBook);
	if (Verse2.m_iChapter > nChapters)
		Verse2.m_iChapter = 1;
	if (Verse2.m_iChapter != Verse.m_iChapter)
	{
		csChapter.Format("%d",Verse2.m_iChapter);
		m_QuickChapter.SetWindowText(csChapter);
	}

	// Test the from verse
	int nVerses = g_pApp->LookupNBVerses(Verse2.m_iTranslation,Verse2.m_iBook,Verse2.m_iChapter);
	if (Verse2.m_iVerseFrom > nVerses)
		Verse2.m_iVerseFrom = 1;
	if (Verse2.m_iVerseFrom != Verse.m_iVerseFrom)
	{
		csFromVerse.Format("%d",Verse2.m_iVerseFrom);
		m_QuickFromVerse.SetWindowText(csFromVerse);
	}

	// Test the to verse
	if (Verse2.m_iVerseTo < Verse2.m_iVerseFrom || Verse2.m_iVerseTo > nVerses)
		Verse2.m_iVerseTo = Verse2.m_iVerseFrom;
	if (Verse2.m_iVerseTo != Verse.m_iVerseTo)
	{
		csToVerse.Format("%d",Verse2.m_iVerseTo);
		m_QuickToVerse.SetWindowText(csToVerse);
	}

	// Test a missing from verse
	if (csFromVerse.IsEmpty() && csToVerse.GetLength())
	{
		csFromVerse = csToVerse;
		m_QuickFromVerse.SetWindowText(csFromVerse);
	}

	// Test a missing to verse
	if (csToVerse.IsEmpty() && csFromVerse.GetLength())
	{
		csToVerse = csFromVerse;
		m_QuickToVerse.SetWindowText(csToVerse);
	}

	// Update the button
	m_AddQuickVerse.EnableWindow(csTranslationText.GetLength() && csBookText.GetLength() && csChapter.GetLength() && csFromVerse.GetLength() && csToVerse.GetLength());

	// Update the controls
	UpdateData(FALSE);
}

// Add a verse that was keyed by the quick verse controls
void CProjectDlg::OnAddQuickVerse()
{
	// Get the contents of the Quick Verse controls
	int iTranslation = 0,iBook = 0;
	CString csTranslationText,csBookText,csChapter,csFromVerse,csToVerse;
	CStringArray csaBookList,csaChapterList,csaVerseList;
	GetQuickText(iTranslation,iBook,csTranslationText,csaBookList,csBookText,csaChapterList,csChapter,csaVerseList,csFromVerse,csToVerse);

	// Lookup the verse
	CVerse Verse;
	Verse.m_iTranslation = iTranslation;
	Verse.m_iBook = iBook;
	Verse.m_iChapter = atoi(csChapter);
	Verse.m_iVerseFrom = atoi(csFromVerse);
	Verse.m_iVerseTo = atoi(csToVerse);
	CString csVerse = Verse.LookupVerse();

	if (csVerse.Find(g_pApp->QBString(225)) != 0)
	{
		// Add the display portion
		Verse.m_iPointSize = GetPointSize();
		Verse.m_csFont = GetCurrentFontName();
		Verse.m_iMaxPointSize = GetMaxPointSize();

		// Add the verse
		m_VerseList.AddExistingVerse(Verse);

		// Resize the columns
		m_VerseList.ResizeColumns();

		// Redraw the settings view
		m_VerseList.UpdateSettingsView();

		// Set the focus back to the quick book
		m_QuickBook.SetFocus();
		m_QuickBook.SetSel(0,-1);
	}
	else
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(226);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

// Show the content management dialog
void CProjectDlg::OnManageContent()
{
#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
	g_pApp->m_pManageContentDlg->ShowWindow(SW_SHOW);
#endif
}

// Process the click
void CProjectDlg::ProcessClick(CListCtrl * pList,int iItem,int iMode)
{
	// The verses
	CVerseList * pVerseList = NULL;
	if (iMode == 1)
		pVerseList = (CVerseList *)pList;

	// The settings
	CSettingsList * pSettingsList = NULL;
	if (iMode == 2)
		pSettingsList = (CSettingsList *)pList;

	// Test the key state
	if (!m_iCtrl && !m_iShift)
	{
		// Unselect all
		if (pVerseList) pVerseList->UnSelectAll();
		if (pSettingsList) pSettingsList->UnSelectAll();
	}

	if (!m_bFirstVerse)
	{
		// Save the item
		m_bFirstVerse = TRUE;
		m_iFirstItem = iItem;

		// Toggle the item
		if (pVerseList) pVerseList->Toggle(iItem);
		if (pSettingsList) pSettingsList->Toggle(iItem);
	}
	else if (m_iShift)
	{
		if (m_bFirstVerse)
		{
			// Save the item
			m_bFirstVerse = FALSE;
			m_iLastItem = iItem;

			// Get the items
			int nFirstItem = min(m_iFirstItem,m_iLastItem);
			int nLastItem = max(m_iFirstItem,m_iLastItem);

			// Select the items
			for (iItem = nFirstItem;iItem <= nLastItem;++iItem)
			{
				if (pVerseList) pVerseList->Select(iItem);
				if (pSettingsList) pSettingsList->Select(iItem);
			}
		}
	}
	else
	{
		// Toggle the item
		if (pVerseList) pVerseList->Toggle(iItem);
		if (pSettingsList) pSettingsList->Toggle(iItem);
	}
}