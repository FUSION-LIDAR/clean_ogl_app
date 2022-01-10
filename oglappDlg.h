// oglappDlg.h : header file
//

#if !defined(AFX_OGLAPPDLG_H__F3C19B96_65BA_4DC8_858F_5E2F8FC99995__INCLUDED_)
#define AFX_OGLAPPDLG_H__F3C19B96_65BA_4DC8_858F_5E2F8FC99995__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CleanGLWnd.h"

/////////////////////////////////////////////////////////////////////////////
// COGLappDlg dialog

class COGLappDlg : public CDialog
{
// Construction
public:
	CCleanGLWnd* m_pOGLDisplayWindow;
	~COGLappDlg(void);
	COGLappDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COGLappDlg)
	enum { IDD = IDD_CLEAN_OGL_APP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLappDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COGLappDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGLAPPDLG_H__F3C19B96_65BA_4DC8_858F_5E2F8FC99995__INCLUDED_)
