// Copyright (C) 2007 Andrew S. Bantly
#if !defined(AFX_FULLWND_H__A5C032EA_2EF4_481F_9742_8E644D8C4B50__INCLUDED_)
#define AFX_FULLWND_H__A5C032EA_2EF4_481F_9742_8E644D8C4B50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FullWnd.h : header file
//

#if !defined(PERSONAL) && !defined(LITE)

/////////////////////////////////////////////////////////////////////////////
// CFullWnd window

class CFullWnd : public CWnd
{
	// Construction
public:
	CFullWnd();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFullWnd)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CFullWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFullWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_x;
	int m_y;
};

#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FULLWND_H__A5C032EA_2EF4_481F_9742_8E644D8C4B50__INCLUDED_)
