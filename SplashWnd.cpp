// SplashWnd.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "SplashWnd.h"


// CSplashWnd

IMPLEMENT_DYNAMIC(CSplashWnd, CWnd)

CSplashWnd::CSplashWnd() : m_nXStart(0), m_nYStart(0), m_nWidth(0), m_nHeight(0)
{
	// Load the splash screen bitmap
	m_SplashBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_SPLASH));
	BITMAP SplashBM;
	m_SplashBitmap.GetBitmap(&SplashBM);
	
	// Calculate the starting coordinates
	m_nXStart = GetSystemMetrics(SM_CXSCREEN) / 2 - SplashBM.bmWidth / 2;
	m_nYStart = GetSystemMetrics(SM_CYSCREEN) / 2 - SplashBM.bmHeight / 2;
	m_nWidth = SplashBM.bmWidth;
	m_nHeight = SplashBM.bmHeight;
}

CSplashWnd::~CSplashWnd()
{
}

int CSplashWnd::GetXStart() const
{
	return m_nXStart;
}

int CSplashWnd::GetYStart() const
{
	return m_nYStart;
}

int CSplashWnd::GetWidth() const
{
	return m_nWidth;
}

int CSplashWnd::GetHeight() const
{
	return m_nHeight;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CSplashWnd message handlers
int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set a timer for 3 seconds
	SetTimer(0,3000,NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	// Create an in-memory DC compatible with the display
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(NULL);

	// Draw the splash bitmap
	int nFlags = DST_BITMAP | DSS_NORMAL;
	dc.DrawState(CPoint(0,0),CSize(m_nWidth,m_nHeight),(HBITMAP)m_SplashBitmap,nFlags,(HBRUSH)0);
}

void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	CWnd::OnTimer(nIDEvent);
	if (nIDEvent == 0)
	{
		KillTimer(0);
		DestroyWindow();
	}
}

void CSplashWnd::PostNcDestroy()
{
	// Clean up
	delete this;
	CWnd::PostNcDestroy();
}

void CSplashWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	KillTimer(0);
	DestroyWindow();
}
