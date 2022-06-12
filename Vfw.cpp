#include "stdafx.h"
#include "Project.h"
#include "Vfw.h"
#include "atlimage.h"
using namespace std;

extern CProjectApp * g_pApp;

CVfw::CVfw()
{
	// Initialize the common variables
	InitCommon();
}

CVfw::~CVfw()
{
	// Free the AVI file
	Free();

	// Exit the AVI file library
	AVIFileExit();
}

CVfw::CVfw(LPCTSTR szFile,int nWidth,int nHeight)
{
	// Initialize the common variables
	InitCommon();

	// Initialize
	BOOL bInit = Init(szFile,nWidth,nHeight);
}

// Initialize the common elements and variables
void CVfw::InitCommon()
{
	// Initialize the AVI file library
	AVIFileInit();

	// Initialize the file name
	m_strAviFile.clear();

	// Initialize the common display variables
	m_AVIStream = NULL;
	m_AVIFrame = NULL;
	m_hDrawDib = NULL;

	// Initialize the common FPS variables
	m_nWidth = 0;
	m_nHeight = 0;
	m_nOrigWidth = 0;
	m_nOrigHeight = 0;
	m_dwSamples = 0;
	m_dwRate = 0;
	m_dwScale = 0;
	m_dSamples = 0.0;
	m_dRate = 0.0;
	m_dScale = 0.0;
	m_dSamplesPerSecond = 0.0;
	m_dTotalSeconds = 0.0;
	m_lLastFrame = 0;
}

// Initialize the video usage of this class
BOOL CVfw::Init(LPCTSTR szFile,int nWidth,int nHeight)
{
	// Test for being initialized
	if (((nWidth == m_nOrigWidth) && (nHeight == m_nOrigHeight)) && m_AVIStream && (m_strAviFile.compare(szFile) == 0))
		return TRUE;

	// Test for file based AVI
	if (szFile)
	{
		// Test for a previous video
		if (m_strAviFile.length())
		{
			// Free the previous video
			Free();
		}

		// Copy the video file name
		m_strAviFile = szFile;

		// Initialize the AVI subsytem
		if (!InitAVI(szFile))
			return FALSE;
	}

	// Get the current start time in milliseconds
	m_clk = clock();

	// Create a DrawDib DC for video
	m_hDrawDib = DrawDibOpen();

	// Get the aspect ratio of the image
	CPreviewDlg * pPrevDlg = g_pApp->m_pPreviewDlg;
	double dc = (double)nWidth / (double)nHeight;
	std::pair<int,int> xy(nWidth,nHeight);
	std::pair<int,int> xy2;
	pPrevDlg->GetAspect(dc,xy,xy2);

	// Set the new width and height
	m_nWidth = xy2.first;
	m_nHeight = xy2.second;

	// Set the starting coordinates
	m_iStartX = (m_nWidth - xy.first) / 2;
	m_iStartY = (m_nHeight - xy.second) / 2;

	// Initialize the Frame
	m_DIBFrame.Init(m_nWidth,m_nHeight,false);

	// Set the original width
	m_nOrigWidth = xy.first;
	m_nOrigHeight = xy.second;

	return TRUE;
}

// Initialize the AVI system
BOOL CVfw::InitAVI(LPCTSTR szFile)
{
	// Open the AVI file
	IAVIFile * AviFile = 0;
	if (FAILED(AVIFileOpen(&AviFile,szFile,OF_SHARE_DENY_WRITE,NULL)) || AviFile == 0)
	{
		m_strErr = "Failed to open AVI file";
		return FALSE;
	}

	// Reference count the AVI file
	AVIFileAddRef(AviFile);

	// Get the number of streams in the AVI file
	AVIFILEINFO AviFileInfo;
	if (!AVIFileInfo(AviFile,&AviFileInfo,sizeof(AVIFILEINFO)) == 0)
	{
		m_strErr = "Failed to get AVI file info";
		return FALSE;
	}

	// Get the AVI video stream from the file
	IAVIStream * AviStream = NULL;

	// Clear the error string
	m_strErr.clear();

	// Locate the first video stream available
	HRESULT hr = AVIFileGetStream(AviFile,&AviStream,streamtypeVIDEO,0);
	if (FAILED(hr) || AviStream == 0)
	{
		m_strErr = "Failed to get AVI video stream";
		if (hr == AVIERR_MEMORY)
		{
			m_strErr += _T("\n\n");
			m_strErr += _T("Out of memory!");
		}
		else if (hr == AVIERR_NODATA)
		{
			m_strErr += _T("\n\n");
			m_strErr += _T("No Data!");
		}
	}

	// Test for a no data error message
	if (m_strErr.length())
		return FALSE;

	// Get the AVI stream information
	AVISTREAMINFO AviInfo;
	AVIStreamInfo(AviStream,&AviInfo,sizeof(AVISTREAMINFO));

	// Make a copy of the stream so the video can be used without worry of other processes modifying
	IAVIStream * Streams[1]= {AviStream};
	IAVIFile * TmpAviFile = 0;
	if (FAILED(AVIMakeFileFromStreams(&TmpAviFile,1,&Streams[0])) || TmpAviFile == 0)
	{
		m_strErr = "Failed to make a copy of the AVI video stream";
		return FALSE;
	}

	// Get the stream from the copy of the AVI video file
	m_AVIStream = 0;
	if (FAILED(AVIFileGetStream(TmpAviFile,&m_AVIStream,streamtypeVIDEO,0)) || m_AVIStream == 0)
	{
		m_strErr = "Failed to get copied AVI video stream";
		return FALSE;
	}

	// Get the sampling information
	if (FAILED(AVIStreamInfo(m_AVIStream,&m_AVIStreamInfo,sizeof(m_AVIStreamInfo))))
	{
		m_strErr = "Failed to get copied AVI video stream info";
		return FALSE;
	}

	// Get the frames dimensions
	m_AVIRect = m_AVIStreamInfo.rcFrame;

	// Get the length in samples
	m_dwSamples = m_AVIStreamInfo.dwLength;

	// Get the sample rate and scale
	m_dwRate = m_AVIStreamInfo.dwRate;
	m_dwScale = m_AVIStreamInfo.dwScale;

	// Get the values in double precision
	m_dSamples = (double)m_dwSamples;
	m_dRate = (double)m_dwRate;
	m_dScale = (double)m_dwScale;

	// Get the samples per unit time
	m_dSamplesPerSecond = m_dRate / m_dScale;

	// Get the run time
	m_dTotalSeconds = m_dSamples / m_dSamplesPerSecond;

	// Set the starting frame
	m_lStartFrame = m_AVIStreamInfo.dwStart;

	// Unreference count the file
	AVIFileRelease(TmpAviFile);
	TmpAviFile = 0;

	// Release the stream
	AVIStreamRelease(AviStream);
	AviStream = 0;

	// Unreference count the file
	AVIFileRelease(AviFile);

	// Close the file
	AVIFileClose(AviFile);
	AviFile = 0;

	// Prepare to get AVI video frames
	m_AVIFrame = AVIStreamGetFrameOpen(m_AVIStream,(LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
	if (m_AVIFrame == 0)
	{
		m_strErr = "Failed to find a suitable decompressor for the AVI video stream";
		return FALSE;
	}
	return TRUE;
}

// Free the AVI file
void CVfw::Free()
{
	// Clean up the frame
	m_DIBFrame.DeleteFrame();

	// Test for closing the video
	if (m_strAviFile.length())
	{
		// Initialize the file name
		m_strAviFile.clear();

		// Close the DrawDib DIB
		if (m_hDrawDib)
		{
			DrawDibClose(m_hDrawDib);
			m_hDrawDib = NULL;
		}

		// Release the resources for the AVI video frames
		if (m_AVIFrame)
		{
			AVIStreamGetFrameClose(m_AVIFrame);
			m_AVIFrame = NULL;
		}

		// Release the temporary stream
		if (m_AVIStream)
		{
			AVIStreamRelease(m_AVIStream);
			m_AVIStream = 0;
		}
	}
}

// Create the frame
BOOL CVfw::CreateFrame()
{
	// Set the frame dimension
	if (m_nWidth == 0)
		return FALSE;
	if (m_nHeight == 0)
		return FALSE;

	// Initialize the frame
	m_DIBFrame.Init(m_nWidth,m_nHeight,false);
	return TRUE;
}

// Draw the AVI frame based on the time
BOOL CVfw::DrawFrame(LONG lTime)
{
	// Calculate the seconds position
	double dTime = (double)lTime;
	double dClk = (double)m_clk;
	double dClksPerSec = CLOCKS_PER_SEC;
	double dFrameSecond = (dTime - dClk) / dClksPerSec;

	// Calculate the frame, based on the seconds
	double dFrame = dFrameSecond * m_dSamplesPerSecond;
	DWORD dwFrame = (DWORD)dFrame;

	// Scale the frame back to range
	LONG lFrame = dwFrame % m_dwSamples + m_lStartFrame;

	// Debug print the frame number
	TRACE("%d\n",lFrame);

	// Test if this is the same as the last frame
	if (lFrame == m_lLastFrame)
		return TRUE;
	m_lLastFrame = lFrame;

	// Get the DIB at position p
	LPSTR pFrame = (LPSTR)AVIStreamGetFrame(m_AVIFrame,lFrame);
	if (pFrame == NULL)
		return FALSE;

	// Get the bitmap info structure for the DIB section
	BITMAPINFO * pBitmapInfo = (BITMAPINFO*)pFrame;
	BITMAPINFOHEADER * pBitmapInfoHeader = (BITMAPINFOHEADER*)&(pBitmapInfo->bmiHeader);
	
	// Get the DIB Bits
	LPSTR pFrameBytes = FindDIBBits(pFrame);

	// Draw the image, scaling it to the new dimensions, to the internal DIB
	if (!DrawDibDraw(m_hDrawDib,m_DIBFrame,0,0,m_nWidth,m_nHeight,pBitmapInfoHeader,(LPVOID)pFrameBytes,0,0,m_AVIRect.Width(),m_AVIRect.Height(),0))
		return FALSE;
	return TRUE;
}

// Locate the bits in the DIB
LPSTR CVfw::FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

// Get the size of the palette
WORD CVfw::PaletteSize(LPSTR lpbi)
{
   // calculate the size required by the palette
   if (IsWin30DIB(lpbi))
	  return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

// Get the number of colors in the DIB
WORD CVfw::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	// If this is a Windows-style DIB, the number of colors in the
	// color table can be less than the number of bits per pixel
	// allows for (i.e. lpbi->biClrUsed can be set to some value).
	// If this is the case, return the appropriate value.

	if (IsWin30DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	//  Calculate the number of colors in the color table based on
	//  the number of bits per pixel for the DIB.
	if (IsWin30DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	// return number of colors based on bits per pixel
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}

BOOL CVfw::IsWin30DIB(LPSTR lpbi)
{
	return ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER) ? TRUE : FALSE);
}