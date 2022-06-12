// QuickEdit.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "QuickEdit.h"

extern CProjectApp * g_pApp;

IMPLEMENT_DYNAMIC(CQuickEdit, CEdit)

CQuickEdit::CQuickEdit()
{
	SetReal(false);
}

CQuickEdit::~CQuickEdit()
{
}

// CQuickEdit message handlers
BEGIN_MESSAGE_MAP(CQuickEdit, CEdit)
	//{{AFX_MSG_MAP(CQuickEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickEdit message handlers

BOOL CQuickEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		WPARAM WP = pMsg->wParam;
		if (WP == VK_RETURN || WP == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

// Further handle 'enter' or 'esc' keys for ending edits
void CQuickEdit::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// Get the old text for processing
	CString csLastText;
	GetWindowText(csLastText);

	// Get the next character
	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Get the new text for processing
	CString csText;
	GetWindowText(csText);

	CString csValue;
	bool bFound = SearchList(csText,csValue);
	if (bFound)
	{
		SetWindowText(csValue);
		SetSel(csText.GetLength(),csValue.GetLength());
	}
	else
	{
		SetWindowText(csLastText);
		SetSel(0,-1);
	}

	// Redraw in case of validation color changes
	InvalidateRect(NULL,FALSE);
}