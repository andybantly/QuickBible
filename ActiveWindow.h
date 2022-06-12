// ActiveWindow.h : header file to capture the active window
// Copyright (C) 2008 Andrew S. Bantly
//

#pragma once

class CActiveWindow
{
public:
	CActiveWindow() : m_hActiveWnd(NULL)
	{
		m_hActiveWnd = GetActiveWindow();
	}
	~CActiveWindow()
	{
		if (IsWindow(m_hActiveWnd))
			BringWindowToTop(m_hActiveWnd);
	}
private:
	CActiveWindow(const CActiveWindow & rhs);
	CActiveWindow & operator = (const CActiveWindow & rhs);
	HWND m_hActiveWnd;
};