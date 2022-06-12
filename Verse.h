// Copyright (C) 2007 Andrew S. Bantly
#pragma once

// CVerse - class to store a single verse
class CVerse : public CObject
{
	DECLARE_SERIAL(CVerse)
public:
	CVerse() : 
		m_iTranslation(1), m_iBook(1), m_iChapter(1), m_iVerseFrom(1), m_iVerseTo(1),
		m_bUseSettings(FALSE),m_bBkgCapture(FALSE),m_bBkgOn(FALSE),m_bBkgVid(FALSE),m_bBkgMix(TRUE),m_iMixAmt(29),
		m_bBkgSolid(FALSE),m_crBkgSolid(RGB(0,0,128)),m_crTxt(RGB(255,255,255)),
		m_crBkg(RGB(0,0,0)),m_csFont("Georgia"),m_iPointSize(50),m_iMaxPointSize(1000),
		m_bAutoSizeFont(TRUE),m_bCenterJustify(TRUE),m_bTransition(TRUE),m_nTransStep(16) {}
	CVerse(const CVerse & rhs);
	CVerse & operator = (const CVerse & rhs);
	operator bool () const;
	CString LookupVerse(int iTranslation,int iBook,int iChapter,int iVerseFrom,int iVerseTo);
	CString LookupVerse();
	virtual void Serialize(CArchive & ar);

	// UI data
	BOOL m_bSelected;

	// Verse data
	int m_iTranslation;
	int m_iBook;
	int m_iChapter;
	int m_iVerseFrom;
	int m_iVerseTo;
	CString m_csVerse;
	CString m_csTranslation;
	CString m_csBook;

	// Verse settings
	BOOL m_bBkgCapture;
	BOOL m_bUseSettings;
	BOOL m_bBkgOn;
	CString m_csBkgImage;
	BOOL m_bBkgVid;
	CString m_csBkgVid;
	BOOL m_bBkgMix;
	UINT m_iMixAmt;
	BOOL m_bBkgSolid;
	COLORREF m_crBkgSolid;
	COLORREF m_crTxt;
	COLORREF m_crBkg;
	CString m_csFont;
	UINT m_iPointSize;
	UINT m_iMaxPointSize;
	BOOL m_bAutoSizeFont;
	BOOL m_bCenterJustify;
	BOOL m_bTransition;
	int m_nTransStep;
};