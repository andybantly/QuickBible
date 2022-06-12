#pragma once
#include "afxwin.h"
#include "ImageButton.h"

class CChooseFont;

// CFontPreview
class CFontPreview : public CButton
{
	DECLARE_DYNAMIC(CFontPreview)

public:
	CFontPreview();
	virtual ~CFontPreview();

public:
	CChooseFont * m_pParent;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

// CChooseFont dialog
class CChooseFont : public CDialog
{
	friend class CFontPreview;

	DECLARE_DYNAMIC(CChooseFont)

public:
	CChooseFont(CStringArray & csaFont,CString csFont,UINT iPointSize,UINT iPointSizeMin,UINT iPointSizeMax,CWnd* pParent = NULL);   // standard constructor
	virtual ~CChooseFont(){};
	CString GetFont();
	UINT GetPointSize();
	void CreatePreviewFont();

// Dialog Data
	enum { IDD = IDD_CHOOSE_FONT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	CFontPreview m_FontPreview;
	CStringArray & m_csaFont;
	CListBox m_FontList;
	CString m_csFont;
	UINT m_iPointSize;
	UINT m_iPointSizeMin;
	UINT m_iPointSizeMax;
	CScrollBar m_VertSB;
	CScrollBar m_HorzSB;
	CFont m_PreviewFont;
	CStringArray m_csaChars;
	CImageButton m_Ok;
	CImageButton m_Cancel;

	// The font DIB frame
	CDIBFrame m_DIBFont;

	int m_iHorzMax;
	int m_iHorzPos;
	int m_iVertMax;
	int m_iVertPos;
	int m_nMaxX;
	int m_nMaxY;

public:
	afx_msg void OnChangeFontlist();
	afx_msg void OnChangePtSize();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};