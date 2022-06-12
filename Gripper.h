// Gripper.h : header file
//

#pragma once

class CGripper : public CScrollBar
{
// Construction
public:
	CGripper();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGripper)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGripper();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGripper)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
