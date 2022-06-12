#pragma once
#include <afxwin.h>
#include "Vfw.h"

// CCaptureThread
class CCaptureThread : public CWinThread
{
	DECLARE_DYNCREATE(CCaptureThread)

public:
	CCaptureThread();

protected:
	virtual ~CCaptureThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	afx_msg void OnStartCapture(WPARAM wParam,LPARAM lParam);
	afx_msg void OnStopCapture(WPARAM wParam,LPARAM lParam);
	afx_msg void OnEndThread(WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK OnCaptureVideo(HWND hWnd,LPVIDEOHDR lpVHdr);

protected:
	// Bitmap format
	BITMAPINFO m_BMI;

	// The drawdib handle
	HDRAWDIB m_hDrawDib;

protected:
	DECLARE_MESSAGE_MAP()
};