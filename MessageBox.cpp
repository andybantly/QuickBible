// AboutDlg.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "MessageBox.h"

extern CProjectApp * g_pApp;

CMessageBox::CMessageBox(CString csCaption,CString csMessage,int iType,int iImageId) : CDialog(CMessageBox::IDD)
{
	//{{AFX_DATA_INIT(CMessageBox)
	//}}AFX_DATA_INIT
	m_csCaption = csCaption;
	m_csMessage = csMessage;
	m_iType = iType;
	m_iImageId = iImageId;
}

CString CMessageBox::GetValue() const
{
	return m_csValue;
}

void CMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageBox)
	DDX_Control(pDX,IDC_MESSAGETEXT,m_MessageText);
	DDX_Control(pDX,IDC_VALUE,m_Value);
	DDX_Text(pDX,IDC_VALUE,m_csValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMessageBox, CDialog)
	//{{AFX_MSG_MAP(CMessageBox)
	ON_BN_CLICKED(IDOTHER,&CMessageBox::OnOther)
	ON_MESSAGE(WM_RESIZEMB, &CMessageBox::OnResizeMessageBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMessageBox::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	SetWindowText(m_csCaption);
	m_Image.CreateBtmp(this,m_iImageId,IDC_MBIMG);

	m_OK.CreateText(this,IDOK);
	m_Cancel.CreateText(this,IDCANCEL);
	m_Other.CreateText(this,IDOTHER);

	if (m_iType == OKCANCEL || m_iType == OK || m_iType == OKCANCELEDIT)
	{
		m_OK.SetWindowText(g_pApp->QBString(276));
		m_Cancel.SetWindowText(g_pApp->QBString(275));
	}
	else if (m_iType == YESNO)
	{
		m_OK.SetWindowText(g_pApp->QBString(213));
		m_Cancel.SetWindowText(g_pApp->QBString(214));
	}
	else if (m_iType == YESNOCANCEL)
	{
		m_OK.SetWindowText(g_pApp->QBString(213));
		m_Other.SetWindowText(g_pApp->QBString(214));
		m_Cancel.SetWindowText(g_pApp->QBString(275));
	}

	// Set the messagebox text
	m_MessageText.ParseText(this,m_csMessage);

	return bRet;
}

// Resize the message box controls
LRESULT CMessageBox::OnResizeMessageBox(WPARAM wParam,LPARAM lParam)
{
	// Increase the width by the given amount
	CPoint ptDiff(m_MessageText.GetXResize(),m_MessageText.GetYResize());

	// Get the height of the edit control
	int nEditHeight = 0;
	if (m_iType == OKCANCELEDIT)
	{
		CRect ValueRect;
		m_Value.GetWindowRect(&ValueRect);
		ScreenToClient(&ValueRect);
		nEditHeight = ValueRect.Height() + 10;
	}

	// Resize the dialog
	CRect Rect;
	GetWindowRect(&Rect);
	SetWindowPos(NULL,Rect.left - ptDiff.x / 2,Rect.top - ptDiff.y / 2 - nEditHeight / 2,Rect.Width() + ptDiff.x, Rect.Height() + ptDiff.y + nEditHeight,SWP_NOZORDER|SWP_NOACTIVATE);

	// Resize the image space
	m_Image.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	m_Image.SetWindowPos(NULL, Rect.left, Rect.top, Rect.Width(), Rect.Height() + ptDiff.y, SWP_NOZORDER|SWP_NOACTIVATE);

	// Resize the message
	m_MessageText.GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	int nWidth = Rect.Width() + ptDiff.x;
	m_MessageText.SetWindowPos(NULL, Rect.left, Rect.top, nWidth, Rect.Height() + ptDiff.y, SWP_NOZORDER|SWP_NOACTIVATE);
	int nLeft = Rect.left;

	// Adjust the UI based on the type
	if (m_iType == OK)
	{
		m_Cancel.ShowWindow(SW_HIDE);
		m_OK.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		int nButtonWidth = Rect.Width();
		m_OK.SetWindowPos(NULL, nLeft + nWidth / 2 - nButtonWidth / 2, Rect.top + ptDiff.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}
	else if (m_iType == OKCANCEL || m_iType == YESNO || m_iType == OKCANCELEDIT)
	{
		// Move the OK button
		m_OK.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		int nButtonWidth = Rect.Width();
		m_OK.SetWindowPos(NULL, nLeft + nWidth / 4 - nButtonWidth / 2, Rect.top + ptDiff.y + nEditHeight, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		// Move the Cancel button
		m_Cancel.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		nButtonWidth = Rect.Width();
		m_Cancel.SetWindowPos(NULL, nLeft + (nWidth / 4) * 3 - nButtonWidth / 2, Rect.top + ptDiff.y + nEditHeight, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		// Show the edit control
		if (m_iType == OKCANCELEDIT)
		{
			m_Value.ShowWindow(SW_SHOW);
			m_Value.SetFocus();
		}
	}
	else if (m_iType == YESNOCANCEL)
	{
		// Show the "other" button
		m_Other.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		int nButtonWidth = Rect.Width();
		m_Other.SetWindowPos(NULL, nLeft + (nWidth / 4) * 2 - nButtonWidth / 2, Rect.top + ptDiff.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		m_Other.ShowWindow(SW_SHOW);

		// Move the OK button
		m_OK.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		nButtonWidth = Rect.Width();
		m_OK.SetWindowPos(NULL, nLeft + (nWidth / 4) - nButtonWidth / 2, Rect.top + ptDiff.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		// Move the Cancel button
		m_Cancel.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		nButtonWidth = Rect.Width();
		m_Cancel.SetWindowPos(NULL, nLeft + (nWidth / 4) * 3 - nButtonWidth / 2, Rect.top + ptDiff.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}

	// Redraw the window
	Invalidate();
	return 0;
}

// The "other" button was pressed
afx_msg void CMessageBox::OnOther()
{
	EndDialog(NO);
}
