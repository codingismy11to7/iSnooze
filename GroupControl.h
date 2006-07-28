////////////////////////////////////////////////////////////////////////////
// File:	GroupControl.h
// Version:	1
// Created:	24-Jul-2002
//
// Author:	Paul S. Vickery
// E-mail:	paul@vickeryhome.freeserve.co.uk
//
// CButton-derived class to make using groups easier. CGroupControl will 
// automatically show/hide or enable/disable controls within the group when 
// the group is shown/hidden or enabled/disabled. Also, if the group box is 
// moved then its controls are moved with it.
//
// You are free to use or modify this code, with no restrictions, other than
// you continue to acknowledge me as the original author in this source code,
// or any code derived from it.
//
// If you use this code, or use it as a base for your own code, it would be 
// nice to hear from you simply so I know it's not been a waste of time!
//
// Copyright (c) 2002 Paul S. Vickery
//
////////////////////////////////////////////////////////////////////////////
// Version History:
//
// Version 1 - 24-Jul-2002
// =======================
// Initial version
// 
//
// http://www.thecodeproject.com/buttonctrl/groupcontrol.asp
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

#if !defined(AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_)
#define AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef BOOL (*GROUPCONTROLACTIONFUNC)(CWnd*, LPARAM);

/////////////////////////////////////////////////////////////////////////////
// CGroupControl window

class CGroupControl : public CButton
{
// Construction
public:
	CGroupControl();

// Attributes
public:

// Operations
public:
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupControl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsInGroup(CWnd* pCtrl, BOOL& bOverlapped);
	void SetUseTabOrder(BOOL bUseTabOrder = TRUE) { m_bUseTabOrder = bUseTabOrder; }
	void SetAllowOverlap(BOOL bAllowOverlap = TRUE) { m_bAllowOverlap = bAllowOverlap; }
	BOOL GetUseTabOrder() { return m_bUseTabOrder; }
	BOOL GetAllowOverlap() { return m_bAllowOverlap; }
	BOOL DoGroupControlAction(GROUPCONTROLACTIONFUNC pfnGCAF, LPARAM lParam = 0);
	virtual ~CGroupControl();

	// Generated message map functions
protected:
	BOOL m_bAllowOverlap;
	BOOL m_bUseTabOrder;
	//{{AFX_MSG(CGroupControl)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_)
