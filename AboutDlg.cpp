// AboutDlg.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "AboutDlg.h"
#include <memory>
using namespace std;

extern CProjectApp * g_pApp;

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	SetWindowText(g_pApp->QBString(260));
	GetDlgItem(IDC_STATIC_QB)->SetWindowText(g_pApp->QBString(261));
	GetDlgItem(IDC_STATIC_CPY)->SetWindowText(g_pApp->QBString(262));
	GetDlgItem(IDC_STATIC_ARR)->SetWindowText(g_pApp->QBString(263));
	GetDlgItem(IDC_STATIC_EMAIL)->SetWindowText(g_pApp->QBString(264));
	GetDlgItem(IDC_STATIC_QUICKBIBLE)->SetWindowText(g_pApp->QBString(265));
	GetDlgItem(IDC_STATIC_WEB)->SetWindowText(g_pApp->QBString(266));
	GetDlgItem(IDC_STATIC_WEBLINK)->SetWindowText(g_pApp->QBString(215));
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(g_pApp->QBString(300));
	GetDlgItem(IDC_VERSION)->SetWindowText(GetVersionNumber());

	m_Ok.CreateText(this,IDOK,g_pApp->QBString(276));
	m_HomePage.SubclassDlgItem(IDC_STATIC_WEBLINK, this, g_pApp->QBString(215));
	m_Email.SubclassDlgItem(IDC_STATIC_QUICKBIBLE, this, g_pApp->QBString(101));

	return bRet;
}

CString CAboutDlg::GetVersionNumber()
{
	return g_pApp->GetVersionNumber();
}