#if !defined(AFX_RANGEPOLEOPTIONDLG_H__E3D22EB2_BB7E_4FC0_BCAD_2B709F0D7B2B__INCLUDED_)
#define AFX_RANGEPOLEOPTIONDLG_H__E3D22EB2_BB7E_4FC0_BCAD_2B709F0D7B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RangepoleOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RangepoleOptionDlg dialog

class RangepoleOptionDlg : public CDialog
{
// Construction
public:
	RangepoleOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(RangepoleOptionDlg)
	enum { IDD = IDD_RANGEPOLE_DLG };
	BOOL	m_DrawPoleLL;
	BOOL	m_DrawPoleLR;
	BOOL	m_DrawPoleMB;
	BOOL	m_DrawPoleML;
	BOOL	m_DrawPoleMR;
	BOOL	m_DrawPoleMT;
	BOOL	m_DrawPoleUL;
	BOOL	m_DrawPoleUR;
	BOOL	m_DrawPoleCenter;
	double	m_RangepoleTotalHeight;
	double	m_RangepoleSegmentHeight;
	BOOL	m_ScaleRangepoleToData;
	BOOL	m_UseRangepoleHeightToScaleData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RangepoleOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RangepoleOptionDlg)
	afx_msg void OnAll();
	afx_msg void OnCenteronly();
	afx_msg void OnCorneronly();
	afx_msg void OnSidesonly();
	afx_msg void OnScaletodata();
	afx_msg void OnNone();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANGEPOLEOPTIONDLG_H__E3D22EB2_BB7E_4FC0_BCAD_2B709F0D7B2B__INCLUDED_)
