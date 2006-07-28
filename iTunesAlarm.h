// iTunesAlarm.h : main header file for the iTunesAlarm application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CiTunesAlarmApp:
// See iTunesAlarm.cpp for the implementation of this class
//

class CiTunesAlarmApp : public CWinApp
{
public:
	CiTunesAlarmApp();

	CArray<HWND, HWND> m_aryFrames;
public:

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	DECLARE_MESSAGE_MAP()
};

extern CiTunesAlarmApp theApp;