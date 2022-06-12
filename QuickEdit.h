// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include "CommonEdit.h"

class CQuickEdit : public CEdit, public CCommonEdit
{
	DECLARE_DYNAMIC(CQuickEdit)

public:
	CQuickEdit();
	virtual ~CQuickEdit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};