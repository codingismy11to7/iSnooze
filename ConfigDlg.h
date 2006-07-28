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

#pragma once

#include "RegMap.h"
#include "ITI.h"
#include "ToolTipButton.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "groupcontrol.h"

// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(const RegMap &reg, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

    void Show()
    {     LoadFromReg(); ShowWindow( SW_SHOW ); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    void LoadFromReg();
    void FillBoxes();
    void GetPlaylists();
    void SelectPlaylist();
	void LoadTimeFromReg();
	void SaveTimeToReg();

    BOOL OnInitDialog();

    RegMap m_reg;
    std::vector<tstring> m_playlists;
    std::vector<ITID> m_plids;

    HWND m_parentHwnd;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedOk();
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnNeedText(NMHDR *pnmh, LRESULT *pResult);
protected:
	CToolTipButton m_increase;
    CToolTipButton m_minimize;
	CSliderCtrl m_secondsSlider;
public:
	afx_msg void OnBnClickedIncreaseCheck();
protected:
    CEdit m_snoozeTime;
    CSpinButtonCtrl m_snoozeSpin;
public:
    afx_msg void OnBnClickedEnableSnooze();
protected:
	CToolTipButton m_muteCheck;
	CToolTipButton m_addButton;
	CToolTipButton m_deleteButton;
public:
	afx_msg void OnBnClickedDelete();
protected:
	CGroupControl m_daysBox;
};
