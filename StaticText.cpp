// StaticText.cpp : implementation file
// Copyright (C) 2008 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "StaticText.h"

// CStaticText
IMPLEMENT_DYNAMIC(CStaticText, CButton)

CStaticText::CStaticText() : m_pParent(NULL), m_bResize(false), m_nXExtra(0), m_nYExtra(0)
{
}

CStaticText::~CStaticText()
{
}

void CStaticText::ParseText(CWnd * pParent,CString csText)
{
	// Set the parent
	m_pParent = pParent;

	// Only parse text that has been set
	int nLength = csText.GetLength();
	if (nLength)
	{
		// Count the lines
		int iPos = 0,iLast = nLength - 1;
		while ((iPos = csText.Find('\n',0)) != -1)
		{
			if (iPos < iLast)
			{
				// Get the current line of text
				CString csLine = csText.Left(iPos);
				m_csaLines.Add(csLine);

				// Update the text
				csText = csText.Mid(iPos + 1);
				nLength = csText.GetLength();
				iLast = nLength - 1;
			}
			else
				break;
		}
		if (csText.GetLength())
			m_csaLines.Add(csText);
	}
}

// Return the horizontal amount that the area needs to be resized in order to fit the text
int CStaticText::GetXResize()
{
	return m_nXExtra;
}

// Return the vertical amount that the area needs to be resized in order to fit the text
int CStaticText::GetYResize()
{
	return m_nYExtra;
}

BEGIN_MESSAGE_MAP(CStaticText, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CStaticText message handlers
void CStaticText::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Don't draw until the parent is set
	if (!m_pParent)
		return;

	// Attach to the static text device context
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Get the character dimensions
	TEXTMETRIC tm;
//	dc.GetTextMetrics(&tm);
	dc.GetOutputTextMetrics(&tm);

	// Resize the control based on the font text metrics
	if (!m_bResize)
	{
		// Get the rectangular area
		CRect Rect(lpDrawItemStruct->rcItem);
		int y = Rect.Height();
		int x = Rect.Width();

		// Get the X and Y character sizes
		m_cxChar = tm.tmAveCharWidth;
		m_cyChar = tm.tmHeight + tm.tmExternalLeading;

		// Calculate the resize amount
		int nLines = (int)m_csaLines.GetCount();
		if (nLines)
		{
			// Calculate the Y resize amount
			int cyHeight = (nLines + 1) * m_cyChar;
			if (cyHeight > y)
				m_nYExtra = cyHeight - y;

			// Calculate the X resize amount
			int nMaxCx = 0;
			for (int iLine = 0;iLine < nLines;++iLine)
			{
				CString csLine = m_csaLines.GetAt(iLine);
				if (!csLine.IsEmpty())
				{
//					CSize TextExtent = dc.GetTextExtent(csLine);
					CSize TextExtent = dc.GetOutputTextExtent(csLine);
					if (TextExtent.cx > nMaxCx)
						nMaxCx = TextExtent.cx;
				}
			}
			if (nMaxCx > x)
				m_nXExtra = nMaxCx - x;
		}

		// Detach
		dc.Detach();

		// Don't resize again
		m_bResize = true;

		// Tell the parent to resize the controls
		m_pParent->SendMessage(WM_RESIZEMB,0,0);
		return;
	}
	// Get the rectangular area for the text
	CRect Rect(lpDrawItemStruct->rcItem);
	int x = Rect.Width();
	int y = Rect.Height();

	// Get the number of lines to render
	int nLines = (int)m_csaLines.GetCount();
	if (nLines)
	{
		// Calculate the height of the text
		int cyHeight = nLines * m_cyChar;

		// Calculate the starting Y of the text
		int cyStart = y / 2 - cyHeight / 2 - m_cyChar / 2;

		// Render the text
		int iLastBkMode = dc.SetBkMode(TRANSPARENT);
		for (int iLine = 0;iLine < nLines;++iLine)
		{
			CString csText = m_csaLines.GetAt(iLine);
//			CSize TextExtent = dc.GetTextExtent(csText);
			CSize TextExtent = dc.GetOutputTextExtent(csText);
			int cxStart = x /2 - TextExtent.cx / 2;
			dc.ExtTextOut(cxStart,cyStart,ETO_CLIPPED,&lpDrawItemStruct->rcItem,csText,NULL);
			cyStart += m_cyChar;
		}
		dc.SetBkMode(iLastBkMode);
	}

	// Detach
	dc.Detach();
}

BOOL CStaticText::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}