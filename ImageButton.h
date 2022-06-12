// Copyright (C) 2007 Andrew S. Bantly
#pragma once

#include "BitmapIcon.h"

// Handle Windows XP theming in a backwords compatible way
typedef HTHEME (WINAPI* PFNOpenThemeData)(HWND,LPCWSTR);
typedef HRESULT (WINAPI* PFNDrawThemeBackground)(HTHEME,HDC,int,int,const RECT *,const RECT *);
typedef HRESULT (WINAPI* PFNDrawThemeText)(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,DWORD,const RECT *);
typedef HRESULT (WINAPI* PFNCloseThemeData)(HTHEME);

/////////////////////////////////////////////////////////////////////////////
// CImageButton window

class CImageButton : public CButton
{
	DECLARE_DYNAMIC(CImageButton)

protected:
	HMODULE m_hUxTheme;
	PFNOpenThemeData OpenThemeData;
	PFNDrawThemeBackground DrawThemeBackground;
	PFNDrawThemeText DrawThemeText;
	PFNCloseThemeData CloseThemeData;

// Construction
public:
	enum eBitmapAlignment {LEFT,RIGHT};
	CImageButton();

// Attributes
public:

// Operations
public:
	BOOL CreateBtmp(CWnd * pWnd,int nBitmapId,int nButtonId,const char * pszText = NULL,COLORREF crTextColor = COLOR_BTNTEXT,eBitmapAlignment = LEFT);
	BOOL CreateIcon(CWnd * pWnd,int nIconId,int nButtonId,const char * pszText = NULL,COLORREF crTextColor = COLOR_BTNTEXT,eBitmapAlignment eAlignment = LEFT);
	BOOL CreateRect(CWnd * pWnd,int nButtonId,COLORREF crFillColor,const char * pszText = NULL,eBitmapAlignment eAlignment = LEFT,int iWidth = 24,int iHeight = 12);
	BOOL CreateText(CWnd * pWnd,int nButtonId,const char * pszText = NULL,COLORREF crTextColor = COLOR_BTNTEXT);
	COLORREF SetFillColor(COLORREF crFillColor);
	void UpdateBitmap(int nBitmapId);
	void UpdateIcon(int nIconId);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageButton)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	afx_msg void OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

protected:
	void CreateButton();

// Implementation
public:
	virtual ~CImageButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG

	BOOL m_bHasFocus;
	BOOL m_bMouseOver;

	CWnd * m_pWnd;
	int m_nButtonId;
	COLORREF m_crTextColor;
	eBitmapAlignment m_eAlignment;

	// Rectangle fill color
	COLORREF m_crFillColor;
	int m_iWidth;
	int m_iHeight;

	// Bitmap-Icon
	CBitmapIcon m_BitmapIcon;

	// Image
	BOOL m_bImage;

	// Filled Rectangle
	BOOL m_bRect;

	// Text button
	BOOL m_bText;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.