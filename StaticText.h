// Copyright (C) 2008 Andrew S. Bantly
#pragma once

// CStaticText
class CStaticText : public CButton
{
	DECLARE_DYNAMIC(CStaticText)

public:
	CStaticText();
	virtual ~CStaticText();
	void ParseText(CWnd * pParent,CString csText);
	int GetXResize();
	int GetYResize();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CWnd * m_pParent;
	bool m_bResize;
	CStringArray m_csaLines;
	int m_cxChar,m_cyChar;
	int m_nXExtra,m_nYExtra;

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


