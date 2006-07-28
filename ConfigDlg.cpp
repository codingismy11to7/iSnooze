// ConfigDlg.cpp : implementation file
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
#include ".\configdlg.h"
#include "MainFrm.h"

// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)
CConfigDlg::CConfigDlg(const RegMap &reg, CWnd* pParent /*=NULL*/)
	: m_reg(reg)
{
	Create( CConfigDlg::IDD, pParent );

    m_parentHwnd = pParent->GetSafeHwnd();
}

BOOL CConfigDlg::OnInitDialog()
{
    if( !CDialog::OnInitDialog() )
        return FALSE;

	CString tip;
	tip.LoadString( IDS_INCREASE_TIP );
	m_increase.SetToolTipText( &tip );

    tip.LoadString( IDS_MINIMIZE_TIP );
    m_minimize.SetToolTipText( &tip );

	tip.LoadString( IDS_PAUSE_TIP );
	m_muteCheck.SetToolTipText( &tip );

    GetPlaylists();
    FillBoxes();

    m_snoozeTime.SetWindowText( _T("1") );
    m_snoozeSpin.SetRange( 1, 60 );

	CSliderCtrl *secs = (CSliderCtrl*)GetDlgItem( IDC_SECONDS_SLIDER );

	secs->SetPageSize( 10 );
	secs->SetRangeMin( 10 );
	secs->SetRangeMax( 120 );
	secs->EnableWindow( false );

    SetFocus();
    SetForegroundWindow();
    GetDlgItem(IDC_PLAYLISTS)->SetFocus();

    return TRUE;
}

void CConfigDlg::LoadFromReg()
{
    /*long id[4];
    memset( id, 0, 4 * sizeof(long) );
    binary b;
    b.data = std::string( (char*)id, 4 * sizeof(long) );*/

    CComboBox *hours = (CComboBox*)GetDlgItem( IDC_HOURS );
    hours->SetCurSel( m_reg[_T("Hour")] );

    CComboBox *mins = (CComboBox*)GetDlgItem( IDC_MINS );
    mins->SetCurSel( m_reg[_T("Minute")] );

    SelectPlaylist();

    CButton *shuffle = (CButton*)GetDlgItem( IDC_CHECK_SHUFFLE );
    shuffle->SetCheck( (((bool)m_reg[_T("Shuffle")])?BST_CHECKED:BST_UNCHECKED) );

	m_secondsSlider.SetPos( m_reg[_T("IncreaseTime")] );
	m_increase.SetCheck( (((bool)m_reg[_T("IncreaseVolume")]) ? BST_CHECKED : BST_UNCHECKED) );
	OnBnClickedIncreaseCheck();

    ((CButton*)GetDlgItem(IDC_DO_MUTE))->SetCheck( (((bool)m_reg[_T("MuteOnReturn")]) ? BST_CHECKED : BST_UNCHECKED) );

    ((CButton*)GetDlgItem(IDC_ENABLE_SNOOZE))->SetCheck( (((bool)m_reg[_T("EnableSnooze")]) ? BST_CHECKED : BST_UNCHECKED) );
    OnBnClickedEnableSnooze();

    CButton *runatstartup = (CButton*)GetDlgItem( IDC_STARTUP_CHECK );
    RegMap t(HKEY_CURRENT_USER);
    t = t[_T("Software")][_T("Microsoft")][_T("Windows")][_T("CurrentVersion")][_T("Run")];
    runatstartup->SetCheck( ((t.has_key(_T("iTunesAlarm")))?BST_CHECKED:BST_UNCHECKED) );

    long st = 0xff & (long)m_reg[_T("SnoozeTime")];
    if( st < 1 ) st = 1; if( st > 60 ) st = 60;
    TCHAR tm[4];
    _stprintf( tm, _T("%d"), st );
    GetDlgItem(IDC_SNOOZE_TIME)->SetWindowText( tm );

    m_minimize.SetCheck( (((bool)m_reg[_T("MinimizeOnAlarm")])?BST_CHECKED:BST_UNCHECKED) );
}

void CConfigDlg::SelectPlaylist()
{
    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    //ITID cur;
    //cur.fromBin( ((binary)(m_reg[_T("Playlist")])).data.c_str() );
    tstring cur = m_reg[_T("PlaylistName")];
    for( ULONG j = 0; j < m_playlists.size(); j++ )
    {
        //if( cur == m_plids[j] )
        if( cur == m_playlists[j] )
            playlists->SetCurSel( j );
    }
}

void CConfigDlg::FillBoxes()
{
    /*CComboBox *hours = (CComboBox*)GetDlgItem( IDC_HOURS );
    TCHAR buf[3];
    for( ULONG j = 0; j < 24; j++ )
    {
        _stprintf( buf, _T("%02d"), j );
        hours->AddString( buf );
    }

    CComboBox *mins = (CComboBox*)GetDlgItem( IDC_MINS );
    for( ULONG j = 0; j < 60; j++ )
    {
        _stprintf( buf, _T("%02d"), j );
        mins->AddString( buf );
    }*/
    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    std::vector<tstring>::const_iterator i, end = m_playlists.end();
    for( i = m_playlists.begin(); i != end; ++i )
        playlists->AddString( i->c_str() );
}

CConfigDlg::~CConfigDlg()
{
	delete m_increase;
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INCREASE_CHECK, m_increase);
	DDX_Control(pDX, IDC_SECONDS_SLIDER, m_secondsSlider);
	DDX_Control(pDX, IDC_MINIMIZE_CHECK, m_minimize);
	DDX_Control(pDX, IDC_SNOOZE_TIME, m_snoozeTime);
	DDX_Control(pDX, IDC_SNOOZE_SPIN, m_snoozeSpin);
	DDX_Control(pDX, IDC_DO_MUTE, m_muteCheck);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TTN_NEEDTEXT, NULL, OnNeedText)
	ON_BN_CLICKED(IDC_INCREASE_CHECK, OnBnClickedIncreaseCheck)
    ON_BN_CLICKED(IDC_ENABLE_SNOOZE, OnBnClickedEnableSnooze)
END_MESSAGE_MAP()

//	ON_WM_HSCROLL()

// CConfigDlg message handlers

void CConfigDlg::GetPlaylists()
{
    //m_playlists.push_back(_T("Playlist 1"));
    //m_playlists.push_back(_T("Playlist 2"));
	try {
		ITI::Connect();
		ITI::GetPlaylists( m_playlists/*, m_plids*/ );
		ITI::Disconnect();
	}
	catch( trterror &e )
	{
		MessageBox( e.error().c_str(), _T("iTunes Error") );
	}
}

void CConfigDlg::OnBnClickedButton1()
{
    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    int selected = playlists->GetCurSel();
    playlists->ResetContent();
    m_playlists.clear();
    //m_plids.clear();
    GetPlaylists();
    FillBoxes();
    if( selected < playlists->GetCount() )
        playlists->SetCurSel( selected );
    else
        SelectPlaylist();
}

void CConfigDlg::OnBnClickedOk()
{
    CComboBox *hours = (CComboBox*)GetDlgItem( IDC_HOURS );
    m_reg[_T("Hour")] = hours->GetCurSel();

    CComboBox *mins = (CComboBox*)GetDlgItem( IDC_MINS );
    m_reg[_T("Minute")] = mins->GetCurSel();

    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    int pl = playlists->GetCurSel();
    if( pl != -1 )
    {
        /*long id[4];
        m_plids[pl].toBin( (void*)id );
        binary b;
        b.data = std::string( (char*)id, 4 * sizeof(long) );
        m_reg[_T("Playlist")] = b;*/
        m_reg[_T("PlaylistName")] = m_playlists[pl];
    }

    CButton *shuffle = (CButton*)GetDlgItem( IDC_CHECK_SHUFFLE );
    m_reg[_T("Shuffle")] = ( shuffle->GetCheck() == BST_CHECKED ) ? true : false;

	m_reg[_T("IncreaseVolume")] = ( m_increase.GetCheck() == BST_CHECKED ) ? true : false;
	m_reg[_T("IncreaseTime")] = m_secondsSlider.GetPos();

    m_reg[_T("MinimizeOnAlarm")] = ( m_minimize.GetCheck() == BST_CHECKED ) ? true : false;

    m_reg[_T("MuteOnReturn")] = ( ((CButton*)GetDlgItem(IDC_DO_MUTE))->GetCheck() == BST_CHECKED ) ? true : false;

    m_reg[_T("EnableSnooze")] = ( ((CButton*)GetDlgItem(IDC_ENABLE_SNOOZE))->GetCheck() == BST_CHECKED ) ? true : false;
    TCHAR tm[20];
    GetDlgItem(IDC_SNOOZE_TIME)->GetWindowText( tm, 19 );
    long st = 0xff & _tstol( tm );
    if( st < 1 ) st = 1; if( st > 60 ) st = 60;
    m_reg[_T("SnoozeTime")] = st;

    RegMap t( HKEY_CURRENT_USER );
    t = t[_T("Software")][_T("Microsoft")][_T("Windows")][_T("CurrentVersion")][_T("Run")];
    CButton *runonstartup = (CButton*)GetDlgItem( IDC_STARTUP_CHECK );
    if( runonstartup->GetCheck() == BST_CHECKED )
    {
        TCHAR f[MAX_PATH];
        GetModuleFileName( NULL, f, MAX_PATH );
        t[_T("iTunesAlarm")] = f;
    }
    else
    {
        if( t.has_key( _T("iTunesAlarm") ) )
            t.deleteValue( _T("iTunesAlarm") );
    }

    m_reg[_T("BeenRun")] = true;

    ::PostMessage( m_parentHwnd, WM_CONFIG_UPDATE, 0, 0 );

    OnOK();
}


void CConfigDlg::OnNeedText(NMHDR *pnmh, LRESULT *pResult)
{
	TOOLTIPTEXT *ttt = (TOOLTIPTEXT*)pnmh;
	if( ttt->hdr.idFrom == (UINT) m_secondsSlider.GetSafeHwnd() )
	{
		TCHAR buf[20];
		_stprintf( buf, _T("%d Seconds"), m_secondsSlider.GetPos() );
		_tcscpy( ttt->szText, buf );
	}
}

void CConfigDlg::OnBnClickedIncreaseCheck()
{
	m_secondsSlider.EnableWindow( ( (m_increase.GetCheck() == BST_CHECKED) ? true : false ) );
}

void CConfigDlg::OnBnClickedEnableSnooze()
{
    if( ((CButton*)GetDlgItem(IDC_ENABLE_SNOOZE))->GetCheck() == BST_CHECKED )
    {
        GetDlgItem(IDC_SNOOZE_TIME)->EnableWindow( true );
        GetDlgItem(IDC_SNOOZE_LABEL)->EnableWindow( true );
        GetDlgItem(IDC_SNOOZE_LABEL2)->EnableWindow( true );
    }
    else
    {
        GetDlgItem(IDC_SNOOZE_TIME)->EnableWindow( false );
        GetDlgItem(IDC_SNOOZE_LABEL)->EnableWindow( false );
        GetDlgItem(IDC_SNOOZE_LABEL2)->EnableWindow( false );
    }
}

