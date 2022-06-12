// Preview.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "Preview.h"
#include "ProjectDlg.h"
#include "CaptureThread.h"
#include <cstringt.h>
#include <math.h>
#include <atlimage.h>

using namespace std;

#ifdef _DEBUG
static CString g_csRender;
#endif

// The main application object
extern CProjectApp * g_pApp;
static CProjectDlg * g_pDlg = NULL;

IMPLEMENT_DYNAMIC(CPreviewDlg, CDialog)

CPreviewDlg::CPreviewDlg(CWnd * pParent /*=NULL*/) :
	CDialog(CPreviewDlg::IDD, pParent)
{
	g_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;
	m_hIcon = g_pApp->LoadIcon(IDR_MAINFRAME);
	m_pParent = pParent;
	m_nID = CPreviewDlg::IDD;
	m_bInitDialog = false;
	m_bSearchPreview = FALSE;
	m_iMaxPtSize = 10000;
	m_pSizeVerse = NULL;
	m_bMMTimer = TRUE;
	m_pFullLock = NULL;
	m_pPrevLock = NULL;
	m_pCaptureThread = NULL;

	// Initialize the timers
	m_mmFullTimer = 0;
	m_mmPrevTimer = 0;

	// Test for multimedia timers
	if (timeGetDevCaps(&m_tc,sizeof(TIMECAPS)) == TIMERR_NOERROR)
	{
		// Begin the period
		m_wTimerRes = min(max(m_tc.wPeriodMin,TARGET_RESOLUTION),m_tc.wPeriodMax);
		timeBeginPeriod(m_wTimerRes); 
		m_dwMMTIME = timeGetTime();
	}
	else
		m_bMMTimer = FALSE;

	// Set the video-only status
	m_bPrevVideoOnly = FALSE;
	m_bFullVideoOnly = FALSE;

	// Create the Display DC
	m_DCDisplay.CreateCompatibleDC(NULL);
}

CPreviewDlg::~CPreviewDlg()
{
	// End the capture thread
	m_pCaptureThread->PostThreadMessage(WM_ENDTHREAD,0,0);

	// Delete the locking objects
	if (m_pFullLock)
		delete m_pFullLock;
	if (m_pPrevLock)
		delete m_pPrevLock;

	// End the period
	if (m_bMMTimer)
		timeEndPeriod(m_wTimerRes); 

	// Cleanup the frame
	DeleteFrame(m_DIBPrevFore);

	// Cleanup the frame
	DeleteFrame(m_DIBPrevBack);

	// Cleanup the frame
	DeleteFrame(m_DIBFullFore);

	// Cleanup the frame
	DeleteFrame(m_DIBFullBack);

	// Cleanup the frame
	DeleteFrame(m_DIBFullBackImg);

	// Cleanup the frame
	DeleteFrame(m_DIBPrevBackImg);

	// Cleanup the frame
	DeleteFrame(m_DIBBkgImage);

	// Clean up the frame
	DeleteFrame(m_DIBPreview);

	// Clean up the frame
	DeleteFrame(m_DIBPreview2);

	// Clean up the frame
	DeleteFrame(m_DIBFull);

	// Clean up the frame
	DeleteFrame(m_DIBFull2);

	// Clean up the frame
	DeleteFrame(m_DIBFullTrans);

	// Clean up the frame
	DeleteFrame(m_DIBPrevTrans);
}

BOOL CPreviewDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		WPARAM WP = pMsg->wParam;
		if (WP == VK_RIGHT || WP == VK_LEFT)
		{
			// Set the next verse
			if (WP == VK_RIGHT)
				g_pDlg->OnNextVersePreview();
			else
				g_pDlg->OnLastVersePreview();
			
			// Restore the preview window
			BringWindowToTop();

			// Give the preview window input
			SetFocus();

			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CPreviewDlg::Create()
{
	return CDialog::Create(m_nID,m_pParent);
}

void CPreviewDlg::Destroy()
{
	DestroyWindow();
}

void CPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CPreviewDlg::PostNcDestroy()
{
	delete this;
}

BOOL CPreviewDlg::OnInitDialog()
{
	// Set the icon
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	return CDialog::OnInitDialog();
}

// Setup the preview window
void CPreviewDlg::SetupPreview()
{
	// Get the window size of the active window
	m_x = m_vMonRect[m_iMonitor].Width();
	m_y = m_vMonRect[m_iMonitor].Height();

	// Get the caption height
	m_nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
	m_nFrameWidth = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	m_nFrameHeight = GetSystemMetrics(SM_CYSIZEFRAME) * 2;

	// The Max and Min sizes of the preview window
	int iYTot = m_y + m_nCaptionHeight + m_nFrameHeight;
	int iYMax = 768 + m_nFrameHeight + m_nCaptionHeight;
	int iYMin = 100 + m_nFrameHeight + m_nCaptionHeight;
	m_ptMaxTrack.y = min(iYMax,iYTot);
	m_ptMaxTrack.x = min(XFromY(iYMax),XFromY(iYTot));
	m_ptMinTrack.y = min(iYMin,m_ptMaxTrack.y);
	m_ptMinTrack.x = min(XFromY(iYMin),m_ptMaxTrack.x);

	// The start size of the preview window
	int iYStart = 200 + m_nFrameHeight + m_nCaptionHeight;
	m_nY = min(iYStart,m_ptMaxTrack.y);
	m_nX = min(XFromY(m_nY),m_ptMaxTrack.x);
	MoveWindow(0,0,m_nX,m_nY,1);

	// Get the dialog dimensions
	GetWindowRect(&m_DialogRect);

	// Work in client coordinates
	ScreenToClient(&m_DialogRect);
	m_nX = m_DialogRect.Width() - m_nFrameWidth;
	m_nY = m_DialogRect.Height() - m_nFrameHeight - m_nCaptionHeight;

	// Create a locking object
	m_pFullLock = new CSingleLock(&m_FullCritSect);
	m_pPrevLock = new CSingleLock(&m_PrevCritSect);

	// Create the capture thread
	m_pCaptureThread = new CCaptureThread;
	m_pCaptureThread->CreateThread();

	// Allow the sizing code to activate
	m_bInitDialog = true;
}

BEGIN_MESSAGE_MAP(CPreviewDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CPreviewDlg message handlers
void CPreviewDlg::OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI)
{
	if (m_DialogRect.IsRectEmpty())
		return;
	lpMMI->ptMinTrackSize = m_ptMinTrack;
	lpMMI->ptMaxTrackSize = m_ptMaxTrack;
}

void CPreviewDlg::OnSize(UINT nType,int cx,int cy)
{
	if (!m_bInitDialog)
		return;

	// Turn off drawing
	SetRedraw(FALSE);

	// Get the dialog rectangle
	CRect DialogRect;
	GetWindowRect(&DialogRect);
	ScreenToClient(&DialogRect);

	// Get the dialog size delta
	CPoint ptDiff(DialogRect.right - m_DialogRect.right,DialogRect.bottom - m_DialogRect.bottom);

	// Update the size of the window
	m_nX = DialogRect.Width() - m_nFrameWidth;
	m_nY = DialogRect.Height() - m_nFrameHeight - m_nCaptionHeight;

	// Redraw the resized controls
	SetRedraw();
	Invalidate();
	UpdateWindow();

	// Update the dialog size
	m_DialogRect = DialogRect;
}

// Only allow proportional resizing of the preview window (retain the aspect ratio)
void CPreviewDlg::OnSizing(UINT nSide,LPRECT lpRect)
{
	int iX = XFromY(lpRect);
	lpRect->right = lpRect->left + iX;
	CDialog::OnSizing(nSide,lpRect);
}

// Compute the rectangles proportional width based on the height
int CPreviewDlg::XFromY(LPRECT lpRect,double dC)
{
	double dY = lpRect->bottom - lpRect->top  - m_nFrameHeight - m_nCaptionHeight;
	dY = dY * dC + .5;
	int iX = (int)dY + m_nFrameWidth;
	return iX;
}

// Compute the rectangles proportional width based on the height
int CPreviewDlg::XFromY(int iY,double dC)
{
	CRect Rect(0,0,0,iY);
	return XFromY(&Rect,dC);
}

// Create the frame
void CPreviewDlg::CreateFrame(CDIBFrame & Frame,bool bThreaded,int x,int y)
{
	// Set the frame dimension
	if (x == 0)
		x = m_x;
	if (y == 0)
		y = m_y;

	// Initialize the frame
	Frame.Init(x,y,bThreaded);
}

// Delete the frame
void CPreviewDlg::DeleteFrame(CDIBFrame & Frame)
{
	// Delete the frame
	Frame.DeleteFrame();
}
							   
// Copy one frame to another
void CPreviewDlg::CopyFrame(CDIBFrame & DIBSrc,CDIBFrame & DIBDest)
{
	CDC & DCDest = DIBDest;
	DCDest.BitBlt(0,0,DIBDest.m_x,DIBDest.m_y,DIBSrc,0,0,SRCCOPY);
}

BOOL CPreviewDlg::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

void CPreviewDlg::OnPaint()
{
	// Test for a valid rectangle
	if (!m_x || !m_y)
		return;

	// Device context for painting
	CPaintDC dc(this);

	// Check for drawing the last frame
	Render(m_nX,m_nY);

	// Test for per verse transitioning
	BOOL bTransition = g_pDlg->m_bTransition;
	int nTransStep = g_pDlg->m_nTransStep;

	// Test for rendering invisibly to detect best size
	if (!m_pSizeVerse)
	{
		if (m_bSearchPreview)
		{
			CVerse & Verse = m_SearchVerse;
			if (Verse.m_bUseSettings)
			{
				bTransition = Verse.m_bTransition;
				if (bTransition)
					nTransStep = Verse.m_nTransStep;
			}
		}
		else
		{
			// Get the first selected list item
			int nVerses = (int)g_pApp->m_vecVerses.size();
			for (int iVerse = 0;iVerse < nVerses;++iVerse)
			{
				CVerse & Verse = g_pApp->m_vecVerses[iVerse];
				if (Verse.m_bSelected && Verse.m_bUseSettings)
				{
					bTransition = Verse.m_bTransition;
					if (bTransition)
						nTransStep = Verse.m_nTransStep;
					break;
				}
			}
		}
	}

	// Detect the transition
	if (bTransition)
	{
		// Initialize the transition frame
		int x = m_DIBPreview.m_x;
		int y = m_DIBPreview.m_y;
		m_DIBPrevTrans.Init(x,y,true);

		// Transition the frames
		for (int iMixAmt = 0;iMixAmt < 256;iMixAmt += nTransStep)
		{
			// Transition the next to the last frame with the last frame
			m_DIBPrevTrans.TransitionFrame(iMixAmt,m_DIBPreview,m_DIBPreview2);

			// Transfer the memory based bitmap to the display
			dc.BitBlt(0,0,m_nX,m_nY,m_DIBPrevTrans,0,0,SRCCOPY);
		}
	}

	// Transfer the memory based bitmap to the display
	dc.BitBlt(0,0,m_nX,m_nY,m_DIBPreview,0,0,SRCCOPY);

	return;
}

// Create the font used for preview and full size text, it will be proportional to the full window height and requested point size
void CPreviewDlg::CreateTheFont(int y,CFont & HorzFont)
{
	// Create the font that is sized to the window and is proportional to the point size
	HorzFont.DeleteObject();
	double dy = (double)y / (double)m_y;
	int iLogPixelsY = GetDeviceCaps(m_DCDisplay,LOGPIXELSY);
	LONG lfHeight = MulDiv(m_iPtSize,iLogPixelsY,72);
	lfHeight = (LONG)((double)lfHeight * dy + .5);
	LOGFONT LogFont;
	memset(&LogFont,0,sizeof(LOGFONT));
	LogFont.lfHeight = lfHeight;
	LogFont.lfCharSet = DEFAULT_CHARSET;
	LogFont.lfQuality = PROOF_QUALITY;
	strncpy(LogFont.lfFaceName,m_csFontName.GetBuffer(),min(m_csFontName.GetLength(),sizeof(LogFont.lfFaceName)));
	const LOGFONT * lpLogFont = &LogFont;
	HorzFont.CreatePointFontIndirect(lpLogFont);
}

// Multimedia timer callback for the full sized frame
void CALLBACK CPreviewDlg::FullTimeProc(UINT uID,UINT uMsg,DWORD_PTR dwUser,DWORD_PTR dw1,DWORD_PTR dw2)
{
	// Test for the screen being visible
	if (g_pDlg && g_pDlg->m_bFullScreen)
	{
		// Render the full screen window
		g_pDlg->m_FullWnd.RedrawWindow();
	}
}

// Multimedia timer callback for the preview sized frame
void CALLBACK CPreviewDlg::PrevTimeProc(UINT uID,UINT uMsg,DWORD_PTR dwUser,DWORD_PTR dw1,DWORD_PTR dw2)
{
	// Test for the screen being visible
	if (g_pDlg && g_pDlg->m_bPreviewOn)
	{
		// Render the preview dialog
		g_pApp->UpdateLastFrame(true);
	}
}

// Render the frame
void CPreviewDlg::Render(int x,int y)
{
	// Test for locking
	if (m_pFullLock->IsLocked() || m_pPrevLock->IsLocked())
		return;

	// Test for a full size render
	bool bFullSize = false;
	if (x == -1 && y == -1)
	{
		// Set the coordinates
		x = m_x;
		y = m_y;
		bFullSize = true;
	}

	// Lock the resource
	CSingleLock * pLock = bFullSize ? m_pFullLock : m_pPrevLock;
	pLock->Lock();

	// Initialize the verse
	CVerse * pVerse = NULL;

	// Test for a search preview
	if (m_bSearchPreview)
		pVerse = &m_SearchVerse;
	else if (m_pSizeVerse)
		pVerse = m_pSizeVerse;

	// Initialize the background effect
	m_bBkgCapture = FALSE;
	m_bBkgImage = FALSE;
	m_bBkgVid = FALSE;
	m_bBkgMix = FALSE;
	m_bBkgSolid = FALSE;

	// Per verse customization
	bool bInitSettings = false;

	// Get the first selected list item, unless the verse is being auto-sized or search previewed
	int nVerses = (int)g_pApp->m_vecVerses.size();
	if (pVerse)
		nVerses = 1;
	for (int iVerse = 0;iVerse < nVerses;++iVerse)
	{
		CVerse & Verse = pVerse ? *pVerse : g_pApp->m_vecVerses[iVerse];
		if (Verse.m_bSelected && Verse.m_bUseSettings)
		{
			// Get the point sizes
			m_iPtSize = Verse.m_iPointSize * 10;
			m_iMaxPtSize = Verse.m_iMaxPointSize * 10;

			// Get the text and background colors
			m_crTxt = Verse.m_crTxt;
			m_crBkg = Verse.m_crBkg;

			// Get the  text justification
			m_bCenterJustify = Verse.m_bCenterJustify;

			// Test for a background capture
			m_bBkgCapture = Verse.m_bBkgCapture;

			// Test for a background image
			m_csBkgImage.Empty();
			if (Verse.m_bBkgOn)
			{
				// Get the background image
				m_csBkgImage = Verse.m_csBkgImage;
				m_bBkgImage = TRUE;
			}

			// Test for a background video
			m_csBkgVid.Empty();
			if (Verse.m_bBkgVid)
			{
				// Get the background video
				m_csBkgVid = Verse.m_csBkgVid;
				m_bBkgVid = TRUE;
			}

			// Test for a background color used like an image
			if (Verse.m_bBkgSolid)
			{
				// Get the background colors
				m_bBkgSolid = Verse.m_bBkgSolid;
				m_crBkgSolid = Verse.m_crBkgSolid;
			}

			// Test for alphablending
			m_bBkgMix = Verse.m_bBkgMix;
			if (m_bBkgMix)
			{
				// Get the blend level
				m_iMixAmt = (UINT)((double)Verse.m_iMixAmt * 2.55 + .5);
				if (m_iMixAmt > 255)
					m_iMixAmt = 255;
			}

			// Set the autosize option
			m_bAutoSizeFont = Verse.m_bAutoSizeFont;

			// Get the font
			m_csFontName = Verse.m_csFont;

			// Per verse customization is used
			bInitSettings = true;
			break;
		}
	}

	// Test for using global verse settings
	if (!bInitSettings)
	{
		// Set the point sizes
		m_iPtSize = g_pDlg->m_iPointSize * 10;
		m_iMaxPtSize = g_pDlg->m_iMaxPointSize * 10;

		// Set the text and background colors
		m_crTxt = g_pDlg->m_crTxt;
		m_crBkg = g_pDlg->m_crBkg;

		// Set the justification
		m_bCenterJustify = g_pDlg->m_bCenterJustify;
		m_csBkgImage.Empty();

		// Set the capture setting
		m_bBkgCapture = g_pDlg->m_bBkgCapture;

		// Test for a background image
		if (g_pDlg->m_bBkgOn)
		{
			// Set the background image
			m_csBkgImage = g_pDlg->m_csBkgImage;
			m_bBkgImage = TRUE;
		}

		// Test for a background video
		m_csBkgVid.Empty();
		if (g_pDlg->m_bBkgVid)
		{
			// Get the background video
			m_csBkgVid = g_pDlg->m_csBkgVid;
			m_bBkgVid = TRUE;
		}

		// Test for a background color used like an image
		if (g_pDlg->m_bBkgSolid)
		{
			// Set the masking background color
			m_bBkgSolid = g_pDlg->m_bBkgSolid;
			m_crBkgSolid = g_pDlg->m_crBkgSolid;
		}

		// Test for alphablending
		m_bBkgMix = g_pDlg->m_bBkgMix;
		if (m_bBkgMix)
		{
			// Set the blend level
			m_iMixAmt = (UINT)((double)g_pDlg->m_iMixAmt * 2.55 + .5);
			if (m_iMixAmt > 255)
				m_iMixAmt = 255;
		}

		// Set the autosize option
		m_bAutoSizeFont = g_pDlg->m_bAutoSizeFont;

		// Get the font
		m_csFontName = g_pDlg->m_csFont;
	}

	// Showing the translation and passage is only a global setting
	m_bShowTranslation = g_pDlg->m_bShowTranslation;
	m_bShowPassage = g_pDlg->m_bShowPassage;

	// Set the mask color for alphablending
	m_crMask = RGB(127,127,127);
	if (m_bBkgMix && (m_crMask == m_crBkg))
		m_crMask = RGB(GetRValue(m_crMask) - 1,GetGValue(m_crMask) - 1,GetBValue(m_crMask) - 1);
	if ((m_crMask == m_crTxt) || (m_bBkgSolid && m_crMask == m_crBkgSolid))
		m_crMask = RGB(GetRValue(m_crMask) - 1,GetGValue(m_crMask) - 1,GetBValue(m_crMask) - 1);

	// Get the working frames
	CDIBFrame & DIBOut = bFullSize ? m_DIBFull : m_DIBPreview;
	CDIBFrame & DIBLast = bFullSize ? m_DIBFull2 : m_DIBPreview2;
	CDIBFrame & DIBCapture = bFullSize ? m_DIBFullCapture : m_DIBPrevCapture;
	DIBOut.Init(x,y,true);
	DIBLast.Init(x,y,false);

	// Make a copy of the last frame for the transition effect, if necessary
	if (!m_pSizeVerse)
		CopyFrame(DIBOut,DIBLast);

	// Reference the correct video file and refresh timer
	CVfw & Avi = bFullSize ? m_AviFull : m_AviPrev;
	UINT_PTR nCmd = bFullSize ? WM_FULLVIDEO : WM_PREVVIDEO;
	MMRESULT & mmTimer = bFullSize ? m_mmFullTimer : m_mmPrevTimer;
	LPTIMECALLBACK lpTimeProc = m_bBkgVid ? (bFullSize ? &CPreviewDlg::FullTimeProc : &CPreviewDlg::PrevTimeProc) : NULL;

	// Initialize the video
	if (m_bBkgVid)
		m_bBkgVid = Avi.Init(m_csBkgVid,x,y);

	// Initialize the background frame
	CDC * pDCBkgVideo = 0;
	int iStartX,iStartY;
	if (m_bBkgVid)
	{
		// Load the background image and select it into the DC and scale it to our coordinates
		LoadBackgroundVideoFrame(Avi,iStartX,iStartY);
		pDCBkgVideo = Avi;
	}
	else if (m_bBkgImage)
	{
		// Load the background image and select it into the DC and scale it to our coordinates
		LoadBackground(x,y,m_csBkgImage,m_DIBBkgImage,iStartX,iStartY);
	}
	else if (m_bBkgCapture)
	{
		// Copy the captured image
		m_DIBBkgImage.Init(x,y,false);
		CDC * pDIBBkgDC = m_DIBBkgImage;
		pDIBBkgDC->StretchBlt(0,0,x,y,DIBCapture,0,0,DIBCapture.m_x,DIBCapture.m_y,SRCCOPY);
	}

	// Isolate the full or preview frames
	CDIBFrame & DIBFore = bFullSize ? m_DIBFullFore : m_DIBPrevFore;
	CDIBFrame & DIBBack = bFullSize ? m_DIBFullBack : m_DIBPrevBack;

	// Initialize the background image/text frame
	DIBFore.Init(x,y,false);
	CDC & DCFore = DIBFore;

	// Initialize the backround image frame
	DIBBack.Init(x,y,false);
	CDC & DCBack = DIBBack;

	// Screens rectangle
	CRect Rect(0,0,x,y);

	// Paint the background image, video or background color being used as an image
	if (m_bBkgCapture)
		DCBack.BitBlt(0,0,x,y,m_DIBBkgImage,0,0,SRCCOPY);
	else if (m_bBkgVid)
		DCBack.BitBlt(0,0,x,y,pDCBkgVideo,iStartX,iStartY,SRCCOPY);
	else if (m_bBkgImage)
		DCBack.BitBlt(0,0,x,y,m_DIBBkgImage,iStartX,iStartY,SRCCOPY);
	else if (m_bBkgSolid)
		DCBack.FillSolidRect(&Rect,m_crBkgSolid);

	// Autosize start setting
	BOOL bAbortText;
	if (m_bAutoSizeFont)
		m_iPtSize = m_iMaxPtSize;

	// Point size adjustment starting values
	double dPtStart = 100.0 / (double)m_x * (double)x + .5;
	double dPtTenth = dPtStart / 10.0;
	int nPtStart = (int)dPtStart;
	if (nPtStart == 1)
		nPtStart = 10;
	int nPtTenth = (int)dPtTenth;
	if (nPtTenth == 1)
		nPtTenth = min(nPtTenth + 1,nPtStart);
	int nPtDiff = nPtStart;

	CString csOutText;
	BOOL bAutoFit = FALSE;
	BOOL bAutoSizeFont = m_bAutoSizeFont;
	int cyChar = 0;
	int iLine = 0,nLineStart = 0;
	double dMargin = 40.0 / (double)m_x * (double)x + .5;
	int cxMargin = (int)dMargin;
	int cxHMargin = cxMargin / 2,iX1 = 0,iY1 = 0,iX2 = 0,iY2 = 0;
	BOOL bCentered = FALSE;
	int nLPP = 0;
	int cyExtra = 0;
	BOOL bBkgCapture = FALSE;
	BOOL bBkgVideo = FALSE;
	BOOL bBkgImage = FALSE;
	BOOL bBkgSolid = FALSE;

CenterIt:
	while (!bAutoFit)
	{
		// Auto fit flag
		bAbortText = FALSE;

		// Set the background color to the mask
		DCFore.FillSolidRect(&Rect,m_crMask);

		// Create and select the font for the text/image display
		CFont HorzFont;
		CreateTheFont(y,HorzFont);
		CFont * pLastFont = DCFore.SelectObject(&HorzFont);

		// Get the character dimensions
		TEXTMETRIC tm;
//		DCFore.GetTextMetrics(&tm);
		DCFore.GetOutputTextMetrics(&tm);

		// Get the X and Y character sizes
		int cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		// Get the lines per page and offset for odd amounts of lines
		nLPP = y / cyChar;

		// Set the text color
		COLORREF crTxt = DCFore.SetTextColor(m_crTxt);

		// Set the background mode
		COLORREF crOldMask;
		int iMode;
		if (bBkgCapture || bBkgVideo || bBkgImage || bBkgSolid)
		{
			if (m_bBkgMix)
			{
				// Set a text background color mode
				iMode = DCFore.SetBkMode(OPAQUE);

				// Set the background color
				crOldMask = DCFore.SetBkColor(m_crBkg);
				if (iX1 && iY1 && iX2 && iY2)
					DCFore.FillSolidRect(CRect(CPoint(iX1,iY1 + cyExtra),CPoint(iX2,iY2 + cyExtra)),m_crBkg);
			}
			else
				iMode = DCFore.SetBkMode(TRANSPARENT);
		}
		else
			iMode = DCFore.SetBkMode(TRANSPARENT);

		// Start the line count
		iLine = nLineStart;

		// Get a general size of all verses, by total number of lines, for quick evaluation of the font
		BOOL bContinue = TRUE;
		if (bAutoSizeFont)
		{
			bContinue = FALSE;
			CString csAll;
			for (int iVerse = 0;iVerse < nVerses;++iVerse)
			{
				// Get the verse
				CVerse & Verse = pVerse ? *pVerse : g_pApp->m_vecVerses[iVerse];
				if (!Verse.m_bSelected)
					continue;

				// Load the translation and book strings
				CString & csTranslation = Verse.m_csTranslation;
				CString & csBook = Verse.m_csBook;

				// Get the from and to verse
				int iVerseFrom = Verse.m_iVerseFrom;
				int iVerseTo = Verse.m_iVerseTo;

				// Format the book, chapter, and from verse
				CString csText;
				if (m_bShowPassage)
				{
					csText.Format("%s %d:%d",csBook,Verse.m_iChapter,iVerseFrom);
					if (iVerseFrom != iVerseTo)
					{
						// Format the to verse
						CString csToVerse;
						csToVerse.Format("-%d",iVerseTo);
						csText += csToVerse;
					}
				}

				// Format the translation
				if (m_bShowTranslation)
				{
					if (m_bShowPassage)
					{
						CString cs = csText;
						csText.Format("%s - %s",csTranslation,cs);
					}
					else
						csText = csTranslation;
				}

				// Calculate how many segments there are.  Segment 1 is the scripture, 2 is the translations and/or passage
				int nSegments = (m_bShowTranslation || m_bShowPassage) ? 2 : 1;

				for (int i = 1;i <= nSegments;++i)
				{
					// Get the bible verse or title string
					CString csVerse = (i == 1 ? Verse.m_csVerse : csText);
					csAll += csVerse;
				}
			}

			// Characters per line
			int nTotCharsPerLine = (Rect.Width() - cxMargin) / cxChar;
			if (nTotCharsPerLine)
			{
				int nTotLines = csAll.GetLength() / nTotCharsPerLine;
				nTotLines = nTotLines + nVerses + 1;
				if (nTotLines > nLPP)
				{
					iLine = nTotLines;
					bAbortText = TRUE;
				}
				else
					bContinue = TRUE;
			}
			else
			{
				bAbortText = TRUE;
				iLine = 0;
			}
		}

		if (bContinue)
		{
			for (int iVerse = 0;(iVerse < nVerses) && !bAbortText;++iVerse)
			{
				// Get the verse
				CVerse & Verse = pVerse ? *pVerse : g_pApp->m_vecVerses[iVerse];
				if (!Verse.m_bSelected)
					continue;

				// Load the translation and book strings
				CString & csTranslation = Verse.m_csTranslation;
				CString & csBook = Verse.m_csBook;

				// Get the from and to verse
				int iVerseFrom = Verse.m_iVerseFrom;
				int iVerseTo = Verse.m_iVerseTo;

				// Format the book, chapter, and from verse
				CString csText;
				if (m_bShowPassage)
				{
					csText.Format("%s %d:%d",csBook,Verse.m_iChapter,iVerseFrom);
					if (iVerseFrom != iVerseTo)
					{
						// Format the to verse
						CString csToVerse;
						csToVerse.Format("-%d",iVerseTo);
						csText += csToVerse;
					}
				}

				// Format the translation
				if (m_bShowTranslation)
				{
					if (m_bShowPassage)
					{
						CString cs = csText;
						csText.Format("%s - %s",csTranslation,cs);
					}
					else
						csText = csTranslation;
				}

				// Align to the left and manually center when needed
				UINT uiAlign = DCFore.SetTextAlign(TA_LEFT);

				// Calculate how many segments there are.  Segment 1 is the scripture, 2 is the translations and/or passage
				int nSegments = (m_bShowTranslation || m_bShowPassage) ? 2 : 1;

				// Render the verse
				for (int i = 1;(i <= nSegments) && !bAbortText;++i)
				{
					// Characters per line
					int nMaxX = Rect.Width() - cxMargin;
					int nCPL = nMaxX / cxChar;

					// Get the bible verse or title strnig
					CString csVerse = (i == 1 ? Verse.m_csVerse : csText);

					// Size the verse
//					CSize size = DCFore.GetTextExtent(csVerse);
					CSize size = DCFore.GetOutputTextExtent(csVerse);
					if (size.cx > nMaxX)
					{
						// Process the verse to fit the number of characters per line
						while (nCPL && csVerse.GetLength())
						{
							// Recompute the CPL on each pass
							nCPL = nMaxX / cxChar;

							// Parse the verse into screen lines
							CString csLine;
							while (nCPL > 0)
							{
								// Get one screen line
								csLine = csVerse.Mid(0,nCPL);

								// Test the screen line to see if it fits
//								size = DCFore.GetTextExtent(csLine);
								size = DCFore.GetOutputTextExtent(csLine);
								if (size.cx <= nMaxX)
									break;
								nCPL--;
							}

							// If no characters fit on a line then abort
							if (nCPL == 0)
							{
								bAbortText = TRUE;
								continue;
							}

							// Trim the line
							csLine.TrimLeft();
							RightTrim(csLine);

							// Split the line at the first space so it fits and looks right
							if (csVerse.GetLength() > nCPL)
							{
								// We need at least two words on this line to let it pass
								while (!bAbortText)
								{
									// Get our remaining characters to process
									nCPL = min(nCPL,csLine.GetLength());
									while (!bAbortText)
									{
										// Look for a space
										TCHAR tch = csLine[nCPL - 1];
										if (tch == ' ')
											break;
										nCPL--;

										// Only have one word here so abort
										if (nCPL == 0)
										{
											bAbortText = TRUE;
											continue;
										}
									}

									if (bAbortText)
										continue;

									// Get the new size of the split line
									csLine = csVerse.Mid(0,nCPL);
									csLine.TrimLeft();
									RightTrim(csLine);
//									size = DCFore.GetTextExtent(csLine);
									size = DCFore.GetOutputTextExtent(csLine);

									// Split up the string for more handling
									csVerse = csVerse.Mid(nCPL);
									csVerse.TrimLeft();
									RightTrim(csVerse);
									break;
								}
							}
							else
								csVerse.Empty();
							if ((bBkgCapture || bBkgVideo || bBkgImage || bBkgSolid) && bAutoSizeFont && m_bBkgMix)
								DCFore.FillSolidRect(CRect(CPoint(cxHMargin - 2,iLine * cyChar + cyExtra),CPoint(Rect.Width() - cxHMargin + 2,(iLine + 1) * cyChar + cyExtra)),m_crBkg);

							// Output the text, check for centering
							int cxExtra = 0;
							if (m_bCenterJustify)
								cxExtra = (nMaxX - size.cx) / 2;
							DCFore.ExtTextOut(cxHMargin + cxExtra,iLine * cyChar + cyExtra,ETO_CLIPPED,&Rect,csLine,NULL);
							iLine++;
						}
					}
					else
					{
						if ((bBkgCapture || bBkgVideo || bBkgImage || bBkgSolid) && bAutoSizeFont && m_bBkgMix)
							DCFore.FillSolidRect(CRect(CPoint(cxHMargin - 2,iLine * cyChar + cyExtra),CPoint(Rect.Width() - cxHMargin + 2,(iLine + 1) * cyChar + cyExtra)),m_crBkg);

						// The entire verse fits on one line without breaking, 
						// Output the text, check for centering
						int cxExtra = 0;
						if (m_bCenterJustify)
							cxExtra = (nMaxX - size.cx) / 2;
						DCFore.ExtTextOut(cxHMargin + cxExtra,iLine * cyChar + cyExtra,ETO_CLIPPED,&Rect,csVerse,NULL);
						iLine++;
					}
				}

				// Reset the alignment
				DCFore.SetTextAlign(uiAlign);

				// Add a line between verses
				iLine++;
			}

			// Remove the last line added
			iLine--;
		}

		// Autosize the font
		if (bAutoSizeFont)
		{
			// Calculate the size to see if it fits on the screen
			int iHeight = iLine * cyChar;
			if (bAbortText || iHeight > Rect.Height())
			{
				m_iPtSize -= nPtDiff;
				if (m_iPtSize < 6)
				{
					m_iPtSize = nPtDiff;
					if (nPtDiff == nPtStart)
						nPtDiff = nPtTenth;
					else if (m_iPtSize == nPtTenth)
						nPtDiff = 1;
					else
					{
						m_iPtSize = 6;
						bAutoFit = TRUE;
					}
				}
			}
			else
			{
				// If we fit and are at the max point size then there is no more scaling
				if (m_iPtSize == m_iMaxPtSize)
					bAutoFit = TRUE;
				else
				{
					// Text fits, now size it up to fit as close to the max point size
					if (nPtDiff == nPtStart)
					{
						m_iPtSize += nPtDiff - nPtTenth;
						nPtDiff = nPtTenth;
					}
					else if (nPtDiff == nPtTenth)
					{
						m_iPtSize += nPtDiff - 1;
						nPtDiff = 1;
					}
					else
					{
						// Don't go past the maximum point size that the user set.
						if (m_iPtSize > m_iMaxPtSize)
							m_iPtSize = m_iMaxPtSize;
						else
							bAutoFit = TRUE;
					}
				}
			}
		}
		else
			bAutoFit = TRUE;

		// Reset the text color and background mode
		if ((bBkgCapture || bBkgVideo || bBkgImage || bBkgSolid) && m_bBkgMix)
			DCFore.SetBkColor(crOldMask);

		// Reset the text and background color
		DCFore.SetTextColor(crTxt);
		DCFore.SetBkMode(iMode);

		// Reset the font
		DCFore.SelectObject(pLastFont);

		// Compute the Y margin
		if (bAutoSizeFont && bAutoFit && cyExtra == 0)
		{
			int iHeight = iLine * cyChar;
			cyExtra = (Rect.Height() - iHeight) / 2;
			if (cyExtra)
				bAutoFit = FALSE;
			if (m_bBkgCapture && !bBkgCapture)
			{
				bAutoFit = FALSE;
				bBkgCapture = TRUE;
			}
			else if (m_bBkgVid && !bBkgVideo)
			{
				bAutoFit = FALSE;
				bBkgVideo = TRUE;
			}
			else if (m_bBkgImage && !bBkgImage)
			{
				bAutoFit = FALSE;
				bBkgImage = TRUE;
			}
			else if (m_bBkgSolid && !bBkgSolid)
			{
				bAutoFit = FALSE;
				bBkgSolid = TRUE;
			}
		}
	}

	// Center the text on the screen
	nLineStart = (nLPP / 2) - (iLine / 2);
	if (!bCentered && !m_bAutoSizeFont)
	{
		bAutoSizeFont = FALSE;
		bAutoFit = FALSE;
		bCentered = TRUE;
		if (m_bBkgCapture)
			bBkgCapture = m_bBkgCapture;
		else if (m_bBkgVid)
			bBkgVideo = m_bBkgVid;
		else if (m_bBkgImage)
			bBkgImage = m_bBkgImage;
		else if (m_bBkgSolid)
			bBkgSolid = m_bBkgSolid;

		iX1 = cxHMargin - 2;
		iX2 = Rect.Width() - cxHMargin + 2;
		iY1 = nLineStart * cyChar - 2;
		iY2 = (nLineStart + iLine) * cyChar + 2;

		goto CenterIt;
	}

	// Test for alphablending the result
	if (!m_pSizeVerse)
	{
		// Mix the output frame
		DIBOut.MixFrame(m_crMask,m_crTxt,m_crBkg,m_bBkgMix,m_iMixAmt,DIBFore,DIBBack);

		// Test for availability of the MM timer
		if (m_bBkgVid && !mmTimer && m_bMMTimer)
		{
			// Initialze the multimedia timer
			mmTimer = timeSetEvent(MMDELAY,MMRES,lpTimeProc,NULL,TIME_PERIODIC);
		}
	}

	// Unlock the resource
	pLock->Unlock();
}

// Load the background image
void CPreviewDlg::LoadBackground(int x,int y,CString csBkgImage,CDIBFrame & DIBBkgImage,int & iStartX,int & iStartY)
{
	// DC and CImage for drawing the frame to the background image
	bool bGDIPlus = true;
	CDC BkgDC;
	BkgDC.CreateCompatibleDC(NULL);
	CImage BkgImage;
	HBITMAP hTempBkg = NULL;
	int nBmWidth = 0,nBmHeight = 0;

	// Attempt to use GDI plus to load the image file
	HBITMAP hBkgImage = NULL;
	if (SUCCEEDED(BkgImage.Load(csBkgImage)))
		hBkgImage = (HBITMAP)BkgImage;
	else
	{
		// We can only load .BMP when GDI+ is not available
		bGDIPlus = false;
		if (csBkgImage.GetLength() > 4 && csBkgImage.Right(4).CompareNoCase(".bmp") == 0)
			hBkgImage = (HBITMAP)LoadImage(NULL,csBkgImage,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	}

	// Test for a valid HBITMAP
	if (hBkgImage)
	{
		hTempBkg = (HBITMAP)BkgDC.SelectObject(hBkgImage);
		BITMAP Bitmap;
		GetObject(hBkgImage,sizeof(BITMAP),&Bitmap);
		nBmWidth = Bitmap.bmWidth;
		nBmHeight = Bitmap.bmHeight;

		// Get the aspect ratio of the image
		double dc = (double)nBmWidth / (double)nBmHeight;

		// Fit the coordinates to the aspect ratio
		std::pair<int,int> xy(x,y);
		std::pair<int,int> xy2;
		GetAspect(dc,xy,xy2);
		int nx = xy2.first;
		int ny = xy2.second;

		// Create the background image frame
		CreateFrame(DIBBkgImage,false,nx,ny);

		// Copy and scale the background image
		iStartX = (nx - x) / 2;
		iStartY = (ny - y) / 2;
		CDC & DCBkgImage = DIBBkgImage;
		DCBkgImage.StretchBlt(0,0,nx,ny,&BkgDC,0,0,nBmWidth,nBmHeight,SRCCOPY);

		// Detach from the Background Loaded Image DC
		BkgDC.SelectObject(hTempBkg);

		// Clean up from load image
		if (!bGDIPlus)
			DeleteObject(hBkgImage);
	}
}

// Load the background video frame and scale it to our coordinates, preserving the aspect ratio
void CPreviewDlg::LoadBackgroundVideoFrame(CVfw & Avi,int & iStartX,int & iStartY)
{
	// Get the current video frame
	LONG lFrame = (LONG)clock();
	if (!Avi.DrawFrame(lFrame))
		return;

	// Get the dimensions
	int x = Avi.m_nWidth;
	int y = Avi.m_nHeight;

	// Get the starting coordinates
	iStartX = Avi.m_iStartX;
	iStartY = Avi.m_iStartY;
}

// Convert a size to the proper aspect ratio
void CPreviewDlg::GetAspect(double dc,std::pair<int,int> & xy,std::pair<int,int> & xy2)
{
	int x,y;
	x = xy.first;
	y = xy.second;
	int nx = x,ny = y;

	bool bAdd = true;
	std::map<std::pair<int,int>,std::pair<int,int> >::iterator itxy = m_mAspect.find(xy);
	if (itxy != m_mAspect.end())
	{
		// Get the saved coordinate
		nx = (*itxy).second.first;
		ny = (*itxy).second.second;
		bAdd = false;
	}
	else
	{
		// Fit the coordinates
		nx = XFromY(ny,dc);
		while (nx < x)
		{
			ny++;
			nx = XFromY(ny,dc);
		}
	}

	// Get the fit coordinates
	xy2 = std::pair<int,int>(nx,ny);

	// Add the new coordinate
	if (bAdd)
	{
		// Map the coordinates
		m_mAspect[xy] = xy2;
	}
}

int CPreviewDlg::GetPtSize()
{
	return m_iPtSize;
}

void CPreviewDlg::OnCancel()
{
	((CProjectDlg *)g_pApp->m_pMainWnd)->OnShowHidePreview();
}