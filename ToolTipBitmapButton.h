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


#if !defined(AFX_TOOLTIPAOEUBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_)
#define AFX_TOOLTIPAOEUBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0500

#include <afxwin.h>
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <tchar.h>

// ToolTipBitmapButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTipButton window

//#define baseCToolTipButton CBitmapButton
//#define baseCToolTipButton CBitmapButton
// if you do not want to profit of CBitmapButton, put simply CButton instead

// A cool CButton with tooltips
class CToolTipBitmapButton : public CBitmapButton
{
// Construction
public:
	CToolTipBitmapButton();

// Attributes
public:

protected:
	CToolTipCtrl m_ToolTip;

// Operations
public:
	void SetToolTipText(CString* spText, BOOL bActivate = TRUE);
protected:
	void InitToolTip();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipBitmapButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolTipBitmapButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipBitmapButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPAOEUBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_)
