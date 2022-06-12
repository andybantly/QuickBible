#include "stdafx.h"
#include "DIBFrame.h"

// CDIBThread

IMPLEMENT_DYNCREATE(CDIBThread, CWinThread)

CDIBThread::CDIBThread(HANDLE hHandle,LPSTR pRGBDEST) : 
	m_hHandle(hHandle), m_pRGBDEST(pRGBDEST),
	m_pRGBDIB1(NULL), m_pRGBDIB2(NULL)
{
}

CDIBThread::CDIBThread() : 
	m_hHandle(NULL), m_pRGBDEST(NULL),
	m_pRGBDIB1(NULL), m_pRGBDIB2(NULL)
{
}

CDIBThread::~CDIBThread()
{
}

BOOL CDIBThread::InitInstance()
{
	return TRUE;
}

int CDIBThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CDIBThread::SetFrame2(LPSTR pRGBDIB1,LPSTR pRGBDIB2,COLORREF crMask,COLORREF crTxt,COLORREF crBkg,BOOL bMixBkg,int iMixAmt,DWORD dwBkgImageBytes)
{
	m_pRGBDIB1 = pRGBDIB1;
	m_pRGBDIB2 = pRGBDIB2;
	m_crMask = crMask;
	m_crTxt = crTxt;
	m_crBkg = crBkg;
	m_bMixBkg = bMixBkg;
	m_iMixAmt = iMixAmt;
	m_dwBkgImageBytes = dwBkgImageBytes;
}

// CDIBThread message handlers
BEGIN_MESSAGE_MAP(CDIBThread, CWinThread)
	ON_THREAD_MESSAGE(WM_MIXFRAME,&CDIBThread::OnMixFrame)
	ON_THREAD_MESSAGE(WM_TRANSFRAME,&CDIBThread::OnTransFrame)
	ON_THREAD_MESSAGE(WM_ENDTHREAD,&CDIBThread::OnEndThread)
END_MESSAGE_MAP()

// Mix two frames together
void CDIBThread::OnMixFrame(WPARAM wParam,LPARAM lParam)
{
	// Get the data block to process
	int iBlock = (int)wParam;

	// Get the mask color components
	BYTE MaskBlue = (BYTE)GetBValue(m_crMask);
	BYTE MaskGreen = (BYTE)GetGValue(m_crMask);
	BYTE MaskRed = (BYTE)GetRValue(m_crMask);

	// Get the foreground color components
	BYTE TxtBlue = (BYTE)GetBValue(m_crTxt);
	BYTE TxtGreen = (BYTE)GetGValue(m_crTxt);
	BYTE TxtRed = (BYTE)GetRValue(m_crTxt);

	// Get the source and destination Bytes
	BYTE * pDestBytes = (BYTE *)(LPSTR)m_pRGBDEST;
	BYTE * pForeBytes = (BYTE *)(LPSTR)m_pRGBDIB1;
	BYTE * pBackBytes = (BYTE *)(LPSTR)m_pRGBDIB2;

	// Alpha blending level (0-255)
	BYTE alpha = m_iMixAmt;
	BYTE alpha2 = 255 - alpha;

	// Process the DIB
	DWORD dwMaskBytes = m_dwBkgImageBytes / 4;
	DWORD dw = iBlock * dwMaskBytes;
	DWORD dwMaskEnd = dw + dwMaskBytes;
	for (;dw < dwMaskEnd;dw += 4)
	{
		// Detect the mask and srcpy from the background
		if ((pForeBytes[dw] == MaskBlue) && 
			(pForeBytes[dw + 1] == MaskGreen) && 
			(pForeBytes[dw + 2] == MaskRed))
		{
			// Update from the background color data
			pDestBytes[dw] = pBackBytes[dw];
			pDestBytes[dw + 1] = pBackBytes[dw + 1];
			pDestBytes[dw + 2] = pBackBytes[dw + 2];
		}
		else if ((pForeBytes[dw] == TxtBlue) && 
			(pForeBytes[dw + 1] == TxtGreen) && 
			(pForeBytes[dw + 2] == TxtRed))
		{
			// Update from the foreground text color
			pDestBytes[dw] = TxtBlue;
			pDestBytes[dw + 1] = TxtGreen;
			pDestBytes[dw + 2] = TxtRed;
		}
		else
		{
			// Update by blending the sources to the destination
			pDestBytes[dw] = (pBackBytes[dw] * alpha2 + pForeBytes[dw] * alpha) >> 8;
			pDestBytes[dw + 1] = (pBackBytes[dw + 1] * alpha2 + pForeBytes[dw + 1] * alpha) >> 8;
			pDestBytes[dw + 2] = (pBackBytes[dw + 2] * alpha2 + pForeBytes[dw + 2] * alpha) >> 8;
		}
	}

	// Signal completion
	SetEvent(m_hHandle);
}

// Transition from one frame to the next
void CDIBThread::OnTransFrame(WPARAM wParam,LPARAM lParam)
{
	// Get the data block to process
	int iBlock = (int)wParam;

	// Get the source and destination Bytes
	BYTE * pDestBytes = (BYTE *)(LPSTR)m_pRGBDEST;
	BYTE * pForeBytes = (BYTE *)(LPSTR)m_pRGBDIB1;
	BYTE * pBackBytes = (BYTE *)(LPSTR)m_pRGBDIB2;

	// Alpha blending level (0-255)
	BYTE alpha = m_iMixAmt;
	BYTE alpha2 = 255 - alpha;

	// Process the DIB
	DWORD dwMaskBytes = m_dwBkgImageBytes / 4;
	DWORD dw = iBlock * dwMaskBytes;
	DWORD dwMaskEnd = dw + dwMaskBytes;
	for (;dw < dwMaskEnd;dw += 4)
	{
		// Blend the sources to the destination
		pDestBytes[dw] = (pForeBytes[dw] * alpha + pBackBytes[dw] * alpha2) >> 8;
		pDestBytes[dw + 1] = (pForeBytes[dw + 1] * alpha + pBackBytes[dw + 1] * alpha2) >> 8;
		pDestBytes[dw + 2] = (pForeBytes[dw + 2] * alpha + pBackBytes[dw + 2] * alpha2) >> 8;
	}

	// Signal completion
	SetEvent(m_hHandle);
}

void CDIBThread::OnEndThread(WPARAM wParam,LPARAM lParam)
{
	// End the thread
	PostQuitMessage(0);
}

// Construct the frame
CDIBFrame::CDIBFrame() : m_x(0), m_y(0), 
	m_hBkgFrame(NULL), m_pBkgBits(NULL), m_hLastBkgFrame(NULL), 
	m_pBitmapInfo(NULL), m_pBitmapInfoHdr(NULL), m_dwBkgImageBytes(0),
	m_bThreaded(true)
{
	// Initialize the threads and handles
	ZeroMemory(&m_pDIBThread[0],sizeof(CDIBThread *) * 4);
	ZeroMemory(&m_hHandle[0],sizeof(HANDLE) * 4);
}

// Construct the frame
CDIBFrame::CDIBFrame(int x,int y) : m_x(x), m_y(y), 
	m_hBkgFrame(NULL), m_pBkgBits(NULL), m_hLastBkgFrame(NULL), 
	m_pBitmapInfo(NULL), m_pBitmapInfoHdr(NULL), m_dwBkgImageBytes(0),
	m_bThreaded(true)
{
	// Initialize the threads and handles
	ZeroMemory(&m_pDIBThread[0],sizeof(CDIBThread *) * 4);
	ZeroMemory(&m_hHandle[0],sizeof(HANDLE) * 4);

	// Create the frame
	CreateFrame();
}

// Deconstruct the frame
CDIBFrame::~CDIBFrame()
{
	// Delete the frame
	DeleteFrame();
}

// Set the dimensions and create the frame
void CDIBFrame::Init(int x,int y,bool bThreaded)
{
	// Test the dimensions
	if (x < 1 || y < 1)
		return;

	// Test for already being initialized
	if (m_FrameDC && m_x == x && m_y == y)
		return;

	// Set the new dimensions
	m_x = x;
	m_y = y;

	// Set the thread attribute
	m_bThreaded = bThreaded;

	// Create the frame
	CreateFrame();
}

// Create the frame
void CDIBFrame::CreateFrame()
{
	// Cleanup the last frame
	DeleteFrame();

	// Create the last frame DC
	m_FrameDC.CreateCompatibleDC(NULL);
	if (m_FrameDC)
	{
		// Calculate the size of the bitmap info structure (header + color table)
		DWORD dwLen = (DWORD)((WORD)sizeof(BITMAPINFOHEADER) + 0 * sizeof(RGBQUAD));

		// Allocate the bitmap structure
		m_Buffer.resize(dwLen,0);
		BYTE * pBuffer = &m_Buffer[0];

		// Set up the bitmap info structure for the DIB section
		m_pBitmapInfo = (BITMAPINFO*)pBuffer;
		m_pBitmapInfoHdr = (BITMAPINFOHEADER*)&(m_pBitmapInfo->bmiHeader);
		m_pBitmapInfoHdr->biSize = sizeof(BITMAPINFOHEADER);
		m_pBitmapInfoHdr->biWidth = m_x;
		m_pBitmapInfoHdr->biHeight = m_y;
		m_pBitmapInfoHdr->biPlanes = 1;
		m_pBitmapInfoHdr->biBitCount = 32;
		m_pBitmapInfoHdr->biCompression = BI_RGB;

		// Create the DIB for the frame
		m_hBkgFrame = CreateDIBSection(m_FrameDC,m_pBitmapInfo,DIB_RGB_COLORS,(void**)&m_pBkgBits,NULL,0);

		// Flush the GDI
		GdiFlush();

		// Get the byte storage amount for the DIB bits
		BITMAP bmMask;
		GetObject(m_hBkgFrame,sizeof(BITMAP),&bmMask);
		m_dwBkgImageBytes = bmMask.bmWidthBytes * bmMask.bmHeight;

		// Prepare the frame DIB for painting
		m_hLastBkgFrame = (HBITMAP)m_FrameDC.SelectObject(m_hBkgFrame);

		// Initialize the DIB to black
		m_FrameDC.PatBlt(0,0,m_x,m_y,BLACKNESS);

		// Create the DIB data events
		if (m_bThreaded)
		{
			for (int iHandle = 0;iHandle < 4;++iHandle)
			{
				m_hHandle[iHandle] = CreateEvent(NULL,FALSE,FALSE,NULL);
				m_pDIBThread[iHandle] = new CDIBThread(m_hHandle[iHandle],m_pBkgBits);
				m_pDIBThread[iHandle]->CreateThread();
			}
		}
	}
}

// Delete the frame
void CDIBFrame::DeleteFrame()
{
	// Check for a frame to cleanup
	if (m_FrameDC)
	{
		// Flush the GDI
		GdiFlush();

		// UnSelect the DIB
		m_FrameDC.SelectObject(m_hLastBkgFrame);

		// Delete the DIB for the frame
		DeleteObject(m_hBkgFrame);
		m_pBkgBits = NULL;

		// Delete the last frame DC
		m_FrameDC.DeleteDC();

		if (m_bThreaded)
		{
			// Shutdown the threads and handles
			for (int iHandle = 0;iHandle < 4;++iHandle)
			{
				m_pDIBThread[iHandle]->PostThreadMessage(WM_ENDTHREAD,0,0);
				CloseHandle(m_hHandle[iHandle]);
			}

			// Initialize the threads and handles
			ZeroMemory(&m_pDIBThread[0],sizeof(CDIBThread *) * 4);
			ZeroMemory(&m_hHandle[0],sizeof(HANDLE) * 4);
		}
	}
}

// Blend two frames together to the destination, factoring out the key color of the foreground image
void CDIBFrame::MixFrame(COLORREF crMask,COLORREF crTxt,COLORREF crBkg,BOOL bMixBkg,int iMixAmt,CDIBFrame & DIBFore,CDIBFrame & DIBBack)
{
#ifdef _DEBUG
	CDuration dur("MixFrame");
#endif

	// Test for using threads in this DIB
	if (!m_bThreaded)
		return;

	// Get the foreground color data
	LPSTR pDIBForeBits = (LPSTR)DIBFore;

	// Get the background color data
	LPSTR pDIBBackBits = (LPSTR)DIBBack;

	// Set the color channel data and process the block
	for (int iThread = 0;iThread < 4;++iThread)
	{
		m_pDIBThread[iThread]->SetFrame2(pDIBForeBits,pDIBBackBits,crMask,crTxt,crBkg,bMixBkg,iMixAmt,m_dwBkgImageBytes);
		m_pDIBThread[iThread]->PostThreadMessage(WM_MIXFRAME,iThread,0);
	}

	// Wait for the threads to complete
	WaitForMultipleObjects(4,&m_hHandle[0],TRUE,INFINITE);
}

// Transition two frames together based on a blend percentage of source 2 coming through source 1
void CDIBFrame::TransitionFrame(int iMixAmt,CDIBFrame & DIBSrc1,CDIBFrame & DIBSrc2)
{
#ifdef _DEBUG
	CDuration dur("TransitionFrame");
#endif

	// Test for using threads in this DIB
	if (!m_bThreaded)
		return;

	// Get the foreground color data
	LPSTR pDIBForeBits = (LPSTR)DIBSrc1;

	// Get the background color data
	LPSTR pDIBBackBits = (LPSTR)DIBSrc2;

	// Set the color channel data and process the block
	for (int iThread = 0;iThread < 4;++iThread)
	{
		m_pDIBThread[iThread]->SetFrame2(pDIBForeBits,pDIBBackBits,0,0,0,false,iMixAmt,m_dwBkgImageBytes);
		m_pDIBThread[iThread]->PostThreadMessage(WM_TRANSFRAME,iThread,0);
	}

	// Wait for the threads to complete
	WaitForMultipleObjects(4,&m_hHandle[0],TRUE,INFINITE);
}