// Copyright (C) 2008 Andrew S. Bantly
#pragma once

#include "BitmapIcon.h"

/////////////////////////////////////////////////////////////////////////////
// CImage window

class CImageStatic : public CButton
{
	DECLARE_DYNAMIC(CImageStatic)

// Construction
public:
	CImageStatic();
	virtual ~CImageStatic();

// Attributes
public:

// Operations
public:
	BOOL CreateBtmp(CWnd * pWnd,int nBitmapId,int nButtonId);
	void UpdateBitmap(int nBitmapId);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageStatic)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	CWnd * m_pWnd;
	int m_iWidth;
	int m_iHeight;

	// Bitmap-Icon
	CBitmapIcon m_BitmapIcon;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.