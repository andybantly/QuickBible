// ChooseFont.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "ChooseFont.h"
#include "MessageBox.h"

extern CProjectApp * g_pApp;

// CFontPreview

IMPLEMENT_DYNAMIC(CFontPreview, CButton)

CFontPreview::CFontPreview() : m_pParent(NULL)
{
}

CFontPreview::~CFontPreview()
{
}

BEGIN_MESSAGE_MAP(CFontPreview, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CFontPreview message handlers
BOOL CFontPreview::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CFontPreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Don't draw until the parent dialog is initialized
	if (!m_pParent)
		return;

	// Attach to the controls device context for custom drawing
	CDC DC;
	DC.Attach(lpDrawItemStruct->hDC);

	// Create a memory device context
	CDC MemDC;
	MemDC.CreateCompatibleDC(&DC);
	CDC * pDC = &MemDC;

	// Get the controls rectangle
	CRect Rect;
	GetClientRect(Rect);

	// Create an offscreen bitmap
	CBitmap MemBMP;
	MemBMP.CreateCompatibleBitmap(&DC,Rect.Width(),Rect.Height());
	CBitmap * pLastBMP = pDC->SelectObject(&MemBMP);

	// create and select a solid brush
	CBrush Background(GetSysColor(COLOR_WINDOW));
	CBrush * pLastBackground = pDC->SelectObject(&Background);

	// create the border pen
	CPen Edge;
	Edge.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
	CPen * pLastEdge = pDC->SelectObject(&Edge);

	// Draw the control rectangle
	pDC->Rectangle(Rect);

	// Cleanup GDI
	pDC->SelectObject(pLastBackground);
	pDC->SelectObject(pLastEdge);

	// Write the sample text
	CFont * pLastFont = pDC->SelectObject(&m_pParent->m_PreviewFont);
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkMode(TRANSPARENT);

	// Blit the image starting at the scroll bar positions
	int iX = m_pParent->m_HorzSB.GetScrollPos();
	int iY = m_pParent->m_VertSB.GetScrollPos();
	pDC->BitBlt(1,1,Rect.Width() - 2,Rect.Height() - 2,m_pParent->m_DIBFont,iX,iY,SRCCOPY);

	// Draw the offscreen buffer to the screen
	DC.BitBlt(0,0,Rect.Width(),Rect.Height(),pDC,0,0,SRCCOPY);

	// Clean up the offscreen bitmap
	pDC->SelectObject(pLastBMP);

	// Detach from the controls device context because we don't want to delete it
	DC.Detach();
}

// CChooseFont dialog

IMPLEMENT_DYNAMIC(CChooseFont, CDialog)

CChooseFont::CChooseFont(CStringArray & csaFont,CString csFont,UINT iPointSize,UINT iPointSizeMin,UINT iPointSizeMax,CWnd* pParent /*=NULL*/)
	: CDialog(CChooseFont::IDD, pParent),
	m_csaFont(csaFont), m_csFont(csFont),
	m_iPointSize(iPointSize), m_iPointSizeMin(iPointSizeMin), m_iPointSizeMax(iPointSizeMax),
	m_iHorzMax(0), m_iHorzPos(0), m_iVertMax(0), m_iVertPos(0),
	m_nMaxX(0), m_nMaxY(0)
{
	// Add the sample text
	m_csaChars.Add(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	m_csaChars.Add(_T("abcdefghijklmnopqrstuvwxyz"));
	m_csaChars.Add(_T("0123456789"));
}

void CChooseFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLE,m_FontPreview);
	DDX_Control(pDX, IDC_FONTLIST, m_FontList);
	DDX_Text(pDX, IDC_EDIT_FONT, m_csFont);
	DDX_Text(pDX, IDC_PTSIZE, m_iPointSize);
	DDX_Control(pDX, IDC_VERTSB, m_VertSB);
	DDX_Control(pDX, IDC_HORZSB, m_HorzSB);
}

BOOL CChooseFont::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(g_pApp->QBString(317));
	GetDlgItem(IDC_STATIC_FONT)->SetWindowText(g_pApp->QBString(256));
	GetDlgItem(IDC_STATIC_PTSIZE)->SetWindowText(g_pApp->QBString(257));
	GetDlgItem(IDC_GROUP_SAMPLE)->SetWindowText(g_pApp->QBString(318));

	m_Ok.CreateText(this,IDOK,g_pApp->QBString(276));
	m_Cancel.CreateText(this,IDCANCEL,g_pApp->QBString(275));

	int nFont = (int)m_csaFont.GetCount();
	for (int iFont = 0;iFont < nFont;++iFont)
	{
		CString csTempFont = m_csaFont.GetAt(iFont);
		m_FontList.AddString(csTempFont);
	}

	int iPos = m_FontList.FindStringExact(0,m_csFont);
	if (iPos != -1)
		m_FontList.SetCurSel(iPos);

	// Create the font for the preview
	CreatePreviewFont();

	// Set the parent
	m_FontPreview.m_pParent = this;

	// Set the focus to the list
	m_FontList.SetFocus();

	return FALSE;
}

// Create the font for the sample text
void CChooseFont::CreatePreviewFont()
{
	// Create the specified font with the specified point size
	m_PreviewFont.DeleteObject();
	CDC * pDC = GetDC();
	int iLogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
	LONG lfHeight = MulDiv(m_iPointSize * 10,iLogPixelsY,72);
	LOGFONT LogFont;
	memset(&LogFont,0,sizeof(LOGFONT));
	LogFont.lfHeight = lfHeight;
	LogFont.lfCharSet = DEFAULT_CHARSET;
	LogFont.lfQuality = PROOF_QUALITY;
	strncpy(LogFont.lfFaceName,m_csFont.GetBuffer(),min(m_csFont.GetLength(),sizeof(LogFont.lfFaceName)));
	const LOGFONT * lpLogFont = &LogFont;
	m_PreviewFont.CreatePointFontIndirect(lpLogFont);

	// Get the font characteristics
	CFont * pFont = pDC->SelectObject(&m_PreviewFont);

	// Get the Y dimensions for offsetting the sample text
	TEXTMETRIC tm;
//	pDC->GetTextMetrics(&tm);
	pDC->GetOutputTextMetrics(&tm);
	int cyChar = tm.tmHeight + tm.tmExternalLeading;

	// Get the dimensions of the sample text in the current font
	m_nMaxX = 0;
	m_nMaxY = 0;
	int iCount,nCount = (int)m_csaChars.GetCount();
	for (iCount = 0;iCount < nCount;++iCount)
	{
		CString csChars = m_csaChars.GetAt(iCount);
//		CSize size = pDC->GetTextExtent(csChars);
		CSize size = pDC->GetOutputTextExtent(csChars);
		size.cx += 10;
		if (size.cx > m_nMaxX)
			m_nMaxX = size.cx;
		m_nMaxY += max(size.cy,cyChar);
	}

	// Clean up the font
	pDC->SelectObject(pFont);

	// Release the DC
	ReleaseDC(pDC);
	pDC = NULL;

	// Create the frame
	m_DIBFont.Init(m_nMaxX,m_nMaxY,false);
	CDC & DCFont = m_DIBFont;
	DCFont.PatBlt(0,0,m_nMaxX,m_nMaxY,WHITENESS);

	// Select the font for the sample text
	pFont = DCFont.SelectObject(&m_PreviewFont);

	// Fill the frame with the current window background color
	DWORD dwWindowColor = GetSysColor(COLOR_WINDOW);
	DCFont.FillSolidRect(0,0,m_nMaxX,m_nMaxY,dwWindowColor);

	// Set the colors
	DCFont.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	DCFont.SetBkMode(TRANSPARENT);

	// Draw the text to the bitmap with the preview font
	int nY = 0;
	for (iCount = 0;iCount < nCount;++iCount)
	{
		// Store the last frame
		CString csChars = m_csaChars.GetAt(iCount);
		DCFont.TextOut(5,iCount * cyChar,csChars);
	}

	// Clean up the font
	DCFont.SelectObject(pFont);

	// Calculate the current and maximum scrollbar positions
	CRect Rect;
	m_FontPreview.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	int nXChar = Rect.Width() - 2;
	m_iHorzMax = max(0,m_nMaxX - nXChar);
	m_iHorzPos = min(m_iHorzPos,m_iHorzMax);
	int nYChar = Rect.Height() - 2;
	m_iVertMax = max(0,m_nMaxY + 2 - nYChar);
	m_iVertPos = min(m_iVertPos,m_iVertMax);

	// Apply the horizontal positions unless they are 0, then hide the scroll bars
	if (m_iHorzMax || m_iHorzPos)
	{
		m_HorzSB.ShowWindow(SW_SHOW);
		m_HorzSB.SetScrollRange(0,m_iHorzMax,FALSE);
		m_HorzSB.SetScrollPos(m_iHorzPos,TRUE);
	}
	else
	{
		m_iHorzPos = 0;
		m_HorzSB.SetScrollPos(m_iHorzPos,TRUE);
		m_HorzSB.ShowWindow(SW_HIDE);
	}

	// Apply the vertical positions unless they are 0, then hide the scroll bars
	if (m_iVertMax || m_iVertPos)
	{
		m_VertSB.ShowWindow(SW_SHOW);
		m_VertSB.SetScrollRange(0,m_iVertMax,FALSE);
		m_VertSB.SetScrollPos(m_iVertPos,TRUE);
	}
	else
	{
		m_iVertPos = 0;
		m_VertSB.SetScrollPos(m_iVertPos,TRUE);
		m_VertSB.ShowWindow(SW_HIDE);
	}
}

void CChooseFont::OnOK()
{
	UpdateData(TRUE);
	if (m_iPointSize < m_iPointSizeMin || m_iPointSize > m_iPointSizeMax)
	{
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(319);
		CString csMessage;
		csMessage.Format(csFmt,m_iPointSizeMin,m_iPointSizeMax);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
	else
		EndDialog(IDOK);
}

BEGIN_MESSAGE_MAP(CChooseFont, CDialog)
	ON_LBN_SELCHANGE(IDC_FONTLIST, &CChooseFont::OnChangeFontlist)
	ON_EN_CHANGE(IDC_PTSIZE, &CChooseFont::OnChangePtSize)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CChooseFont message handlers

void CChooseFont::OnChangeFontlist()
{
	int iCurSel = m_FontList.GetCurSel();
	if (iCurSel != -1)
	{
		m_FontList.GetText(iCurSel,m_csFont);
		UpdateData(FALSE);
		CreatePreviewFont();
		m_FontPreview.RedrawWindow();
	}
}

void CChooseFont::OnChangePtSize()
{
	UpdateData(TRUE);
	CreatePreviewFont();
	m_FontPreview.RedrawWindow();
}

CString CChooseFont::GetFont()
{
	return m_csFont;
}

UINT CChooseFont::GetPointSize()
{
	return m_iPointSize;
}

// Scroll horizontally
void CChooseFont::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iInc;
	switch (nSBCode)
	{
	case SB_LINEUP:
		iInc = -1;
		break;
	case SB_LINEDOWN:
		iInc = 1;
		break;
	case SB_PAGEUP:
		iInc = -1;
		break;
	case SB_PAGEDOWN:
		iInc = 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		iInc = nPos - m_iHorzPos;
		break;
	default:
		iInc = 0;
	}

	// Calculate and set the new scroll bar position
	iInc = max(-m_iHorzPos,min(iInc,m_iHorzMax - m_iHorzPos));
	if (iInc != 0)
	{
		m_iHorzPos += iInc;
		m_HorzSB.SetScrollPos(m_iHorzPos,TRUE);
		m_FontPreview.RedrawWindow();
	}
}

// Scroll vertically
void CChooseFont::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iInc;
	switch (nSBCode)
	{
	case SB_TOP:
		iInc = -m_iVertPos;
		break;
	case SB_BOTTOM:
		iInc = m_iVertMax - m_iVertPos;
		break;
	case SB_LINEUP:
		iInc = -1;
		break;
	case SB_LINEDOWN:
		iInc = 1;
		break;
	case SB_PAGEUP:
		iInc = -1;
		break;
	case SB_PAGEDOWN:
		iInc = 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		iInc = nPos - m_iVertPos;
		break;
	default:
		iInc = 0;
	}

	// Calculate and set the new scroll bar position
	iInc = max(-m_iVertPos,min(iInc,m_iVertMax - m_iVertPos));
	if (iInc != 0)
	{
		m_iVertPos += iInc;
		m_VertSB.SetScrollPos(m_iVertPos,TRUE);
		m_FontPreview.RedrawWindow();
	}
}