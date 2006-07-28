// SnoozeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iTunesAlarm.h"
#include "SnoozeDlg.h"
#include ".\snoozedlg.h"


// CSnoozeDlg dialog

IMPLEMENT_DYNAMIC(CSnoozeDlg, CDialog)
CSnoozeDlg::CSnoozeDlg(CWnd* pParent /*=NULL*/, bool enableSnooze /*= true*/ )
	: CDialog(CSnoozeDlg::IDD, pParent), m_enableSnooze(enableSnooze)
{
}

CSnoozeDlg::~CSnoozeDlg()
{
}

BOOL CSnoozeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_stopFont.CreatePointFont( 60, _T("Arial") );
    m_stopButton.SetFont( &m_stopFont );

    m_snoozeFont.CreatePointFont( 200, _T("Arial") );
    GetDlgItem(IDOK)->SetFont( &m_snoozeFont );

    GetDlgItem(IDOK)->EnableWindow( m_enableSnooze );

    SetFocus();
    SetForegroundWindow();

    return TRUE;
}

void CSnoozeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, ID_STOP_BUTTON, m_stopButton);
}

void CSnoozeDlg::OnCancel()
{
    return;
}

BEGIN_MESSAGE_MAP(CSnoozeDlg, CDialog)
    ON_BN_CLICKED(ID_STOP_BUTTON, OnBnClickedStopButton)
END_MESSAGE_MAP()


// CSnoozeDlg message handlers


void CSnoozeDlg::OnBnClickedStopButton()
{
    EndDialog(IDCANCEL);
}

void CSnoozeDlg::OnOK()
{
    CDialog::OnOK();
}