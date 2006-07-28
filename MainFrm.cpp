// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "iTunesAlarm.h"
#include "ITI.h"

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
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_APP_CONFIGURE, OnAppConfigure)
	ON_COMMAND(ID_POOP_TESTBUBBLE, OnPoopTestbubble)
    ON_COMMAND(ID_APP_TESTLAUNCH, OnTestLaunch)
END_MESSAGE_MAP()

DWORD WINAPI TimerThread( LPVOID param )
{
    CMainFrame *caller = (CMainFrame*)param;
    SYSTEMTIME t;
    GetLocalTime( &t );
    if( t.wSecond )
        Sleep( ( 60 - t.wSecond ) * 1000 ); //sleep until this minute completes
    while( true )
    {
        caller->timeCheck();
        Sleep(60000);
    }
    return 0;
}

DWORD WINAPI VolumeThread( LPVOID param )
{
	CMainFrame *caller = (CMainFrame*)param;
	int wait = 10 * caller->getVolumeLength(); //1000 ms per second, 100 steps
	for( int i = 0; i < 100; i++ )
	{
		Sleep( wait );
		caller->setVolume( i + 1 );
	}
	caller->closeITI();
	return 0;
}

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_config(NULL)
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
    if( time.wHour == m_hour && time.wMinute == m_minute )
    {
		PostMessage( WM_DO_ALARM, 0, 0 );
    }
}

void CMainFrame::setVolume( UINT level )
{
	PostMessage(WM_SET_VOLUME, level, 0);
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
    bool y = ( m_mainThread == GetCurrentThreadId() );
    m_systray.ShowBalloon( _T("Starting alarm, we'll see what this will do later."),
        _T("DANGER DANGER"), NIIF_WARNING, 30 );
    try
    {
        ITI::Connect();
        ITI::PlayPlaylist( m_pls, m_shuffle );
        ITI::Disconnect();
    }
    catch( trterror &e )
    {
        MessageBox( e.error().c_str() );
    }

	try
	{
		ITI::Connect();
		ITI::ZeroVolume();
		CreateThread( NULL, 0, VolumeThread, this, 0, NULL );
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
    //binary b;
    if( !m_reg.has_key( _T("PlaylistName") ) )
        m_reg[_T("PlaylistName")] = _T("");//b;
    if( !m_reg.has_key( _T("Shuffle") ) )
        m_reg[_T("Shuffle")] = (DWORD)0;
	if( !m_reg.has_key( _T("IncreaseVolume") ) )
		m_reg[_T("IncreaseVolume")] = (DWORD)0;
	if( !m_reg.has_key( _T("IncreaseTime") ) )
		m_reg[_T("IncreaseTime")] = (DWORD)10;
}

void CMainFrame::LoadReg()
{
	m_hour = m_reg[_T("Hour")];
    m_minute = m_reg[_T("Minute")];
    m_shuffle = (m_reg[_T("Shuffle")] != 0);
    //m_pls.fromBin( ((binary)m_reg[_T("Playlist")]).data.c_str() );
    m_pls = m_reg[_T("PlaylistName")];
	m_increase = ( (m_reg[_T("IncreaseVolume")] == 0) ) ? false : true;
	m_inclength = m_reg[_T("IncreaseTime")];

	static TCHAR buf[200];
	_stprintf( buf, _T("iTooonz Alaaarrrm!!\nAlarm Set for %d:%02d"), m_hour, m_minute );
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
	HICON ico = ::LoadIcon( AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME) );
	m_systray.Create( this, WM_ICON_NOTIFY, _T("iTooonz Alaaarrrm!!"), ico, IDR_POPUP_MENU );

	InitReg();
    LoadReg();

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
	m_systray.ShowBalloon( _T("This is a sample bubble popup thing.  It will most likely be used for snooze handling.  Click me!"),
		_T("WAKE UP"), NIIF_WARNING, 30 );
}

void CMainFrame::OnTestLaunch()
{
    DoAlarm(0,0);
}
