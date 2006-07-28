/////////////////////////////////////////////////////////////////////////////
// SystemTray.h : header file
//
// Written by Chris Maunder (cmaunder@mail.com)
// Copyright (c) 1998.
// http://www.codeproject.com/shell/systemtray.asp?df=100
//
// Additional code by Steven Scott (progoth@gmail.com)
// Copyright (c) 2004
//
// This file with my changes is licensed under the GPL, see
// above link for the original.
//
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
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_SYSTEMTRAY_H_
#define _INCLUDED_SYSTEMTRAY_H_

#ifdef NOTIFYICONDATA_V1_SIZE   // If NOTIFYICONDATA_V1_SIZE, then we can use fun stuff
#define SYSTEMTRAY_USEW2K
#else
#define NIIF_NONE 0
#endif

// #include <afxwin.h>
#include <afxtempl.h>
#include <afxdisp.h>    // COleDateTime

/////////////////////////////////////////////////////////////////////////////
// CSystemTray window

class CSystemTray : public CWnd
{
// Construction/destruction
public:
    CSystemTray();
    CSystemTray(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID, 
                BOOL bhidden = FALSE,
                LPCTSTR szBalloonTip = NULL, LPCTSTR szBalloonTitle = NULL, 
                DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);
    virtual ~CSystemTray();

    DECLARE_DYNAMIC(CSystemTray)

// Operations
public:
    BOOL Enabled() { return m_bEnabled; }
    BOOL Visible() { return !m_bHidden; }

    // Create the tray icon
    BOOL Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID,
                BOOL bHidden = FALSE,
                LPCTSTR szBalloonTip = NULL, LPCTSTR szBalloonTitle = NULL, 
                DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);

    // Change or retrieve the Tooltip text
    BOOL    SetTooltipText(LPCTSTR pszTooltipText);
    BOOL    SetTooltipText(UINT nID);
    CString GetTooltipText() const;

	UINT CheckMenuItem( UINT nIDCheckItem, UINT nCheck )
	{ return m_menu.CheckMenuItem( nIDCheckItem, nCheck ); }
	UINT GetMenuItemChecked( UINT nIDCheckItem, UINT nCheck = 0 );

    // Change or retrieve the icon displayed
    BOOL  SetIcon(HICON hIcon);
    BOOL  SetIcon(LPCTSTR lpszIconName);
    BOOL  SetIcon(UINT nIDResource);
    BOOL  SetStandardIcon(LPCTSTR lpIconName);
    BOOL  SetStandardIcon(UINT nIDResource);
    HICON GetIcon() const;

    void  SetFocus();
    BOOL  HideIcon();
    BOOL  ShowIcon();
    BOOL  AddIcon();
    BOOL  RemoveIcon();
    BOOL  MoveToRight();

    BOOL ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,
                     DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);

    // For icon animation
    BOOL  SetIconList(UINT uFirstIconID, UINT uLastIconID); 
    BOOL  SetIconList(HICON* pHIconList, UINT nNumIcons); 
    BOOL  Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  StepAnimation();
    BOOL  StopAnimation();

    // Change menu default item
    void GetMenuDefaultItem(UINT& uItem, BOOL& bByPos);
    BOOL SetMenuDefaultItem(UINT uItem, BOOL bByPos);

    // Change or retrieve the window to send notification messages to
    BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
    CWnd* GetNotificationWnd() const;

    // Change or retrieve the window to send menu commands to
    BOOL  SetTargetWnd(CWnd* pTargetWnd);
    CWnd* GetTargetWnd() const;

    // Change or retrieve  notification messages sent to the window
    BOOL  SetCallbackMessage(UINT uCallbackMessage);
    UINT  GetCallbackMessage() const;

    UINT_PTR GetTimerID() const   { return m_nTimerID; }

// Static functions
public:
    static void MinimiseToTray(CWnd* pWnd, BOOL bForceAnimation = FALSE);
    static void MaximiseFromTray(CWnd* pWnd, BOOL bForceAnimation = FALSE);

public:
    // Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSystemTray)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
    void Initialise();
    void InstallIconPending();

	virtual void CustomizeMenu(CMenu*) {}	// Used for customizing the menu

// Implementation
protected:
    NOTIFYICONDATA  m_tnd;
    BOOL            m_bEnabled;         // does O/S support tray icon?
    BOOL            m_bHidden;          // Has the icon been hidden?
    BOOL            m_bRemoved;         // Has the icon been removed?
    BOOL            m_bShowIconPending; // Show the icon once tha taskbar has been created
    BOOL            m_bWin2K;           // Use new W2K features?
	CWnd*           m_pTargetWnd;       // Window that menu commands are sent

	CMenu m_menu;

    CArray<HICON, HICON> m_IconList;
    UINT_PTR     m_uIDTimer;
    INT_PTR      m_nCurrentIcon;
    COleDateTime m_StartTime;
    UINT         m_nAnimationPeriod;
    HICON        m_hSavedIcon;
    UINT         m_DefaultMenuItemID;
    BOOL         m_DefaultMenuItemByPos;
	UINT         m_uCreationFlags;

// Static data
protected:
    static BOOL RemoveTaskbarIcon(CWnd* pWnd);

    static const UINT m_nTimerID;
    static UINT  m_nMaxTooltipLength;
    static const UINT m_nTaskbarCreatedMsg;
    static CWnd  m_wndInvisible;

    static BOOL GetW2K();
#ifndef _WIN32_WCE
    static void GetTrayWndRect(LPRECT lprect);
    static BOOL GetDoWndAnimation();
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CSystemTray)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
#ifndef _WIN32_WCE
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
    LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};


#endif

/////////////////////////////////////////////////////////////////////////////
