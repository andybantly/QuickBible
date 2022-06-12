// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "RegisterDlg.h"
#include "AuthSMTPConnection.h"
#include <comutil.h>

extern CProjectApp * g_pApp;

// CRegisterDlg dialog

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialog)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
	, m_csName(_T(""))
	, m_csEmail(_T(""))
	, m_csOrderNumber(_T(""))
	, m_csOrderCode(_T(""))
{
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_csName);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_csEmail);
	DDX_Text(pDX, IDC_EDIT_ORDERNUMBER, m_csOrderNumber);
	DDX_Text(pDX, IDC_EDIT_ORDERCODE, m_csOrderCode);
}

BOOL CRegisterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the dialog caption
	CString csWindowText = g_pApp->QBString(307);
	SetWindowText(csWindowText);

	m_Register.CreateText(this,IDC_COPY,g_pApp->QBString(304));
	m_Cancel.CreateText(this,IDCANCEL,g_pApp->QBString(275));

	GetDlgItem(IDC_STATIC_NAME)->SetWindowText(g_pApp->QBString(312));
	GetDlgItem(IDC_STATIC_EMAILADDRESS)->SetWindowText(g_pApp->QBString(309));
	GetDlgItem(IDC_STATIC_ORDERNUMBER)->SetWindowText(g_pApp->QBString(313));
	GetDlgItem(IDC_STATIC_ORDERCODE)->SetWindowText(g_pApp->QBString(316));

	CString csMsg,csFmt = g_pApp->QBString(310);
	csMsg.Format(csFmt,g_pApp->QBString(304));
	GetDlgItem(IDC_STATIC_EMAIL)->SetWindowText(csMsg);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	ON_BN_CLICKED(IDC_COPY, &CRegisterDlg::OnClickedRegister)
END_MESSAGE_MAP()

void CRegisterDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}

void CRegisterDlg::OnOK()
{
	OnClickedRegister();
}

void CRegisterDlg::OnClickedRegister()
{
	CString csCaption = g_pApp->QBString(259);
	UpdateData();

	bool bEnd = true;
	CString csMessage,csField,csFormat = g_pApp->QBString(315);
	if (m_csName.GetLength() == 0)
	{
		csField = g_pApp->QBString(312);
		bEnd = false;
	}
	else if (m_csEmail.GetLength() == 0)
	{
		csField = g_pApp->QBString(309);
		bEnd = false;
	}

	if (!bEnd)
	{
		CString csCaption = g_pApp->QBString(259);
		csMessage.Format(csFormat,csField);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		return;
	}

	CString csName = m_csName;
	CString csEmail = m_csEmail;
	CString csOrderNumber = m_csOrderNumber;
	CString csOrderCode = m_csOrderCode;
	CString csSubject = "QuickBible Registration";

	// Get the registration path
	CString csRegistrationPath = g_pApp->GetRegistrationPath();

	// Build the hyperlink
	CString csLanguage,csEdition;
	g_pApp->GetLanguageEdition(csLanguage,csEdition);
	CString csRegister = g_pApp->QBString(314);
	CString csRegLink,csLF = "\r\n";
	csRegLink.Format(csRegister,csLF,csLF,csLanguage,csLF,csEdition,csLF,csName,csLF,csEmail,csLF,csOrderNumber,csLF,csOrderCode,csLF,g_pApp->GetUID(),csLF,csRegistrationPath,csLF);

	// Get and Open the file
	CStdioFile RegFile;
	CFileException Error;
	CString csRegFile = g_pApp->GetTempFileName(NULL,"tmp");

	const DWORD dwWriteFlags = CFile::modeWrite|CFile::modeCreate|CFile::shareDenyWrite;
	if (!RegFile.Open(csRegFile,dwWriteFlags,&Error))
	{
		Error.GetErrorMessage(csMessage.GetBufferSetLength(255),255);
		csMessage.ReleaseBuffer();
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		return;
	}

	// Write the file
	RegFile.WriteString(csRegLink);
	RegFile.Close();

	// Destination email address
	CString csSenderName = g_pApp->QBString(259);
	CString csDestEmail = g_pApp->QBString(346);

	// Build the email command
	CString csDestPwd = g_pApp->QBString(308);
	CString csServer = g_pApp->QBString(348);
	CString csPort = g_pApp->QBString(349);

	// Send the email
	CWaitCursor wait;

	CAuthSMTPConnection SMTP(atoi(csPort),csServer,csDestEmail,csDestPwd);
	BOOL bRes = SMTP.Connect();
	if (bRes)
	{
		// Create the text email message
		CMimeMessage msg;
		msg.SetSubject(csSubject);
		msg.SetSender(csDestEmail);
		msg.SetSenderName(csSenderName);
		msg.AddRecipient(csDestEmail);
		msg.AddText(csRegLink);

		// Send the registration email
		bRes = SMTP.SendMessage(msg);
	}

	// Remove the file
	RegFile.Remove(csRegFile);

	// Tell the user the result
	CMessageBox MsgBox(csCaption,bRes ? g_pApp->QBString(311) : g_pApp->QBString(305),CMessageBox::OK);
	MsgBox.DoModal();

	// Close the dialog
	EndDialog(IDOK);
}

/*
//		CString csAttachBlatter;
//		csAttachBlatter.Format("%s\\%s -to %s -subject \"%s\" -server smtp.1and1.com -port 587 -u support@quickbiblesoftware.com -pw Gr@vyL@b5 -attacht %s\\%s -f support@quickbiblesoftware.com",g_lpszPath,g_lpszMsgFile,csEmail,csSubject,g_lpszPath,g_lpszLicFile);
//		Send(csAttachBlatter);

	if (OpenClipboard())
	{
		// Allocate the clipboard data
		EmptyClipboard();
		int nLen = csRegLink.GetLength() * 2;
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE,nLen + 2);
		wchar_t * wsz = (wchar_t*)GlobalLock(hClipboardData);
		_bstr_t bt = csRegLink;

		// Copy to the clipboard
		wcscpy(wsz,(wchar_t*)bt);
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_UNICODETEXT,hClipboardData);
		CloseClipboard();
	}

*/