// AboutDlg.h : header file
// Copyright (C) 2007 Andrew S. Bantly
//

#pragma once
#include "AboutDlg.h"
#include "StaticLink.h"
#include "ImageButton.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CString GetVersionNumber();

	// Implementation
protected:
	CImageButton m_Ok;
	CStaticLink m_HomePage;
	CStaticLink m_Email;

	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};