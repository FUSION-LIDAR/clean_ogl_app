#pragma once


// CClassificationCodeDlg dialog

class CClassificationCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CClassificationCodeDlg)

public:
	CClassificationCodeDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CClassificationCodeDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLASS_LIST };
#endif

protected:
	CWnd* m_pParent;
	int m_nID;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
//	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
	virtual BOOL Create();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedApply();

	BOOL m_ColumnChecks[8];
	BOOL m_ShowClass[256];
	BOOL m_Synthetic;
	BOOL m_Overlap;
	BOOL m_Keypoint;
	BOOL m_Withheld;
	afx_msg void OnClickedAllColumn1();
	afx_msg void OnClickedAllColumn2();
	afx_msg void OnClickedAllColumn3();
	afx_msg void OnClickedAllColumn4();
	afx_msg void OnClickedAllColumn5();
	afx_msg void OnClickedAllColumn6();
	afx_msg void OnClickedAllColumn7();
	afx_msg void OnClickedAllColumn8();
};
