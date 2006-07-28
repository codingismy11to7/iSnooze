// Copyright (c) 2004, Steven Scott (progoth@gmail.com)
//
// This file is part of iSnooze.
//
// iSnooze is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// iSnooze is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with iSnooze; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#pragma once
#include "afxext.h"
#include "afxwin.h"

// CSnoozeDlg dialog

class CSnoozeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSnoozeDlg)

public:
	CSnoozeDlg(CWnd* pParent = NULL, bool enableSnooze = true );   // standard constructor
	virtual ~CSnoozeDlg();

// Dialog Data
	enum { IDD = IDD_SNOOZE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    void OnOK();
    void OnCancel();

    BOOL OnInitDialog();

    CFont m_stopFont;
    CFont m_snoozeFont;

    bool m_enableSnooze;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedStopButton();
protected:
    CButton m_stopButton;
};
