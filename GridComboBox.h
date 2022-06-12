// Copyright (C) 2007 Andrew S. Bantly
#pragma once

// CGridComboBox
class CGridComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CGridComboBox)

public:
	CGridComboBox(int iItem,int iSubItem,CString csDefault);
	virtual ~CGridComboBox();
	CGridComboBox & operator = (const CComboBox & rhs);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
	void NotifyParent();
	CString m_csDefault;
	int m_iItem;
	int m_iSubItem;
	bool m_bDestroyed;
public:
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnCloseUp();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
};


