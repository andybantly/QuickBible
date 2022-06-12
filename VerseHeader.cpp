// VerseHeader.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "VerseHeader.h"
#include ".\verseheader.h"


// VerseHeader

IMPLEMENT_DYNAMIC(CVerseHeader, CHeaderCtrl)
CVerseHeader::CVerseHeader()
{
}

CVerseHeader::~CVerseHeader()
{
}


// Initialize the column locking to unlocked for all header items
void CVerseHeader::SetColumnCount(int nCount)
{
   m_vLockDivider.resize(nCount);
   for (int idx = 0;idx < nCount;++idx)
      SetColumnLock(idx,FALSE);
}

// Set the locking status for a column
void CVerseHeader::SetColumnLock(int iItem,BOOL bLock)
{
   if (iItem >= 0 && iItem < (int)m_vLockDivider.size())
      m_vLockDivider[iItem] = bLock;
}

BEGIN_MESSAGE_MAP(CVerseHeader, CHeaderCtrl)
	ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKA, &CVerseHeader::OnHdnBegintrack)
	ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKW, &CVerseHeader::OnHdnBegintrack)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CVerseHeader message handlers
BOOL CVerseHeader::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = FALSE;
	unsigned int iItem = phdr->iItem;
	if (iItem < m_vLockDivider.size())
		*pResult = m_vLockDivider[iItem];
	return TRUE;
}

BOOL CVerseHeader::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
}
