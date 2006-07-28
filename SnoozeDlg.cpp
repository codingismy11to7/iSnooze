// SnoozeDlg.cpp : implementation file
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
