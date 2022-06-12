// LCComboBox.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "GridComboBox.h"

// CGridComboBox

IMPLEMENT_DYNAMIC(CGridComboBox, CComboBox)
CGridComboBox::CGridComboBox(int iItem,int iSubItem,CString csDefault) : m_bDestroyed(false)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_csDefault = csDefault;
}

CGridComboBox::~CGridComboBox()
{
}

CGridComboBox & CGridComboBox::operator = (const CComboBox & rhs)
{
	if (this != &rhs)
	{
		ResetContent();
		int nItems = rhs.GetCount();
		for (int iItem = 0;iItem < nItems;++iItem)
		{
			CString csItem;
			rhs.GetLBText(iItem,csItem);
			AddString(csItem);
		}
	}
	return *this;
}

// Handle the keypress of ESC from closing the application
BOOL CGridComboBox::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int iShift = HIBYTE(GetKeyState(VK_SHIFT)) >> 7;
		WORD wType = MAKEWORD(iShift,1);

		WPARAM WP = pMsg->wParam;
		if (WP == VK_ESCAPE)
		{
			if (!m_bDestroyed)
			{
				m_bDestroyed = true;
				DestroyWindow();
			}
			return TRUE;
		}
		else if (WP == VK_RETURN || WP == VK_TAB)
		{
			NotifyParent();
			WPARAM wParam = MAKEWPARAM(m_iItem,m_iSubItem);
			LPARAM lParam = MAKELPARAM(WP,wType);
			GetParent()->SendMessage(WM_RAPIDEDIT,wParam,lParam);
			if (!m_bDestroyed)
			{
				m_bDestroyed = true;
				DestroyWindow();
			}
			return TRUE;
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

void CGridComboBox::NotifyParent()
{
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	CString csText;
	csText.Format("%d",GetCurSel());
	dispinfo.item.pszText = LPTSTR((LPCTSTR)csText);
	dispinfo.item.cchTextMax = csText.GetLength();
	GetParent()->GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&dispinfo);
}

BEGIN_MESSAGE_MAP(CGridComboBox, CComboBoxEx)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CGridComboBox::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CGridComboBox::OnCbnCloseUp)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

// CGridComboBox message handlers
void CGridComboBox::OnCbnSelchange()
{
	NotifyParent();
}

void CGridComboBox::OnCbnCloseUp()
{
	NotifyParent();
}

void CGridComboBox::OnKillFocus(CWnd* pNewWnd)
{
	if (!m_bDestroyed)
	{
		m_bDestroyed = true;
		DestroyWindow();
	}
}

void CGridComboBox::OnNcDestroy()
{
	CComboBox::OnNcDestroy();
	delete this;	
}