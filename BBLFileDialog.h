// BBLFileDialog.h : header file
// Copyright (C) 2007 Andrew S. Bantly
//

#pragma once

#include "ImageButton.h"

class CBBLFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CBBLFileDialog)

public:
	CBBLFileDialog(BOOL bOpenFileDialog = FALSE, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszTitle = NULL,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING,
		LPCTSTR lpszFilter = NULL,
		CWnd * pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange * pDX);
	virtual void OnInitDone();
	DECLARE_MESSAGE_MAP()
	CString m_csTitle;
};