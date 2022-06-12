// SettingsList.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "VerseList.h"
#include "GridComboBox.h"
#include "GridEdit.h"
#include "Verse.h"
#include "ProjectDlg.h"
#include "ImageSettings.h"
#include "ChooseFont.h"
#include "SettingsList.h"
#include <vector>
#include <cmath>

using namespace std;

extern CProjectApp * g_pApp;
static CProjectDlg * g_pDlg = NULL;

// CSettingsList

CSettingsList::CSettingsList() : m_bEdit(false), m_bRedraw(false)
{
}

CSettingsList::~CSettingsList()
{
}

BEGIN_MESSAGE_MAP(CSettingsList, CListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CSettingsList::OnClick)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, &CSettingsList::OnEndLabelEdit)
	ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, &CSettingsList::OnColorDisplay)
	ON_NOTIFY_REFLECT_EX(LVN_GETDISPINFO, &CSettingsList::OnUpdateDisplay)
	ON_MESSAGE(WM_BKGIMG, &CSettingsList::OnBkgImg)
	ON_MESSAGE(WM_TXTCOLOR, &CSettingsList::OnTxtColor)
	ON_MESSAGE(WM_BKGCOLOR, &CSettingsList::OnBkgColor)
	ON_MESSAGE(WM_CHOOSEFONT, &CSettingsList::OnChooseFont)
	ON_MESSAGE(WM_RAPIDEDIT, &CSettingsList::OnRapidEdit)
END_MESSAGE_MAP()

// CSettingsList message handlers
void CSettingsList::PreSubclassWindow()
{
	// Initialize the control
	CString acsHdr[] = {"","",g_pApp->QBString(203),g_pApp->QBString(204),g_pApp->QBString(205),g_pApp->QBString(206),g_pApp->QBString(207),g_pApp->QBString(208),g_pApp->QBString(209),g_pApp->QBString(210),g_pApp->QBString(211),g_pApp->QBString(212),g_pApp->QBString(301)};
	CString acsEx[] = {"","XX","XXXX","XXXX","XXXX","XXXX","XXXX","XXXXXXXXXXXXXXX","XXXX","XXXX","XXXX","XXXX","XXXX"};

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

	// Add a dummy column to prevent the imagelist width from being part of the output width of item 0
	InsertColumn(0,NULL,LVCFMT_LEFT,0,-1);

	// Insert the header columns, sized to the width of the data
	for (int iLoop = 1;iLoop < nb;++iLoop)
		InsertColumn(iLoop,m_acsHeader[iLoop],LVCFMT_LEFT,m_aiWidths[iLoop]);

	// Set the listview style for the variables list view
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);

	// Set the cell height
	int nHeight = GetSystemMetrics(SM_CYSMICON);
	m_ColorImageList.Create(1,nHeight, ILC_COLORDDB | ILC_MASK, 1, 0);

	// Assign the image list to the list-view control
	SetImageList(&m_ColorImageList,LVSIL_SMALL);
	SetImageList(&m_ColorImageList,LVSIL_NORMAL);

	// Initialize the item count
	SetItemCount(0);

	// Subclass the header control
	m_Header.SubclassWindow(GetHeaderCtrl()->m_hWnd);
	m_Header.SetColumnCount(nb);

	// Prevent the hidden, item and verse columns from being resized
	m_Header.SetColumnLock(HIDDEN,TRUE);
	m_Header.SetColumnLock(ITEM,TRUE);
	m_Header.SetColumnLock(VERSE,TRUE);

	// Get the project dialog
	g_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;

	// Finish subclassing the window
	CListCtrl::PreSubclassWindow();
}

// Create the image list with the solid, text, and background colors
void CSettingsList::SetImageColor()
{
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;
	if (vecVerses.empty())
		return;
	int nVerses = (int)vecVerses.size();

	// Delete the last image lists in preparation to set the new lists
	CImageList * pLastIL = GetImageList(LVSIL_SMALL);
	if (pLastIL)
		pLastIL->DeleteImageList();
	pLastIL = GetImageList(LVSIL_NORMAL);
	if (pLastIL)
		pLastIL->DeleteImageList();

	// Create a DC
	CDC DC;
	DC.CreateCompatibleDC(NULL);
	HDC hDC = (HDC)DC;

	// Set the size of the icon
	int nX = 32;
	int nY = GetSystemMetrics(SM_CYSMICON);

	// Create an image list for drawing the 3 color rectangles (solid, text, background)
	m_ColorImageList.Create(nX,nY,ILC_COLORDDB,0,3 * nVerses);

	// Initialize the bitmap
	CBitmap bmpColor,bmpMask;
	bmpColor.CreateBitmap(nX,nY,1,32,NULL);
	bmpMask.CreateBitmap(nX,nY,1,1,NULL);
	BITMAP bm,bmMask;
	bmpColor.GetObject(sizeof(BITMAP),&bm);
	bmpMask.GetObject(sizeof(BITMAP),&bmMask);

	// Create the main bitmap bits array
	std::auto_ptr<unsigned char> Data(new unsigned char [bm.bmHeight * bm.bmWidthBytes]);
	unsigned char * pData = Data.get();

	// Create the mask bits array
	std::auto_ptr<unsigned char> Mask(new unsigned char [bmMask.bmHeight * bmMask.bmWidthBytes]);
	unsigned char * pMask = Mask.get();
	memset(pMask,0,bmMask.bmHeight * bmMask.bmWidthBytes);

	// Get the bits per pixel
	int nBPP = GetDeviceCaps(hDC,BITSPIXEL);

	// Color all the bitmaps
	for (int iVerse = 0;iVerse < nVerses;++iVerse)
	{
		CVerse & verse = vecVerses[iVerse];
		COLORREF acr[] = {verse.m_bBkgSolid ? verse.m_crBkgSolid : RGB(255,255,255),verse.m_crTxt,verse.m_crBkg};

		for (int i = 0;i < 3;++i)
		{
			COLORREF cr = acr[i];
			CBitmap bmp;
			MakeDDB(bmp,nX,nY,cr);
			m_ColorImageList.Add(&bmp,(CBitmap*)NULL);
		}
	}

	// Assign the image list
	SetImageList(&m_ColorImageList,LVSIL_SMALL);
	SetImageList(&m_ColorImageList,LVSIL_NORMAL);
}

// Make the bitmap for the image list
void CSettingsList::MakeDDB(CBitmap & DDB,int nWidth,int nHeight,COLORREF cr)
{
	// Create a DIB
	CDIBFrame DIBFrame;
	DIBFrame.Init(nWidth,nHeight,false);
	CDC & DCDIB = DIBFrame;
	DCDIB.FillSolidRect(0,0,nWidth,nHeight,GetBkColor());
	DCDIB.FillSolidRect(8,2,nWidth - 8,nHeight - 4,cr);

	// Make a DDB compatible with the DC of the list control
	CPaintDC DCDDB(this);
	CBitmap * pDDB = &DDB;
	pDDB->CreateCompatibleBitmap(&DCDDB,nWidth,nHeight);
	CDC DCBmp;
	DCBmp.CreateCompatibleDC(&DCDDB);
	CBitmap * pLastDDB = DCBmp.SelectObject(pDDB);
	DCBmp.BitBlt(0,0,nWidth,nHeight,&DCDIB,0,0,SRCCOPY);
	DCBmp.SelectObject(pLastDDB);
}

// Set the text and background colors for the list items
BOOL CSettingsList::OnColorDisplay(NMHDR *pNMHDR,LRESULT *pResult)
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

		// Get the pointer to the main dialog
		std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;
		CVerse & verse = vecVerses[iItem];

		// Set the color
		if (iSubItem == BKGIMG)
		{
			if (verse.m_bBkgSolid)
				pListView->clrText = verse.m_crBkgSolid;
			else
				pListView->clrText = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
		}
		else if (iSubItem == TXTCOL)
			pListView->clrText = verse.m_crTxt;
		else if (iSubItem == BKGCOL)
			pListView->clrText = verse.m_crBkg;
		else
			pListView->clrText = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);

		// Set the background color
		pListView->clrTextBk = bSelected ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW);
		*pResult = CDRF_DODEFAULT;
	}
	else
		*pResult = CDRF_DODEFAULT;

	return TRUE;
}

// Display the per scripture settings
BOOL CSettingsList::OnUpdateDisplay(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LVITEM * pItem = &(pDispInfo)->item;
	*pResult = 0;
	
	// Get the pointer to the main dialog
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the item and subitem
	int iItem = pItem->iItem;
	int iSubItem = pItem->iSubItem;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Text records
	if (pItem->mask & LVIF_TEXT)
	{
		// Get the cell text
		if (iSubItem > ITEM)
		{
			CString csText;
			if (iSubItem == USE)
			{
				if (verse.m_bUseSettings)
					csText = g_pApp->QBString(213);
				else
					csText = g_pApp->QBString(214);
			}
			else if (iSubItem == VERSE)
			{
				// Format the verse display string
				csText = FormatVerse(verse);
			}
			else if (iSubItem == BKGIMG)
			{
				if (verse.m_bBkgOn || verse.m_bBkgVid)
				{
					if (verse.m_bBkgOn)
						csText = verse.m_csBkgImage;
					else
						csText = verse.m_csBkgVid;
					CString csDrive,csDir,csFname,csExt;
					char * pDrive = csDrive.GetBufferSetLength(_MAX_DRIVE);
					char * pDir = csDir.GetBufferSetLength(_MAX_DIR);
					char * pFname = csFname.GetBufferSetLength(_MAX_FNAME);
					char * pExt = csExt.GetBufferSetLength(_MAX_EXT);
					_splitpath(csText,pDrive,pDir,pFname,pExt);
					csDrive.ReleaseBuffer();
					csDir.ReleaseBuffer();
					csFname.ReleaseBuffer();
					csExt.ReleaseBuffer();
					csText.Format("%s%s",pFname,pExt);
				}
			}
			else if (iSubItem == FONT)
				csText = verse.m_csFont;
			else if (iSubItem == SIZE)
				csText.Format("%d",verse.m_iPointSize);
			else if (iSubItem == MAX)
				csText.Format("%d",verse.m_iMaxPointSize);
			else if (iSubItem == AUTO)
			{
				if (verse.m_bAutoSizeFont)
					csText = g_pApp->QBString(213);
				else
					csText = g_pApp->QBString(214);
			}
			else if (iSubItem == CENTER)
			{
				if (verse.m_bCenterJustify)
					csText = g_pApp->QBString(213);
				else
					csText = g_pApp->QBString(214);
			}
			else if (iSubItem == TRANSITION)
			{
				if (verse.m_bTransition)
					csText = g_pApp->QBString(213);
				else
					csText = g_pApp->QBString(214);
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
	
	if (pItem->mask & LVIF_IMAGE)
	{
		if (iSubItem == BKGIMG)
		{
			if (verse.m_bBkgSolid)
				pItem->iImage = iItem * 3;
		}
		if (iSubItem == TXTCOL)
			pItem->iImage = iItem * 3 + 1;
		else if (iSubItem == BKGCOL)
			pItem->iImage = iItem * 3 + 2;
	}

	return TRUE;
}

// Format the verse display string
CString CSettingsList::FormatVerse(CVerse & verse)
{
	CString csText;

	// Load the translation and book strings
	int iTranslation = verse.m_iTranslation;
	int iBook = verse.m_iBook;
	int iChapter = verse.m_iChapter;
	int iVerseFrom = verse.m_iVerseFrom;
	int iVerseTo = verse.m_iVerseTo;

	// Get the translation abbreviation and book name
	CString csTranslation = g_pApp->LookupTranslationAbbr(iTranslation);
	CString csBook = g_pApp->LookupBookName(iTranslation,iBook);

	// Format the book, chapter, and from verse
	csText.Format("%s - %s %d:%d",csTranslation,csBook,iChapter,iVerseFrom);
	if (iVerseFrom != iVerseTo)
	{
		// Format the to verse
		CString csToVerse;
		csToVerse.Format("-%d",iVerseTo);
		csText += csToVerse;
	}

	return csText;
}

// Update the settings view and resize the verse column
void CSettingsList::UpdateSettingsView(int nVerses)
{
	// Turn off redrawing
	SetRedraw(FALSE);

	// Set the new count of items
	SetItemCount(nVerses);

	// Autosize the new column widths
	int & nTotWidth = m_aiWidths[VERSE];
	int nExtra = GetStringWidth(m_acsExtra[VERSE]);
	nTotWidth = GetStringWidth(m_acsHeader[VERSE]) + nExtra;
	for (int iVerse = 0;iVerse < GetItemCount();++iVerse)
	{
		CString csVerse = GetItemText(iVerse,VERSE);
		int nWidth = GetStringWidth(csVerse) + nExtra;
		if (nWidth > nTotWidth)
			nTotWidth = nWidth;
	}
	SetColumnWidth(VERSE,m_aiWidths[VERSE]);
	EnsureVisible(nVerses - 1,FALSE);

	// Update the bitmaps
	SetImageColor();

	// Turn on redrawing
	SetRedraw(TRUE);

	// Redraw
	RedrawWindow();
}

// Edit the settings
BOOL CSettingsList::OnClick(NMHDR * pNMHDR, LRESULT * pResult)
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
		g_pDlg->ProcessClick(this,iItem,2);

		// Lookup this verse
		CVerse & verse = g_pApp->m_vecVerses[iItem];
		verse.LookupVerse();

		// Test for which subitem was clicked
		unsigned int iSubItem = pItem->iItem;
		if (iSubItem > ITEM && iSubItem < VERSE)
			BeginCombo(iItem,iSubItem);
		else if (iSubItem == VERSE)
		{
			g_pDlg->UpdateData();
			g_pApp->UpdateLastFrame(true);
		}
		else if (iSubItem > VERSE && iSubItem < SIZE)
		{
			if (iSubItem == BKGIMG)
				SendMessage(WM_BKGIMG,0,(LPARAM)iItem);
			else if (iSubItem == TXTCOL)
				SendMessage(WM_TXTCOLOR,0,(LPARAM)iItem);
			else if (iSubItem == BKGCOL)
				SendMessage(WM_BKGCOLOR,0,(LPARAM)iItem);
			else if (iSubItem == FONT)
				SendMessage(WM_CHOOSEFONT,0,(LPARAM)iItem);
		}
		else if (iSubItem > MAX)
			BeginCombo(iItem,iSubItem);
		else if (iSubItem == SIZE || iSubItem == MAX)
			BeginEdit(iItem,iSubItem,true);
	}
	else
		UnSelectAll();

	// Turn on redrawing
	SetRedraw();
	RedrawWindow();

	*pResult = 0;
	return TRUE;
}

void CSettingsList::BeginCombo(int iItem,int iSubItem)
{
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

	// Get the verse list
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Add the strings to the combo box
	if (iSubItem == USE || iSubItem == AUTO || iSubItem == CENTER || iSubItem == TRANSITION)
	{
		pCombo->AddString(g_pApp->QBString(213));
		pCombo->AddString(g_pApp->QBString(214));
		if (iSubItem == USE)
			pCombo->SetCurSel(verse.m_bUseSettings ? 0 : 1);
		else if (iSubItem == AUTO)
			pCombo->SetCurSel(verse.m_bAutoSizeFont ? 0 : 1);
		else if (iSubItem == CENTER)
			pCombo->SetCurSel(verse.m_bCenterJustify ? 0 : 1);
		else if (iSubItem == TRANSITION)
			pCombo->SetCurSel(verse.m_bTransition ? 0 : 1);
	}

	// Give the combo box focus
	pCombo->SetFocus();
}

// Start editing a subitem using an embedded edit control
void CSettingsList::BeginEdit(int iItem,int iSubItem,bool bReal)
{
	// Set the edit flag
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

	// Give the control focus
	pEdit->SetFocus();
}

// Update the verse with the edited setting
BOOL CSettingsList::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM * pItem = &(pDispInfo)->item;
	*pResult = 0;

	if (pItem->pszText != NULL)
	{
		// Turn off redrawing
		SetRedraw(FALSE);

		// Get the item and update structure
		int iItem = pItem->iItem;
		int iSubItem = pItem->iSubItem;

		// Get the pointer to the main dialog
		std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

		// Get the verse
		CVerse & verse = vecVerses[iItem];

		// Get the selection
		int iCurSel = atoi(pItem->pszText);

		// Adjust for the combo box
		if (!m_bEdit)
			iCurSel++;

		bool bChange = false;
		if (iSubItem == USE)
			verse.m_bUseSettings = iCurSel == 1 ? TRUE : FALSE;
		else if (iSubItem == SIZE)
		{
			if (iCurSel >= g_pDlg->m_iPointSizeMin && iCurSel <= g_pDlg->m_iPointSizeMax)
			{
				if (verse.m_iPointSize != iCurSel)
				{
					// Set the point size
					verse.m_iPointSize = iCurSel;

					// Made a change, must want to use per verse settings
					bChange = true;
				}
			}
		}
		else if (iSubItem == MAX)
		{
			if (iCurSel >= g_pDlg->m_iPointSizeMin && iCurSel <= g_pDlg->m_iPointSizeMax)
			{
				if (verse.m_iMaxPointSize != iCurSel)
				{
					// Set the max point size
					verse.m_iMaxPointSize = iCurSel;

					// Made a change, must want to use per verse settings
					bChange = true;
				}
			}
		}
		else if (iSubItem == AUTO)
		{
			BOOL bAutoSizeFont = iCurSel == 1 ? TRUE : FALSE;
			if (bAutoSizeFont != verse.m_bAutoSizeFont)
			{
				// Set the autosize font
				verse.m_bAutoSizeFont = bAutoSizeFont;

				// Made a change, must want to use per verse settings
				bChange = true;
			}
		}
		else if (iSubItem == CENTER)
		{
			BOOL bCenterJustify = iCurSel == 1 ? TRUE : FALSE;
			if (bCenterJustify != verse.m_bCenterJustify)
			{
				// Set the center justify
				verse.m_bCenterJustify = iCurSel == 1 ? TRUE : FALSE;

				// Made a change, must want to use per verse settings
				bChange = true;
			}
		}
		else if (iSubItem == TRANSITION)
		{
			BOOL bTransition = iCurSel == 1 ? TRUE : FALSE;
			if (bTransition != verse.m_bTransition)
			{
				// Set the transition
				verse.m_bTransition = iCurSel == 1 ? TRUE : FALSE;

				// Made a change, must want to use per verse settings
				bChange = true;
			}
		}

		// Made a change, must want to use per verse settings
		if (bChange)
		{
			// Turn on per verse settings
			verse.m_bUseSettings = TRUE;

			// Update the other selections
			UpdateOtherVerse(verse);

			// Redraw the verse
			RedrawWindow();
		}

		// Turn on redrawing
		SetRedraw();
	}

	return FALSE;
}

// Handle the custom WM_APP message to edit the background image of a verse
LRESULT CSettingsList::OnBkgImg(WPARAM wParam,LPARAM lParam)
{
	// Get the verse index
	int iItem = (int)lParam;

	// Get the pointer to the main dialog
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Edit the background image
	CImageSettings dlg(verse.m_bBkgCapture,verse.m_csBkgVid,verse.m_csBkgImage,verse.m_bBkgMix,verse.m_iMixAmt,verse.m_crBkgSolid,verse.m_bTransition,verse.m_nTransStep);
	INT_PTR iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		// Get the background capture setting
		verse.m_bBkgCapture = dlg.m_bBkgCapture;

		// Get the background video
		verse.m_bBkgVid = dlg.m_bBkgVideo;
		if (verse.m_bBkgVid)
			verse.m_csBkgVid = dlg.m_csBkgVideo;
		else
			verse.m_csBkgVid.Empty();

		// Get the background image
		verse.m_bBkgOn = dlg.m_bBkgOn;
		if (verse.m_bBkgOn)
			verse.m_csBkgImage = dlg.m_csBkgImage;
		else
			verse.m_csBkgImage.Empty();

		// Get the background video
		verse.m_bBkgVid = dlg.m_bBkgVideo;
		if (verse.m_bBkgVid)
			verse.m_csBkgVid = dlg.m_csBkgVideo;
		else
			verse.m_csBkgVid.Empty();

		// Get the background mix
		verse.m_bBkgMix = dlg.m_bBkgMix;
		if (verse.m_bBkgMix)
			verse.m_iMixAmt = dlg.m_iMixAmt;

		// Get the background color
		verse.m_bBkgSolid = dlg.m_bBkgSolid;
		if (verse.m_bBkgSolid)
		{
			// Update the solid color
			verse.m_crBkgSolid = dlg.m_crColor;

			// Update the bitmap
			SetImageColor();
		}

		// Get the transition effect
		verse.m_bTransition = dlg.m_bTransition;
		if (verse.m_bTransition)
			verse.m_nTransStep = dlg.m_iStepAmt;

		// Made a change, must want to use per verse settings
		verse.m_bUseSettings = TRUE;

		// Update the other selections
		UpdateOtherVerse(verse);
	}
	RedrawWindow();
	return iRet;
}

// Handle the custom WM_APP message to edit the text color of a verse
LRESULT CSettingsList::OnTxtColor(WPARAM wParam,LPARAM lParam)
{
	// Get the verse index
	int iItem = (int)lParam;

	// Get the pointer to the main dialog
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Edit the text color
	CColorDialog dlg;
	INT_PTR iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		// Get the new text color
		verse.m_crTxt = dlg.GetColor();

		// Update the bitmap
		SetImageColor();

		// Made a change, must want to use per verse settings
		verse.m_bUseSettings = TRUE;

		// Update the other selections
		UpdateOtherVerse(verse);
	}
	RedrawWindow();
	return iRet;
}

// Handle the custom WM_APP message to edit the background color of a verse
LRESULT CSettingsList::OnBkgColor(WPARAM wParam,LPARAM lParam)
{
	// Get the verse index
	int iItem = (int)lParam;

	// Get the pointer to the main dialog
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Edit the background color
	CColorDialog dlg;
	INT_PTR iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		// Get the new background color
		verse.m_crBkg = dlg.GetColor();

		// Update the bitmap
		SetImageColor();

		// Made a change, must want to use per verse settings
		verse.m_bUseSettings = TRUE;

		// Update the other selections
		UpdateOtherVerse(verse);
	}
	RedrawWindow();
	return iRet;
}

// Handle the custom WM_CHOOSEFONT message to change the font of a verse
LRESULT CSettingsList::OnChooseFont(WPARAM wParam,LPARAM lParam)
{
	// Get the verse index
	int iItem = (int)lParam;

	// Get the pointer to the main dialog
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Get the verse
	CVerse & verse = vecVerses[iItem];

	// Choose the font
	CChooseFont dlg(g_pDlg->m_csaFont,verse.m_csFont,verse.m_iPointSize,g_pDlg->m_iPointSizeMin,g_pDlg->m_iPointSizeMax);
	INT_PTR iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		// Get the font
		BOOL bChange = FALSE;
		CString csFont = dlg.GetFont();

		// If it is a new font then autosize
		if (csFont != verse.m_csFont)
		{
			// Update the font
			verse.m_csFont = csFont;
			verse.m_iPointSize = dlg.GetPointSize();

			// Made a change, must want to use per verse settings
			bChange = TRUE;

			// Autosize
			if (g_pDlg->m_bAskCheckUpdatePointSize)
			{
				int iMinPointSize = verse.m_iPointSize;
				int nTranslations = g_pApp->LookupNBTrans();
				for (int iTranslation = 1;iTranslation <= nTranslations;++iTranslation)
				{
					if (g_pApp->IsLoaded(iTranslation))
					{
						CVerse LongestVerse = g_pApp->LookupLongestVerse(iTranslation);
						LongestVerse.m_bUseSettings = true;
						LongestVerse.m_iPointSize = verse.m_iPointSize;
						LongestVerse.m_csFont = verse.m_csFont;
						int iPointSize = g_pDlg->DetectFontSize(&LongestVerse);
						if (iPointSize >= g_pDlg->m_iPointSizeMin && iPointSize < iMinPointSize)
						{
							CString csMinPointSizeBible = g_pApp->LookupTranslationName(iTranslation);
							CString csFontFormat = g_pApp->QBString(278);
							CString csFontMessage;
							csFontMessage.Format(csFontFormat,csMinPointSizeBible,iPointSize);
							CString csCaption = g_pApp->QBString(259);
							CMessageBox MsgBox(csCaption,csFontMessage,CMessageBox::YESNOCANCEL);
							INT_PTR iChoice = MsgBox.DoModal();
							if (iChoice == IDOK)
								verse.m_iPointSize = iPointSize;
							else if (iChoice == IDCANCEL)
								g_pDlg->m_bAskCheckUpdatePointSize = false;
						}
					}
				}
			}
		}

		// Made a change, must want to use per verse settings
		verse.m_bUseSettings = bChange;

		if (bChange)
		{
			// Update the other selections
			UpdateOtherVerse(verse);
		}
	}

	RedrawWindow();
	return iRet;
}

// Handle a rapid edit message from a child control
LRESULT CSettingsList::OnRapidEdit(WPARAM wParam,LPARAM lParam)
{
	int iItem = LOWORD(wParam);
	int iSubItem = HIWORD(wParam);
	int WP = LOWORD(lParam);
	WORD wType = HIWORD(lParam);
	int iShift = LOBYTE(wType);
	int iType = HIBYTE(wType);

	return 1;
}

// Test for multi-selection after edit and copy over the other verses with this verse
void CSettingsList::UpdateOtherVerse(CVerse & Verse)
{
	// Get the list of verses
	std::vector<CVerse> & vecVerses = g_pApp->m_vecVerses;

	// Select the settings verse
	POSITION Pos = g_pDlg->m_VerseList.GetFirstSelectedItemPosition();
	if (Pos)
	{
		// Set the focus to the list control
		SetFocus();
		
		// Select the verses
		int iVerse = 0;
		do
		{
			// Get the selected item in the verse list
			iVerse = g_pDlg->m_VerseList.GetNextSelectedItem(Pos);

			// Update the selected verse with the all the settings, including new setting
			vecVerses[iVerse] = Verse;

			// Select the item in the settings list
			Toggle(iVerse);
		}
		while (Pos);
	}
}

// UnSelect all the select items
void CSettingsList::UnSelectAll()
{
	POSITION Pos = GetFirstSelectedItemPosition();
	if (Pos)
		do {UnSelect(GetNextSelectedItem(Pos));} while (Pos);
}

// UnSelect the item
void CSettingsList::UnSelect(int iVerse)
{
	if (iVerse < (int)g_pApp->m_vecVerses.size())
		SetItemState(iVerse,UNSELECTED,MASK);
}

// Select the item
void CSettingsList::Select(int iVerse)
{
	if (iVerse < (int)g_pApp->m_vecVerses.size())
		SetItemState(iVerse,SELECTED,MASK);
}

// Toggle the item
void CSettingsList::Toggle(int iVerse)
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
void CSettingsList::Toggle(CListCtrl * pList)
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
POSITION CSettingsList::GetFirstSelectedItemPosition()
{
	return g_pApp->GetFirstSelectedItemPosition();
}

// Get the position of the next selected verse
int CSettingsList::GetNextSelectedItem(POSITION & Pos)
{
	return g_pApp->GetNextSelectedItem(Pos);
}

// Set the selection property
BOOL CSettingsList::SetItemState(int iVerse,int iSELECTED,int iMASK)
{
	return g_pApp->SetItemState(iVerse,iSELECTED);
}