// ContentList.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "ContentList.h"

extern CProjectApp * g_pApp;

// CContentList

IMPLEMENT_DYNAMIC(CContentList, CListCtrl)

CContentList::CContentList() : m_bInitial(true), m_iSelectedItem(-1)
{
}

CContentList::~CContentList()
{
}

// Resize the value column
void CContentList::ResizeColumns()
{
	int nb = (int)m_vecExtra.size();

	// Size the width of the last column to be the unused width
	CRect Rect;
	GetClientRect(&Rect);
	int rw = Rect.Width();
	int iWidth = 0;
	for (int iLoop = 1;iLoop < (nb - 1);++iLoop)
		iWidth += m_aiWidths[iLoop];
	m_aiWidths[nb - 1] = Rect.Width() - iWidth;

	if (!m_bInitial)
		SetColumnWidth(VALUE,m_aiWidths[VALUE]);
}

// Give the list focus, select the item, and make sure it is viewable
void CContentList::SelectItem(int iItem)
{
	// Set the selected item
	m_iSelectedItem = iItem;

	// Redraw the window
	RedrawWindow();

	// Scroll the item into view
	EnsureVisible(m_iSelectedItem,TRUE);
}

int CContentList::GetSelectedItem() const
{
	return m_iSelectedItem;
}

void CContentList::SetList(const std::vector<CString> & vecList)
{
	m_vecList = vecList;
}

BEGIN_MESSAGE_MAP(CContentList, CListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, &CContentList::OnColor)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CContentList::OnDisplay)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CContentList::OnClick)
END_MESSAGE_MAP()

// CContentList message handlers

void CContentList::PreSubclassWindow()
{
	// Get the string for the header
	int iStr;
	switch (m_iID)
	{
	case IDC_BOOKS:
		iStr = 220;
		break;
	case IDC_CHAPTERS:
		iStr = 221;
		break;
	case IDC_VERSES:
		iStr = 204;
		break;
	default:
		iStr = 259;
	}

	// Initialize the control
	CString acsHdr[] = {"","",g_pApp->QBString(iStr)};
	CString acsEx[] = {"","XX","XXXX"};

	// Generate the vector of column headers that will be used to create the list control header
	int nb = sizeof(acsHdr)/sizeof(acsHdr[0]);
	for (int iLoop = 0;iLoop < nb;++iLoop)
	{
		m_vecHeader.push_back(acsHdr[iLoop]);
		m_vecExtra.push_back(acsEx[iLoop]);
	}

	// Set the default widths of the columns to the header string length
	for (int iLoop = 0;iLoop < nb;++iLoop)
	{
		int nTotWidth = GetStringWidth(m_vecHeader[iLoop]) + GetStringWidth(m_vecExtra[iLoop]);
		m_aiWidths.push_back(nTotWidth);
	}

	// Resize the value column
	ResizeColumns();

	// Add a dummy column to prevent the imagelist width from being part of the output width of item 0
	InsertColumn(0,NULL,LVCFMT_LEFT,0,-1);

	// Insert the header columns, sized to the width of the data
	for (int iLoop = 1;iLoop < nb;++iLoop)
		InsertColumn(iLoop,m_vecHeader[iLoop],LVCFMT_LEFT,m_aiWidths[iLoop]);

	// Set the listview style for the variables list view
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	// Set the cell height
	int nHeight = GetSystemMetrics(SM_CYSMICON);
	m_SmallImageList.Create(1,nHeight, ILC_COLORDDB | ILC_MASK, 1, 0);

	// Assign the image list to the list-view control
	SetImageList(&m_SmallImageList,LVSIL_SMALL);
	SetImageList(&m_SmallImageList,LVSIL_NORMAL);

	// Subclass the header control
	m_Header.SubclassWindow(GetHeaderCtrl()->m_hWnd);
	m_Header.SetColumnCount(nb);

	// Prevent the hidden and item columns from being resized
	m_Header.SetColumnLock(HIDDEN,TRUE);
	m_Header.SetColumnLock(ITEM,TRUE);

	// Initialize the item count
	SetItemCount(0);

	// Finish subclassing the window
	m_bInitial = false;
	__super::PreSubclassWindow();
}

// Set the text and background colors for the list items
BOOL CContentList::OnColor(NMHDR *pNMHDR,LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	NMCUSTOMDRAW * pnmcd = static_cast<NMCUSTOMDRAW*>(&(pLVCD->nmcd));

	DWORD dwDrawStage = pnmcd->dwDrawStage;
	if (dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;
	else if (dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// Test item states for the entire row
		int iItem = (int)pnmcd->dwItemSpec;
		UINT uItemState = pnmcd->uItemState;
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (dwDrawStage & CDDS_SUBITEM)
	{
		// Get the text colors for individual columns
		int iItem = (int)pLVCD->nmcd.dwItemSpec;
		int iSubItem = pLVCD->iSubItem;
		if (iItem == m_iSelectedItem)
		{
			pLVCD->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			pLVCD->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
		}
		*pResult = CDRF_DODEFAULT;
	}
	else
		*pResult = CDRF_DODEFAULT;

	return TRUE;
}

void CContentList::OnDisplay(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LVITEM * pItem = &(pDispInfo)->item;
	
	// Get the item and subitem
	int iItem = pItem->iItem;
	int iSubItem = pItem->iSubItem;

	// Text records
	if (pItem->mask & LVIF_TEXT)
	{
		if (iSubItem == VALUE)
		{
			CString csValue;
			csValue = m_vecList[iItem];

			// Update the cell text
			if (!csValue.IsEmpty())
			{
				int nLen = csValue.GetLength() + 1;
				if (nLen > pItem->cchTextMax)
					nLen = pItem->cchTextMax;
				lstrcpyn(pItem->pszText,csValue,nLen);
			}
		}
	}
	
	*pResult = 0;
}

BOOL CContentList::OnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &(pDispInfo)->item;
	*pResult = 0;

	// Get the selected item
	m_iSelectedItem = pItem->mask;

	// Redraw the window
	RedrawWindow();

	// Return FALSE to bubble event up to parent, TRUE to suppress it
	return FALSE; 
}
