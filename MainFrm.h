// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "ChildView.h"
#include "SystemTray.h"
#include "ConfigDlg.h"

#define WM_ICON_NOTIFY  WM_APP+10
#define WM_CONFIG_UPDATE WM_APP+11
#define WM_DO_ALARM WM_APP+12

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

	void StartTray();

// Generated message map functions
protected:
	CSystemTray m_systray;
	CConfigDlg *m_config;
	RegMap m_reg;

    long m_hour;
    long m_minute;
    tstring m_pls;
    bool m_shuffle;

    DWORD m_mainThread;

    void LoadReg();

	afx_msg void OnFileClose();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnTrayNotification(UINT wParam, LONG lParam);
    afx_msg LRESULT OnConfigUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT DoAlarm(UINT wParam, LONG lParam);
	afx_msg void OnAppConfigure();
	afx_msg void OnPoopTestbubble();
    afx_msg void OnTestLaunch();
};


