#pragma once
#include "afxwin.h"
#include "ImageButton.h"

// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRegisterDlg();

// Dialog Data
	enum { IDD = IDD_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CImageButton m_Register;
	CImageButton m_Cancel;

	CString m_csName;
	CString m_csEmail;
	CString m_csOrderNumber;
	CString m_csOrderCode;

	afx_msg void OnClickedRegister();
};
