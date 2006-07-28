#pragma once
#include "afxext.h"
#include "afxwin.h"

// CSnoozeDlg dialog

class CSnoozeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSnoozeDlg)

public:
	CSnoozeDlg(CWnd* pParent = NULL, bool enableSnooze = true );   // standard constructor
	virtual ~CSnoozeDlg();

// Dialog Data
	enum { IDD = IDD_SNOOZE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    void OnOK();
    void OnCancel();

    BOOL OnInitDialog();

    CFont m_stopFont;
    CFont m_snoozeFont;

    bool m_enableSnooze;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedStopButton();
protected:
    CButton m_stopButton;
};
