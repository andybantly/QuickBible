// Copyright (C) 2007 Andrew S. Bantly
#pragma once

class CSplashWnd : public CWnd
{
	DECLARE_DYNAMIC(CSplashWnd)

	CBitmap m_SplashBitmap;
	int m_nXStart;
	int m_nYStart;
	int m_nWidth;
	int m_nHeight;

public:
	CSplashWnd();
	virtual ~CSplashWnd();
	int GetXStart() const;
	int GetYStart() const;
	int GetWidth() const;
	int GetHeight() const;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

protected:
	virtual void PostNcDestroy();
};


