// QuickEdit.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "CommonEdit.h"

CCommonEdit::CCommonEdit()
{
}

CCommonEdit::~CCommonEdit()
{
}

// Set the edit control for hosting a number or a string
void CCommonEdit::SetReal(bool bReal)
{
	m_bReal = bReal;
}

// Set the list of predictive text to the edit control
void CCommonEdit::SetPredictiveText(CStringArray & csaList)
{
	m_csaList.RemoveAll();
	m_csaList.Append(csaList);
}

// Test the data for numerical only input
bool CCommonEdit::IsValid(CString csText)
{
	// Test for a number if the edit control hosts a number
	if (m_bReal)
	{
		int nLen = csText.GetLength();
		for (int iPos = 0;iPos < nLen;++iPos)
			if (!isdigit(csText.GetAt(iPos)))
				return false;
	}
	return true;
}

// Substring match the predictive text to the list
bool CCommonEdit::SearchList(CString & csText,CString & csValue)
{
	bool bRet = false;
	INT_PTR nItems = m_csaList.GetCount();
	for (INT_PTR iItem = 0;iItem < nItems;++iItem)
	{
		CString csSearch = csText;
		csSearch.MakeLower();
		CString csCompare = m_csaList.GetAt(iItem);
		csCompare.MakeLower();
		if (csCompare.Find(csSearch) == 0)
		{
			csValue = m_csaList.GetAt(iItem);
			bRet = true;
			break;
		}
	}
	return bRet;
}