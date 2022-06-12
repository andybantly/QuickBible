// ManageContent.cpp : implementation file
//

#include "stdafx.h"
#if !defined(LITE) && !defined(PERSONAL) && !defined(BASIC)
#include "Project.h"
#include "ManageContentDlg.h"
#include "AC.h"

// CManageContentDlg dialog

extern CProjectApp * g_pApp;

IMPLEMENT_DYNAMIC(CManageContentDlg, CDialog)

CManageContentDlg::CManageContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManageContentDlg::IDD, pParent)
{
	m_hIcon = g_pApp->LoadIcon(IDR_MAINFRAME);
	m_pParent = pParent;
	m_nID = CManageContentDlg::IDD;
	m_Books.m_iID = IDC_BOOKS;
	m_Chapters.m_iID = IDC_CHAPTERS;
	m_Verses.m_iID = IDC_VERSES;
	m_bInit = true;
}

CManageContentDlg::~CManageContentDlg()
{
}

BOOL CManageContentDlg::Create()
{
	return CDialog::Create(m_nID,m_pParent);
}

void CManageContentDlg::Destroy()
{
	DestroyWindow();
}

void CManageContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BOOKS, m_Books);
	DDX_Control(pDX, IDC_CHAPTERS, m_Chapters);
	DDX_Control(pDX, IDC_VERSES, m_Verses);
	DDX_Control(pDX, IDC_EDIT_VERSETEXT, m_VerseText);
	DDX_Text(pDX, IDC_EDIT_VERSETEXT, m_csVerseText);
}

void CManageContentDlg::PostNcDestroy()
{
	delete this;
}

BOOL CManageContentDlg::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	// Set the pointer to the main window
	m_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;

	// Set the icon
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// Set the caption
	SetWindowText(g_pApp->QBString(327));

	// Set the group control text
	GetDlgItem(IDC_STATIC_EDITCONTENT)->SetWindowText(g_pApp->QBString(337));
	GetDlgItem(IDC_STATIC_EDITVERSETEXT)->SetWindowText(g_pApp->QBString(344));

	// Create the buttons
	m_AddBook.CreateBtmp(this,IDB_ADD,IDC_ADD_BOOK,g_pApp->QBString(338));
	m_EditBook.CreateBtmp(this,IDB_EDIT,IDC_EDIT_BOOK,g_pApp->QBString(339));
	m_DeleteBook.CreateBtmp(this,IDB_DELETE,IDC_DELETE_BOOK,g_pApp->QBString(340));
	m_AddChapter.CreateBtmp(this,IDB_ADD,IDC_ADD_CHAPTER,g_pApp->QBString(338));
	m_EditChapter.CreateBtmp(this,IDB_EDIT,IDC_EDIT_CHAPTER,g_pApp->QBString(339));
	m_DeleteChapter.CreateBtmp(this,IDB_DELETE,IDC_DELETE_CHAPTER,g_pApp->QBString(340));
	m_AddVerse.CreateBtmp(this,IDB_ADD,IDC_ADD_VERSE,g_pApp->QBString(338));
	m_EditVerse.CreateBtmp(this,IDB_EDIT,IDC_EDIT_VERSE,g_pApp->QBString(339));
	m_DeleteVerse.CreateBtmp(this,IDB_DELETE,IDC_DELETE_VERSE,g_pApp->QBString(340));
	m_UpdateVerseText.CreateText(this,IDC_UPDATEVERSETEXT,g_pApp->QBString(345));
	m_Save.CreateText(this,IDC_SAVE,g_pApp->QBString(229));

	return bRet;
}

// Initialize
void CManageContentDlg::Init()
{
/*
	// Create V2 of the bibles
	int nBibles = (int)g_pApp->m_vBibles.size();
	for (int iBible = 0;iBible < nBibles;++iBible)
	{
		CBible & Bible = g_pApp->m_vBibles[iBible];
		g_pApp->SaveTranslation(Bible);
	}

	// Create the custom verse
	{
		CBible & Bible = g_pApp->m_vBibles[0];
		g_pApp->SaveTranslation(Bible);
	}
*/

	// Lookup the custom translation index
	CString csTranslation = "Custom";
	m_iTranslation = g_pApp->LookupTranslationIndex(csTranslation);

	// Set the translation to the lists
	m_Books.m_iTranslation = m_iTranslation;
	m_Chapters.m_iTranslation = m_iTranslation;
	m_Verses.m_iTranslation = m_iTranslation;

	// Select the initial items
	SelectItems(0,0,0);

	// Set the focus to the book list
	m_Books.SetFocus();
}

// Select items in the list
void CManageContentDlg::SelectItems(int iBook,int iChapter,int iVerse)
{
	// Load the custom translation
	CBible & Bible = g_pApp->m_vBibles[m_iTranslation - 1];
	if (!Bible.m_bLoaded)
		if (!g_pApp->LoadTranslation(m_iTranslation))
			return;

	// Initialize the verse
	m_csVerseText = "";
	BOOL bBook = FALSE,bChapter = FALSE,bVerse = FALSE;

	// Get the custom content "bible"
	CDBBible DBBible;
	BOOL bBible = g_pApp->GetBible(m_iTranslation,DBBible);
	if (bBible)
	{
		// Get the number of books
		int nBooks = (int)DBBible.m_vBook.size();
		m_Books.SetItemCount(nBooks);

		if (nBooks)
		{
			// Build/Set the book list
			std::vector<CString> vecBookList;
			for (int idx = 1;idx <= nBooks;++idx)
			{
				CString csValue = g_pApp->LookupBookName(m_iTranslation,idx);
				vecBookList.push_back(csValue);
			}
			m_Books.SetList(vecBookList);

			// Select the book
			if (iBook != -1)
			{
				bBook = TRUE;
				m_Books.SelectItem(iBook);

				// Chapters
				CDBBook & DBBook = DBBible.m_vBook[iBook];
				int nChapters = (int)DBBook.m_vChap.size();
				m_Chapters.SetItemCount(nChapters);
				if (nChapters)
				{
					// Build/Set the chapter list
					std::vector<CString> vecChapterList;
					for (int idx = 1;idx <= nChapters;++idx)
					{
						CString csValue = g_pApp->LookupChapterName(m_iTranslation,iBook + 1,idx);
						vecChapterList.push_back(csValue);
					}
					m_Chapters.SetList(vecChapterList);

					// Select the chapter
					if (iChapter != -1)
					{
						bChapter = TRUE;
						m_Chapters.SelectItem(iChapter);

						// Verses
						CDBChap & DBChap = DBBook.m_vChap[iChapter];
						int nVerses = (int)DBChap.m_vVerse.size();
						m_Verses.SetItemCount(nVerses);
						if (nVerses)
						{
							// Build/Set the verse list
							std::vector<CString> vecVerseList;
							for (int idx = 1;idx <= nVerses;++idx)
							{
								CString csValue = g_pApp->LookupVerseName(m_iTranslation,iBook + 1,iChapter + 1,idx);
								vecVerseList.push_back(csValue);
							}
							m_Verses.SetList(vecVerseList);

							// Select the verse
							if (iVerse != -1)
							{
								bVerse = TRUE;
								m_Verses.SelectItem(iVerse);

								// Get the verse
								m_csVerseText = g_pApp->LookupDB(m_iTranslation,iBook + 1,iChapter + 1,iVerse + 1,iVerse + 1);
							}
						}
					}
				}
				else
					m_Verses.SetItemCount(0);
			}
		}
	}

	// Enable the update feature if there is text available
	m_VerseText.EnableWindow(bBook && bChapter && bVerse);

	// Enable the update feature if there is text available
	m_UpdateVerseText.EnableWindow(!m_csVerseText.IsEmpty());

	// Update the verse text
	UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CManageContentDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDCANCEL, &CManageContentDlg::OnClose)
	ON_BN_CLICKED(IDC_ADD_BOOK, &CManageContentDlg::OnAddBook)
	ON_BN_CLICKED(IDC_EDIT_BOOK, &CManageContentDlg::OnEditBook)
	ON_BN_CLICKED(IDC_DELETE_BOOK, &CManageContentDlg::OnDeleteBook)
	ON_BN_CLICKED(IDC_ADD_CHAPTER, &CManageContentDlg::OnAddChapter)
	ON_BN_CLICKED(IDC_EDIT_CHAPTER, &CManageContentDlg::OnEditChapter)
	ON_BN_CLICKED(IDC_DELETE_CHAPTER, &CManageContentDlg::OnDeleteChapter)
	ON_BN_CLICKED(IDC_ADD_VERSE, &CManageContentDlg::OnAddVerse)
	ON_BN_CLICKED(IDC_EDIT_VERSE, &CManageContentDlg::OnEditVerse)
	ON_BN_CLICKED(IDC_DELETE_VERSE, &CManageContentDlg::OnDeleteVerse)
	ON_EN_CHANGE(IDC_EDIT_VERSETEXT, &CManageContentDlg::OnChangeEditVerseText)
	ON_BN_CLICKED(IDC_UPDATEVERSETEXT, &CManageContentDlg::OnUpdateEditVerseText)
	ON_BN_CLICKED(IDC_SAVE, &CManageContentDlg::OnClickedSave)
	ON_NOTIFY(NM_CLICK, IDC_BOOKS, &CManageContentDlg::OnClickBooks)
	ON_NOTIFY(NM_CLICK, IDC_CHAPTERS, &CManageContentDlg::OnClickChapters)
	ON_NOTIFY(NM_CLICK, IDC_VERSES, &CManageContentDlg::OnClickVerses)
END_MESSAGE_MAP()

// Event to show or hide the dialog
void CManageContentDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Test for showing the window
	if (bShow)
	{
		// Test for initializing
		if (m_bInit)
		{
			// Initialize the window for the first time
			Init();
			m_bInit = false;
		}
	}
}

// CManageContentDlg message handlers
void CManageContentDlg::OnClose()
{
	ShowWindow(SW_HIDE);
}

// Add a new book
void CManageContentDlg::OnAddBook()
{
	// Use the edit messagebox for simple input
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(333);
	CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		// Test for a good book
		CString csBook = MsgBox.GetValue();
		if (csBook.IsEmpty())
		{
			csMessage = g_pApp->QBString(336);
			CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
			BadEntry.DoModal();
			return;
		}

		// Add a new book
		if (!g_pApp->AddBookName(m_iTranslation,csBook))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
			SelectItems(0,0,0);
		}
		else
			// Select the new book
			SelectItems(m_Books.GetItemCount(),0,0);
	}
}

// Change the name of a book
void CManageContentDlg::OnEditBook()
{
	// Get the messagebox caption
	CString csCaption = g_pApp->QBString(259);

	// Get the current book name
	CString csCurrentBook = m_Books.GetItemText(m_Books.GetSelectedItem(),CContentList::VALUE);
	if (csCurrentBook.IsEmpty())
	{
		CString csMessage = g_pApp->QBString(347);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
	}
	else
	{
		// Use the edit messagebox for simple input
		CString csMessage = g_pApp->QBString(333);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

		// Get the result
		INT_PTR Ret = MsgBox.DoModal();
		if (Ret == IDOK)
		{
			// Test for a good book
			CString csBook = MsgBox.GetValue();
			if (csBook.IsEmpty())
			{
				csMessage = g_pApp->QBString(336);
				CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
				BadEntry.DoModal();
			}
			else
			{
				// Get the current books index
				int iCurrentBook = g_pApp->LookupBookIndex(m_iTranslation,csCurrentBook);
				if (iCurrentBook)
				{
					// Rename the book, by index, to the new name
					g_pApp->RenameBookIndex(m_iTranslation,iCurrentBook,csBook);

					// Select the book
					SelectItems(iCurrentBook - 1);
				}
			}
		}
	}
}

// Delete a book
void CManageContentDlg::OnDeleteBook()
{
	// Ask the user if they really want to delete the book
	CString csCaption = g_pApp->QBString(259);
	CString csMessage,csFormat = g_pApp->QBString(341);
	int iBook = m_Books.GetSelectedItem();
	CString csBook = m_Books.GetItemText(iBook,CContentList::VALUE);
	csMessage.Format(csFormat,csBook);
	CMessageBox MsgBox(csCaption,csMessage);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		if (!g_pApp->DeleteBookIndex(m_iTranslation,iBook + 1))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
		}
		SelectItems(0,0,0);
	}
}

// Add a new chapter to a book
void CManageContentDlg::OnAddChapter()
{
	// Use the edit messagebox for simple input
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(334);
	CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		// Test for a good chapter
		CString csChapter = MsgBox.GetValue();
		if (csChapter.IsEmpty())
		{
			csMessage = g_pApp->QBString(336);
			CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
			BadEntry.DoModal();
			return;
		}

		// Get the book associated with the item
		int iBook = m_Books.GetSelectedItem();
		CString csBook = m_Books.GetItemText(iBook,CContentList::VALUE);

		// Add a new chapter
		if (!g_pApp->AddChapterName(m_iTranslation,iBook + 1,csChapter))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
			SelectItems(0,0,0);
		}
		else
			// Select the new chapter
			SelectItems(iBook,m_Chapters.GetItemCount(),0);
	}
}

// Rename a chapter
void CManageContentDlg::OnEditChapter()
{
	// Use the edit messagebox for simple input
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(334);
	CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		// Test for a good chapter
		CString csChapter = MsgBox.GetValue();
		if (csChapter.IsEmpty())
		{
			csMessage = g_pApp->QBString(336);
			CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
			BadEntry.DoModal();
			return;
		}

		// Get the book and chapter associated with the item
		int iBook = m_Books.GetSelectedItem();
		int iChapter = m_Chapters.GetSelectedItem();

		// Add a new chapter
		if (!g_pApp->RenameChapterIndex(m_iTranslation,iBook + 1,iChapter + 1,csChapter))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
			SelectItems(0,0,0);
		}
		else
			// Select the chapter
			SelectItems(iBook,iChapter,0);
	}
}

// Delete a chapter
void CManageContentDlg::OnDeleteChapter()
{
	// Ask the user if they really want to delete the chapter
	CString csCaption = g_pApp->QBString(259);
	CString csMessage,csFormat = g_pApp->QBString(342);
	int iBook = m_Books.GetSelectedItem();
	int iChapter = m_Chapters.GetSelectedItem();
	CString csChapter = m_Chapters.GetItemText(iChapter,CContentList::VALUE);
	csMessage.Format(csFormat,csChapter);
	CMessageBox MsgBox(csCaption,csMessage);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		if (!g_pApp->DeleteChapterIndex(m_iTranslation,iBook + 1,iChapter + 1))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
		}
		SelectItems(0,0,0);
	}
}

// Add a verse
void CManageContentDlg::OnAddVerse()
{
	// Use the edit messagebox for simple input
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(335);
	CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		// Test for a good chapter
		CString csVerse = MsgBox.GetValue();
		if (csVerse.IsEmpty())
		{
			csMessage = g_pApp->QBString(336);
			CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
			BadEntry.DoModal();
			return;
		}

		// Get the book and chapter associated with the item
		int iBook = m_Books.GetSelectedItem();
		int iChapter = m_Chapters.GetSelectedItem();

		// Add a new verse
		if (!g_pApp->AddVerseName(m_iTranslation,iBook + 1,iChapter + 1,csVerse))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
			SelectItems(0,0,0);
		}
		else
			// Select the verse
			SelectItems(iBook,iChapter,m_Verses.GetItemCount());
	}
}

// Rename the verse
void CManageContentDlg::OnEditVerse()
{
	// Use the edit messagebox for simple input
	CString csCaption = g_pApp->QBString(259);
	CString csMessage = g_pApp->QBString(335);
	CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OKCANCELEDIT);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		// Test for a good verse
		CString csVerse = MsgBox.GetValue();
		if (csVerse.IsEmpty())
		{
			csMessage = g_pApp->QBString(336);
			CMessageBox BadEntry(csCaption,csMessage,CMessageBox::OK);
			BadEntry.DoModal();
			return;
		}

		// Get the book,chapter, and verse associated with the item
		int iBook = m_Books.GetSelectedItem();
		int iChapter = m_Chapters.GetSelectedItem();
		int iVerse = m_Verses.GetSelectedItem();

		// Add a new verse
		if (!g_pApp->RenameVerseIndex(m_iTranslation,iBook + 1,iChapter + 1,iVerse + 1,csVerse))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
			SelectItems(0,0,0);
		}
		else
			// Select the verse
			SelectItems(iBook,iChapter,iVerse);
	}
}

// Delete the verse
void CManageContentDlg::OnDeleteVerse()
{
	// Ask the user if they really want to delete the verse
	CString csCaption = g_pApp->QBString(259);
	CString csMessage,csFormat = g_pApp->QBString(343);
	int iBook = m_Books.GetSelectedItem();
	int iChapter = m_Chapters.GetSelectedItem();
	int iVerse = m_Verses.GetSelectedItem();
	CString csVerse = m_Verses.GetItemText(iVerse,CContentList::VALUE);
	csMessage.Format(csFormat,csVerse);
	CMessageBox MsgBox(csCaption,csMessage);

	// Get the result
	INT_PTR Ret = MsgBox.DoModal();
	if (Ret == IDOK)
	{
		if (!g_pApp->DeleteVerseIndex(m_iTranslation,iBook + 1,iChapter + 1,iVerse + 1))
		{
			// Error
			CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
		}
		SelectItems(0,0,0);
	}
}

// User typed in the verse box
void CManageContentDlg::OnChangeEditVerseText()
{
	UpdateData();

	// Enable the update feature if there is text available
	m_UpdateVerseText.EnableWindow(!m_csVerseText.IsEmpty());
}

// User clicked update
void CManageContentDlg::OnUpdateEditVerseText()
{
	// Get the book,chapter, and verse associated with the item
	int iBook = m_Books.GetSelectedItem();
	int iChapter = m_Chapters.GetSelectedItem();
	int iVerse = m_Verses.GetSelectedItem();

	// Update the verse text
	if (!g_pApp->UpdateVerseIndex(m_iTranslation,iBook + 1,iChapter + 1,iVerse + 1,m_csVerseText))
	{
		// Error
		CString csCaption = g_pApp->QBString(259);
		CMessageBox ErrMsg(csCaption,g_pApp->QBString(347));
		SelectItems(0,0,0);
	}
	else
		// Select the verse
		SelectItems(iBook,iChapter,iVerse);
}

// User wants to save the changes to the custom content
void CManageContentDlg::OnClickedSave()
{
	CMessageBox Msg("Custom Content","Save the current custom content?");
	INT_PTR iRet = Msg.DoModal();
	if (iRet == IDOK)
	{
		CBible & Bible = g_pApp->m_vBibles[m_iTranslation - 1];
		g_pApp->SaveTranslation(Bible);
	}
}

void CManageContentDlg::OnClickBooks(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	// Get the current selected book
	const int iBook = m_Books.GetSelectedItem();
	if (iBook == -1)
		return;

	// Select the book
	SelectItems(iBook,0,0);
}

void CManageContentDlg::OnClickChapters(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	// Get the current selected book
	const int iBook = m_Books.GetSelectedItem();
	if (iBook == -1)
		return;

	// Get the current selected chapter
	const int iChapter = m_Chapters.GetSelectedItem();
	if (iChapter == -1)
		return;

	// Select the book and chapter
	SelectItems(iBook,iChapter,0);
}

void CManageContentDlg::OnClickVerses(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	// Get the current selected book
	const int iBook = m_Books.GetSelectedItem();
	if (iBook == -1)
		return;

	// Get the current selected chapter
	const int iChapter = m_Chapters.GetSelectedItem();
	if (iChapter == -1)
		return;

	// Get the current selected verse
	const int iVerse = m_Verses.GetSelectedItem();
	if (iVerse == -1)
		return;

	// Select the book, chapter, and verse
	SelectItems(iBook,iChapter,iVerse);
}

#endif