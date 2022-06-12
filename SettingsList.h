// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include "VerseHeader.h"
#include "Verse.h"
#include <map>
#include <vector>
#include "afxole.h"

// CSettingsList - class to manage the per-scripture display settings
class CSettingsList : public CListCtrl
{
public:
	enum {HIDDEN,ITEM,USE,VERSE,BKGIMG,TXTCOL,BKGCOL,FONT,SIZE,MAX,AUTO,CENTER,TRANSITION};
	CSettingsList();
	virtual ~CSettingsList();
	CString FormatVerse(CVerse & verse);
	void UpdateSettingsView(int nVerses);
	void BeginCombo(int iItem,int iSubItem);
	void BeginEdit(int iItem,int iSubItem,bool bReal);
	void SetImageColor();
	void MakeDDB(CBitmap & Square,int nWidth,int nHeight,COLORREF cr);

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
	bool m_bRedraw;
	CVerseHeader m_Header;
	CImageList m_ColorImageList;
	std::vector<int> m_aiWidths;
	std::vector<CString> m_acsHeader;
	std::vector<CString> m_acsExtra;

protected:
	void UpdateOtherVerse(CVerse & Verse);

protected:
	int m_iTimerOp;
	int m_iTimerItem;

public:
	bool m_bEdit;

public:
	afx_msg BOOL OnClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnColorDisplay(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnUpdateDisplay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnBkgImg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTxtColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnBkgColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnChooseFont(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRapidEdit(WPARAM wParam,LPARAM lParam);
};