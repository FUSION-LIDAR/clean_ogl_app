// TranslationController.h: interface for the CTranslationController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATIONCONTROLLER_H__C88F633E_1C85_4D13_B964_3F7ED8DC979B__INCLUDED_)
#define AFX_TRANSLATIONCONTROLLER_H__C88F633E_1C85_4D13_B964_3F7ED8DC979B__INCLUDED_
#include "gl\gl.h"
#include "gl\glu.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		MOTION		1
#define		TRANSLATE	2
#define		TILT_HEAD	3

class CTranslationController  
{
public:
	void DoGLTranslation();
	void ClientAreaResize(const CRect& newSize);
	void MouseMove(const CPoint& location);
	void MouseUp(const CPoint& location);
	void MouseDown(const CPoint& location);
	BOOL Key(UINT nChar);
	CTranslationController();
	virtual ~CTranslationController();
	double m_MovementY;
	double m_MovementX;
	int m_TypeOfMovement;

private:
	BOOL m_MouseButtonDown;
	double m_HomeY;
	double m_HomeX;
	double m_LastMovementY;
	double m_LastMovementX;
	double m_WinWidth;
	double m_WinHeight;
};

#endif // !defined(AFX_TRANSLATIONCONTROLLER_H__C88F633E_1C85_4D13_B964_3F7ED8DC979B__INCLUDED_)
