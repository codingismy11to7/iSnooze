// MainFrm.cpp : implementation of the CMainFrame class
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
#include "ITI.h"
#include "SnoozeDlg.h"

#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
    ON_MESSAGE(WM_CONFIG_UPDATE, OnConfigUpdate)
    ON_MESSAGE(WM_DO_ALARM, DoAlarm)
	ON_MESSAGE(WM_SET_VOLUME, OnSetVolume)
    ON_MESSAGE(WM_USER_RETURNED, OnUserReturned)
    ON_MESSAGE(WM_USER_GONE, OnUserGone)
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_APP_CONFIGURE, OnAppConfigure)
	ON_COMMAND(ID_POOP_TESTBUBBLE, OnPoopTestbubble)
    ON_COMMAND(ID_APP_TESTLAUNCH, OnTestLaunch)
	ON_COMMAND(ID_APP_ALARMENABLED, OnAlarmEnabled)
END_MESSAGE_MAP()

DWORD WINAPI TimerThread( LPVOID param )
{
    CMainFrame *caller = (CMainFrame*)param;
    SYSTEMTIME t;
    unsigned char i = 0;
    while( true )
    {
        caller->timeCheck();
        if( (i++ % 16) != 0 )
            Sleep(60000);
        else
        {
            GetLocalTime( &t );
            Sleep( ( 60 - t.wSecond ) * 1000 );  //reset our count every 16 minutes
        }
    }
    return 0;
}

DWORD WINAPI VolumeThread( LPVOID param )
{
	CMainFrame *caller = (CMainFrame*)param;
    int begin = 0;
    int end = 100;
    int increaseStep = 1;
	int wait = (increaseStep * 1000 * caller->getVolumeLength()) / (end - begin);
	for( int i = 0; (i < (end-begin)) && (caller->increasingVolume() != VI_NOOP); i+=increaseStep )
	{
        Sleep( wait );
        if( caller->increasingVolume() == VI_PAUSE )
        {
            Sleep( 100 );
            i-=increaseStep;
        }
        else
        {
		    caller->setVolume( i + begin + increaseStep );
        }
	}
	caller->closeITI();
	return 0;
}

DWORD WINAPI IdleThread( LPVOID param )
{
    CMainFrame *caller = (CMainFrame*)param;

    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);
    GetLastInputInfo( &lii );
    DWORD lastidle = lii.dwTime;
    while( caller->snoozeDlgOpened() && (lii.dwTime <= lastidle) )
    {
        Sleep(250);
        GetLastInputInfo( &lii );
        //lastidle = lii.dwTime;
    }
    caller->userReturned();

    return 0;
}

DWORD WINAPI CheckForAway( LPVOID param )
{
    CMainFrame *caller = (CMainFrame*)param;

    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);
    GetLastInputInfo( &lii );
    //DWORD lastinput = lii.dwTime;
    while( caller->snoozeDlgOpened() )
    {
        Sleep( 1000 );
        GetLastInputInfo( &lii );
        if( GetTickCount() - lii.dwTime >= 10000 )
        {
            caller->userGone();
            return 0;
        }
    }

    return 0;
}

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_config(NULL), m_snoozing(false)
{
	m_reg = RegMap( HKEY_CURRENT_USER )[_T("Software")];
	if( !m_reg.has_key( _T("iTunesAlarm") ) )
		m_reg[_T("iTunesAlarm")] = RegMap();
	m_reg = m_reg[_T("iTunesAlarm")];

    m_mainThread = GetCurrentThreadId();

    //MSG t;
    //PeekMessage(&t, NULL, WM_USER, WM_USER, PM_NOREMOVE);
}

CMainFrame::~CMainFrame()
{
	delete m_config;
}

void CMainFrame::timeCheck()
{
    static SYSTEMTIME time;
    GetLocalTime( &time );
    if( (!m_snoozing && time.wHour == m_hour && time.wMinute == m_minute)
        || (time.wHour == m_thour && time.wMinute == m_tminute ) )
    {
		if( m_alarmEnabled )
			PostMessage( WM_DO_ALARM, 0, 0 );
    }
}

void CMainFrame::setVolume( UINT level )
{
	PostMessage(WM_SET_VOLUME, level, 0);
}

void CMainFrame::userReturned()
{
    PostMessage(WM_USER_RETURNED, 0, 0);
}

void CMainFrame::userGone()
{
    PostMessage(WM_USER_GONE, 0, 0);
}

LRESULT CMainFrame::OnUserGone(UINT wParam, LONG lParam)
{
    if( !m_snoozeDlgOpen )
        return 1;
    if( m_volumeIncreasing == VI_PAUSE )
        m_volumeIncreasing = VI_INCREASE;

    ITI::Connect();
    ITI::Play();
    ITI::Disconnect();

    CreateThread( NULL, 0, IdleThread, this, 0, NULL );

    return 1;
}

LRESULT CMainFrame::OnUserReturned(UINT wParam, LONG lParam)
{
    if( !m_snoozeDlgOpen )
        return 1;
    if( m_volumeIncreasing == VI_INCREASE )
        m_volumeIncreasing = VI_PAUSE;

    ITI::Connect();
    ITI::Pause();
    ITI::Disconnect();

    CreateThread( NULL, 0, CheckForAway, this, 0, NULL );

    return 1;
}

LRESULT CMainFrame::OnSetVolume(UINT wParam, LONG lParam)
{
	if( GetCurrentThreadId() != m_mainThread ) return 1;
 	ITI::SetVolume( wParam );
	return 1;
}

void CMainFrame::closeITI()
{
	ITI::Disconnect();
}

LRESULT CMainFrame::DoAlarm(UINT wParam, LONG lParam)
{
    //bool y = ( m_mainThread == GetCurrentThreadId() );
    //m_systray.ShowBalloon( _T("Starting alarm, we'll see what this will do later."),
    //    _T("DANGER DANGER"), NIIF_WARNING, 30 );
    try
    {
        if( m_increase )
        {
            ITI::Connect();
            m_volumeIncreasing = VI_INCREASE;
            // this will disconnect() when it ends
            ITI::ZeroVolume();
            CreateThread( NULL, 0, VolumeThread, this, 0, NULL );
        }

        ITI::Connect();
        if( m_minimize ) ITI::Minimize();
        ITI::PlayPlaylist( m_pls, m_plsname, m_shuffle );
        ITI::Disconnect();

        if( m_muteOnRet )
            CreateThread( NULL, 0, IdleThread, this, 0, NULL );
        m_snoozeDlgOpen = true;
        CSnoozeDlg sd( this, m_enableSnooze );
        if( sd.DoModal() == IDOK ) // snooze
        {
            m_snoozeDlgOpen = false;
            ITI::Connect();
            ITI::Stop();
            ITI::Disconnect();
            m_volumeIncreasing = VI_NOOP;
            /*if( !m_snoozing ) // first snooze hit
            {
                m_thour = m_hour;
                m_tminute = m_minute;
            }*/
            m_snoozing = true;
            SYSTEMTIME tm;
            GetLocalTime( &tm );
            m_thour = tm.wHour;
            m_tminute = tm.wMinute + m_snoozeTime;
            if( m_tminute > 59 )
            {
                m_thour++;
                if( m_thour > 23 ) m_thour = 0;
                m_tminute-= 60;
            }
			SetToolTip();
        }
        else // stop
        {
            m_snoozeDlgOpen = false;
            ITI::Connect();
            ITI::Stop();
            ITI::Disconnect();
            m_volumeIncreasing = VI_NOOP;
            m_snoozing = false;
			SetToolTip();
        }
    }
    catch( trterror &e )
    {
        MessageBox( e.error().c_str() );
    }

    return 1;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

void CMainFrame::InitReg()
{
    if( !m_reg.has_key( _T("Hour") ) )
        m_reg[_T("Hour")] = (DWORD)8;
    if( !m_reg.has_key( _T("Minute") ) )
        m_reg[_T("Minute")] = (DWORD)0;
    binary b;
    if( !m_reg.has_key( _T("Playlist") ) )
        m_reg[_T("Playlist")] = b;
    if( !m_reg.has_key( _T("PlaylistName") ) )
        m_reg[_T("PlaylistName")] = _T("");
    if( !m_reg.has_key( _T("Shuffle") ) )
        m_reg[_T("Shuffle")] = false;
	if( !m_reg.has_key( _T("IncreaseVolume") ) )
		m_reg[_T("IncreaseVolume")] = false;
	if( !m_reg.has_key( _T("IncreaseTime") ) )
		m_reg[_T("IncreaseTime")] = (DWORD)10;
    if( !m_reg.has_key( _T("MinimizeOnAlarm") ) )
        m_reg[_T("MinimizeOnAlarm")] = true;
    if( !m_reg.has_key( _T("BeenRun") ) )
        m_reg[_T("BeenRun")] = false;
    if( !m_reg.has_key( _T("EnableSnooze") ) )
        m_reg[_T("EnableSnooze")] = true;
    if( !m_reg.has_key( _T("SnoozeTime") ) )
        m_reg[_T("SnoozeTime")] = 7L;
    if( !m_reg.has_key( _T("MuteOnReturn") ) )
        m_reg[_T("MuteOnReturn")] = true;
	if( !m_reg.has_key( _T("AlarmEnabled") ) )
		m_reg[_T("AlarmEnabled")] = true;

    RegMap t( HKEY_CURRENT_USER );
    t = t[_T("Software")][_T("Microsoft")][_T("Windows")][_T("CurrentVersion")];
    if( !t.has_key( _T("Run") ) )
      t[_T("Run")] = RegMap();
}

void CMainFrame::LoadReg()
{
	m_hour = m_reg[_T("Hour")];
    m_minute = m_reg[_T("Minute")];
    m_shuffle = m_reg[_T("Shuffle")];
    m_pls.fromBin( ((binary)m_reg[_T("Playlist")]).data.c_str() );
    m_plsname = m_reg[_T("PlaylistName")];
	m_increase = m_reg[_T("IncreaseVolume")];
	m_inclength = m_reg[_T("IncreaseTime")];
    m_minimize = m_reg[_T("MinimizeOnAlarm")];
    m_enableSnooze = m_reg[_T("EnableSnooze")];
    m_snoozeTime = (unsigned char)(0xff & (long)m_reg[_T("SnoozeTime")]);
    m_muteOnRet = m_reg[_T("MuteOnReturn")];

	m_alarmEnabled = m_reg[_T("AlarmEnabled")];
	if( m_alarmEnabled )
		m_systray.CheckMenuItem( ID_APP_ALARMENABLED, MF_CHECKED );
	else
		m_systray.CheckMenuItem( ID_APP_ALARMENABLED, MF_UNCHECKED );

	SetToolTip();
}

void CMainFrame::SetToolTip()
{
	static TCHAR buf[1024];
	if( m_alarmEnabled )
	{
		if( m_snoozing )
			_stprintf( buf, _T("iTooonz Alaaarrrm!!\nSnoozing until %d:%02d"), m_thour, m_tminute );
		else
			_stprintf( buf, _T("iTooonz Alaaarrrm!!\nAlarm Set for %d:%02d"), m_hour, m_minute );
	}
	else
		_tcscpy( buf, _T("iTooonz Alaaarrrm!!\nAlarm disabled") );
	m_systray.SetTooltipText( buf );
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == NULL)
		pApp->m_pMainWnd = this;

	// replace Exit option on File menu with Close for secondary windows
	if (AfxGetApp()->m_pMainWnd != this)
	{
		CMenu *pMenu = GetMenu();
		ASSERT(pMenu);
		pMenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND | MF_STRING, ID_FILE_CLOSE, _T("&Close"));
	}

	return TRUE;
}

void CMainFrame::StartTray()
{
	//m_systrayMenu.LoadMenu( IDR_POPUP_MENU );

	//HICON ico = ::LoadIcon( AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME) );
	HICON ico = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	m_systray.Create( this, WM_ICON_NOTIFY, _T("iTooonz Alaaarrrm!!"), ico, IDR_POPUP_MENU );

	InitReg();
    LoadReg();

    if( !((bool)m_reg[_T("BeenRun")]) )
        OnAppConfigure();

    CreateThread( NULL, 0, TimerThread, this, 0, NULL );
}

void CMainFrame::OnFileClose()
{
   DestroyWindow();
}

void CMainFrame::OnClose() 
{
	CiTunesAlarmApp *pApp = reinterpret_cast<CiTunesAlarmApp*>(AfxGetApp());
	// main window is being closed so cleanup
	if (pApp->m_pMainWnd == this)
	{
		for (int iCntr=0; iCntr < pApp->m_aryFrames.GetSize(); iCntr++)
		{
			HWND hFrame = pApp->m_aryFrames.GetAt(iCntr);
			if (::IsWindow(hFrame))
				::SendMessage(hFrame, WM_CLOSE, 0, 0);
		}
	}
	CFrameWnd::OnClose();
}

LRESULT CMainFrame::OnTrayNotification(UINT wParam, LONG lParam) 
{
	if( LOWORD(lParam) == NIN_BALLOONUSERCLICK )
    {
		//MessageBox( _T("This should be a snooze dialog, or stop, or something") );
    }
	else
		return m_systray.OnTrayNotification(wParam, lParam);
    return 1;
}

LRESULT CMainFrame::OnConfigUpdate(UINT wParam, LONG lParam)
{
    LoadReg();
    return 1;
}

void CMainFrame::OnAppConfigure()
{
	if( !m_config )
		m_config = new CConfigDlg( m_reg, this );

	m_config->Show();
}

void CMainFrame::OnPoopTestbubble()
{
	/*m_systray.ShowBalloon( _T("This is a sample bubble popup thing.  It will most likely be used for snooze handling.  Click me!"),
		_T("WAKE UP"), NIIF_WARNING, 30 );*/
    CSnoozeDlg x(this);
    x.DoModal();
}

void CMainFrame::OnTestLaunch()
{
    DoAlarm(0,0);
}

void CMainFrame::OnAlarmEnabled()
{
	if( m_systray.GetMenuItemChecked( ID_APP_ALARMENABLED ) == MF_CHECKED )
	{
		m_reg[_T("AlarmEnabled")] = m_alarmEnabled = false;
		m_systray.CheckMenuItem( ID_APP_ALARMENABLED, MF_UNCHECKED );
	}
	else
	{
		m_reg[_T("AlarmEnabled")] = m_alarmEnabled = true;
		m_systray.CheckMenuItem( ID_APP_ALARMENABLED, MF_CHECKED );
	}
	SetToolTip();
}
