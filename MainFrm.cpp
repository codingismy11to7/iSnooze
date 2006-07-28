// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "iTunesAlarm.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_APP_CONFIGURE, OnAppConfigure)
	ON_COMMAND(ID_POOP_TESTBUBBLE, OnPoopTestbubble)
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_config(NULL)
{
	m_reg = RegMap( HKEY_CURRENT_USER )[_T("Software")];
	if( !m_reg.has_key( _T("iTunesAlarm") ) )
		m_reg[_T("iTunesAlarm")] = RegMap();
	m_reg = m_reg[_T("iTunesAlarm")];
}

CMainFrame::~CMainFrame()
{
	delete m_config;
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
		MessageBox( _T("This should be a snooze dialog, or stop, or something") );
	else
		return m_systray.OnTrayNotification(wParam, lParam);
    return 1;
}

void CMainFrame::OnAppConfigure()
{
	//MessageBox(L"uehaon");
	if( !m_config )
		m_config = new CConfigDlg( m_reg, IDD_CONFIG_DIALOG, this );
	
	/*if( config.DoModal() == IDOK )
	{
		//save settings
		CButton *chk = (CButton*)config.GetDlgItem( IDC_CHECK1 );
		if( chk->GetCheck() == BST_CHECKED )
			MessageBox( _T("checked") );
		else
			MessageBox( _T("not checked") );
	}
	else
		MessageBox( _T("not saving") );*/
	m_config->Show();
}

void CMainFrame::OnPoopTestbubble()
{
	m_systray.ShowBalloon( _T("This is a sample bubble popup thing.  It will most likely be used for snooze handling.  Click me!"),
		_T("WAKE UP"), NIIF_WARNING, 30 );
}
