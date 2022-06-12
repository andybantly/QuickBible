// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Copyright (C) 2007 Andrew S. Bantly

#if !defined(AFX_STDAFX_H__238D42C6_9DDE_4521_B813_B8251721F9A8__INCLUDED_)
#define AFX_STDAFX_H__238D42C6_9DDE_4521_B813_B8251721F9A8__INCLUDED_

// Automatically link with VFW32.LIB and WINMM.LIB
#pragma comment(lib,"vfw32")
#pragma comment(lib,"winmm")

// Automatically link with other needed libraries
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"comsuppw.lib")
#pragma comment(lib,"version.lib")
#pragma comment(lib,"wsock32.lib")
#define WINVER 0x0501

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winsock2.h>
#include <afxsock.h>
#include "iphlpapi.h"

// SMTP
#include "AuthSMTPConnection.h"

// Lists
#define MASK (LVIS_FOCUSED | LVIS_SELECTED)
#define SELECTED (LVIS_SELECTED)
#define UNSELECTED (NULL)

// Editing
#define WM_RAPIDEDIT (WM_APP + 1)
#define WM_BKGIMG (WM_APP + 2)
#define WM_TXTCOLOR (WM_APP + 3)
#define WM_BKGCOLOR (WM_APP + 4)
#define WM_CHOOSEFONT (WM_APP + 5)
#define WM_RESIZEMB (WM_APP + 6)
#define WM_PREVVIDEO (WM_APP + 7)
#define WM_FULLVIDEO (WM_APP + 8)

// DIB Frame/Thread
#define WM_MIXFRAME (WM_APP + 9)
#define WM_TRANSFRAME (WM_APP + 10)
#define WM_STARTCAPTURE (WM_APP + 11)
#define WM_STOPCAPTURE (WM_APP + 12)
#define WM_ENDTHREAD (WM_APP + 13)

// Files
#define _MAX_PATHNAME (_MAX_DIR + _MAX_DRIVE + _MAX_EXT + _MAX_FNAME + _MAX_PATH)

// Multimedia timers
#define TARGET_RESOLUTION 1
#define MMDELAY 15
#define MMRES 0

// Debugging
void DebugMsg(const char * pwszFormat,...);

// Debugging timer used for detecting the duration of an operation
static CString g_csDuration;
class CDuration
{
   const char * p;
   clock_t start;
   clock_t finish;
public:
   CDuration(const char * sz) : p(sz),start(clock()),finish(start) {};
   ~CDuration()
   {
      finish = clock();
      double duration = (double)(finish - start) / CLOCKS_PER_SEC;
      g_csDuration.Format("%s Duration = %f seconds\n",p,duration);
      OutputDebugString(g_csDuration);
   }
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__238D42C6_9DDE_4521_B813_B8251721F9A8__INCLUDED_)
