// VerseList.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "VerseList.h"
#include "SettingsList.h"
#include "GridComboBox.h"
#include "GridEdit.h"
#include "Verse.h"
#include "ProjectDlg.h"
#include <vector>

using namespace std;

extern CProjectApp * g_pApp;
static CProjectDlg * g_pDlg = NULL;

// CVerseList

IMPLEMENT_SERIAL(CVerseList, CListCtrl, VERSIONABLE_SCHEMA|2)

CVerseList::CVerseList() : m_nVerses(0)

{
	// Register the drag & drop format
	m_cf = RegisterClipboardFormat("QuickBible");
}

CVerseList::~CVerseList()
{
	m_DropTarget.Revoke();
}

void CVerseList::Serialize(CArchive & ar)
{
	try
	{
		ar.SerializeClass(RUNTIME_CLASS(CVerseList));
		if (ar.IsStoring())
		{
			ar << m_nVerses;
			for (int iVerse = 0;iVerse < m_nVerses;++iVerse)
			{
				CVerse & verse = g_pApp->m_vecVerses[iVerse];
				verse.Serialize(ar);
			}
		}
		else
		{
			// Get the file version
			int nVersion = ar.GetObjectSchema();
			if (nVersion == 1 || nVersion == 2)
			{
				m_nVerses = 0;
				g_pApp->m_vecVerses.clear();
				int nVerses = 0;
				ar >> nVerses;
				for (int iVerse = 0;iVerse < nVerses;++iVerse)
				{
					CVerse verse;
					verse.Serialize(ar);
					g_pApp->m_vecVerses.push_back(verse);
					SetItemCount(++m_nVerses);
				}
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

BEGIN_MESSAGE_MAP(CVerseList, CListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CVerseList::OnClick)
	ON_NOTIFY_REFLECT_EX(NM_RCLICK, &CVerseList::OnRightClick)
	ON_NOTIFY_REFLECT_EX(NM_DBLCLK, &CVerseList::OnDoubleClick)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, &CVerseList::OnEndLabelEdit)
	ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, &CVerseList::OnColorDisplay)
	ON_NOTIFY_REFLECT_EX(LVN_GETDISPINFO, &CVerseList::OnUpdateDisplay)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINDRAG, &CVerseList::OnBeginDrag)
	ON_MESSAGE(WM_RAPIDEDIT, &CVerseList::OnRapidEdit)
END_MESSAGE_MAP()

// CVerseList message handlers
void CVerseList::PreSubclassWindow()
{
	m_bInitial = true;

	// Initialize the control
	CString acsHdr[] = {"","",g_pApp->QBString(219),g_pApp->QBString(220),g_pApp->QBString(221),g_pApp->QBString(222),g_pApp->QBString(223),g_pApp->QBString(224)};
	CString acsEx[] = {"","XX","XXXX","XXXX","XXX","XXX","XXX","XXXX"};

	// Generate the vector of column headers that will be used to create the list control header
	int nb = sizeof(acsHdr)/sizeof(acsHdr[0]);
	for (int iLoop = 0;iLoop < nb;++iLoop)
	{
		m_acsHeader.push_back(acsHdr[iLoop]);
		m_acsExtra.push_back(acsEx[iLoop]);
	}

	// Set the default widths of the columns to the header string length
	for (int iLoop = 0;iLoop < nb;++iLoop)
	{
		int nTotWidth = GetStringWidth(m_acsHeader[iLoop]) + GetStringWidth(m_acsExtra[iLoop]);
		m_aiWidths.push_back(nTotWidth);
	}

	// Resize the book and verse columns
	ResizeColumns();

	// Add a dummy column to prevent the imagelist width from being part of the output width of item 0
	InsertColumn(0,NULL,LVCFMT_LEFT,0,-1);

	// Insert the header columns, sized to the width of the data
	for (int iLoop = 1;iLoop < nb;++iLoop)
		InsertColumn(iLoop,m_acsHeader[iLoop],LVCFMT_LEFT,m_aiWidths[iLoop]);

	// Set the listview style for the variables list view
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	// Set the cell height
	int nHeight = GetSystemMetrics(SM_CYSMICON);
	m_SmallImageList.Create(1,nHeight, ILC_COLORDDB | ILC_MASK, 1, 0);

	// Assign the image list to the list-view control
	SetImageList(&m_SmallImageList,LVSIL_SMALL);
	SetImageList(&m_SmallImageList,LVSIL_NORMAL);

	// Initialize the item count
	SetItemCount(0);

	// Subclass the header control
	m_Header.SubclassWindow(GetHeaderCtrl()->m_hWnd);
	m_Header.SetColumnCount(nb);

	// Prevent the hidden and item columns from being resized
	m_Header.SetColumnLock(HIDDEN,TRUE);
	m_Header.SetColumnLock(ITEM,TRUE);

	// Register as a drop target
	CWnd * pWnd = STATIC_DOWNCAST(CWnd,this);
	m_DropTarget.Register(pWnd);

	// Finish subclassing the window
	CListCtrl::PreSubclassWindow();
	m_bInitial = false;

	// Get the project dialog
	g_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;
}

// Resize the translation, book, and scripture columns
void CVerseList::ResizeColumns()
{
	int nb = (int)m_acsExtra.size();

	// Adjust the translation column width to fit the combobox strings
	CString csTranslation;
	int nbTrans = g_pApp->LookupNBTrans();
	for (int iLoop = 1;iLoop <= nbTrans;++iLoop)
	{
		csTranslation = g_pApp->LookupTranslationName(iLoop);
		int nWidth = GetStringWidth(csTranslation) + GetStringWidth(m_acsExtra[TRANSLATION]);
		int & nTotWidth = m_aiWidths[TRANSLATION];
		if (nWidth > nTotWidth)
			nTotWidth = nWidth;
	}

	// Adjust the book column width to fit the combobox strings
	for (int iTrans = 1;iTrans <= nbTrans;++iTrans)
	{
		int nBooks = g_pApp->LookupNBBooks(iTrans);
		for (int iBook = 1;iBook <= nBooks;++iBook)
		{
			CString csBook = g_pApp->LookupBookName(iTrans,iBook);
			int nWidth = GetStringWidth(csBook) + GetStringWidth(m_acsExtra[BOOK]);
			int & nTotWidth = m_aiWidths[BOOK];
			if (nWidth > nTotWidth)
				nTotWidth = nWidth;
		}
	}

	// Size the width of the last column to be the unused width
	CRect Rect;
	GetClientRect(&Rect);
	int rw = Rect.Width();
	int iWidth = 0;
	for (int iLoop = 1;iLoop < (nb - 1);++iLoop)
		iWidth += m_aiWidths[iLoop];
	m_aiWidths[nb - 1] = Rect.Width() - iWidth;
	m_nVerseColumn = nb - 1;
	m_nVerseColumnWidth = m_aiWidths[nb - 1];

	if (!m_bInitial)
	{
		SetColumnWidth(TRANSLATION,m_aiWidths[TRANSLATION]);
		SetColumnWidth(BOOK,m_aiWidths[BOOK]);
		SetColumnWidth(SCRIPTURE,m_aiWidths[SCRIPTURE]);
	}
}

// Set the text and background colors for the list items
BOOL CVerseList::OnColorDisplay(NMHDR * pNMHDR,LRESULT * pResult)
{
	BOOL bSelected = FALSE;
	LV_DISPINFO * pDispInfo = (LV_DISPINFO *)pNMHDR;
	NMLVCUSTOMDRAW * pListView = reinterpret_cast<NMLVCUSTOMDRAW *>(pNMHDR);
	NMCUSTOMDRAW * pCustomDraw = static_cast<NMCUSTOMDRAW *>(&(pListView->nmcd));

	DWORD dwDrawStage = pCustomDraw->dwDrawStage;
	if (dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;
	else if (dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// Test item states for the entire row
		int iItem = (int)pCustomDraw->dwItemSpec;
		bSelected = g_pApp->m_vecVerses[iItem].m_bSelected;

		CListCtrl::SetItemState(iItem,bSelected ? SELECTED : UNSELECTED,MASK);
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (dwDrawStage & CDDS_SUBITEM)
	{
		// Get the text colors for individual columns
		int iItem = (int)pCustomDraw->dwItemSpec;
		int iSubItem = pListView->iSubItem;
		bSelected = g_pApp->m_vecVerses[iItem].m_bSelected;

		// Set the colors
		pListView->clrText = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
		pListView->clrTextBk = bSelected ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW);
		*pResult = CDRF_DODEFAULT;
	}
	else
		*pResult = CDRF_DODEFAULT;

	return TRUE;
}

BOOL CVerseList::OnUpdateDisplay(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLVDISPINFO * pDispInfo = reinterpret_cast<NMLVDISPINFO *>(pNMHDR);
	LVITEM * pItem = &(pDispInfo)->item;
	*pResult = 0;
	
	// Get the item and subitem
	int iItem = pItem->iItem;
	int iSubItem = pItem->iSubItem;

	// Get the verse
	CVerse & verse = g_pApp->m_vecVerses[iItem];

	// Text records
	if (pItem->mask & LVIF_TEXT)
	{
		// Get the cell text
		if (iSubItem > ITEM)
		{
			CString csText;
			if (iSubItem == TRANSLATION)
				csText = g_pApp->LookupTranslationName(verse.m_iTranslation);
			else if (iSubItem == BOOK)
				csText = g_pApp->LookupBookName(verse.m_iTranslation,verse.m_iBook);
			else if (iSubItem == CHAPTER)
				csText.Format("%d",verse.m_iChapter);
			else if (iSubItem == FROMVERSE)
				csText.Format("%d",verse.m_iVerseFrom);
			else if (iSubItem == TOVERSE)
				csText.Format("%d",verse.m_iVerseTo);
			else if (iSubItem == SCRIPTURE)
			{
				verse.m_csVerse = verse.LookupVerse();
				csText = verse.m_csVerse;
			}

			// Update the cell text
			if (!csText.IsEmpty())
			{
				int nLen = csText.GetLength() + 1;
				if (nLen > pItem->cchTextMax)
					nLen = pItem->cchTextMax;
				lstrcpyn(pItem->pszText,csText,nLen);
			}
		}
	}

	return TRUE;
}

BOOL CVerseList::OnEndLabelEdit(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLVDISPINFO * pDispInfo = reinterpret_cast<NMLVDISPINFO *>(pNMHDR);
	LV_ITEM * pItem = &(pDispInfo)->item;
	*pResult = 0;

	if (pItem->pszText != NULL)
	{
		// Get the item and update structure
		int iItem = pItem->iItem;
		if (iItem < 0)
			return FALSE;
		int iSubItem = pItem->iSubItem;

		// Get the selection
		int iCurSel = atoi(pItem->pszText);

		// Adjust for the combo box
		if (!m_bEdit)
			iCurSel++;

		// Lower bounds check
		if (m_bEdit && iSubItem > BOOK)
		{
			if (iCurSel < 1)
				return FALSE;
		}

		// Update the verse
		CVerse & verse = g_pApp->m_vecVerses[iItem];

		if (iSubItem == TRANSLATION)
		{
			// Get the translation
			int iTranslation;
			if (m_bEdit)
			{
				CString csText = pItem->pszText;
				iTranslation = g_pApp->LookupTranslationIndex(csText);
			}
			else
				iTranslation = iCurSel;

			// Load the translation if it is not yet loaded
			if (!g_pApp->IsLoaded(iTranslation))
				g_pApp->LoadTranslation(iTranslation);

			// Get the number of books in the translation
			int nBooks = g_pApp->LookupNBBooks(iTranslation);
			if (!nBooks)
			{
				iTranslation = verse.m_iTranslation;
				nBooks = g_pApp->LookupNBBooks(iTranslation);
			}

			verse.m_iTranslation = iTranslation;
			if (verse.m_iBook > nBooks)
				verse.m_iBook = 1;
			int nChapters = g_pApp->LookupNBChap(verse.m_iTranslation,verse.m_iBook);
			if (verse.m_iChapter > nChapters)
				verse.m_iChapter = 1;
			int nVerses = g_pApp->LookupNBVerses(verse.m_iTranslation,verse.m_iBook,verse.m_iChapter);
			if (verse.m_iVerseFrom > nVerses)
				verse.m_iVerseFrom = 1;
			if (verse.m_iVerseTo < verse.m_iVerseFrom || verse.m_iVerseTo > nVerses)
				verse.m_iVerseTo = verse.m_iVerseFrom;
		}
		else if (iSubItem == BOOK)
		{
			if (m_bEdit)
			{
				CString csBookName = pItem->pszText;
				iCurSel = g_pApp->LookupBookIndex(verse.m_iTranslation,csBookName);
				if (!iCurSel)
					iCurSel = 1;
			}
			verse.m_iBook = iCurSel;
			int nChapters = g_pApp->LookupNBChap(verse.m_iTranslation,verse.m_iBook);
			if (verse.m_iChapter > nChapters)
				verse.m_iChapter = 1;
			int nVerses = g_pApp->LookupNBVerses(verse.m_iTranslation,verse.m_iBook,verse.m_iChapter);
			if (verse.m_iVerseFrom > nVerses)
				verse.m_iVerseFrom = 1;
			if (verse.m_iVerseTo < verse.m_iVerseFrom || verse.m_iVerseTo > nVerses)
				verse.m_iVerseTo = verse.m_iVerseFrom;
		}
		else if (iSubItem == CHAPTER)
		{
			verse.m_iChapter = iCurSel;
			int nChapters = g_pApp->LookupNBChap(verse.m_iTranslation,verse.m_iBook);
			if (verse.m_iChapter > nChapters)
				verse.m_iChapter = 1;
			int nVerses = g_pApp->LookupNBVerses(verse.m_iTranslation,verse.m_iBook,verse.m_iChapter);
			if (verse.m_iVerseFrom > nVerses)
				verse.m_iVerseFrom = 1;
			if (verse.m_iVerseTo < verse.m_iVerseFrom || verse.m_iVerseTo > nVerses)
				verse.m_iVerseTo = verse.m_iVerseFrom;
		}
		else if (iSubItem == FROMVERSE)
		{
			verse.m_iVerseFrom = iCurSel;
			int nVerses = g_pApp->LookupNBVerses(verse.m_iTranslation,verse.m_iBook,verse.m_iChapter);
			if (verse.m_iVerseFrom > nVerses)
				verse.m_iVerseFrom = 1;
			if (verse.m_iVerseFrom > verse.m_iVerseTo)
				verse.m_iVerseTo = verse.m_iVerseFrom;
		}
		else if (iSubItem == TOVERSE)
		{
			verse.m_iVerseTo = iCurSel;
			int nVerses = g_pApp->LookupNBVerses(verse.m_iTranslation,verse.m_iBook,verse.m_iChapter);
			if (verse.m_iVerseTo < verse.m_iVerseFrom || verse.m_iVerseTo > nVerses)
				verse.m_iVerseTo = verse.m_iVerseFrom;
		}
	}

	return FALSE;
}

BOOL CVerseList::OnClick(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Turn off redrawing
	SetRedraw(FALSE);

	// Select the item
	LV_DISPINFO * pDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM * pItem = &(pDispInfo)->item;
	int iItem = pItem->mask;
	if (iItem >= 0 && iItem < GetItemCount())
	{
		// Process the click
		g_pDlg->ProcessClick(this,iItem,1);

		// Test the subitem click area
		unsigned int iSubItem = pItem->iItem;
		if (iSubItem > ITEM && iSubItem < SCRIPTURE)
			BeginCombo(iItem,iSubItem);
	}
	else
		UnSelectAll();

	// Turn on redrawing
	SetRedraw();
	RedrawWindow();

	*pResult = 0;
	return TRUE;
}

BOOL CVerseList::OnRightClick(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Turn off redrawing
	SetRedraw(FALSE);

	// Select the item
	LV_DISPINFO * pDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM * pItem = &(pDispInfo)->item;
	int iItem = pItem->mask;
	if (iItem >= 0 && iItem < GetItemCount())
	{
		// Process the click
		g_pDlg->ProcessClick(this,iItem,1);

		// Test the subitem click area
		unsigned int iSubItem = pItem->iItem;
		if (iSubItem > ITEM && iSubItem < SCRIPTURE)
			BeginEdit(iItem,iSubItem,false);
	}
	else
		UnSelectAll();

	// Turn on redrawing
	SetRedraw();
	RedrawWindow();

	*pResult = 0;
	return TRUE;
}

// Add a new, next verse when the list is double clicked
BOOL CVerseList::OnDoubleClick(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Turn off redrawing
	SetRedraw(FALSE);

	// Add the next verse
	g_pDlg->OnAddNextVerse();

	// Turn on redrawing
	SetRedraw();
	RedrawWindow();

	*pResult = 0;
	return TRUE;
}

BOOL CVerseList::OnBeginDrag(NMHDR * pNMHDR,LRESULT * pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	vector<int> vVerses;
	POSITION Pos = GetFirstSelectedItemPosition();
	while (Pos)
		vVerses.push_back(GetNextSelectedItem(Pos));
	if (vVerses.empty())
		return FALSE;

	// Create an HGLOBAL based stream
	CComPtr<IStream> pStream;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL,FALSE,&pStream);
	if (FAILED(hr))
		return FALSE;

	// Write the drag verses
	int nVerses = (int)vVerses.size();
	pStream->Write(&nVerses,sizeof(nVerses),NULL);
	for (int iVerse = 0;iVerse < nVerses;++iVerse)
	{
		int iItem = vVerses[iVerse];
		pStream->Write(&iItem,sizeof(iItem),NULL);
	}

	HGLOBAL hStream = NULL;
	GetHGlobalFromStream(pStream,&hStream);
	if (!hStream)
		return FALSE;

	// Create a data source using the HGLOBAL data
	COleDataSource DataSource;
	DataSource.CacheGlobalData(m_cf,hStream,NULL);

	// Start drag and drop, hook up to COleDropSource for cursor feedback
	DROPEFFECT de = DataSource.DoDragDrop(DROPEFFECT_MOVE,NULL,&m_DropSource);
	*pResult = 0;
	return TRUE;
}

void CVerseList::BeginCombo(int iItem,int iSubItem)
{
	// Get the list of strings for the item and subitem
	CStringArray csaList;
	if (!GetList(iItem,iSubItem,csaList))
		return;

	// Make sure the item is visible
	EnsureVisible(iItem,FALSE);
	m_bEdit = false;

	// Number of items for the combobox
	int nCount = 10;
	
	// Get the rectangle of the subitem for editing
	CRect Rect;
	int iOffset = 0;
	for (int i = 0;i < iSubItem;++i)
		iOffset += GetColumnWidth(i);
	GetItemRect(iItem,&Rect,LVIR_BOUNDS);
	Rect.top -= 2;
	Rect.bottom -= 2;
	Rect.left += iOffset;
	Rect.right = Rect.left + GetColumnWidth(iSubItem) + 1 ;
	Rect.bottom = Rect.bottom + (nCount * Rect.Height() + 3);

	// Create the combo box control
	CString csDefault = GetItemText(iItem,iSubItem);
	CGridComboBox * pCombo = new CGridComboBox(iItem,iSubItem,csDefault);
	DWORD dwFlags = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL;
	pCombo->Create(dwFlags,Rect,this,WM_USER);
	pCombo->SetFont(GetFont());
	HWND hCombo = pCombo->GetSafeHwnd();

	// Add the strings to the combo box
	INT_PTR nbItems = csaList.GetCount();
	int iCurSel = 0;
	for (INT_PTR iItem = 0;iItem < nbItems;++iItem)
	{
		CString csItem = csaList.GetAt(iItem);
		pCombo->AddString(csItem);
		if (csDefault == csItem)
			iCurSel = (int)iItem;
	}

	// Set the current selection
	pCombo->SetCurSel(iCurSel);

	// Give the combo box focus
	pCombo->SetFocus();
}

// Start editing a subitem using an embedded edit control
void CVerseList::BeginEdit(int iItem,int iSubItem,bool bReal)
{
	// Get the list of strings for the item and subitem
	CStringArray csaList;
	if (!GetList(iItem,iSubItem,csaList))
		return;

	// Reset the edit flag
	m_bEdit = true;

	// Make sure the item is visible
	EnsureVisible(iItem,FALSE);

	// Get the rectangle of the subitem for editing
	CRect Rect;
	int iOffset = 0;
	for (int i = 0;i < iSubItem;++i)
		iOffset += GetColumnWidth(i);
	GetItemRect(iItem,&Rect,LVIR_BOUNDS);
	Rect.left += iOffset;
	Rect.right = Rect.left + GetColumnWidth(iSubItem) + 1 ;

	// Create the edit control and set it's initial text
	CString csDefault = GetItemText(iItem,iSubItem);
	CGridEdit * pEdit = new CGridEdit(iItem,iSubItem,csDefault,bReal);
	DWORD dwStyle = ES_LEFT|WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	pEdit->Create(dwStyle,Rect,this,WM_USER);
	pEdit->SetFont(GetFont());
	pEdit->SetWindowText(csDefault);
	pEdit->SetSel(0,-1);

	// Set the list of predictive text to the edit control
	pEdit->SetPredictiveText(csaList);

	// Give the control focus
	pEdit->SetFocus();
}

// Get the list of strings for the item and subitem
bool CVerseList::GetList(int iItem,int iSubItem,CStringArray & csaList)
{
	if (iSubItem == TRANSLATION)
		g_pApp->GetList(csaList);
	else if (iSubItem == BOOK)
	{
		CString csTranslationName = GetItemText(iItem,TRANSLATION);
		g_pApp->GetList(csaList,csTranslationName);
	}
	else if (iSubItem == CHAPTER)
	{
		CString csTranslationName = GetItemText(iItem,TRANSLATION);
		CString csBookName = GetItemText(iItem,BOOK);
		g_pApp->GetList(csaList,csTranslationName,csBookName);
	}
	else if (iSubItem == FROMVERSE || iSubItem == TOVERSE)
	{
		CString csTranslationName = GetItemText(iItem,TRANSLATION);
		CString csBookName = GetItemText(iItem,BOOK);
		CString csChapter = GetItemText(iItem,CHAPTER);
		g_pApp->GetList(csaList,csTranslationName,csBookName,csChapter);
	}
	return !csaList.IsEmpty();
}

void CVerseList::AddNewVerse()
{
	CVerse Verse;
	CString csVerse = Verse.LookupVerse();
	if (csVerse.Find(g_pApp->QBString(225)) != 0)
	{
		Verse.m_csVerse = csVerse;
		Verse.m_iPointSize = g_pDlg->GetPointSize();
		Verse.m_csFont = g_pDlg->GetCurrentFontName();
		Verse.m_iMaxPointSize = g_pDlg->GetMaxPointSize();
		g_pApp->m_vecVerses.push_back(Verse);
		SetItemCount(++m_nVerses);
		EnsureVisible(m_nVerses - 1,FALSE);

		// Redraw the settings view
		UpdateSettingsView();
	}
	else
	{
		CString csCaption = g_pApp->QBString(259);
		CString csMessage = g_pApp->QBString(226);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
	}
}

// Add the next set of verses in the sequence given using the highlighted verse to get the range
void CVerseList::AddNextVerse(int iVerse)
{
	if (m_nVerses)
	{
		CVerse LastVerse = g_pApp->m_vecVerses[iVerse];
		// Set the point size and font to the global setting
		LastVerse.m_iPointSize = g_pDlg->GetPointSize();
		LastVerse.m_csFont = g_pDlg->GetCurrentFontName();
		int nAmt = LastVerse.m_iVerseTo - LastVerse.m_iVerseFrom;
		bool bAdded = false;

AddNextVerse:

		bool bAddAgain = false;
		int iTranslation = LastVerse.m_iTranslation;
		int iBook = LastVerse.m_iBook;
		int iChapter = LastVerse.m_iChapter;

		int nbBooks = g_pApp->LookupNBBooks(iTranslation);
		int nbChapters = g_pApp->LookupNBChap(iTranslation,iBook);
		int nbVerses = g_pApp->LookupNBVerses(iTranslation,iBook,iChapter);

		int iFrom,iTo;
		iFrom = LastVerse.m_iVerseTo + 1;
		if (iFrom > nbVerses)
		{
			iFrom = 1;
			iChapter++;
			if (iChapter > nbChapters)
			{
				iChapter = 1;
				iBook++;
				if (iBook > nbBooks)
					iBook = 1;
			}
			nbVerses = g_pApp->LookupNBVerses(iTranslation,iBook,iChapter);
		}

		iTo = iFrom + nAmt;
		if (iTo > nbVerses)
		{
			iTo = nbVerses;
			bAddAgain = true;
		}

		CVerse NextVerse = LastVerse;
		NextVerse.m_iBook = iBook;
		NextVerse.m_iChapter = iChapter;
		NextVerse.m_iVerseFrom = iFrom;
		NextVerse.m_iVerseTo = iTo;
		NextVerse.LookupVerse();
		g_pApp->m_vecVerses.push_back(NextVerse);
		SetItemCount(++m_nVerses);
		EnsureVisible(m_nVerses - 1,FALSE);

		if (bAddAgain && !bAdded)
		{
			LastVerse = NextVerse;
			bAdded = true;
			goto AddNextVerse;
		}

		// Redraw the settings view
		UpdateSettingsView();
	}
	else
		AddNewVerse();
}

// Add a verse that has already been looked up
void CVerseList::AddExistingVerse(CVerse & Verse)
{
	g_pApp->m_vecVerses.push_back(Verse);
	SetItemCount(++m_nVerses);
}

void CVerseList::RemVerse(int iVerse)
{
	g_pApp->m_vecVerses.erase(g_pApp->m_vecVerses.begin() + iVerse);
	SetItemCount(--m_nVerses);
	RedrawWindow();

	// Redraw the settings view
	UpdateSettingsView();
}

void CVerseList::RemAll()
{
	g_pApp->m_vecVerses.clear();
	m_nVerses = 0;
	SetItemCount(m_nVerses);
	RedrawWindow();

	// Redraw the settings view
	UpdateSettingsView();
}

// Redraw the "Settings view" whenever a verse changes in the worksheet view
void CVerseList::UpdateSettingsView()
{
	int iMode = g_pDlg->m_Mode.GetCurSel();
	if (iMode == CProjectDlg::SETTINGSVIEW)
	{
		int nVerses = (int)g_pApp->m_vecVerses.size();
		g_pDlg->m_SettingsList.UpdateSettingsView(nVerses);
	}
}

int CVerseList::GetDropItem(CPoint point)
{
	int iItem = -1;
	try
	{
		// Dropping selected items back onto selected items causes an assertion with the HitTest method
		iItem = HitTest(point);
	}
	catch(...)
	{
		// Use the initial item as a starting point for the drop
		iItem = 0;
	}

	if (iItem == -1)
		iItem = m_nVerses;
	return iItem;
}

// Class for a custom drag and drop cursor
CVerseList::CDropSource::CDropSource()
{
	m_hCursor = g_pApp->LoadCursor(IDC_DRAGDROP);
}

CVerseList::CDropSource::~CDropSource()
{
	if (m_hCursor)
		DestroyCursor(m_hCursor);
}

SCODE CVerseList::CDropSource::GiveFeedback(DROPEFFECT de)
{
	// Use custom sort cursor for drop effect
	if (de & DROPEFFECT_MOVE)
	{
		::SetCursor(m_hCursor);
		return S_OK;
	}

	// Use standard OLE cursors for other drop effects (specified by COleDataSource construction)
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

/////////////////////////////////////////////////////////////////////////////
// CDropTarget message handlers

DROPEFFECT CVerseList::CDropTarget::OnDragEnter(CWnd *pWnd,COleDataObject *pDataObject,DWORD dwKeyState,CPoint point)
{
	DROPEFFECT de = GetDropEffect(pWnd, pDataObject, dwKeyState, point);
	return de;
}

DROPEFFECT CVerseList::CDropTarget::OnDragOver(CWnd *pWnd,COleDataObject *pDataObject,DWORD dwKeyState,CPoint point)
{
	return GetDropEffect(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CVerseList::CDropTarget::GetDropEffect(CWnd * pWnd,COleDataObject *pDataObject,DWORD dwKeyState,CPoint point)
{
	CVerseList * pVerseList = (CVerseList *)pWnd;
	DROPEFFECT de = DROPEFFECT_NONE;
	if (pDataObject->IsDataAvailable(pVerseList->m_cf))
		de = DROPEFFECT_MOVE;
	return de;
}

void CVerseList::CDropTarget::OnDragLeave(CWnd *pWnd)
{
	COleDropTarget::OnDragLeave(pWnd);
}

BOOL CVerseList::CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CVerseList * pVerseList = (CVerseList *)pWnd;

	int nCount;
	if (pDataObject->IsDataAvailable(pVerseList->m_cf))
	{
		// Get the drop position
		int iItem = pVerseList->GetDropItem(point);

		// Get the drop data
		HGLOBAL hGlobal = pDataObject->GetGlobalData(pVerseList->m_cf);
		int *pData = (int*)GlobalLock(hGlobal);
		nCount = *pData;

		vector<CVerse> vecOld;
		vector<CVerse> vecNew;

		vector<CVerse> & vecVerses = g_pApp->m_vecVerses;
		vector<CVerse>::iterator it = vecVerses.begin();
		int iVerse = 0,iCount = 1;
		bool bBack = false;
		for (;it != vecVerses.end();++it,++iVerse)
		{
			bool bAdd = false;
			if (nCount)
			{
				if (pData[iCount] == iVerse)
				{
					if (iVerse > iItem)
						bBack = true;

					// Move the dragged item from the source to the move list
					CVerse & DragVerse = vecVerses[iVerse];
					vecNew.push_back(DragVerse);
					nCount--;
					iCount++;

					// Remove the highlighting
					pVerseList->UnSelect(iVerse);
					bAdd = true;
				}
			}

			if (!bAdd)
			{
				CVerse & Verse = *it;
				vecOld.push_back(Verse);
			}
		}

		int iPos = 0;
		vecVerses.clear();
		vector<CVerse>::iterator itOld;
		for (itOld = vecOld.begin();itOld != vecOld.end();++itOld,++iPos)
		{
			if (bBack)
			{
				if (iPos == iItem)
					break;
			}
			else
			{
				if (iPos == (iItem - 1))
					break;
			}
			vecVerses.push_back(*itOld);
		}
		for (it = vecNew.begin();it != vecNew.end();++it)
		{
			vecVerses.push_back(*it);
			pVerseList->Toggle((int)vecVerses.size() - 1);
		}
		for (;itOld != vecOld.end();++itOld)
			vecVerses.push_back(*itOld);

		// Clean up
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);

		pVerseList->RedrawWindow();
	}
	return TRUE;
}

// Handle a rapid edit message from a child control
LRESULT CVerseList::OnRapidEdit(WPARAM wParam,LPARAM lParam)
{
	int iItem = LOWORD(wParam);
	int iSubItem = HIWORD(wParam);
	int WP = LOWORD(lParam);
	WORD wType = HIWORD(lParam);
	int iShift = LOBYTE(wType);
	int iType = HIBYTE(wType);

	// Check for the proper control character
	if (WP != VK_TAB && WP != VK_RETURN)
		return 0;

	// Check for moving to the next or previous control for rapid entry
	if (WP == VK_TAB)
	{
		if (!iShift)
		{
			iSubItem++;
			if (iSubItem == SCRIPTURE)
			{
				iSubItem = TRANSLATION;
				if (iItem == (GetItemCount() - 1))
					AddNextVerse(iItem);
				iItem++;
			}
		}
		else
		{
			iSubItem--;
			if (iSubItem == ITEM)
			{
				iSubItem = TOVERSE;
				iItem--;
				if (iItem < 0)
					iItem = GetItemCount() - 1;
			}
		}
	}
	else if (WP == VK_RETURN)
	{
		if (!iShift)
		{
			if (iItem == (GetItemCount() - 1))
				AddNextVerse(iItem);
			iItem++;
		}
		else
		{
			iItem--;
			if (iItem < 0)
				iItem = GetItemCount() - 1;
		}
	}

	// Start editing the next cell
	if (iType == 0)
		BeginEdit(iItem,iSubItem,false);
	else
		BeginCombo(iItem,iSubItem);

	return 1;
}

// UnSelect all the select items
void CVerseList::UnSelectAll()
{
	POSITION Pos = GetFirstSelectedItemPosition();
	if (Pos)
		do {UnSelect(GetNextSelectedItem(Pos));} while (Pos);
}

// UnSelect the item
void CVerseList::UnSelect(int iVerse)
{
	if (iVerse < (int)g_pApp->m_vecVerses.size())
		SetItemState(iVerse,UNSELECTED,MASK);
}

// Select the item
void CVerseList::Select(int iVerse)
{
	if (iVerse < (int)g_pApp->m_vecVerses.size())
		SetItemState(iVerse,SELECTED,MASK);
}

// Toggle the item
void CVerseList::Toggle(int iVerse)
{
	if (iVerse < (int)g_pApp->m_vecVerses.size())
	{
		BOOL bSelected = g_pApp->m_vecVerses[iVerse].m_bSelected;
		if (bSelected)
			UnSelect(iVerse);
		else
			Select(iVerse);
	}
}

// Toggle the items selected in the list
void CVerseList::Toggle(CListCtrl * pList)
{
	// Get the first selection
	POSITION Pos = pList->GetFirstSelectedItemPosition();
	if (Pos)
	{
		do
		{
			// Select the item that is selected
			Toggle(pList->GetNextSelectedItem(Pos));
		}
		while (Pos);
	}
}

// Get the position of the first selected verse
POSITION CVerseList::GetFirstSelectedItemPosition()
{
	return g_pApp->GetFirstSelectedItemPosition();
}

// Get the position of the next selected verse
int CVerseList::GetNextSelectedItem(POSITION & Pos)
{
	return g_pApp->GetNextSelectedItem(Pos);
}

// Set the selection property
BOOL CVerseList::SetItemState(int iVerse,int iSELECTED,int iMASK)
{
	return g_pApp->SetItemState(iVerse,iSELECTED);
}