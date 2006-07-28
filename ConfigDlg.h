#pragma once
#include "afxwin.h"
#include "RegMap.h"

class CConfigDlg :
	public CDialog
{
public:
	CConfigDlg( RegMap &reg, UINT id, CWnd *parent );

	void Show()
	{ ShowWindow( SW_SHOW ); }
	//~CConfigDlg(void);
private:
	RegMap m_reg;
};
