// Copyright (C) 2007 Andrew S. Bantly
#include "stdafx.h"
#include "Project.h"
#include "DBFormat.h"
#include <vector>

using namespace std;

extern CProjectApp * g_pApp;

IMPLEMENT_SERIAL(CDBVerse, CObject, VERSIONABLE_SCHEMA|2)

CDBVerse::CDBVerse()
{
}

CDBVerse & CDBVerse::operator = (const CDBVerse & rhs)
{
	if (this != &rhs)
	{
		m_vecCryptName = rhs.m_vecCryptName;
		m_vecCryptVerse = rhs.m_vecCryptVerse;
	}
	return * this;
}

void CDBVerse::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CDBVerse));
		if (ar.IsStoring())
		{
			#if !defined(DEMO)
			unsigned short int nLength = (unsigned short int)m_vecCryptName.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptName[0],nLength);
			nLength = (unsigned short int)m_vecCryptVerse.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptVerse[0],nLength);
			#endif
		}
		else
		{ 
			int nVersion = ar.GetObjectSchema();
			unsigned short int nLength;
			ar >> nLength;
			m_vecCryptName.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptName[0],nLength);
			ar >> nLength;
			m_vecCryptVerse.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptVerse[0],nLength);
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

IMPLEMENT_SERIAL(CDBChap, CObject, VERSIONABLE_SCHEMA|2)

CDBChap::CDBChap()
{
}

CDBChap & CDBChap::operator = (const CDBChap & rhs)
{
	if (this != &rhs)
	{
		m_vecCryptName = rhs.m_vecCryptName;
		m_vVerse = rhs.m_vVerse;
	}
	return * this;
}

void CDBChap::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CDBChap));
		if (ar.IsStoring())
		{
			#if !defined(DEMO)
			unsigned short int nLength = (unsigned short int)m_vecCryptName.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptName[0],nLength);
			unsigned char nVerses = (unsigned char)m_vVerse.size();
			ar << nVerses;
			for (unsigned char iver = 0;iver < nVerses;++iver)
			{
				CDBVerse & DBVerse = m_vVerse[iver];
				DBVerse.Serialize(ar);
			}
			#endif
		}
		else
		{
			int nVersion = ar.GetObjectSchema();
			unsigned short int nLength;
			ar >> nLength;
			m_vecCryptName.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptName[0],nLength);
			unsigned char nVerses;
			ar >> nVerses;
			m_vVerse.resize(nVerses);
			for (unsigned char iver = 0;iver < nVerses;++iver)
			{
				CDBVerse & DBVerse = m_vVerse[iver];
				DBVerse.Serialize(ar);
			}
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

IMPLEMENT_SERIAL(CDBBook, CObject, VERSIONABLE_SCHEMA|2)

CDBBook::CDBBook()
{
}

CDBBook & CDBBook::operator = (const CDBBook & rhs)
{
	if (this != &rhs)
	{
		m_vecCryptName = rhs.m_vecCryptName;
		m_vChap = rhs.m_vChap;
	}
	return * this;
}

void CDBBook::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CDBBook));
		if (ar.IsStoring())
		{
			#if !defined(DEMO)
			unsigned short int nLength = (unsigned short int)m_vecCryptName.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptName[0],nLength);
			unsigned char nChaps = (unsigned char)m_vChap.size();
			ar << nChaps;
			for (unsigned char ich = 0;ich < nChaps;++ich)
			{
				CDBChap & DBChap = m_vChap[ich];
				DBChap.Serialize(ar);
			}
			#endif
		}
		else
		{
			int nVersion = ar.GetObjectSchema();
			unsigned short int nLength;
			ar >> nLength;
			m_vecCryptName.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptName[0],nLength);
			unsigned char nChaps;
			ar >> nChaps;
			m_vChap.resize(nChaps);
			for (unsigned char ich = 0;ich < nChaps;++ich)
			{
				CDBChap & DBChap = m_vChap[ich];
				DBChap.Serialize(ar);
			}
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

IMPLEMENT_SERIAL(CDBBible, CObject, VERSIONABLE_SCHEMA|2)

CDBBible::CDBBible()
{
}

CDBBible & CDBBible::operator = (const CDBBible & rhs)
{
	if (this != &rhs)
	{
		m_vecCryptName = rhs.m_vecCryptName;
		m_vBook = rhs.m_vBook;
	}
	return * this;
}

void CDBBible::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CDBBible));
		if (ar.IsStoring())
		{
			#if !defined(DEMO)
			unsigned short int nLength = (unsigned short int)m_vecCryptName.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptName[0],nLength);
			unsigned char nBooks = (unsigned char)m_vBook.size();
			ar << nBooks;
			for (unsigned char inb = 0;inb < nBooks;++inb)
			{
				CDBBook & DBBook = m_vBook[inb];
				DBBook.Serialize(ar);
			}
			#endif
		}
		else
		{
			int nVersion = ar.GetObjectSchema();
			unsigned short int nLength;
			ar >> nLength;
			m_vecCryptName.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptName[0],nLength);
			unsigned char nBooks;
			ar >> nBooks;
			m_vBook.resize(nBooks);
			for (unsigned char inb = 0;inb < nBooks;++inb)
			{
				CDBBook & DBBook = m_vBook[inb];
				DBBook.Serialize(ar);
			}
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

IMPLEMENT_SERIAL(CDBTranslation, CObject, VERSIONABLE_SCHEMA|2)

CDBTranslation::CDBTranslation()
{
}

CDBTranslation & CDBTranslation::operator = (const CDBTranslation & rhs)
{
	if (this != &rhs)
	{
		m_vecCryptName = rhs.m_vecCryptName;
		m_vBible = rhs.m_vBible;
	}
	return * this;
}

void CDBTranslation::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CDBTranslation));
		if (ar.IsStoring())
		{
			#if !defined(DEMO)
			unsigned short int nLength = (unsigned short int)m_vecCryptName.size();
			ar << nLength;
			if (nLength)
				ar.Write(&m_vecCryptName[0],nLength);
			unsigned char nTrans = (unsigned char)m_vBible.size();
			ar << nTrans;
			for (unsigned char itr = 0;itr < nTrans;++itr)
			{
				CDBBible & DBBible = m_vBible[itr];
				DBBible.Serialize(ar);
			}
			#endif
		}
		else
		{
			int nVersion = ar.GetObjectSchema();
			unsigned short int nLength;
			ar >> nLength;
			m_vecCryptName.resize(nLength);
			if (nLength)
				ar.Read(&m_vecCryptName[0],nLength);
			unsigned char nTrans;
			ar >> nTrans;
			m_vBible.resize(nTrans);
			for (unsigned char itr = 0;itr < nTrans;++itr)
			{
				CDBBible & DBBible = m_vBible[itr];
				DBBible.Serialize(ar);
			}
		}
	}
	catch (CArchiveException * p)
	{
		TCHAR szErrorMessage[255];
		p->GetErrorMessage(szErrorMessage,255);
		CString csCaption = g_pApp->QBString(259);
		CString csFmt = g_pApp->QBString(322);
		CString csMessage;
		csMessage.Format(csFmt,ar.m_strFileName);
		csMessage += "\n\n";
		csMessage += szErrorMessage;
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}