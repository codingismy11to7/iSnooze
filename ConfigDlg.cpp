// ConfigDlg.cpp : implementation file
//

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

	CString tip;
	tip.LoadString( IDS_INCREASE_TIP );
	m_increase.SetToolTipText( &tip );

    GetPlaylists();
    FillBoxes();
    m_parentHwnd = pParent->GetSafeHwnd();


	CSliderCtrl *secs = (CSliderCtrl*)GetDlgItem( IDC_SECONDS_SLIDER );

	secs->SetPageSize( 10 );
	secs->SetRangeMin( 10 );
	secs->SetRangeMax( 120 );
	secs->EnableWindow( false );
}

void CConfigDlg::LoadFromReg()
{
    long id[4];
    memset( id, 0, 4 * sizeof(long) );
    binary b;
    b.data = std::string( (char*)id, 4 * sizeof(long) );

    CComboBox *hours = (CComboBox*)GetDlgItem( IDC_HOURS );
    hours->SetCurSel( m_reg[_T("Hour")] );

    CComboBox *mins = (CComboBox*)GetDlgItem( IDC_MINS );
    mins->SetCurSel( m_reg[_T("Minute")] );

    SelectPlaylist();

    CButton *shuffle = (CButton*)GetDlgItem( IDC_CHECK_SHUFFLE );
    if( m_reg[_T("Shuffle")] == 0 )
        shuffle->SetCheck( BST_UNCHECKED );
    else
        shuffle->SetCheck( BST_CHECKED );

	m_secondsSlider.SetPos( m_reg[_T("IncreaseTime")] );
	m_increase.SetCheck( ((((int)m_reg[_T("IncreaseVolume")]) == 0) ? BST_UNCHECKED : BST_CHECKED) );
	OnBnClickedIncreaseCheck();
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
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TTN_NEEDTEXT, NULL, OnNeedText)
	ON_BN_CLICKED(IDC_INCREASE_CHECK, OnBnClickedIncreaseCheck)
END_MESSAGE_MAP()

//	ON_WM_HSCROLL()

// CConfigDlg message handlers

void CConfigDlg::GetPlaylists()
{
    //m_playlists.push_back(_T("Playlist 1"));
    //m_playlists.push_back(_T("Playlist 2"));
    ITI::Connect();
    ITI::GetPlaylists( m_playlists/*, m_plids*/ );
    ITI::Disconnect();
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
    if( shuffle->GetCheck() == BST_CHECKED )
        m_reg[_T("Shuffle")] = (DWORD)1;
    else
        m_reg[_T("Shuffle")] = (DWORD)0;


	m_reg[_T("IncreaseVolume")] = ( m_increase.GetCheck() == BST_CHECKED ) ? (DWORD)1 : (DWORD)0;
	m_reg[_T("IncreaseTime")] = m_secondsSlider.GetPos();

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
