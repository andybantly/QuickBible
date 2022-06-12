// BBFFileDialog.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "BBLFileDialog.h"
#include "resource.h"

// File Dialog
IMPLEMENT_DYNAMIC(CBBLFileDialog, CFileDialog)

CBBLFileDialog::CBBLFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszTitle, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
							   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	// Initialize the file structure
	m_ofn.lStructSize = sizeof(OPENFILENAME);

	// Set the dialog customization template and help id
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_SAVEASEX);

	// Set the title
	m_csTitle = lpszTitle;
	m_ofn.lpstrTitle = m_csTitle;
}

void CBBLFileDialog::OnInitDone()
{
}

void CBBLFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBBLFileDialog, CFileDialog)
END_MESSAGE_MAP()