// FullWnd.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"

#if !defined(PERSONAL) && !defined(LITE)

#include "Project.h"
#include "FullWnd.h"
#include "ProjectDlg.h"
#include <atlimage.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProjectApp * g_pApp;
static CProjectDlg * g_pDlg = NULL;
static CPreviewDlg * g_pPrevDlg = NULL;

/////////////////////////////////////////////////////////////////////////////
// CFullWnd
CFullWnd::CFullWnd()
{
}

CFullWnd::~CFullWnd()
{
}

BEGIN_MESSAGE_MAP(CFullWnd, CWnd)
	//{{AFX_MSG_MAP(CFullWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFullWnd message handlers

int CFullWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Get the dialog pointers
	g_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;
	g_pPrevDlg = g_pApp->m_pPreviewDlg;

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Hide the window until we want to project it
	m_x = 0;
	m_y = 0;
	MoveWindow(0,0,m_x,m_y);

	return 0;
}

void CFullWnd::OnPaint()
{
	// Device context for painting
	CPaintDC dc(this);

	// Check for drawing the frame
	g_pPrevDlg->Render();

	// Test for per verse transitioning
	BOOL bTransition = g_pDlg->m_bTransition;
	int nTransStep = g_pDlg->m_nTransStep;

	// Get the first selected list item
	POSITION Pos = g_pDlg->m_VerseList.GetFirstSelectedItemPosition();
	if (Pos)
	{
		int iVerse = g_pDlg->m_VerseList.GetNextSelectedItem(Pos);
		CVerse & verse = g_pApp->m_vecVerses[iVerse];
		if (verse.m_bUseSettings)
		{
			bTransition = verse.m_bTransition;
			if (bTransition)
				nTransStep = verse.m_nTransStep;
		}
	}

	// Detect the transition
	if (bTransition)
	{
		// Initialize the transition frame
		int x = g_pPrevDlg->m_DIBFull.m_x;
		int y = g_pPrevDlg->m_DIBFull.m_y;
		g_pPrevDlg->m_DIBFullTrans.Init(x,y,true);

		// Transition the frames
		for (int iMixAmt = 0;iMixAmt < 256;iMixAmt += nTransStep)
		{
			// Transition the next to the last frame with the last frame
			g_pPrevDlg->m_DIBFullTrans.TransitionFrame(iMixAmt,g_pPrevDlg->m_DIBFull,g_pPrevDlg->m_DIBFull2);

			// Transfer the memory based bitmap to the display
			dc.BitBlt(0,0,m_x,m_y,g_pPrevDlg->m_DIBFullTrans,0,0,SRCCOPY);
		}
	}

	// Transfer the preview frame buffer to the display
	dc.BitBlt(0,0,m_x,m_y,g_pPrevDlg->m_DIBFull,0,0,SRCCOPY);
}

#endif