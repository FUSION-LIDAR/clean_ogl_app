// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__7A15AEF9_7571_459C_AD3E_9D4EAC64707D__INCLUDED_)
#define AFX_MAINFRM_H__7A15AEF9_7571_459C_AD3E_9D4EAC64707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;

// Generated message map functions
protected:
	LRESULT OnRemoteOpen(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnFileOpen();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg LRESULT OnSynchronize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClonepdq();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetAppName(LPCTSTR Title);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__7A15AEF9_7571_459C_AD3E_9D4EAC64707D__INCLUDED_)
