#pragma once

#include "RegMap.h"
#include "ITI.h"
#include "ToolTipButton.h"
#include "afxwin.h"
#include "afxcmn.h"

// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(const RegMap &reg, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

    void Show()
    {     LoadFromReg(); ShowWindow( SW_SHOW ); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    void LoadFromReg();
    void FillBoxes();
    void GetPlaylists();
    void SelectPlaylist();

    BOOL OnInitDialog();

    RegMap m_reg;
    std::vector<tstring> m_playlists;
    //std::vector<ITID> m_plids;

    HWND m_parentHwnd;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedOk();
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnNeedText(NMHDR *pnmh, LRESULT *pResult);
protected:
	CToolTipButton m_increase;
    CToolTipButton m_minimize;
	CSliderCtrl m_secondsSlider;
public:
	afx_msg void OnBnClickedIncreaseCheck();
protected:
    CEdit m_snoozeTime;
    CSpinButtonCtrl m_snoozeSpin;
public:
    afx_msg void OnBnClickedEnableSnooze();
};
