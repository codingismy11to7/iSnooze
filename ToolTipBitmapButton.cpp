// ToolTipBitmapButton.cpp : implementation file
//
// CToolTipRessourceButton by Jean-Louis GUENEGO
// Thanks to Niek Albers.
// http://www.codeproject.com/buttonctrl/tooltipressourcebutton.asp
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

#include "stdafx.h"

#include "ToolTipBitmapButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolTipBitmapButton

CToolTipBitmapButton::CToolTipBitmapButton()
{
}

CToolTipBitmapButton::~CToolTipBitmapButton()
{
}


BEGIN_MESSAGE_MAP(CToolTipBitmapButton, CBitmapButton)
	//{{AFX_MSG_MAP(CToolTipBitmapButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipBitmapButton message handlers

// Set the tooltip with a CString
void CToolTipBitmapButton::SetToolTipText(CString *spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, *spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText(*spText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CToolTipBitmapButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create active
		m_ToolTip.Activate(TRUE);
	}
} // End of InitToolTip


BOOL CToolTipBitmapButton::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);		
	return CBitmapButton::PreTranslateMessage(pMsg);
}


