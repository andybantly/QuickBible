// Copyright (C) 2007 Andrew S. Bantly
#pragma once

class CCommonEdit
{
public:
	CCommonEdit();
	virtual ~CCommonEdit();

	void SetReal(bool bReal);
	void SetPredictiveText(CStringArray & csaList);

protected:
	bool IsValid(CString csText);
	bool SearchList(CString & csText,CString & csValue);

protected:
	bool m_bReal;
	CStringArray m_csaList;
};