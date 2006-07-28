// AboutDlg.cpp : implementation file
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
#include "AboutDlg.h"
#include ".\aboutdlg.h"


// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)
CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PAYPAL, m_donateButton);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_PAYPAL, OnBnClickedPaypal)
END_MESSAGE_MAP()


// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog(void)
{
	m_donateButton.AutoLoad( IDC_PAYPAL, this );

	CString tip;
	tip.LoadString( IDS_PAYPAL_TIP );

	m_donateButton.SetToolTipText( &tip );

	return TRUE;
}

void CAboutDlg::OnBnClickedPaypal()
{
	ShellExecute( GetSafeHwnd(), _T("open"), _T("https://www.paypal.com/xclick/business=progoth%40gmail.com&no_shipping=1&item_name=Donation%20to%20iTunesAlarm!&no_note=1&tax=0&currency_code=USD"),
		NULL, NULL, 0 );
}
