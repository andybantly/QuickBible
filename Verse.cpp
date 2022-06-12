// Copyright (C) 2007 Andrew S. Bantly
#include "stdafx.h"
#include "Verse.h"
#include "Project.h"
#include "ProjectDlg.h"

//////////////////////////////////////////////////////////

extern CProjectApp * g_pApp;

IMPLEMENT_SERIAL(CVerse, CObject, VERSIONABLE_SCHEMA|2)

CVerse::CVerse(const CVerse & rhs)
{
	*this = rhs;
}

CVerse & CVerse::operator = (const CVerse & rhs)
{
    if (this != &rhs)
    {
		// UI data
		m_bSelected = rhs.m_bSelected;

		// Verse data
		m_iTranslation = rhs.m_iTranslation;
		m_iBook = rhs.m_iBook;
		m_iChapter = rhs.m_iChapter;
		m_iVerseFrom = rhs.m_iVerseFrom;
		m_iVerseTo = rhs.m_iVerseTo;
		m_csVerse = rhs.m_csVerse;
		m_csTranslation = rhs.m_csTranslation;
		m_csBook = rhs.m_csBook;

		// Verse settings
		m_bBkgCapture = rhs.m_bBkgCapture;
		m_bUseSettings = rhs.m_bUseSettings;
		m_bBkgOn = rhs.m_bBkgOn;
		m_csBkgImage = rhs.m_csBkgImage;
		m_bBkgVid = rhs.m_bBkgVid;
		m_csBkgVid = rhs.m_csBkgVid;
		m_bBkgMix = rhs.m_bBkgMix;
		m_iMixAmt = rhs.m_iMixAmt;
		m_bBkgSolid = rhs.m_bBkgSolid;
		m_crBkgSolid = rhs.m_crBkgSolid;
		m_crTxt = rhs.m_crTxt;
		m_crBkg = rhs.m_crBkg;
		m_csFont = rhs.m_csFont;
		m_iPointSize = rhs.m_iPointSize;
		m_iMaxPointSize = rhs.m_iMaxPointSize;
		m_bAutoSizeFont = rhs.m_bAutoSizeFont;
		m_bCenterJustify = rhs.m_bCenterJustify;
		m_bTransition = rhs.m_bTransition;
		m_nTransStep = rhs.m_nTransStep;
    }
    return * this;
}

CVerse::operator bool() const
{
	bool bValid = (m_iTranslation && m_iBook && m_iChapter && m_iVerseFrom && m_iVerseTo);
	return bValid;
}

CString CVerse::LookupVerse(int iTranslation,int iBook,int iChapter,int iVerseFrom,int iVerseTo)
{
	m_csVerse = g_pApp->LookupDB(iTranslation,iBook,iChapter,iVerseFrom,iVerseTo);
	m_csTranslation = g_pApp->LookupTranslationName(iTranslation);
	m_csBook = g_pApp->LookupBookName(iTranslation,iBook);
	return m_csVerse;
}

CString CVerse::LookupVerse()
{
	return LookupVerse(m_iTranslation,m_iBook,m_iChapter,m_iVerseFrom,m_iVerseTo);
}

void CVerse::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CVerse));
		if (ar.IsStoring())
		{
			// UI data
			ar << m_bSelected;

			// Verse data
			ar << m_iTranslation;
			ar << m_iBook;
			ar << m_iChapter;
			ar << m_iVerseFrom;
			ar << m_iVerseTo;

			// Verse settings
			ar << m_bUseSettings;
			ar << m_bBkgOn;
			ar << m_csBkgImage;
			ar << m_bBkgVid;
			ar << m_csBkgVid;
			ar << m_bBkgCapture;
			ar << m_bBkgMix;
			ar << m_iMixAmt;
			ar << m_bBkgSolid;
			ar << m_crBkgSolid;
			ar << m_crTxt;
			ar << m_crBkg;
			ar << m_csFont;
			ar << m_iPointSize;
			ar << m_iMaxPointSize;
			ar << m_bAutoSizeFont;
			ar << m_bCenterJustify;
			ar << m_bTransition;
			ar << m_nTransStep;
		}
		else
		{
			int nVersion = ar.GetObjectSchema();
			if (nVersion > 1)
			{
				// UI data
				ar >> m_bSelected;
			}

			// Verse data
			ar >> m_iTranslation;
			ar >> m_iBook;
			ar >> m_iChapter;
			ar >> m_iVerseFrom;
			ar >> m_iVerseTo;
			m_csVerse = LookupVerse();

			// Verse settings
			ar >> m_bUseSettings;
			ar >> m_bBkgOn;
			ar >> m_csBkgImage;
			if (nVersion > 1)
			{
				ar >> m_bBkgVid;
				ar >> m_csBkgVid;
				ar >> m_bBkgCapture;
			}
			ar >> m_bBkgMix;
			ar >> m_iMixAmt;
			ar >> m_bBkgSolid;
			ar >> m_crBkgSolid;
			ar >> m_crTxt;
			ar >> m_crBkg;
			ar >> m_csFont;
			ar >> m_iPointSize;
			ar >> m_iMaxPointSize;
			ar >> m_bAutoSizeFont;
			ar >> m_bCenterJustify;
			ar >> m_bTransition;
			ar >> m_nTransStep;
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