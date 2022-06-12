// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include <vector>

// VerseHeader

class CVerseHeader : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CVerseHeader)

public:
	CVerseHeader();
	virtual ~CVerseHeader();

	void SetColumnCount(int nCount);
	void SetColumnLock(int iItem,BOOL bLock);
	std::vector<BOOL> m_vLockDivider;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


