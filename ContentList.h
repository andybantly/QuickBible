#pragma once

#include "VerseHeader.h"
#include <vector>

class CContentList : public CListCtrl
{
	DECLARE_DYNAMIC(CContentList)

	int m_iSelectedItem;
	CImageList m_SmallImageList;
	std::vector<int> m_aiWidths;
	std::vector<CString> m_vecHeader;
	std::vector<CString> m_vecExtra;
	CVerseHeader m_Header;
	bool m_bInitial;
	std::vector<CString> m_vecList;

public:
	int m_iID;
	int m_iTranslation;
	enum {HIDDEN,ITEM,VALUE};

public:
	CContentList();
	virtual ~CContentList();
	void ResizeColumns();
	void SelectItem(int iItem);
	int GetSelectedItem() const;
	void SetList(const std::vector<CString> & vecList);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

public:
	afx_msg BOOL OnColor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDisplay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnClick(NMHDR *pNMHDR, LRESULT *pResult);
};


