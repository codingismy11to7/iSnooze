// MainFrm.h : interface of the CMainFrame class
//
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

#include "ChildView.h"
#include "SystemTray.h"
#include "ConfigDlg.h"

#include <list>

#define WM_ICON_NOTIFY  WM_APP+10
#define WM_CONFIG_UPDATE WM_APP+11
#define WM_DO_ALARM WM_APP+12
#define WM_SET_VOLUME WM_APP+13
#define WM_USER_RETURNED WM_APP+14
#define WM_USER_GONE WM_APP+15
#define WM_CLOSE_ITI WM_APP+16

typedef enum { VI_NOOP, VI_INCREASE, VI_PAUSE } VolumeIncreaseState;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
    void timeCheck();
	void setVolume( UINT level );
	void closeITI();
	int getVolumeLength()
	{ return m_inclength; }
    VolumeIncreaseState increasingVolume()
    { return m_volumeIncreasing; }
    bool snoozeDlgOpened()
    { return m_snoozeDlgOpen; }

    void userReturned();
    void userGone();

	void StartTray();

// Generated message map functions
protected:
	CSystemTray m_systray;
	CMutex m_systrayMutex;
	CConfigDlg *m_config;
	RegMap m_reg;

    /*long m_hour;
    long m_minute;
    long m_thour;
    long m_tminute;*/
	//DayTime::TimeAndDays m_alarmTime;
	DayTime::TimeAndDays m_snoozeAlarmTime;
	CMutex m_snoozeTimeMutex;
    volatile bool m_snoozing;
    ITID m_pls;
	tstring m_plsname;
    volatile bool m_shuffle;
	volatile bool m_increase;
    volatile bool m_minimize;
    volatile bool m_enableSnooze;
    volatile unsigned char m_snoozeTime;
	volatile int m_inclength;
    volatile VolumeIncreaseState m_volumeIncreasing;
    volatile bool m_muteOnRet;
    volatile bool m_snoozeDlgOpen;

	volatile bool m_alarmEnabled;

	volatile bool m_oneAlarmExists;
	volatile bool m_oneAlarmEnabled;

    DWORD m_mainThread;

	std::vector< std::list<DayTime::TimeAndDays*> > m_alarms;
	CMutex m_alarmsMutex;

    void LoadReg();
	void InitReg();
	void SetToolTip();

	void SetIcon();

	afx_msg void OnFileClose();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnTrayNotification(UINT wParam, LONG lParam);
    afx_msg LRESULT OnConfigUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT DoAlarm(UINT wParam, LONG lParam);
	afx_msg LRESULT DoCloseITI(UINT wParam, LONG lParam);
	afx_msg LRESULT OnSetVolume(UINT wParam, LONG lParam);
    afx_msg LRESULT OnUserReturned(UINT wParam, LONG lParam);
    afx_msg LRESULT OnUserGone(UINT wParam, LONG lParam);
	afx_msg void OnAppConfigure();
	afx_msg void OnPoopTestbubble();
    afx_msg void OnTestLaunch();
	afx_msg void OnAlarmEnabled();
	afx_msg void OnAppCancelsnooze();
	afx_msg void OnUpdateAppCancelsnooze(CCmdUI *pCmdUI);
};


