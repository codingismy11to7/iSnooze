// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "ChildView.h"
#include "SystemTray.h"
#include "ConfigDlg.h"

#define WM_ICON_NOTIFY  WM_APP+10

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
	
	void StartTray();

// Generated message map functions
protected:
	CSystemTray m_systray;
	CConfigDlg *m_config;
	RegMap m_reg;

	afx_msg void OnFileClose();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnTrayNotification(UINT wParam, LONG lParam);
	afx_msg void OnAppConfigure();
};


