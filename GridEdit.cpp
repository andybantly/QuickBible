// GridEdit.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "GridEdit.h"

extern CProjectApp * g_pApp;

IMPLEMENT_DYNAMIC(CGridEdit, CEdit)

CGridEdit::CGridEdit(int iItem,int iSubItem,CString csDefault,bool bReal) : m_iItem(iItem), m_iSubItem(iSubItem), m_csDefault(csDefault), m_bDestroyed(false)
{
	SetReal(bReal);
}

CGridEdit::~CGridEdit()
{
}

// CGridEdit message handlers
BEGIN_MESSAGE_MAP(CGridEdit, CEdit)
	//{{AFX_MSG_MAP(CGridEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CGridEdit::OnEditKillFocus)
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridEdit message handlers

BOOL CGridEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		WPARAM WP = pMsg->wParam;
		if (WP == VK_TAB || WP == VK_RETURN || WP == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

// Further handle 'enter' or 'esc' keys for ending edits
void CGridEdit::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// Detect the view for special handling
	CProjectDlg *pDlg = (CProjectDlg*)g_pApp->m_pMainWnd;
	bool bWorksheetView = pDlg->m_Mode.GetCurSel() == CProjectDlg::VERSEVIEW ? true : false;

	if (nChar == VK_ESCAPE)
	{
		if (!m_bDestroyed)
		{
			m_bDestroyed = true;
			DestroyWindow();
		}
		return;
	}
	else if (nChar == VK_TAB || nChar == VK_RETURN)
	{
		// Get the shift state
		int iShift = HIBYTE(GetKeyState(VK_SHIFT)) >> 7;
		WORD wType = MAKEWORD(iShift,0);

		// Update the parent
		NotifyParent();

		// Send the rapid edit message
		if (!m_bDestroyed)
		{
			WPARAM wParam = MAKEWPARAM(m_iItem,m_iSubItem);
			LPARAM lParam = MAKELPARAM(nChar,wType);
			GetParent()->SendMessage(WM_RAPIDEDIT,wParam,lParam);
		}

		// Clean up
		if (!m_bDestroyed)
		{
			m_bDestroyed = true;
			DestroyWindow();
		}
		return;
	}

	// Get the old text for processing
	CString csLastText;
	GetWindowText(csLastText);

	// Get the next character
	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Get the new text for processing
	CString csText;
	GetWindowText(csText);

	// Test for a number if the edit control hosts a number
	if (!IsValid(csText))
	{
		SetWindowText(csLastText);
		SetSel(csLastText.GetLength(),csLastText.GetLength());
		InvalidateRect(NULL,FALSE);
		return;
	}

	if (bWorksheetView)
	{
		if (m_iSubItem == CVerseList::TRANSLATION || m_iSubItem == CVerseList::BOOK || m_iSubItem == CVerseList::CHAPTER || m_iSubItem == CVerseList::FROMVERSE || m_iSubItem == CVerseList::TOVERSE)
		{
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
		}
	}

	// Redraw in case of validation color changes
	InvalidateRect(NULL,FALSE);
}

// Send notification to list control that editing is finished
void CGridEdit::OnEditKillFocus() 
{
	// Notify the parent
	NotifyParent();

	// Clean up
	if (!m_bDestroyed)
	{
		m_bDestroyed = true;
		DestroyWindow();
	}
}

// Clean up
void CGridEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();
	delete this;
}

void CGridEdit::NotifyParent()
{
	CWnd * pParent = GetParent();
	CWnd * pDlg = NULL;
	if (pParent)
		pDlg = pParent->GetParent();

	CString csText;
	GetWindowText(csText);
	if (csText.IsEmpty())
		csText = m_csDefault;

	// Test for the control containing valid numerical data
	DWORD dwMask = 1;

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = dwMask;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = LPTSTR((LPCTSTR)csText);
	dispinfo.item.cchTextMax = csText.GetLength();
	if (pDlg)
		pDlg->SendMessage(WM_NOTIFY,pParent->GetDlgCtrlID(),(LPARAM)&dispinfo);
}