// iTunesAlarm.cpp : Defines the class behaviors for the application.
//
// Copyright (c) 2004, Steven Scott (progoth@gmail.com)
//
// This file is part of iTunesAlarm.
//
// iTunesAlarm is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// iTunesAlarm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with iTunesAlarm; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "stdafx.h"
#include "iTunesAlarm.h"
#include "MainFrm.h"
#include "LimitSingleInstance.h"
#include "afxext.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CiTunesAlarmApp

BEGIN_MESSAGE_MAP(CiTunesAlarmApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW_FRAME, OnFileNewFrame)
END_MESSAGE_MAP()


// CiTunesAlarmApp construction

CiTunesAlarmApp::CiTunesAlarmApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CiTunesAlarmApp object

CiTunesAlarmApp theApp;

CLimitSingleInstance g_Single( _T("1a1f3d44-b045-42af-a92c-b8a86503844e") );

// CiTunesAlarmApp initialization

BOOL CiTunesAlarmApp::InitInstance()
{
    if( g_Single.IsAnotherInstanceRunning() )
        return FALSE;

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_iTunesAlarmTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_iTunesAlarmTYPE));
	// The one and only window has been initialized, so show and update it
	/*pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();*/
	pFrame->StartTray();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
    CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY );
	return TRUE;
}


// CiTunesAlarmApp message handlers

void CiTunesAlarmApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CiTunesAlarmApp message handlers

void CiTunesAlarmApp::OnFileNewFrame() 
{
	CMainFrame* pFrame = new CMainFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, 
					  NULL, NULL);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	m_aryFrames.Add(pFrame->GetSafeHwnd());
}
