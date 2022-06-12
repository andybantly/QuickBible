// Image.cpp : implementation file
// Copyright (C) 2008 Andrew S. Bantly
//

#include "stdafx.h"
#include "ImageStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CImageStatic, CButton)

CImageStatic::CImageStatic() : CButton(), m_iWidth(0), m_iHeight(0)
{
}

CImageStatic::~CImageStatic()
{
}

// Create the bitmap button
BOOL CImageStatic::CreateBtmp(CWnd * pWnd,int nBitmapId,int nButtonId)
{
	// Initialize the Bitmap Icon
	m_BitmapIcon.Load(nBitmapId);
	m_iWidth = m_BitmapIcon.GetWidth();
	m_iHeight = m_BitmapIcon.GetHeight();;

	// Create a new button with the proper style settings to replace the button on the form
	m_pWnd = pWnd;

	// Intercept all the buttons control messages
	SubclassDlgItem(nButtonId,m_pWnd);

	return TRUE;
}

void CImageStatic::UpdateBitmap(int nBitmapId)
{
	m_BitmapIcon.Load(nBitmapId);
}

void CImageStatic::DoDataExchange(CDataExchange* pDX)
{
	CButton::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageStatic, CButton)
	//{{AFX_MSG_MAP(CImageStatic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageStatic message handlers

void CImageStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// This code only works with buttons
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// Attach to the buttons device context
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Adjust the text rectange (down and right) if the button is depressed
	CRect ButtonRect = lpDrawItemStruct->rcItem;

	// Image centered to the button
	int nX = (ButtonRect.Width() >> 1) - (m_iWidth >> 1);
	int nY = (ButtonRect.Height() >> 1) - (m_iHeight >> 1);
	UINT nFlags = DST_ICON;
	dc.DrawState(CPoint(nX,nY),CSize(m_iWidth,m_iHeight),m_BitmapIcon,nFlags,(HBRUSH)0);

	// Detach from the device context
	dc.Detach();
}