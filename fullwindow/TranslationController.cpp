// TranslationController.cpp: implementation of the CTranslationController class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fullwindow.h"
#include "TranslationController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTranslationController::CTranslationController()
{
	m_HomeX = m_HomeY = 0.0;
	m_LastMovementX = m_LastMovementY = 0.0;
	m_MovementX = m_MovementY = 0.0;
	m_MouseButtonDown = FALSE;
}

CTranslationController::~CTranslationController()
{

}

BOOL CTranslationController::Key(UINT nChar)
{
	BOOL retcode = FALSE;
	// return TRUE if keystroke is used

	return(retcode);
}

void CTranslationController::MouseDown(const CPoint &location)
{
	// store the initial mouse location...this will be the "neutral" position
	m_HomeX = (2.0 * (double) location.x - m_WinWidth) / m_WinWidth;
	m_HomeY = (m_WinHeight - 2.0 * (double) location.y) / m_WinHeight;

	m_MouseButtonDown = TRUE;
}

void CTranslationController::MouseUp(const CPoint &location)
{
	m_HomeX = m_HomeY = 0.0;
	m_MouseButtonDown = FALSE;
	m_TypeOfMovement = 0;
}

void CTranslationController::MouseMove(const CPoint &location)
{
	if (m_MouseButtonDown) {
		double X = (2.0 * (double) location.x - m_WinWidth) / m_WinWidth;
		double Y = (m_WinHeight - 2.0 * (double) location.y) / m_WinHeight;
		m_MovementX = X - m_HomeX;
		m_MovementY = Y - m_HomeY;
		m_LastMovementX = m_MovementX;
		m_LastMovementY = m_MovementY;

		m_TypeOfMovement = MOTION;

		// check for Shift & Ctrl keys
		if (::GetKeyState(VK_SHIFT) & 0x80 && ::GetKeyState(VK_CONTROL) & 0x80) {
		}
		else if (::GetKeyState(VK_SHIFT) & 0x80) {
			m_TypeOfMovement = TILT_HEAD;
		}
		else if (::GetKeyState(VK_CONTROL) & 0x80) {
			m_TypeOfMovement = TRANSLATE;
		}
		else {
			// 
//			m_Camera.vp.Translate(m_TranslationController.m_MovementX, m_TranslationController.m_MovementY, 0.0);
//			m_Camera.pr.Translate(m_TranslationController.m_MovementX, m_TranslationController.m_MovementY, 0.0);
		}
	}
}

void CTranslationController::ClientAreaResize(const CRect &newSize)
{
	m_WinWidth = (double) newSize.Width();
	m_WinHeight = (double) newSize.Height();
}

void CTranslationController::DoGLTranslation()
{
//	glTranslated(m_MovementX, 0.0, m_MovementY);
}
