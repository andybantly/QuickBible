// Gripper.cpp : implementation file
//

#include "stdafx.h"
#include "Gripper.h"

/////////////////////////////////////////////////////////////////////////////
// CGripper
CGripper::CGripper()
{
}

CGripper::~CGripper()
{
}

BEGIN_MESSAGE_MAP(CGripper, CScrollBar)
	//{{AFX_MSG_MAP(CGripper)
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGripper message handlers

LRESULT CGripper::OnNcHitTest(CPoint point) 
{
	LRESULT ht = CScrollBar::OnNcHitTest(point);
	if (ht == HTCLIENT)
	{
		CRect r;
		GetClientRect(r);
		ClientToScreen(r);
		point.x -= r.left;
		point.y -= r.top;
		if ((point.x + point.y)>= r.Width())
			ht = HTBOTTOMRIGHT;
	}
	return ht;
}
