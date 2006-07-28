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
	: m_reg(reg), m_addingNew(false), m_curAlarmsSelection(-1), m_deleting(false)
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

	tip.LoadString( IDS_ADD_TIP );
	m_addButton.SetToolTipText( &tip );

	tip.LoadString( IDS_DELETE_TIP );
	m_deleteButton.SetToolTipText( &tip );

	m_daysBox.SetUseTabOrder(TRUE);

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

void CConfigDlg::LoadAlarmsFromReg()
{
	binary b = m_reg[_T("Alarms")];
	m_alarms.clear();
	DayTime::binToCont( b.data, m_alarms );
}

void CConfigDlg::SaveAlarmsToReg()
{
	/*std::set<DayTime::TimeAndDays,DayTime::lttm> t;
	std::vector<DayTime::TimeAndDays>::const_iterator i, end = m_alarms.end();
	for( i = m_alarms.begin(); i != end; ++i )
		t.insert( *i );*/
	std::vector<DayTime::TimeAndDays> t;
	std::vector<DayTime::TimeAndDays>::const_iterator o, oe, i, end = m_alarms.end();
	for( i = m_alarms.begin(); i != end; ++i )
	{
		bool found = false;
		oe = t.end();
		for( o = t.begin(); o != oe; ++o )
		{
			if( *i == *o )
			{
				found = true;
				break;
			}
		}
		if( !found )
			t.push_back( *i );
	}

	binary bin;
	DayTime::contToBin( t/*m_alarms*/, bin.data );

	m_reg[_T("Alarms")] = bin;
}

void CConfigDlg::FillAlarmsList()
{
	m_alarmsList.ResetContent();
	std::vector<DayTime::TimeAndDays>::const_iterator i, end = m_alarms.end();
	for( i = m_alarms.begin(); i != end; ++i )
	{
		m_alarmsList.AddString( i->getString().c_str() );
	}
	if( !m_alarms.empty() )
	{
		m_alarmsList.SetCurSel( 0 );
		OnCbnSelchangeAlarmCombo();
	}
	m_deleteButton.EnableWindow( ((m_alarms.empty())?FALSE:TRUE) );
}

void CConfigDlg::SetTimeDlg()
{
	if( m_alarmsList.GetCurSel() == CB_ERR )
	{
		DayTime::TimeAndDays tmp;
		SetDlgTimeFromTAD( tmp );
		m_daysBox.EnableWindow( FALSE );
	}
	else
	{
		m_daysBox.EnableWindow( TRUE );
		SetDlgTimeFromTAD( m_alarms[m_alarmsList.GetCurSel()] );
	}
}

void CConfigDlg::LoadFromReg()
{
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

	//LoadTimeFromReg();
	LoadAlarmsFromReg();
	FillAlarmsList();
	SetTimeDlg();
}

void CConfigDlg::LoadTimeFromReg()
{
	DayTime::TimeAndDays tt;
	tt.setFromBinary( ((binary)m_reg[_T("AlarmTime")]).data.c_str() );
	SetDlgTimeFromTAD( tt );
}

void CConfigDlg::SetDlgTimeFromTAD( const DayTime::TimeAndDays &in )
{
	int hour, minute;
	in.getTime( hour, minute );

	((CComboBox*)GetDlgItem( IDC_HOURS ))->SetCurSel( hour );
    ((CComboBox*)GetDlgItem( IDC_MINS ))->SetCurSel( minute );

	((CButton*)GetDlgItem(IDC_SUNDAY_CHECK))->SetCheck( ((in.day & DayTime::SUNDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_MONDAY_CHECK))->SetCheck( ((in.day & DayTime::MONDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_TUESDAY_CHECK))->SetCheck( ((in.day & DayTime::TUESDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_WEDNESDAY_CHECK))->SetCheck( ((in.day & DayTime::WEDNESDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_THURSDAY_CHECK))->SetCheck( ((in.day & DayTime::THURSDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_FRIDAY_CHECK))->SetCheck( ((in.day & DayTime::FRIDAY)?BST_CHECKED:BST_UNCHECKED) );
	((CButton*)GetDlgItem(IDC_SATURDAY_CHECK))->SetCheck( ((in.day & DayTime::SATURDAY)?BST_CHECKED:BST_UNCHECKED) );
}

void CConfigDlg::SaveTimeToReg()
{
	DayTime::TimeAndDays tt;
	GetTADFromDlgTime( tt );

	binary b;
	b.data.resize(3);
	tt.getToBinary( (BYTE*)b.data.c_str() );

	m_reg[_T("AlarmTime")] = b;
}

void CConfigDlg::GetTADFromDlgTime( DayTime::TimeAndDays &out )
{
	out.setTime( ((CComboBox*)GetDlgItem(IDC_HOURS))->GetCurSel(), ((CComboBox*)GetDlgItem(IDC_MINS))->GetCurSel() );

	if( ((CButton*)GetDlgItem(IDC_SUNDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::SUNDAY;
	if( ((CButton*)GetDlgItem(IDC_MONDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::MONDAY;
	if( ((CButton*)GetDlgItem(IDC_TUESDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::TUESDAY;
	if( ((CButton*)GetDlgItem(IDC_WEDNESDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::WEDNESDAY;
	if( ((CButton*)GetDlgItem(IDC_THURSDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::THURSDAY;
	if( ((CButton*)GetDlgItem(IDC_FRIDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::FRIDAY;
	if( ((CButton*)GetDlgItem(IDC_SATURDAY_CHECK))->GetCheck() == BST_CHECKED ) out.day |= DayTime::SATURDAY;
}

void CConfigDlg::SelectPlaylist()
{
    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    ITID cur;
    cur.fromBin( ((binary)(m_reg[_T("Playlist")])).data.c_str() );
    tstring curstr = m_reg[_T("PlaylistName")];
	bool found = false;
    for( ULONG j = 0; j < m_playlists.size(); j++ )
    {
        if( cur == m_plids[j] )
        //if( cur == m_playlists[j] )
		{
            playlists->SetCurSel( j );
			found = true;
		}
    }
	if( found ) return;
	for( ULONG j = 0; j < m_playlists.size(); j++ )
	{
		if( curstr == m_playlists[j] )
			playlists->SetCurSel( j );
	}
}

void CConfigDlg::FillBoxes()
{
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
	DDX_Control(pDX, IDC_ADD, m_addButton);
	DDX_Control(pDX, IDC_DELETE, m_deleteButton);
	DDX_Control(pDX, IDC_DAYS_GROUP, m_daysBox);
	DDX_Control(pDX, IDC_ALARM_COMBO, m_alarmsList);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TTN_NEEDTEXT, NULL, OnNeedText)
	ON_BN_CLICKED(IDC_INCREASE_CHECK, OnBnClickedIncreaseCheck)
    ON_BN_CLICKED(IDC_ENABLE_SNOOZE, OnBnClickedEnableSnooze)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_CBN_SELCHANGE(IDC_ALARM_COMBO, OnCbnSelchangeAlarmCombo)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_CBN_SELCHANGE(IDC_HOURS, OnCbnSelchangeHours)
	ON_CBN_SELCHANGE(IDC_MINS, OnCbnSelchangeMins)
	ON_BN_CLICKED(IDC_SUNDAY_CHECK, OnBnClickedSundayCheck)
	ON_BN_CLICKED(IDC_MONDAY_CHECK, OnBnClickedMondayCheck)
	ON_BN_CLICKED(IDC_TUESDAY_CHECK, OnBnClickedTuesdayCheck)
	ON_BN_CLICKED(IDC_WEDNESDAY_CHECK, OnBnClickedWednesdayCheck)
	ON_BN_CLICKED(IDC_THURSDAY_CHECK, OnBnClickedThursdayCheck)
	ON_BN_CLICKED(IDC_FRIDAY_CHECK, OnBnClickedFridayCheck)
	ON_BN_CLICKED(IDC_SATURDAY_CHECK, OnBnClickedSaturdayCheck)
END_MESSAGE_MAP()

// CConfigDlg message handlers

void CConfigDlg::GetPlaylists()
{
	try {
		ITI::Connect();
		ITI::GetPlaylists( m_playlists, m_plids );
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
    m_plids.clear();
    GetPlaylists();
    FillBoxes();
    if( selected < playlists->GetCount() )
        playlists->SetCurSel( selected );
    else
        SelectPlaylist();
}

void CConfigDlg::OnBnClickedOk()
{
	SaveAlarmsToReg();

    CComboBox *playlists = (CComboBox*)GetDlgItem( IDC_PLAYLISTS );
    int pl = playlists->GetCurSel();
    if( pl != -1 )
    {
        long id[4];
        m_plids[pl].toBin( (void*)id );
        binary b;
        b.data = std::string( (char*)id, 4 * sizeof(long) );
        m_reg[_T("Playlist")] = b;
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


void CConfigDlg::OnBnClickedDelete()
{
	int ret = MessageBox( _T("Are you sure you want to delete this alarm?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO );
	if( ret == IDNO )
		return;

	int sel = m_alarmsList.GetCurSel();
	m_alarms.erase( m_alarms.begin() + sel );
	m_alarmsList.SetCurSel( sel - 1 );
	m_alarmsList.DeleteString( sel );
	if( sel == 0 && m_alarmsList.GetCount() != CB_ERR )
		m_alarmsList.SetCurSel( 0 );
	if( !m_alarmsList.GetCount() )
	{
		DayTime::TimeAndDays t;
		SetDlgTimeFromTAD( t );
		m_deleteButton.EnableWindow( FALSE );
		m_daysBox.EnableWindow( FALSE );
	}
	else
		OnCbnSelchangeAlarmCombo();
}

void CConfigDlg::OnCbnSelchangeAlarmCombo()
{
	SetTimeDlg();
}

void CConfigDlg::OnBnClickedAdd()
{
	DayTime::TimeAndDays tmp( 8, 0, 0 );
	m_alarms.push_back( tmp );
	m_alarmsList.AddString( tmp.getString().c_str() );
	m_alarmsList.SetCurSel( m_alarmsList.GetCount() - 1 );
	m_daysBox.EnableWindow( TRUE );
	m_deleteButton.EnableWindow( TRUE );
	SetDlgTimeFromTAD( tmp );
}

void CConfigDlg::OnCbnSelchangeHours()
{
	int hour, minute;
	m_alarms[m_alarmsList.GetCurSel()].getTime( hour, minute );
	m_alarms[m_alarmsList.GetCurSel()].setTime( ((CComboBox*)GetDlgItem(IDC_HOURS))->GetCurSel(), minute );
	RedrawCurrentAlarmItem();
}

void CConfigDlg::RedrawCurrentAlarmItem()
{
	int sel = m_alarmsList.GetCurSel();
	m_alarmsList.SetCurSel( -1 );
	m_alarmsList.DeleteString( sel );
	m_alarmsList.InsertString( sel, m_alarms[sel].getString().c_str() );
	m_alarmsList.SetCurSel( sel );
}
void CConfigDlg::OnCbnSelchangeMins()
{
	int hour, minute;
	m_alarms[m_alarmsList.GetCurSel()].getTime( hour, minute );
	m_alarms[m_alarmsList.GetCurSel()].setTime( hour, ((CComboBox*)GetDlgItem(IDC_MINS))->GetCurSel() );
	RedrawCurrentAlarmItem();
}

void CConfigDlg::DoDayCheck(UINT id, BYTE val)
{
	if( ((CButton*)GetDlgItem(id))->GetCheck() == BST_CHECKED )
		m_alarms[m_alarmsList.GetCurSel()].day |= val;
	else
		m_alarms[m_alarmsList.GetCurSel()].day &= ~val;
	RedrawCurrentAlarmItem();
}

void CConfigDlg::OnBnClickedSundayCheck()
{
	DoDayCheck( IDC_SUNDAY_CHECK, DayTime::SUNDAY );
}

void CConfigDlg::OnBnClickedMondayCheck()
{
	DoDayCheck( IDC_MONDAY_CHECK, DayTime::MONDAY );
}

void CConfigDlg::OnBnClickedTuesdayCheck()
{
	DoDayCheck( IDC_TUESDAY_CHECK, DayTime::TUESDAY );
}

void CConfigDlg::OnBnClickedWednesdayCheck()
{
	DoDayCheck( IDC_WEDNESDAY_CHECK, DayTime::WEDNESDAY );
}

void CConfigDlg::OnBnClickedThursdayCheck()
{
	DoDayCheck( IDC_THURSDAY_CHECK, DayTime::THURSDAY );
}

void CConfigDlg::OnBnClickedFridayCheck()
{
	DoDayCheck( IDC_FRIDAY_CHECK, DayTime::FRIDAY );
}

void CConfigDlg::OnBnClickedSaturdayCheck()
{
	DoDayCheck( IDC_SATURDAY_CHECK, DayTime::SATURDAY );
}
