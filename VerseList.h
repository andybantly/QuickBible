// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include "VerseHeader.h"
#include "Verse.h"
#include <map>
#include <vector>
#include "afxole.h"

// CVerseList - class to manage the display of translations, books, chapters and verses
class CVerseList : public CListCtrl
{
	DECLARE_SERIAL(CVerseList)

public:
	enum {HIDDEN,ITEM,TRANSLATION,BOOK,CHAPTER,FROMVERSE,TOVERSE,SCRIPTURE};
	CVerseList();
	virtual ~CVerseList();
	void AddNewVerse();
	void AddNextVerse(int iVerse);
	void AddExistingVerse(CVerse & Verse);
	void RemVerse(int iVerse);
	void RemAll();
	virtual void Serialize(CArchive & ar);
	int GetDropItem(CPoint point);
	void BeginCombo(int iItem,int iSubItem);
	void BeginEdit(int iItem,int iSubItem,bool bReal);
	void ResizeColumns();
	void UpdateSettingsView();
	bool GetList(int iItem,int iSubItem,CStringArray & csaList);

public:
	void UnSelectAll();
	void UnSelect(int iVerse);
	void Select(int iVerse);
	void Toggle(int iVerse);
	void Toggle(CListCtrl * pList);

public:
	POSITION GetFirstSelectedItemPosition();
	int GetNextSelectedItem(POSITION & Pos);
	BOOL SetItemState(int iVerse,int iSELECTED,int iMASK);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

protected:
	CVerseHeader m_Header;
	CImageList m_SmallImageList;
	std::vector<int> m_aiWidths;
	std::vector<CString> m_acsHeader;
	std::vector<CString> m_acsExtra;
	bool m_bInitial;
	int m_nVerses;

	// Class for a custom drag and drop cursor
	class CDropSource : public COleDropSource
	{
	public:
		CDropSource();
		~CDropSource();
		SCODE GiveFeedback(DROPEFFECT de);
	protected:
		HCURSOR m_hCursor;
	};

	// CDropTarget - class for implementing drag and drop
	class CDropTarget : public COleDropTarget
	{
	public:
		DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		DROPEFFECT GetDropEffect(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		void OnDragLeave(CWnd* pWnd);               
		BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	};

	UINT m_cf;
	CDropSource m_DropSource;
	CDropTarget m_DropTarget;

public:
	int m_nVerseColumn;
	int m_nVerseColumnWidth;
	bool m_bEdit;

public:
	afx_msg BOOL OnColorDisplay(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnUpdateDisplay(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnClick(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnRightClick(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnDoubleClick(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnBeginDrag(NMHDR * pNMHDR,LRESULT * pResult);

	// Rapid edit callback method
	afx_msg LRESULT OnRapidEdit(WPARAM wParam,LPARAM lParam);
};