////////////////////////////////////////////////////////////////////////////
// File:	GroupControl.cpp
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
//

#include "stdafx.h"
#include "GroupControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupControl

CGroupControl::CGroupControl()
{
  m_bAllowOverlap = FALSE;
  m_bUseTabOrder = FALSE;
}

CGroupControl::~CGroupControl()
{
}

BEGIN_MESSAGE_MAP(CGroupControl, CButton)
	//{{AFX_MSG_MAP(CGroupControl)
	ON_WM_ENABLE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupControl message handlers

BOOL CGroupControl::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
  dwStyle &= ~(0xF);		// remove styles with conflict with group box
  dwStyle |= BS_GROUPBOX;	// add group box style
  return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

void CGroupControl::PreSubclassWindow() 
{
  CButton::PreSubclassWindow();

  // if this 'button' is not a group then
  // it's a bit meaningless to use this control at all
  ASSERT((GetStyle() & 0xF) & BS_GROUPBOX);
}

BOOL CGroupControl::IsInGroup(CWnd *pCtrl, BOOL &bOverlapped)
{
  BOOL bIsInGroup = FALSE;
  if (pCtrl == NULL)
    return FALSE;
  CRect rcGroup, rc;
  GetWindowRect(&rcGroup);
  pCtrl->GetWindowRect(&rc);
  if (rcGroup.PtInRect(rc.BottomRight()) && rcGroup.PtInRect(rc.TopLeft()))
    bIsInGroup = TRUE;
  if (! bIsInGroup && (rcGroup.PtInRect(rc.BottomRight()) || rcGroup.PtInRect(rc.TopLeft())))
    bOverlapped = bIsInGroup = TRUE;
  return bIsInGroup;
}

BOOL CGroupControl::DoGroupControlAction(GROUPCONTROLACTIONFUNC pfnGCAF, LPARAM lParam/*=0*/)
{
  if (pfnGCAF == NULL)
    return FALSE;

  // don't do anything if we're not a groupbox
  if (!((GetStyle() & 0xF) & BS_GROUPBOX))
    return FALSE;

  // go through all controls that lie inside the group and enable/disable
  // those controls also
  CRect rcGroup;
  GetWindowRect(&rcGroup);

  // go through siblings, and see if they lie within the boundary
  // of this group control
  CWnd* pCtrl = NULL;
  if (m_bUseTabOrder)
    pCtrl = GetNextWindow();
  else
    pCtrl = GetParent()->GetWindow(GW_CHILD);
  while (pCtrl != NULL)
  {
    if (pCtrl->GetSafeHwnd() != GetSafeHwnd())
    {
      BOOL bOverlapped = FALSE;
      BOOL bIsInGroup = IsInGroup(pCtrl, bOverlapped);
      if (bIsInGroup && (m_bAllowOverlap || ! bOverlapped))
      {
	if (! pfnGCAF(pCtrl, lParam))
	  return FALSE;
      }
      else if (! bOverlapped && m_bUseTabOrder)  // found out side of group, so ditch out
	break;
    }
    pCtrl = pCtrl->GetNextWindow();
  }
  return TRUE;
}

static BOOL GroupControlActionFunc_Enable(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  BOOL bEnable = (BOOL)lParam;
  pCtrl->EnableWindow(bEnable);
  return TRUE;
}

void CGroupControl::OnEnable(BOOL bEnable) 
{
  CButton::OnEnable(bEnable);
  DoGroupControlAction(GroupControlActionFunc_Enable, bEnable);
}

static BOOL GroupControlActionFunc_Move(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  short nDeltaX = LOWORD(lParam);
  short nDeltaY = HIWORD(lParam);
  CRect rc;
  pCtrl->GetWindowRect(&rc);
  rc.OffsetRect(nDeltaX, nDeltaY);
  pCtrl->GetParent()->ScreenToClient(&rc);
  pCtrl->MoveWindow(rc);
  return TRUE;
}

void CGroupControl::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
  CButton::OnWindowPosChanging(lpwndpos);

  if (lpwndpos->flags & SWP_NOMOVE)
    return;
  
  // see if we've moved x or y, and 
  // move group controls with group box
  CRect rcGroup;
  GetWindowRect(&rcGroup);
  GetParent()->ScreenToClient(&rcGroup);

  int nDeltaX = lpwndpos->x - rcGroup.left;
  int nDeltaY = lpwndpos->y - rcGroup.top;

  if (nDeltaX == 0 && nDeltaY == 0)
    return;

  DoGroupControlAction(GroupControlActionFunc_Move, MAKELPARAM(nDeltaX, nDeltaY));

  Invalidate();
}

static BOOL GroupControlActionFunc_Show(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  BOOL bShow = (BOOL)lParam;
  pCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  return TRUE;
}

void CGroupControl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
  CButton::OnShowWindow(bShow, nStatus);
  DoGroupControlAction(GroupControlActionFunc_Show, bShow);
}
