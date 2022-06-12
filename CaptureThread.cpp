#include "stdafx.h"
#include "Project.h"
#include "CaptureThread.h"

extern CProjectApp * g_pApp;
static CProjectDlg * g_pDlg = NULL;
static CPreviewDlg * g_pPreviewDlg = NULL;
extern HWND g_hWndCapture;
extern int g_iDriver;

// CCaptureThread

IMPLEMENT_DYNCREATE(CCaptureThread, CWinThread)

CCaptureThread::CCaptureThread()
{
	// Set the dialog pointers
	g_pDlg = (CProjectDlg *)g_pApp->m_pMainWnd;
	g_pPreviewDlg = (CPreviewDlg *)g_pApp->m_pPreviewDlg;

	// Create a DrawDib DC for video
	m_hDrawDib = DrawDibOpen();
}

CCaptureThread::~CCaptureThread()
{
	if (m_hDrawDib)
		DrawDibClose(m_hDrawDib);
}

BOOL CCaptureThread::InitInstance()
{
	// Create the capture window
	g_hWndCapture = capCreateCaptureWindow("QBVidCap",WS_POPUP,0,0,1,1,0,0);
	if (!g_hWndCapture)
		return FALSE;

	// Set the user data to have access to "this" class
	capSetUserData(g_hWndCapture,this);

	// Set the capture callback function
	capSetCallbackOnVideoStream(g_hWndCapture,&CCaptureThread::OnCaptureVideo);
 
	// Connect to the selected capture source
	if (!capDriverConnect(g_hWndCapture,g_iDriver))
		return FALSE;

	return TRUE;
}

int CCaptureThread::ExitInstance()
{
	// Disconnect the driver
	capDriverDisconnect(g_hWndCapture);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCaptureThread, CWinThread)
	ON_THREAD_MESSAGE(WM_STARTCAPTURE,&CCaptureThread::OnStartCapture)
	ON_THREAD_MESSAGE(WM_STOPCAPTURE,&CCaptureThread::OnStopCapture)
	ON_THREAD_MESSAGE(WM_ENDTHREAD,&CCaptureThread::OnEndThread)
END_MESSAGE_MAP()

// Start capturing
void CCaptureThread::OnStartCapture(WPARAM wParam,LPARAM lParam)
{
	// Get the capture parameters
	CAPTUREPARMS CapParms = {0};
	capCaptureGetSetup(g_hWndCapture,&CapParms,sizeof(CapParms));

	// Set the capture parameters
	CapParms.fAbortLeftMouse = FALSE;
	CapParms.fAbortRightMouse = FALSE;
	CapParms.fYield = TRUE;
	CapParms.fCaptureAudio = FALSE;

	// Sets the configuration parameters used with streaming capture
	if (!capCaptureSetSetup(g_hWndCapture,&CapParms,sizeof(CapParms)))
		return;

	// Get Video Format 
	capGetVideoFormat(g_hWndCapture,&m_BMI,sizeof(BITMAPINFO));

	// Start the capture
	capCaptureSequenceNoFile(g_hWndCapture);
}

// Stop capturing
void CCaptureThread::OnStopCapture(WPARAM wParam,LPARAM lParam)
{
	if (g_hWndCapture)
	{
		capCaptureStop(g_hWndCapture);
		capCaptureAbort(g_hWndCapture);

		// Stop the capturing process    
		capCaptureAbort(g_hWndCapture);
	      
		// Disable the callback function..
		capSetCallbackOnVideoStream(g_hWndCapture,NULL);
	}
}

// End the thread
void CCaptureThread::OnEndThread(WPARAM wParam,LPARAM lParam)
{
	PostQuitMessage(0);
}

// Video Capture callback
LRESULT CALLBACK CCaptureThread::OnCaptureVideo(HWND hWnd,LPVIDEOHDR lpVHdr)
{
	BOOL bRet = FALSE;
	CCaptureThread * pThis = (CCaptureThread *)capGetUserData(hWnd);
	if (!pThis)
		return bRet;

	// Lock the resource before using it
	bool bFullScreen = g_pDlg->m_bFullScreen;
	CSingleLock * pLock = bFullScreen ? g_pPreviewDlg->m_pFullLock : g_pPreviewDlg->m_pPrevLock;
	pLock->Lock();

	// Get the dimensions
	int iWidth = pThis->m_BMI.bmiHeader.biWidth;
	int iHeight = pThis->m_BMI.bmiHeader.biHeight;

	// Sync up with the capture DIB
	CDIBFrame & DIBCapture = bFullScreen ? g_pPreviewDlg->m_DIBFullCapture : g_pPreviewDlg->m_DIBFullCapture;
	DIBCapture.Init(iWidth,iHeight,false);

	// Transfer the video to the frame
	bRet = DrawDibDraw(pThis->m_hDrawDib,DIBCapture,
		0,0,iWidth,iHeight,
		&(pThis->m_BMI.bmiHeader),
		(LPVOID)lpVHdr->lpData,
		0,0,iWidth,iHeight,0);

	// Test for the screen being visible
	if (g_pDlg && g_pDlg->m_bFullScreen)
	{
		// Render the full screen window
		g_pDlg->m_FullWnd.RedrawWindow();
	}
	// Test for the screen being visible
	else if (g_pDlg && g_pDlg->m_bPreviewOn)
	{
		// Render the preview dialog
		g_pApp->UpdateLastFrame(true);
	}

	// Unlock the resoruce
	pLock->Unlock();

	return TRUE;
}