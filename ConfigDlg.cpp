#include "StdAfx.h"
#include ".\configdlg.h"

CConfigDlg::CConfigDlg( RegMap &reg, UINT id, CWnd *parent )
: m_reg(reg)
{
	Create( id, parent );
}

/*CConfigDlg::~CConfigDlg(void)
{
}*/
