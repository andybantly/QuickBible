// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include "CommonEdit.h"

class CGridEdit : public CEdit, public CCommonEdit
{
	DECLARE_DYNAMIC(CGridEdit)

public:
	CGridEdit();
	CGridEdit(int iItem,int iSubItem,CString csDefault,bool bReal);
	virtual ~CGridEdit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	void NotifyParent();

	afx_msg void OnEditKillFocus();
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	int m_iItem;
	int m_iSubItem;
	bool m_bDestroyed;
	CString m_csDefault;
	DECLARE_MESSAGE_MAP()
};