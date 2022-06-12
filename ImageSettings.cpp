// ImageSettings.cpp : implementation file
// Copyright (C) 2007 Andrew S. Bantly
//

#include "stdafx.h"
#include "Project.h"
#include "ImageSettings.h"
#include "BBLFileDialog.h"
#include "Vfw.h"
#include <atlimage.h>

extern CProjectApp * g_pApp;
extern HWND g_hWndCapture;

// CImageSettings dialog

IMPLEMENT_DYNAMIC(CImageSettings, CDialog)
CImageSettings::CImageSettings(BOOL bBkgCapture,CString csBkgVideo,CString csBkgImage,BOOL bBkgMix,UINT iMixAmt,COLORREF cr,BOOL bTransition,UINT iStepAmt,CWnd* pParent /*=NULL*/)
	: CDialog(CImageSettings::IDD, pParent)
{
	m_bBkgCapture = g_hWndCapture ? bBkgCapture : FALSE;
	m_csBkgVideo = csBkgVideo;
	m_csBkgImage = csBkgImage;
	if (bBkgCapture)
	{
		m_bBkgCapture = TRUE;
		m_bBkgVideo = FALSE;
		m_bBkgOn = FALSE;
		m_bBkgSolid = FALSE;
	}
	else if (csBkgVideo.GetLength())
	{
		m_bBkgCapture = FALSE;
		m_bBkgVideo = TRUE;
		m_bBkgOn = FALSE;
		m_bBkgSolid = FALSE;
	}
	else if (csBkgImage.GetLength())
	{
		m_bBkgCapture = FALSE;
		m_bBkgVideo = FALSE;
		m_bBkgOn = TRUE;
		m_bBkgSolid = FALSE;
	}
	else
	{
		m_bBkgCapture = FALSE;
		m_bBkgVideo = FALSE;
		m_bBkgOn = FALSE;
		m_bBkgSolid = FALSE;
	}
	m_bBkgMix = bBkgMix;
	m_iMixAmt = iMixAmt;
	m_crColor = cr;
	m_bTransition = bTransition;
	m_iStepAmt = iStepAmt;
}

CImageSettings::~CImageSettings()
{
}

void CImageSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_BKGMIX, m_bBkgMix);
	DDX_Text(pDX, IDC_MIXAMT, m_iMixAmt);
	DDV_MinMaxUInt(pDX, m_iMixAmt, 0, 100);
	DDX_Check(pDX, IDC_BKGCAPTURE, m_bBkgCapture);
	DDX_Check(pDX, IDC_BKGVIDEO, m_bBkgVideo);
	DDX_Check(pDX, IDC_BKGON, m_bBkgOn);
	DDX_Check(pDX, IDC_BKGSOLID, m_bBkgSolid);
	DDX_Control(pDX, IDC_BKGMIX, m_BkgMix);
	DDX_Control(pDX, IDC_MIXAMT, m_MixAmt);
	DDX_Control(pDX, IDC_STATIC_OFF, m_StaticOff);
	DDX_Control(pDX, IDC_MIX, m_MixDimmer);
	DDX_Control(pDX, IDC_STATIC_FULL, m_StaticFull);
	DDX_Control(pDX, IDC_BKGSOLID, m_BkgSolid);
	DDX_Check(pDX, IDC_TRANSITION, m_bTransition);
	DDX_Control(pDX, IDC_STATIC_SLOW, m_StaticSlow);
	DDX_Control(pDX, IDC_STEP, m_StepDimmer);
	DDX_Control(pDX, IDC_STATIC_FAST, m_StaticFast);
	DDX_Control(pDX, IDC_STEPAMT, m_StepAmt);
	DDX_Text(pDX, IDC_STEPAMT, m_iStepAmt);
	DDV_MinMaxUInt(pDX, m_iStepAmt, 1, 100);
}

BOOL CImageSettings::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	// Set the window text of the controls
	SetWindowText(g_pApp->QBString(267));
	GetDlgItem(IDC_BKGCAPTURE)->SetWindowText(g_pApp->QBString(355));
	GetDlgItem(IDC_BKGVIDEO)->SetWindowText(g_pApp->QBString(350));
	GetDlgItem(IDC_BKGON)->SetWindowText(g_pApp->QBString(268));
	GetDlgItem(IDC_BKGSOLID)->SetWindowText(g_pApp->QBString(269));
	GetDlgItem(IDC_BKGMIX)->SetWindowText(g_pApp->QBString(270));
	GetDlgItem(IDC_STATIC_OFF)->SetWindowText(g_pApp->QBString(271));
	GetDlgItem(IDC_STATIC_FULL)->SetWindowText(g_pApp->QBString(272));
	GetDlgItem(IDC_TRANSITION)->SetWindowText(g_pApp->QBString(301));
	GetDlgItem(IDC_STATIC_SLOW)->SetWindowText(g_pApp->QBString(302));
	GetDlgItem(IDC_STATIC_FAST)->SetWindowText(g_pApp->QBString(303));

	// Create the image buttons
	m_Capture.CreateBtmp(this,IDB_CAPTURE,IDC_CAPTURE,g_pApp->QBString(356));
	m_Video.CreateBtmp(this,IDB_VIDEO,IDC_VIDEO,g_pApp->QBString(351));
	m_Image.CreateBtmp(this,IDB_IMAGE,IDC_IMAGE,g_pApp->QBString(273));
	m_Image.EnableWindow(m_bBkgOn);
	m_Color.CreateRect(this,IDC_COLOR,m_crColor,g_pApp->QBString(274));
	m_Color.EnableWindow(m_bBkgSolid);
	m_Ok.CreateText(this,IDOK,g_pApp->QBString(276));
	m_Cancel.CreateText(this,IDCANCEL,g_pApp->QBString(275));

	// Set the slider control
	m_MixDimmer.SetRange(0,100);
	m_MixDimmer.SetPos(m_iMixAmt);
	m_StaticSlow.EnableWindow(m_bTransition);
	m_StepDimmer.EnableWindow(m_bTransition);
	m_StaticFast.EnableWindow(m_bTransition);
	m_StepAmt.EnableWindow(m_bTransition);
	m_StepDimmer.SetRange(1,100);
	m_StepDimmer.SetPos(m_iStepAmt);

	UpdateControls();

	return bRet;
}

BEGIN_MESSAGE_MAP(CImageSettings, CDialog)
	ON_BN_CLICKED(IDC_CAPTURE, &CImageSettings::OnCapture)
	ON_BN_CLICKED(IDC_VIDEO, &CImageSettings::OnVideo)
	ON_BN_CLICKED(IDC_IMAGE, &CImageSettings::OnImage)
	ON_BN_CLICKED(IDC_COLOR, &CImageSettings::OnColor)
	ON_BN_CLICKED(IDC_BKGCAPTURE, &CImageSettings::OnBkgCapture)
	ON_BN_CLICKED(IDC_BKGVIDEO, &CImageSettings::OnBkgVideo)
	ON_BN_CLICKED(IDC_BKGON, &CImageSettings::OnBkgOn)
	ON_BN_CLICKED(IDC_BKGSOLID, &CImageSettings::OnBkgSolid)
	ON_BN_CLICKED(IDC_BKGMIX, &CImageSettings::OnBkgMix)
	ON_BN_CLICKED(IDC_TRANSITION, &CImageSettings::OnTransition)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CImageSettings message handlers
void CImageSettings::OnCapture()
{
	if (g_hWndCapture)
	{
		// Display the video source dialog
		capDlgVideoSource(g_hWndCapture);
	}
}

// Select a video file
void CImageSettings::OnVideo()
{
	// Get the video file filter
	CString csFileFilter = g_pApp->QBString(352);

	// Call the custom load dialog
	CBBLFileDialog dlgFile(TRUE,g_pApp->QBString(353),NULL,NULL,OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING,csFileFilter);
	INT_PTR nRet = dlgFile.DoModal();
	if (nRet != IDCANCEL)
	{
		// Get the files returned
		POSITION myPos = dlgFile.GetStartPosition();
		if (myPos != NULL)
		{
			CString csBkgVideo = dlgFile.GetNextPathName(myPos);
			CVfw Avi;
			if (!Avi.Init(csBkgVideo,1,1))
			{
				CString csCaption = g_pApp->QBString(259);
				CString csMessage = g_pApp->QBString(354);
				csMessage += _T("\n\n");
				CString csError(Avi.GetErrorString().c_str());
				csMessage += csError;
				CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
				MsgBox.DoModal();
			}
			else
				m_csBkgVideo = csBkgVideo;
		}
	}
}

// Select the background as an image
void CImageSettings::OnImage()
{
	// Get the image file filter
	CImage Image;
	CString csFilter,csFileFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult = Image.GetExporterFilterString(csFilter,aguidFileTypes);
	csFileFilter = g_pApp->QBString(321);
	if (FAILED(hResult))
		csFileFilter = g_pApp->QBString(105);

	// Call the custom load dialog
	CBBLFileDialog dlgFile(TRUE,g_pApp->QBString(102),NULL,NULL,OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING,csFileFilter);
	INT_PTR nRet = dlgFile.DoModal();
	if (nRet != IDCANCEL)
	{
		// Get the files returned
		POSITION myPos = dlgFile.GetStartPosition();
		if (myPos != NULL)
		{
			CString csBkgImage = dlgFile.GetNextPathName(myPos);
			if (SUCCEEDED(Image.Load(csBkgImage)))
				m_csBkgImage = csBkgImage;
			else
			{
				CString csCaption = g_pApp->QBString(259);
				CString csMessage = g_pApp->QBString(320);
				CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
				MsgBox.DoModal();
			}
		}
	}
}

void CImageSettings::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar * pScrollBar)
{
	CSliderCtrl * pDimmer = (CSliderCtrl *)pScrollBar;
	HWND hWndMix = m_MixDimmer.m_hWnd;
	HWND hWndStep = m_StepDimmer.m_hWnd;
	HWND hDimmer = pDimmer->m_hWnd;
	int iPos = pDimmer->GetPos();
	if (hDimmer == hWndMix)
		m_iMixAmt = iPos;
	else
		m_iStepAmt = iPos;
	UpdateData(FALSE);
}

void CImageSettings::OnColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_crColor = dlg.GetColor();
		m_Color.SetFillColor(m_crColor);
	}
}

void CImageSettings::OnBkgCapture()
{
	UpdateData();
	m_bBkgVideo = FALSE;
	m_bBkgOn = FALSE;
	m_bBkgSolid = FALSE;
	UpdateData(FALSE);
	UpdateControls();
}

void CImageSettings::OnBkgVideo()
{
	UpdateData();
	m_bBkgCapture = FALSE;
	m_bBkgOn = FALSE;
	m_bBkgSolid = FALSE;
	UpdateData(FALSE);
	UpdateControls();
}

void CImageSettings::OnBkgOn()
{
	UpdateData();
	m_bBkgCapture = FALSE;
	m_bBkgVideo = FALSE;
	m_bBkgSolid = FALSE;
	UpdateData(FALSE);
	UpdateControls();
}

void CImageSettings::OnBkgSolid()
{
	UpdateData();
	m_bBkgCapture = FALSE;
	m_bBkgVideo = FALSE;
	m_bBkgOn = FALSE;
	UpdateData(FALSE);
	UpdateControls();
}

void CImageSettings::OnBkgMix()
{
	UpdateData();
	UpdateControls();
}

void CImageSettings::UpdateControls()
{
	m_Capture.EnableWindow(m_bBkgCapture);
	m_Video.EnableWindow(m_bBkgVideo);
	m_Image.EnableWindow(m_bBkgOn);
	m_Color.EnableWindow(m_bBkgSolid);
	m_BkgMix.EnableWindow(m_bBkgVideo || m_bBkgOn || m_bBkgSolid);
	m_MixDimmer.EnableWindow(m_bBkgMix && m_BkgMix.IsWindowEnabled());
	m_StaticOff.EnableWindow(m_bBkgMix && m_BkgMix.IsWindowEnabled());
	m_StaticFull.EnableWindow(m_bBkgMix && m_BkgMix.IsWindowEnabled());
	m_MixAmt.EnableWindow(m_bBkgMix && m_BkgMix.IsWindowEnabled());
}

void CImageSettings::OnTransition()
{
	UpdateData();
	m_StaticSlow.EnableWindow(m_bTransition);
	m_StepDimmer.EnableWindow(m_bTransition);
	m_StaticFast.EnableWindow(m_bTransition);
	m_StepAmt.EnableWindow(m_bTransition);
}

void CImageSettings::OnOK()
{
	UpdateData();

	CString csCaption,csMessage;
	if (m_bBkgOn && m_csBkgImage.GetLength() == 0)
	{
		csCaption = g_pApp->QBString(259);
		csMessage = g_pApp->QBString(106);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		return;
	}
	else if (m_bBkgVideo && m_csBkgVideo.GetLength() == 0)
	{
		csCaption = g_pApp->QBString(259);
		csMessage = g_pApp->QBString(354);
		CMessageBox MsgBox(csCaption,csMessage,CMessageBox::OK);
		MsgBox.DoModal();
		return;
	}
	EndDialog(IDOK);
}