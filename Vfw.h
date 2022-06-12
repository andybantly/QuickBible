#pragma once
#include "DIBFrame.h"
#include <Vfw.h>
#include <vector>
#include <string>

// Derive this class from CWinThread!  Relying on the timer just isn't working out well enough
class CVfw
{
public:
	CVfw();
	CVfw(LPCTSTR szFile,int nWidth,int nHeight);
	virtual ~CVfw(void);

	BOOL Init(LPCTSTR szFile,int nWidth,int nHeight);
	BOOL DrawFrame(LONG lTime);
	BOOL IsInit() {return m_AVIStream ? TRUE : FALSE;}

	// Utility
	operator HBITMAP () {return m_DIBFrame;}
	operator HDC () {return m_DIBFrame;}
	operator CDC * () {return m_DIBFrame;}
	operator CDC & () {return m_DIBFrame;}

	// Dimensions
	int m_nWidth,m_nHeight;
	int m_nOrigWidth,m_nOrigHeight;
	int m_iStartX,m_iStartY;

protected:
	// Helper
	void InitCommon();

	// Bitmap / AVI
	BOOL CreateFrame();

	// AVI
	BOOL InitAVI(LPCTSTR szFile);
	void Free();

	// FPS
	DWORD m_dwSamples,m_dwRate,m_dwScale;
	double m_dSamples,m_dRate,m_dScale,m_dSamplesPerSecond,m_dTotalSeconds;

	// Bitmap Probes
	LPSTR FindDIBBits(LPSTR lpbi);
	WORD PaletteSize(LPSTR lpbi);
	WORD DIBNumColors(LPSTR lpbi);
	BOOL IsWin30DIB(LPSTR lpbi);

	// The drawdib handle
	HDRAWDIB m_hDrawDib;

	// The DIB Frame
	CDIBFrame m_DIBFrame;

	// AVI
	std::string m_strAviFile;
	IAVIStream * m_AVIStream;
	AVISTREAMINFO m_AVIStreamInfo;
	PGETFRAME m_AVIFrame;
	CRect m_AVIRect;
	clock_t m_clk;
	LONG m_lStartFrame,m_lLastFrame;

	// Errors
public:
	std::string GetErrorString() {return m_strErr;}

protected:
	std::string m_strErr;
};
