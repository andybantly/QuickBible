#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include <Vfw.h>

// CDIBThread
class CDIBThread : public CWinThread
{
	DECLARE_DYNCREATE(CDIBThread)

public:
	CDIBThread();
	CDIBThread(HANDLE hHandle,LPSTR pRGBDEST);
	virtual ~CDIBThread();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	void SetFrame2(LPSTR pRGBDIB1,LPSTR pRGBDIB2,COLORREF crMask,COLORREF crTxt,COLORREF crBkg,BOOL bMixBkg,int iMixAmt,DWORD dwBkgImageBytes);

public:
	afx_msg void OnMixFrame(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTransFrame(WPARAM wParam,LPARAM lParam);
	afx_msg void OnEndThread(WPARAM wParam,LPARAM lParam);

protected:
	LPSTR m_pRGBDEST;
	LPSTR m_pRGBDIB1;
	LPSTR m_pRGBDIB2;
	COLORREF m_crMask,m_crTxt,m_crBkg;
	int m_iMixAmt;
	BOOL m_bMixBkg;
	DWORD m_dwBkgImageBytes;
	HANDLE m_hHandle;

protected:
	DECLARE_MESSAGE_MAP()
};

class CDIBFrame
{
public:
	CDIBFrame();
	CDIBFrame(int x,int y);
	~CDIBFrame();

public:
	operator HBITMAP () {return m_hBkgFrame;}
	operator HDC () {return (HDC)m_FrameDC;}
	operator CDC * () {return &m_FrameDC;}
	operator CDC & () {return m_FrameDC;}
	operator LPSTR () {return m_pBkgBits;}
	operator LPSTR * () {return &m_pBkgBits;}

public:
	void Init(int x,int y,bool bThreaded);
	void CreateFrame();
	void DeleteFrame();

	// Blend two frames together to the output, factoring out the key color of the foreground image and blending the backround color of the backround image
	void MixFrame(COLORREF crMask,COLORREF crTxt,COLORREF crBkg,BOOL bMixBkg,int iMixAmt,CDIBFrame & DIBFore,CDIBFrame & DIBBack);

	// Transition between two frames using a mix amount
	void TransitionFrame(int iMixAmt,CDIBFrame & DIBSrc1,CDIBFrame & DIBSrc2);

public:
	int m_x,m_y;
	DWORD m_dwBkgImageBytes;

protected:
	CDC m_FrameDC;
	HBITMAP m_hBkgFrame;
	LPSTR m_pBkgBits;
	HBITMAP m_hLastBkgFrame;
	std::vector<BYTE> m_Buffer;
	BITMAPINFO * m_pBitmapInfo;
	BITMAPINFOHEADER * m_pBitmapInfoHdr;
	bool m_bThreaded;
	CDIBThread * m_pDIBThread[4];
	HANDLE m_hHandle[4];
};