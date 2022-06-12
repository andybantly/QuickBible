// MessageBox.h : header file
// Copyright (C) 2008 Andrew S. Bantly
//

#pragma once
#include "MessageBox.h"
#include "ImageStatic.h"
#include "ImageButton.h"
#include "StaticText.h"
#include "ActiveWindow.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog used for App About

class CMessageBox : public CDialog
{
public:
	enum {NO = 3};
	enum {OKCANCEL = 0,OK = 1,YESNO = 2,YESNOCANCEL = 3,OKCANCELEDIT = 4};
	CMessageBox(CString csCaption,CString csMessage,int iType = OKCANCEL,int iImageId = IDB_INFO);
	CString GetValue() const;

	// Dialog Data
	//{{AFX_DATA(CMessageBox)
	enum { IDD = IDD_MESSAGEBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageBox)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CActiveWindow m_ActiveWindow;
	CString m_csCaption;
	CString m_csMessage;
	int m_iType;
	int m_iImageId;
	CImageStatic m_Image;
	CImageButton m_OK;
	CImageButton m_Cancel;
	CImageButton m_Other;
	CStaticText m_MessageText;
	CEdit m_Value;
	CString m_csValue;

	//{{AFX_MSG(CMessageBox)
	afx_msg void OnOther();
	afx_msg LRESULT OnResizeMessageBox(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};