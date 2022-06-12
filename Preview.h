// Preview.h : Preview window prototype file
// Copyright (C) 2007 Andrew S. Bantly

#pragma once
#include "ProjectDlg.h"
#include "Verse.h"
#include "CaptureThread.h"
#include "afxmt.h"
#include <map>
#include "resource.h"

// CPreview dialog : prototype for a modeless preview window
class CPreviewDlg : public CDialog
{
	friend class CFullWnd;
	friend class CProjectDlg;

	DECLARE_DYNAMIC(CPreviewDlg)

public:
	CPreviewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreviewDlg();

	// Methods for creating and destroying the modeless dialog
	BOOL Create();
	void Destroy();
	void SetupPreview();
	void CreateTheFont(int y,CFont & HorzFont);
	void Render(int x = -1,int y = -1);
	int GetPtSize();
	int XFromY(LPRECT lpRect,double dC = 1.33333333);
	int XFromY(int iY,double dC = 1.33333333);

// Dialog Data
	enum { IDD = IDD_PREVIEW };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnCancel();

protected:
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnSizing(UINT nSide,LPRECT lpRect);
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	afx_msg void OnPaint();

public:
	static void CALLBACK FullTimeProc(UINT uID,UINT uMsg,DWORD_PTR dwUser,DWORD_PTR dw1,DWORD_PTR dw2);
	static void CALLBACK PrevTimeProc(UINT uID,UINT uMsg,DWORD_PTR dwUser,DWORD_PTR dw1,DWORD_PTR dw2);

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bSearchPreview;
	CVerse m_SearchVerse;
	CVerse * m_pSizeVerse;
	BOOL m_bBkgVid;

protected:
	HICON m_hIcon;
	CWnd * m_pParent;
	int m_nID;
	CRect m_DialogRect;
	CPoint m_ptMinTrack;
	CPoint m_ptMaxTrack;
	bool m_bInitDialog;
	int m_nCaptionHeight;
	int m_nFrameHeight;
	int m_nFrameWidth;
	int m_x;
	int m_y;
	int m_nX;
	int m_nY;
	
protected:
	CString m_csFontName;
	int m_iPtSize;
	int m_iMaxPtSize;
	COLORREF m_crTxt;
	COLORREF m_crBkg;
	COLORREF m_crMask;
	CString m_csBkgImage;
	BOOL m_bBkgImage;
	CString m_csBkgVid;
	BOOL m_bBkgMix;
	BOOL m_bBkgSolid;
	BOOL m_bCenterJustify;
	COLORREF m_crBkgSolid;
	BOOL m_bBkgCapture;
	UINT m_iMixAmt;
	BOOL m_bShowTranslation;
	BOOL m_bShowPassage;
	BOOL m_bAutoSizeFont;
	CDC m_DCDisplay;

public:
	BOOL m_bMMTimer;
	MMRESULT m_mmFullTimer;
	MMRESULT m_mmPrevTimer;
	TIMECAPS m_tc;
	UINT m_wTimerRes;
	DWORD m_dwMMTIME;
	BOOL m_bPrevVideoOnly;
	BOOL m_bFullVideoOnly;

public:
	void CreateFrame(CDIBFrame & Frame,bool bThreaded,int x = 0,int y = 0);
	void DeleteFrame(CDIBFrame & Frame);
	void GetAspect(double dc,std::pair<int,int> & xy,std::pair<int,int> & xy2);

protected:
	void CopyFrame(CDIBFrame & DIBSrc,CDIBFrame & DIBDest);
	void LoadBackground(int x,int y,CString csBkgImage,CDIBFrame & DIBBkgImage,int & iStartX,int & iStartY);
	void LoadBackgroundVideoFrame(CVfw & Avi,int & iStartX,int & iStartY);

	// The video capture thread
	CCaptureThread * m_pCaptureThread;

	// The output frames
	CDIBFrame m_DIBPrevFore,m_DIBPrevBack;
	CDIBFrame m_DIBFullFore,m_DIBFullBack;
	CDIBFrame m_DIBFullBackImg,m_DIBPrevBackImg;

	// The last preview frame
	CDIBFrame m_DIBPreview;

	// The second to the last preview frame
	CDIBFrame m_DIBPreview2;

	// The last full frame
	CDIBFrame m_DIBFull;

	// The second to the last full frame
	CDIBFrame m_DIBFull2;

	// The transition frames
	CDIBFrame m_DIBPrevTrans,m_DIBFullTrans;

	// The background image frame
	CDIBFrame m_DIBBkgImage;

	// Coordinate mapping
	std::map<std::pair<int,int>,std::pair<int,int>> m_mAspect;

public:

	// The AVI video
	CVfw m_AviPrev,m_AviFull;

	// The captured frame
	CDIBFrame m_DIBFullCapture,m_DIBPrevCapture;
	CSingleLock * m_pFullLock;
	CSingleLock * m_pPrevLock;

public:

	// The monitor rectangular coordinates
	std::vector<CRect> m_vMonRect;
	int m_iMonitor;

protected:
	CCriticalSection m_FullCritSect;
	CCriticalSection m_PrevCritSect;
};

// Faster right trim
inline void RightTrim(CString & cs)
{
	int idx = cs.GetLength() - 1;
	int ndx = idx;
	while (idx >= 0 && cs[idx] == ' ')
		--idx;
	if (ndx != idx)
	{
		if (idx >= 0)
			cs.GetBufferSetLength(idx + 1);
		else
			cs.Empty();
	}
}