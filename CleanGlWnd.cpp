// CleanGlWnd.cpp: implementation of the CCleanGlWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "clean_ogl_app.h"
#include "CleanGlWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCleanGLWnd, COpenGLWnd)
	//{{AFX_MSG_MAP(CCleanGLWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCleanGLWnd::CCleanGLWnd() : COpenGLWnd()
{
	m_DoContinuousDrawing = TRUE;
	m_bAntiAlias = FALSE;
	m_BackgroundColor = RGB(255, 255, 255);
	m_InMotion = FALSE;

	m_TrackBall.SetAlternateMethod(FALSE);
}

CCleanGLWnd::~CCleanGLWnd()
{

}

void CCleanGLWnd::OnCreateGL()
{
	COpenGLWnd::OnCreateGL();

	// perform hidden line/surface removal (enabling Z-Buffer)
	glEnable(GL_DEPTH_TEST);

	// set clear Z-Buffer value
	glClearDepth(1.0f);
	
	// enable simple lighting with light at viewpoint
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	// set timer to do continuous drawing
	if (m_DoContinuousDrawing)
		SetTimer(1, 1, NULL);
}

void CCleanGLWnd::OnSizeGL(int cx, int cy)
{
	// handles resizing the window and resetting the viewport
	COpenGLWnd::OnSizeGL(cx, cy);
	CRect rc(0, 0, cx, cy);
	m_TrackBall.ClientAreaResize(rc);
}

void CCleanGLWnd::OnDrawGL()
{
	// main drawing function...called anytime window needs painting
	m_TrackBall.IssueGLrotation();

	// base class has some simple coordinate axes...probably don't need/want
//	COpenGLWnd::OnDrawGL();

	// draw ball in center of screen
	glTranslated(0.0, 0.0, 0.0);
	GLUquadricObj* quad = gluNewQuadric();
	glColor3f(.5f, 0.0f, 0.0f);
	gluSphere(quad, 0.05, 16, 32);

	glTranslated(0.5, 0.0, 0.0);
	glColor3f(0.0f, 0.5f, 0.0f);
	gluSphere(quad, 0.05, 16, 32);

	glTranslated(0.0, 0.5, 0.0);
	glColor3f(0.0f, 0.0f, 0.5f);
	gluSphere(quad, 0.05, 16, 32);

	gluDeleteQuadric(quad);
}

void CCleanGLWnd::OnDrawGDI(CPaintDC *pDC)
{
	// haven't tried GDI drawing on top of openGL...may not be a good idea
	// bSetupPixelFormat() doesn't request GDI compatible context so GDI drawing may not work on all graphics cards

	COpenGLWnd::OnDrawGDI(pDC);
}

void CCleanGLWnd::VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf, BOOL &stereo, BOOL &anaglyph, BOOL &splitscreen, int &alignment)
{
	// set default videomode
	c = MILLIONS;
	z = ACCURATE;
	dbuf = TRUE;

	// support stereo display if available
	stereo = FALSE;
	anaglyph = FALSE;
	splitscreen = FALSE;
	alignment = PARALLEL;
}

void CCleanGLWnd::ResetCamera()
{
	// use base class camera...change camera position and view direction here
	// base class OnPaint() handles camera use
	COpenGLWnd::ResetCamera();
}

void CCleanGLWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_TrackBall.MouseDown(point);
	SetCapture();

	m_InMotion = TRUE;
}

void CCleanGLWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_TrackBall.MouseUp(point);
	ReleaseCapture();
	Invalidate();

	m_InMotion = FALSE;

	// set timer to do continuous drawing
	if (m_DoContinuousDrawing)
		SetTimer(1, 1, NULL);
}

void CCleanGLWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_InMotion) {
		m_TrackBall.MouseMove(point);
		Invalidate();
	}
}

void CCleanGLWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1) {
		// kill and restart to flush extra timer events...due to long render times
		KillTimer(1);
		SetTimer(1, 1, NULL);
		m_TrackBall.ApplyLastRotation();
		Invalidate();
	}
}

void CCleanGLWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_TrackBall.Key(nChar)) {
		Invalidate();
	}
	else {
		switch (nChar) {
		case VK_ESCAPE:
			KillTimer(1);
			break;
		case 'm':
		case 'M':
			m_DoContinuousDrawing = m_DoContinuousDrawing ? FALSE : TRUE;
			KillTimer(1);
			break;
		default:
			COpenGLWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}
