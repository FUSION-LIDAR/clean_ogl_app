// clean_ogl_app.h : main header file for the CLEAN_OGL_APP application
//

#if !defined(AFX_CLEAN_OGL_APP_H__99688B7A_A3AF_448B_BABC_EAD05E6BDC9B__INCLUDED_)
#define AFX_CLEAN_OGL_APP_H__99688B7A_A3AF_448B_BABC_EAD05E6BDC9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COGLapp:
// See clean_ogl_app.cpp for the implementation of this class
//

class COGLapp : public CWinApp
{
public:
	COGLapp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLapp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COGLapp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLEAN_OGL_APP_H__99688B7A_A3AF_448B_BABC_EAD05E6BDC9B__INCLUDED_)
