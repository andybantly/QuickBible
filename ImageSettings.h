// Copyright (C) 2007 Andrew S. Bantly

#pragma once
#include "ImageButton.h"

// CImageSettings dialog

class CImageSettings : public CDialog
{
	DECLARE_DYNAMIC(CImageSettings)

public:
	CImageSettings(BOOL bBkgCapture,CString csBkgVideo,CString csBkgImage,BOOL bBkgMix,UINT iMixAmt,COLORREF cr,BOOL bTransition,UINT iStepAmt,CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageSettings();

// Dialog Data
	enum { IDD = IDD_IMAGESETTINGS };

public:
	CString m_csBkgVideo;
	CString m_csBkgImage;
	BOOL m_bBkgCapture;
	BOOL m_bBkgVideo;
	BOOL m_bBkgOn;
	BOOL m_bBkgMix;
	BOOL m_bBkgSolid;
	UINT m_iMixAmt;
	COLORREF m_crColor;
	BOOL m_bTransition;
	UINT m_iStepAmt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	afx_msg void OnCapture();
	afx_msg void OnVideo();
	afx_msg void OnImage();
	afx_msg void OnColor();
	afx_msg void OnBkgCapture();
	afx_msg void OnBkgVideo();
	afx_msg void OnBkgOn();
	afx_msg void OnBkgSolid();
	afx_msg void OnBkgMix();
	afx_msg void OnTransition();
	afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar * pScrollBar);

protected:
	void UpdateControls();

protected:
	CImageButton m_Capture;
	CImageButton m_Video;
	CImageButton m_Image;
	CButton m_BkgMix;
	CStatic m_StaticOff;
	CStatic m_StaticFull;
	CSliderCtrl m_MixDimmer;
	CEdit m_MixAmt;
	CStatic m_StaticSlow;
	CSliderCtrl m_StepDimmer;
	CStatic m_StaticFast;
	CEdit m_StepAmt;
	CButton m_BkgSolid;
	CImageButton m_Color;
	CImageButton m_Ok;
	CImageButton m_Cancel;

	DECLARE_MESSAGE_MAP()
};
