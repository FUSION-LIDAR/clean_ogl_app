// CleanGlWnd.h: interface for the CCleanGlWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLEANGLWND_H__E58D391E_51B0_46B0_94C8_8568821192D3__INCLUDED_)
#define AFX_CLEANGLWND_H__E58D391E_51B0_46B0_94C8_8568821192D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLWnd.h"
#include "BallController.h"	// Added by ClassView

class CCleanGLWnd : public COpenGLWnd  
{
public:
	BOOL m_InMotion;
	CBallController m_TrackBall;
	void ResetCamera();
	void VideoMode(ColorsNumber& c, ZAccuracy& z, BOOL& dbuf, BOOL& stereo, BOOL &anaglyph, BOOL &splitscreen, int &alignment);
	void OnDrawGDI(CPaintDC* pDC);
	void OnDrawGL();
	void OnSizeGL(int cx, int cy);
	void OnCreateGL();
	CCleanGLWnd();
	virtual ~CCleanGLWnd();

	// Generated message map functions
// NOTE: these have been declared private because they shouldn't be
//		 overridden, use the provided virtual functions instead.
private:
	BOOL m_DoContinuousDrawing;

	//{{AFX_MSG(CCleanGLWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLEANGLWND_H__E58D391E_51B0_46B0_94C8_8568821192D3__INCLUDED_)
