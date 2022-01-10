// OpenGLWnd.cpp : implementation file
//
/* 
NOTE: much of the code for the class COpenGLWnd is based on the class GLEnabledView 
described in the article "GL enabled view for MDI environment" by Alessandro Falappa,
found on the CodeGuru site at http://www.codeguru.com/updates/.

The class has been modified to derive from a generic CWnd.
Text support has been added to the class.
GDI drawing support has been added to the class
The rendered image can be copied to the clipboard

The copyright notice below pertains to the parts of the code which are 
directly copied from the class GLEnabledView.
The rest of the code is Copyright © W. J. Heitler.
The conditions for use are as described below.
The disclaimer of liability is extended to include W. J. Heitler 

Extensive modifications by Robert J. McGaughey including:
	stereo display support with split-screen and anaglyph modes
	general code clean-up (resolved conflict with some ATI cards/drivers
	scene antialiasing using a multiple pass jitter
	saving image to file and clipboard
*/

/*****************************************************
Copyright Notice & Disclaimer

Copyright © Alessandro Falappa

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose is hereby granted without
fee, provided that the above copyright notice, author statement
appear in all copies of this software and related documentation.

If you make enhancement or you discover bugs, please let me
know

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
ANY KIND, EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT
LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR FITNESS FOR A
PARTICULAR PURPOSE.

IN NO EVENT SHALL ALESSANDRO FALAPPA BE LIABLE FOR ANY
SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE,
AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

///////////////////////////////////////////////
History

v 1.0: first version
v 1.1: added CGLDispList helper class
	   changed previous disp list service in StockDispList
v 1.2: added CGLTesselator helper class
	   added GLCommands pair
	   added CGLQuadric wrapper class

******************************************************/

/*******************
 NOTE ABOUT LIBRARIES INCLUSION:
 - Remember to include the appropriate libraries in the link phase
   (look at Project Settings under the Link tab)
 - If you were lucky enough to get SGI's implementation (at present it's
   not availabl nor supported) you can play with it also, just include
   that libraries. SGI's version is faster if you have no GL acceleration
   and if you own a MMX processor
 - These includes below can be moved to stdafx.h to speed up compilation
//*/
/* SGI openGL libraries (link with OPENGL.LIB and GLU.LIB)
#include "[path-of-SGI-sdk]\include\gl\gl.h"
#include "[path-of-SGI-sdk]\include\gl\glu.h"
//*/
//#include "afxtempl.h"

#include "stdafx.h"
#include <math.h>
#include "OpenGLWnd.h"
#include "resource.h"
#include "image.h"
#include "filespec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_LISTS 20
// used to identify a MCD video driver (partial OGL acceleration)
#define INSTALLABLE_DRIVER_TYPE_MASK  (PFD_GENERIC_ACCELERATED|PFD_GENERIC_FORMAT)

// to change number of iterations used for anti-aliasing, search for jarray and change argument, then change AAPASSES
// simply changing AAPASSES won't change which array of jitter values is used to offset iterations
#define		AAPASSES	8
#define		AAWEIGHT	(1.0 / (double) AAPASSES)
#define		jarray(n)	jitterby##n

// angle step size and movement sensitivity for translation controller rotation
#define		ANGLE_STEP		1.0
#define		MOVEMENT_SENSITIVITY	0.005

#define		PI		3.14159265358979
#define		D2R 	(PI / 180.0)

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd

COpenGLWnd::COpenGLWnd() :
	m_dAspectRatio(1.0),
	m_bInsideDispList(FALSE), m_bExternDispListCall(FALSE),
	m_bExternGLCall(FALSE)

{
	// define a default cursor
	m_hMouseCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	
	// set the disp list vector to all zeros
	for (int c = 0; c < MAX_LISTS; c ++)
		m_DispListVector[c] = 0;

	m_FontListBase = 1000;	// initial guess for font display list
	m_bGotFont = FALSE;
	m_bUseAnaglyph = FALSE;
	m_bUseSplitScreen = FALSE;
	m_Alignment = PARALLEL;
	m_BackgroundColor = RGB(255, 0, 0);
	m_bAntiAlias = FALSE;
	m_OnCreateGLComplete = FALSE;
	m_EyeSeparationMultiplier = 1.0;
	m_ProjectionType = PERSPECTIVE;
	
	m_TotalTime.Start();
	m_FramesDrawn = 0;
	m_FrameRate = 0.0;
	m_TimeBetweenFrames = 0.0;
	m_TimeToDrawFrame = 0.0;

	m_BannerMessage.Empty();

	m_DoContinuousDrawing = TRUE;
	m_InMotion = FALSE;
	m_InRedraw = FALSE;

	m_MotionController = TRACKBALL;

	m_TrackBall.SetAlternateMethod(FALSE);

	ResetCamera();
}

COpenGLWnd::~COpenGLWnd()
{
	// select stock font
	SelectObject (m_pCDC->GetSafeHdc(), GetStockObject (SYSTEM_FONT)); 
	
	m_TotalTime.Stop();
}

BEGIN_MESSAGE_MAP(COpenGLWnd, CWnd)
	//{{AFX_MSG_MAP(COpenGLWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////
// COpenGLWnd Constants

// these are used to construct an equilibrated 256 color palette
static unsigned char _threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static unsigned char _twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

static unsigned char _oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

// Windows Default Palette
static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

static double jitterby8[9][2] = {0.0, 0.0, .5625, .4375, .0625, .9375, .3125, .6875, .6875, .8125, .8125, .1875, .9375, .5625, .4375, .0625, .1875, .3125};
static double jitterby6[7][2] = {0.0, 0.0, .4646464646, .4646464646, .1313131313, .7979797979, .5353535353, .8686868686, .8686868686, .5353535353, .7979797979, .1313131313, .2020202020, .2020202020};
static double jitterby5[6][2] = {0.0, 0.0, .5, .5, .3, .1, .7, .9, .9, .3, .1, .7};
static double jitterby4[5][2] = {0.0, 0.0, .375, .25, .125, .75, .875, .25, .625, .75};
static double jitterby3[4][2] = {0.0, 0.0, .5033922635, .8317967229, .7806016275, .2504380877, .2261828938, .4131553612};
static double jitterby2[3][2] = {0.0, 0.0, .25, .75, .75, .25};

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd message handlers

int COpenGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// OpenGL rendering context creation
	PIXELFORMATDESCRIPTOR pfd;
    int         n;

	// initialize the private member
	m_pCDC= new CClientDC(this);

	// choose the requested video mode
    if (!bSetupPixelFormat()) return 0;
	
	// ask the system if the video mode is supported
    n=::GetPixelFormat(m_pCDC->GetSafeHdc());
    ::DescribePixelFormat(m_pCDC->GetSafeHdc(),n,sizeof(pfd),&pfd);

	// create a palette if the requested video mode has 256 colors (indexed mode)
    CreateRGBPalette();

	// link the Win Device Context with the OGL Rendering Context
    m_hRC = wglCreateContext(m_pCDC->GetSafeHdc());

	return 0;
}

void COpenGLWnd::OnDestroy() 
{
//	TRACE("In COpenGLWnd::OnDestroy...\n");	
	// clean up GL window stuff
	OnDestroyGL();

	// specify the target DeviceContext (window) of the subsequent OGL calls
    wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

	// remove all display lists
	for (int c = 0; c < MAX_LISTS; c ++) {
		if (m_DispListVector[c]) 
			glDeleteLists(m_DispListVector[c],1);
	}

	// release definitely OGL Rendering Context
	if (m_hRC != NULL)
		::wglDeleteContext(m_hRC);

	// Select our palette out of the dc
	CPalette palDefault;
	palDefault.CreateStockObject(DEFAULT_PALETTE);
	m_pCDC->SelectPalette(&palDefault, FALSE);

	// destroy Win Device Context
	if (m_pCDC) 
		delete m_pCDC;

//	TRACE("In COpenGLWnd::OnDestroy...Done\n");	

	// finally call the base function
	CWnd::OnDestroy();
}

BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC) 
{
	// OGL has his own background erasing so tell Windows to skip
	return TRUE;
}

void COpenGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// when called with a nonzero window:
	if ( 0 < cx && 0 < cy ) {
		// update the rect and the aspect ratio
		m_ClientRect.right = cx;
		m_ClientRect.bottom = cy;
		m_dAspectRatio=double(cx)/double(cy);
		m_Camera.screenwidth = cx;
		m_Camera.screenheight = cy;

		// specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

		// if window has not been initialized, do it
		if (!m_OnCreateGLComplete) {
			OnCreateGL();
			m_OnCreateGLComplete = TRUE;
		}

		// call the virtual sizing procedure (to be overridden by user)
		OnSizeGL(cx,cy);

		// free the target DeviceContext (window)
		wglMakeCurrent(NULL,NULL);

		// force redraw
		Invalidate(TRUE);
	}
}

// NOTE: this does not work if a derived class captures the mouse. 
// The cursor must then be set explicitly with each Mouse call.
BOOL COpenGLWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT(m_hMouseCursor!=NULL);
	::SetCursor(m_hMouseCursor);
	
	return TRUE;
}

// pass in origin (bottom,left,near), width, height, depth
// NOTE, to make box go into screen from org, z must be negative
void COpenGLWnd::DrawBox(CPoint3D org, double x, double y, double z)
{
	glBegin( GL_LINE_LOOP );
		glVertex3d(org.x, org.y, org.z);
		glVertex3d(org.x, org.y+y, org.z);
		glVertex3d(org.x+x, org.y+y, org.z);
		glVertex3d(org.x+x, org.y, org.z);
		glVertex3d(org.x+x, org.y, org.z+z);
		glVertex3d(org.x+x, org.y+y, org.z+z);
		glVertex3d(org.x, org.y+y, org.z+z);
		glVertex3d(org.x, org.y, org.z+z);
	glEnd();
	glBegin(GL_LINES); 
		glVertex3d(org.x, org.y, org.z);
		glVertex3d(org.x+x, org.y, org.z);
		glVertex3d(org.x, org.y, org.z+z);
		glVertex3d(org.x+x, org.y, org.z+z);

		glVertex3d(org.x, org.y+y, org.z);
		glVertex3d(org.x, org.y+y, org.z+z);
		glVertex3d(org.x+x, org.y+y, org.z);
		glVertex3d(org.x+x, org.y+y, org.z+z);
	glEnd();
}


void COpenGLWnd::OnPaint() 
{
//	TRACE("In COpenGLWnd::OnPaint\n");	
	// use the semaphore to enter this critical section
	if (m_InRedraw) 
		return;

	m_InRedraw = TRUE;

	// if we are allow user to "throw" the model, repeat last mouse-related rotation...only when not holding down a mouse button
	if (m_DoContinuousDrawing && !m_InMotion && m_MotionController == TRACKBALL)
		m_TrackBall.ApplyLastRotation();

	// get time since end of last frame
	m_TimeBetweenFrames = m_FrameTimer.Stop();

	// update the non-GL frame information
	m_FrameTimer.Start();
	OnUpdateGLState();
	m_TimeToUpdateFrameInfo = m_FrameTimer.Stop();

	m_FrameTimer.Start();

   	// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

	// Clear the buffers...clear left and right if in stereo
	glClearColor((float) GetRValue(m_BackgroundColor) / 255.0f, (float) GetGValue(m_BackgroundColor) / 255.0f, (float) GetBValue(m_BackgroundColor) / 255.0f,1.0f );
	glClearDepth(1.0f);
	if (m_bDoubleBufferEnabled)
		glDrawBuffer(GL_BACK);
	else
		glDrawBuffer(GL_FRONT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update camera geometry
	m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
	m_Camera.eyesep = m_Camera.focallength / 1300.0;

	int rvRenderMode;
	glGetIntegerv(GL_RENDER_MODE, &rvRenderMode);

	// do the rendering
	if ((m_bStereoEnabled || m_bUseSplitScreen) && rvRenderMode == GL_RENDER) {
		// stereo...either flicker or splitscreen
		if (m_bAntiAlias) {
			AADraw(LEFT);
			AADraw(RIGHT);
		}
		else {
			if (m_MotionController == FLYBALL) {
				// move camera
				ApplyTranslationControl();

				// allow apps to modify the camera
				ModifyCameraLocation();
			}
			MoveToEyePosition(LEFT);

			OnDrawGLBeforeTrackball();

			if (m_MotionController == TRACKBALL)
				m_TrackBall.IssueGLrotation();

			OnDrawGL();

			MoveToEyePosition(RIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);

			OnDrawGLBeforeTrackball();

			if (m_MotionController == TRACKBALL)
				m_TrackBall.IssueGLrotation();

			OnDrawGL();

			// draw mask...if any
			if (m_bUseSplitScreen) {
				MoveToEyePosition(LEFT);
				OnDrawGLMask();

				MoveToEyePosition(RIGHT);
				glClear(GL_DEPTH_BUFFER_BIT);
				OnDrawGLMask();
			}
			else {
				MoveToEyePosition(MIDDLE);
				OnDrawGLMask();
			}
		}
	}
	else if (m_bUseAnaglyph && rvRenderMode == GL_RENDER) {
		// anaglyph...needs to do color masking so the render code needs to use anaglyph sensitive color functions to change colors
		if (m_bAntiAlias) {
			glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);
			AADraw(LEFT);
			glFlush();

			glClear(GL_DEPTH_BUFFER_BIT);
			// write to green and blue channels...using green channel keeps things looking gray instead of red/pink
			glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
			AADraw(RIGHT);
		}
		else {
			if (m_MotionController == FLYBALL) {
				// move camera
				ApplyTranslationControl();

				// allow apps to modify the camera
				ModifyCameraLocation();
			}

			MoveToEyePosition(LEFT);

			// write to red channel
			glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

			OnDrawGLBeforeTrackball();

			if (m_MotionController == TRACKBALL)
				m_TrackBall.IssueGLrotation();

			OnDrawGL();
			glFlush();

			MoveToEyePosition(RIGHT);

			glClear(GL_DEPTH_BUFFER_BIT);
			// write to green and blue channels...using green channel keeps things looking gray instead of red/pink
			glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);

			OnDrawGLBeforeTrackball();

			if (m_MotionController == TRACKBALL)
				m_TrackBall.IssueGLrotation();

			OnDrawGL();
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			// draw mask...if any
			MoveToEyePosition(MIDDLE);
			OnDrawGLMask();
		}
	}
	else {
		// mono...uses middle eye position
		if (m_bAntiAlias)
			AADraw(MIDDLE);
		else {
			if (m_MotionController == FLYBALL) {
				// move camera
				ApplyTranslationControl();

				// allow apps to modify the camera
				ModifyCameraLocation();
			}

			MoveToEyePosition(MIDDLE);

			OnDrawGLBeforeTrackball();

			if (m_MotionController == TRACKBALL)
				m_TrackBall.IssueGLrotation();

			OnDrawGL();
			OnDrawGLMask();
		}
	}

	// execute OGL commands (flush the OGL graphical pipeline)
//	if (!m_bStereoEnabled)
		glFinish();

	OnDrawHeadsUpDisplay();

	// if double buffering is used it's time to swap the buffers
	SwapBuffers(m_pCDC->GetSafeHdc());

	// compute frame rate
	m_TimeToDrawFrame = m_FrameTimer.Stop();
	m_FrameRate = 1.0 / (m_TimeToDrawFrame + m_TimeBetweenFrames);

	// restart timer
	m_FrameTimer.Start();

	// free the target DeviceContext (window)
    wglMakeCurrent(NULL,NULL);

	m_FramesDrawn ++;

	// turn the semaphore "green"
	m_InRedraw = FALSE;

	// do any GDI drawing
//	CPaintDC dc(this); // device context for painting
//	OnDrawGDI(&dc);

	ValidateRect(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd public members

void COpenGLWnd::VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf, BOOL &stereo, BOOL &anaglyph, BOOL &split_screen, int &alignment, int &projection_type)
{
	// set default videomode
	c = MILLIONS;
	z = ACCURATE;
	dbuf = TRUE;

	// support stereo display if available
	stereo = FALSE;
	anaglyph = FALSE;
	split_screen = FALSE;
	alignment = PARALLEL;

	projection_type = PERSPECTIVE;
}

void COpenGLWnd::SetMouseCursor(HCURSOR mcursor)
{
	// set the specified cursor (only if it is a valid one)
	if(mcursor!=NULL) m_hMouseCursor=mcursor;
}

const CString COpenGLWnd::GetInformation(InfoField type)
{
	PIXELFORMATDESCRIPTOR pfd;
	CString str("Not Available");

	// Get information about the DC's current pixel format 
	::DescribePixelFormat( m_pCDC->GetSafeHdc(), ::GetPixelFormat(m_pCDC->GetSafeHdc()),sizeof(PIXELFORMATDESCRIPTOR), &pfd ); 

	// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

	switch(type)
	{
	// Derive driver information
	case ACCELERATION: if( 0==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Fully Accelerated (ICD)"; // fully in hardware (fastest)
						else if (INSTALLABLE_DRIVER_TYPE_MASK==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Partially Accelerated (MCD)"; // partially in hardware (pretty fast, maybe..)
							 else str="Not Accelerated (Software)";	// software
						break;
	// get the company name responsible for this implementation
	case VENDOR:str=(char*)::glGetString(GL_VENDOR);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				break;
	// get the renderer name; this is specific of an hardware configuration
	case RENDERER:str=(char*)::glGetString(GL_RENDERER);
					if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
					break;
	// get the version
	case VERSION:str=(char*)::glGetString(GL_VERSION);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				break;
	// return a space separated list of extensions
	case EXTENSIONS: str=(char*)::glGetString(GL_EXTENSIONS);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				str.Replace(" ", "\r\n");
				break;
	};

	// free the target DeviceContext (window) and return the result
	wglMakeCurrent(NULL,NULL);
	return str;
}

void COpenGLWnd::DrawStockDispLists()
{
	// check if we are already inside a drawing session
	if(m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() ) {
		// draw directly all display lists
		for (int c=0;c<MAX_LISTS;c++) {
			if(m_DispListVector[c]) 
				glCallList(m_DispListVector[c]);
		}
	}
	else {
		// specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
		// draw all display lists
		for (int c=0;c<MAX_LISTS;c++) {
			if(m_DispListVector[c]) 
				glCallList(m_DispListVector[c]);
		}
		// free the target DeviceContext (window)
		wglMakeCurrent(NULL,NULL);
	}
}

void COpenGLWnd::StartStockDListDef()
{
	int c;
	// check if we aren't inside another couple begin/end
	if(!m_bInsideDispList) {
		// search a free slot
		for (c=0; m_DispListVector[c] != 0; c++);
	
		// check if we are inside a drawing session or not....
		if(!( m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() )) {
			// ...if not specify the target DeviceContext of the subsequent OGL calls
			wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
			// set a warning for EndDispList
			m_bExternDispListCall=TRUE;
		}
		// create a handle to the disp list (actually an integer)
		m_DispListVector[c]=glGenLists(1);
		// set a semaphore
		m_bInsideDispList=TRUE;
		// start the disp list: all subsequent OGL calls will be redirected to the list
		glNewList(m_DispListVector[c],GL_COMPILE);
	}
}

void COpenGLWnd::EndStockListDef()
{
	// close the disp list
	glEndList();
	
	// unset the semaphore
	m_bInsideDispList=FALSE;
	
	// if beginDispList set the warn free the target DeviceContext
	if(m_bExternDispListCall) 
		wglMakeCurrent(NULL,NULL);
}

void COpenGLWnd::ClearStockDispLists()
{
	// check if we are referring to the right Rendering Context
	if(m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() ) {
		// delete active display lists
		for (int c=0;c<MAX_LISTS;c++) {
			if(m_DispListVector[c]) 
				glDeleteLists(m_DispListVector[c],1);
		}
	}
	else {
		// specify the target Rendering Context of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
		// delete active display lists
		for (int c=0;c<MAX_LISTS;c++) {
			if(m_DispListVector[c]) 
				glDeleteLists(m_DispListVector[c],1);
		}
		// free the target Rendering Context (window)
		wglMakeCurrent(NULL,NULL);
	}
}

void COpenGLWnd::BeginGLCommands()
{
	// check if we are inside a drawing session or not....
	if (!(m_hRC == wglGetCurrentContext() && m_pCDC->GetSafeHdc() == wglGetCurrentDC())) {
		// ...if not specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
		// set a warning for EndGLCommands
		m_bExternGLCall = TRUE;
	}
}

void COpenGLWnd::EndGLCommands()
{
	// if BeginGLCommands set the warn free the target DeviceContext
	if (m_bExternGLCall) 
		wglMakeCurrent(NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
//
// Implementation of COpenGLWnd::CGLDispList class.
//
/*** DESCRIPTION

  This is actually a helper class which wraps the
  use of display list in OGL.
  It must be used inside an GLEnabledView cause
  a display list must refer to a Rendering Context.
  At present there is no support for Disp. Lists
  Sharing among multiple RCs (that is multiple MDI
  child windows).

****************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

COpenGLWnd::CGLDispList::CGLDispList():
	m_glListId(0), m_bIsolated(FALSE)
{
}

COpenGLWnd::CGLDispList::~CGLDispList()
{
	DeleteList();
}

void COpenGLWnd::CGLDispList::DeleteList()
{
	if (m_glListId) {
		glDeleteLists(m_glListId, 1);
		m_glListId = 0;
	}
}

//////////////////////////////////////////////////////////////////////
// Member functions

BOOL COpenGLWnd::CGLDispList::IsValid()
{
	return(m_glListId != 0);
}

void COpenGLWnd::CGLDispList::Draw()
{
	// if the list is not empty...
	if(m_glListId) {
		if(m_bIsolated) {
			// save current transformation matrix
			glPushMatrix();
			// save current OGL internal state (lighting, shading, and such)
			glPushAttrib(GL_ALL_ATTRIB_BITS);
		}
	
		// draw the list
		glCallList(m_glListId);
		if(m_bIsolated) {
			// restore transformation matrix
			glPopMatrix();
			// restore OGL internal state
			glPopAttrib();
		}
	}
}

void COpenGLWnd::CGLDispList::StartDef(BOOL bImmediateExec)
{
	// set the context for GL calls (if needed)
//	BeginGLCommands();
	// check if another list is under construction
	int cur = 0;
	glGetIntegerv(GL_LIST_INDEX,&cur);
	if(cur != 0) {
		TRACE("Error: Nested display list definition!");
		ASSERT(FALSE);
	}
	// if the list is empty firstly allocate one
	if(!m_glListId) 
		m_glListId=glGenLists(1);

	// start or replace a list definition
	if (bImmediateExec) 
		glNewList(m_glListId,GL_COMPILE_AND_EXECUTE);
	else  
		glNewList(m_glListId,GL_COMPILE);
}

void COpenGLWnd::CGLDispList::EndDef()
{
	// check the coupling with a preceding call to StartDef()
	int cur;
	glGetIntegerv(GL_LIST_INDEX,&cur);
	if(cur != m_glListId) {
		TRACE("CGLDispList:Missing StartDef() before EndDef()\n");
		return;
	}

	// close list definition
	glEndList();
	// free the context (if needed)
//	EndGLCommands();
}

void COpenGLWnd::CreateRGBPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;

	// get the initially choosen video mode
	n = ::GetPixelFormat(m_pCDC->GetSafeHdc());
    ::DescribePixelFormat(m_pCDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	// if is an indexed one...
    if (pfd.dwFlags & PFD_NEED_PALETTE) {
		// ... construct an equilibrated palette (3 red bits, 3 green bits, 2 blue bits)
		// NOTE: this code is integrally taken from MFC example Cube
		n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

        ASSERT(pPal != NULL);

        pPal->palVersion = 0x300;
        pPal->palNumEntries = n;
        for (i=0; i<n; i++) {
            pPal->palPalEntry[i].peRed=ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
            pPal->palPalEntry[i].peGreen=ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
            pPal->palPalEntry[i].peBlue=ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
            pPal->palPalEntry[i].peFlags=0;
        }

		// fix up the palette to include the default Windows palette
        if ((pfd.cColorBits == 8)                           &&
            (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
            (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)) {
			for (i = 1 ; i <= 12 ; i++)
                pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
        }

        m_CurrentPalette.CreatePalette(pPal);
        delete [] pPal;

		// set the palette
        m_pOldPalette=m_pCDC->SelectPalette(&m_CurrentPalette, FALSE);
        m_pCDC->RealizePalette();
    }
}

unsigned char COpenGLWnd::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) {
    case 1:
        val &= 0x1;
        return _oneto8[val];
    case 2:
        val &= 0x3;
        return _twoto8[val];
    case 3:
        val &= 0x7;
        return _threeto8[val];

    default:
        return 0;
    }
}

BOOL COpenGLWnd::bSetupPixelFormat()
{
	// define default desired video mode (pixel format)
	static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
        1,								// version number
        PFD_DRAW_TO_WINDOW |			// support window
        PFD_SUPPORT_OPENGL |			// support OpenGL
        PFD_DOUBLEBUFFER,				// double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        24,                             // 32-bit z-buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };

	// let the user change some parameters if he wants
	BOOL bDoublBuf;
	BOOL bStereo;
	BOOL bAnaglyph;
	BOOL bSplitScreen;
	int Alignment;
	int Projection;
	ColorsNumber cnum;
	ZAccuracy zdepth;
	VideoMode(cnum, zdepth, bDoublBuf, bStereo, bAnaglyph, bSplitScreen, Alignment, Projection);
	
	//set the changes
	if (bDoublBuf) {
		pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
		if (bStereo)
			pfd.dwFlags |=	PFD_STEREO;
	}
	else {
		pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		if (bStereo)
			pfd.dwFlags |=	PFD_STEREO;
	}
	switch(cnum) {
		case INDEXED: pfd.cColorBits=8;
		case THOUSANDS: pfd.cColorBits=16;
		case MILLIONS: pfd.cColorBits=24;
		case MILLIONS_WITH_TRANSPARENCY: pfd.cColorBits=32;
	};
	switch(zdepth) {
		case NORMAL: pfd.cDepthBits=16;
		case ACCURATE: pfd.cDepthBits=24;			// careful with 32-bit depth...GForce goes to software rendering
	};

	// ask the system for such video mode
    ASSERT(m_pCDC != NULL);

	// get best match for pixel format
	int pixelformat = DecideOnPixelFormat(m_pCDC->GetSafeHdc(), &pfd, TRUE);
	if (pixelformat <= 0) {
		TRACE("DecideOnPixelFormat() Failed %d...using ChoosePixelFormat()\r\n",GetLastError());
		pixelformat = ChoosePixelFormat(m_pCDC->GetSafeHdc(), &pfd);
		if (pixelformat == 0) {
			AfxMessageBox("ChoosePixelFormat failed");
			return FALSE;
		}
	}

// use code below when not using DecideOnPixelFormat()
//	int pixelformat;
//	if ( (pixelformat = ChoosePixelFormat(m_pCDC->GetSafeHdc(), &pfd)) == 0 ) {
//		AfxMessageBox("ChoosePixelFormat failed");
//		return FALSE;
//	}

	// try to set this video mode    
	if (SetPixelFormat(m_pCDC->GetSafeHdc(), pixelformat, &pfd) == FALSE) {
		// the requested video mode is not available so get a default one
        pixelformat = 1;	
		if (DescribePixelFormat(m_pCDC->GetSafeHdc(), pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd)==0) {
			// neither the requested nor the default are available: fail
			AfxMessageBox("SetPixelFormat failed (no OpenGL compatible video mode)");
			return FALSE;
		}
    }
	else {
		// find out if we got stereo
		DescribePixelFormat(m_pCDC->GetSafeHdc(), pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		if (pfd.dwFlags & PFD_STEREO) {
			m_bStereoEnabled = TRUE;
		}
		else {
			m_bStereoEnabled = FALSE;
			m_bUseAnaglyph = bAnaglyph;
			m_bUseSplitScreen = bSplitScreen;
			m_Alignment = Alignment;
		}
		if (pfd.dwFlags & PFD_DOUBLEBUFFER) {
			m_bDoubleBufferEnabled = TRUE;
		}
		else {
			m_bDoubleBufferEnabled = FALSE;
		}

		m_ProjectionType = Projection;

		TRACE1("Pixel format selected for rendering: %i\r\n", pixelformat);
	}

    return TRUE;
}


void COpenGLWnd::OnCreateGL()
{
	// called from OnSize() the first time the window has a valid size
	// set background color to black
	glClearColor((float) GetRValue(m_BackgroundColor) / 255.0f, (float) GetGValue(m_BackgroundColor) / 255.0f, (float) GetBValue(m_BackgroundColor) / 255.0f,1.0f );

	// get font ready for use
	MakeFont();

	// perform hidden line/surface removal (enabling Z-Buffer)
	glEnable(GL_DEPTH_TEST);

	// set clear Z-Buffer value
	glClearDepth(1.0f);
	
	// enable simple lighting with light at viewpoint
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
}


void COpenGLWnd::OnDestroyGL()
{
}

void COpenGLWnd::OnDrawGLBeforeTrackball()
{
//	TRACE("In COpenGLWnd::OnDrawGLBeforeTrackball\n");

	// do nothing...rely on derived class to implement this function
}

void COpenGLWnd::OnDrawGL()
{
//	TRACE("In COpenGLWnd::OnDrawGL\n");	

	SetAnaglyphSensitiveColor(1.0, 1.0, 0.0);

	glDisable(GL_LIGHTING);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0);
	glPushMatrix();
		glTranslated(-0.5, -0.5, -0.5);
		DrawBox(CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0, 1.0);
	glPopMatrix();

	// add labels
	CPoint3D pt;
	CPoint3D orgPt(-0.5,-0.5,-0.5);
	pt = orgPt;
	float endAx = 1.05f;
	pt.Translate(endAx, 0, 0);
	glRasterPos3d(pt.x, pt.y, pt.z);
	PrintString("X+");
	pt = orgPt;
	pt.Translate(0, endAx, 0);
	glRasterPos3d(pt.x, pt.y, pt.z);
	PrintString("Y+");
	pt = orgPt;
	pt.Translate(0, 0, endAx);
	glRasterPos3d(pt.x, pt.y, pt.z);
	PrintString("Z+");

	glEnable(GL_LIGHTING);
}

void COpenGLWnd::OnDrawGDI(CPaintDC *pDC)
{
//	TRACE("In COpenGLWnd::OnDrawGDI\n");	
}

void COpenGLWnd::OnSizeGL(int cx, int cy)
{
	// set correspondence between window and OGL viewport
	glViewport(0,0,cx,cy);

	// update the camera
 	glPushMatrix();
		glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(m_Camera.aperture, m_Camera.screenwidth / m_Camera.screenheight, m_Camera.nearplane, m_Camera.farplane);
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// reset trackball
	CRect rc(0, 0, cx, cy);
	m_TrackBall.ClientAreaResize(rc);

	// reset translation controller
	m_TranslationController.ClientAreaResize(rc);
}

void COpenGLWnd::SetClearCol(COLORREF rgb)
{
	m_BackgroundColor = rgb;
	Invalidate();	// force redraw
}

void COpenGLWnd::MakeFont()
{
	int i;

	BeginGLCommands();

	// check m_FontListBase not in use
	BOOL bUsed = FALSE;
	do {
		for (i = 0; i < 255; i ++) {
			if (glIsList(m_FontListBase+i)) {
				m_FontListBase += 256;
				bUsed = TRUE; 
				break;
			}
		}
	} while (bUsed==TRUE);

	// create font and select it for bitmaps
	m_Font.CreatePointFont(120, "MS Sans Serif");
	SelectObject(m_pCDC->GetSafeHdc(), m_Font); 

	// create the bitmap display lists 
	// we're making images of glyphs 0 thru 255 
	// the display list numbering starts at m_FontListBase, an arbitrary choice 
	m_bGotFont = wglUseFontBitmaps(m_pCDC->GetSafeHdc(), 0, 255, m_FontListBase); 
	
	EndGLCommands();
}

void COpenGLWnd::PrintString(const char* str)
{
	if (!m_bGotFont)
		return;
    glPushAttrib(GL_LIST_BIT);
    glListBase(m_FontListBase);
    glCallLists((GLsizei ) strlen(str), GL_UNSIGNED_BYTE, (GLubyte*)str);
    glPopAttrib();
}

void COpenGLWnd::NewCopyToClipboard()
{
	// Get client geometry 
	CRect rect; 
	GetClientRect(&rect); 
	CSize size(rect.Width(),rect.Height()); 
	/* The following is the bytes per pixel of the data
	returned from glReadPixels().  When GL_BGR_EXT is
	specified, glReadPixels() places the image data into
	the buffer in 24 bits/pixel DIB-compatible format
	regardless of the current video color depth.
	DIB-compatible format requires that the number of bytes
	in each image row be a multiple of 4 (DWORD alignment).
	glReadPixels() adds this padding automatically, so we
	must allocate additional memory so the padding does not
	result in a buffer overrun. */
	const int bytesPerPixel = 3;
	int bytesPerRow = (bytesPerPixel * size.cx + 3) & ~3;
	// bytesPerRow is now an integer multiple of 4
	int NbBytes = bytesPerRow * size.cy; 

	// Fill header 
	BITMAPINFOHEADER header; 
	header.biWidth = size.cx; 
	header.biHeight = size.cy; 
	header.biSizeImage = NbBytes; 
	header.biSize = sizeof(BITMAPINFOHEADER); 
	header.biPlanes = 1; 
	header.biBitCount =  bytesPerPixel * 8; // RGB 
	header.biCompression = 0; 
	header.biXPelsPerMeter = 11811;			// 300 dpi
	header.biYPelsPerMeter = 11811;			// 300 dpi
	header.biClrUsed = 0; 
	header.biClrImportant = 0; 

	// Generate handle 
	HANDLE handle = (HANDLE)::GlobalAlloc (GHND,sizeof(BITMAPINFOHEADER) + NbBytes); 
	if(handle != NULL) { 
		// Lock handle 
		char *pData = (char *) ::GlobalLock((HGLOBAL)handle); 
		// Copy header and data 
		memcpy(pData, &header, sizeof(BITMAPINFOHEADER));
		
		BeginGLCommands();
		// Copy from OpenGL 
		::glReadPixels(0, 0, size.cx,size.cy,GL_BGR_EXT,GL_UNSIGNED_BYTE,pData + sizeof(BITMAPINFOHEADER)); 
		EndGLCommands();

		// Unlock 
		::GlobalUnlock((HGLOBAL)handle); 

		// Push DIB in clipboard 
		OpenClipboard(); 
		EmptyClipboard(); 
		SetClipboardData(CF_DIB,handle); 
		CloseClipboard(); 
	} 
}

void COpenGLWnd::CopyToClipboard()
{
	CRect R;
	GetWindowRect(&R);
	if (!OpenClipboard())
		return;
	CWindowDC dc(this);
	CBitmap* pbmOld = NULL;
	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(&dc));

	// reduce size of client to account for border
	R.DeflateRect(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE), ::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

	CBitmap bm;
	VERIFY(bm.CreateCompatibleBitmap(&dc,R.Width(),R.Height()));
	ASSERT(bm.m_hObject != NULL);
	pbmOld = dcMem.SelectObject(&bm);
	
	dcMem.PatBlt(0,0,R.Width(),R.Height(),WHITENESS);
	VERIFY(dcMem.BitBlt(0, 0, R.Width(), R.Height(), &dc, ::GetSystemMetrics(SM_CXEDGE) - 1, ::GetSystemMetrics(SM_CYEDGE) - 1, SRCCOPY));	
//	VERIFY(dcMem.BitBlt(0, 0, R.Width(), R.Height(), &dc, 0, 0, SRCCOPY));	
	HGDIOBJ hBM=bm.Detach();
   	VERIFY(::EmptyClipboard());
   	VERIFY(::SetClipboardData(CF_BITMAP, hBM));
   	VERIFY(::CloseClipboard());

	dcMem.SelectObject(pbmOld);
	dcMem.DeleteDC();
}


void COpenGLWnd::ResetCamera()
{
//	TRACE("In COpenGLWnd::ResetCamera\n");	

	m_Camera.filmwidth = 36.0;
	m_Camera.filmheight = 24.0;

	m_Camera.focallength = 150;
	m_Camera.focallength = 100;
	m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;

	// viewpoint
	m_Camera.vp.x = 0.0;
	m_Camera.vp.y = 0.0;
	m_Camera.vp.z = 4.0;
//	m_Camera.vp.z = 0.1;

	// view direction
	m_Camera.vd.x = 0.0; 
	m_Camera.vd.y = 0.0; 
	m_Camera.vd.z = -4.0;
//	m_Camera.vd.z = -0.1;

	// focus point
	m_Camera.pr.x = 0.0;
	m_Camera.pr.y = 0.0;
	m_Camera.pr.z = 0.0;
//	m_Camera.pr.z = 0.0;

	// up vector
	m_Camera.vu.x = 0;  
	m_Camera.vu.y = 1; 
	m_Camera.vu.z = 0;

//	m_Camera.nearplane = 0.05;
	m_Camera.nearplane = 3.0;
	m_Camera.farplane = 5.0;
}

BOOL COpenGLWnd::IsStereo()
{
	return(m_bStereoEnabled);
}

BOOL COpenGLWnd::IsDoubleBuffered()
{
	return(m_bDoubleBufferEnabled);
}

BOOL COpenGLWnd::RenderToMemoryAndSaveImage(LPCTSTR filename, int Eye)
{
	BOOL retcode = TRUE;

	// need to make sure we can save the current context
	BeginGLCommands();

	// set up memory context
	if (PrepareForMemoryRendering()) {
		// render
		// Clear the buffers...clear left and right if in stereo
		glClearColor((float) GetRValue(m_BackgroundColor) / 255.0f, (float) GetGValue(m_BackgroundColor) / 255.0f, (float) GetBValue(m_BackgroundColor) / 255.0f,1.0f );
		glClearDepth(1.0f);
		if (m_bDoubleBufferEnabled)
			glDrawBuffer(GL_BACK);
		else
			glDrawBuffer(GL_FRONT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Misc stuff */
		m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
		m_Camera.eyesep = m_Camera.focallength / 1300.0;

		MoveToEyePosition(Eye);
		if (m_MotionController == TRACKBALL)
			m_TrackBall.IssueGLrotation();
		else {
			ApplyTranslationControl();
//			m_TranslationController.DoGLTranslation();
		}

		OnDrawGL();
		OnDrawGLMask();
		OnDrawHeadsUpDisplay();

		// execute OGL commands (flush the OGL graphical pipeline)
		glFinish();

		// save image
		if (!SaveMemoryRendering(filename))
			retcode = FALSE;

	}
	EndGLCommands();

	Invalidate();

	return(retcode);
}

BOOL COpenGLWnd::SaveScreenImage(LPCTSTR filename, CAVIFile* AVIFile)
{
	if (!filename && !AVIFile)
		return(FALSE);

	// convert the saved openGL screen to a device independent bitmap
	GLubyte* SavedScreen;
	BOOL retcode = TRUE;

	CRect clientrc;
	GetClientRect(clientrc);
	int iwidth = clientrc.right - clientrc.left;

	int width = clientrc.right - clientrc.left;
	int height = clientrc.bottom - clientrc.top;

	CSize isz(width, height);
	CImage img;
	img.CreateImage(isz, 24);

	// fix file names
	CString LeftImageFileName("");
	CString RightImageFileName("");

	if (filename) {
		LeftImageFileName = _T(filename);
		if (m_bStereoEnabled) {
			CFileSpec fs(LeftImageFileName);
			CString FileNameOnly = fs.FileTitle();
			fs.SetTitle(FileNameOnly + "_left");
			LeftImageFileName = fs.GetFullSpec();
			fs.SetTitle(FileNameOnly + "_right");
			RightImageFileName = fs.GetFullSpec();
		}
	}

	// do left image
	BeginGLCommands();
	glReadBuffer(GL_FRONT_LEFT);

	// get image from OpenGl buffer
	// scale for full window...uses ortho scaling for 2d drawing
	glPushMatrix();
		glMatrixMode(GL_PROJECTION);
   		glLoadIdentity();

		// set viewport to entire window
		glViewport(0, 0, width + 1, height + 1);
		gluOrtho2D(0.0, 1.0, 0.0, 1.0);
		
		glMatrixMode(GL_MODELVIEW);
   		glLoadIdentity();

		// set up memory for screen image
		SavedScreen = new GLubyte [(width) * (height) * 3];

		if (!SavedScreen)
			retcode = FALSE;

		// get image
		if (retcode) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, SavedScreen);
		}
	glPopMatrix();

	// move image to BMP
	BYTE* lppix;
	int i, j;
	if (retcode) {
		for (j = clientrc.top; j < clientrc.bottom; j ++) {
			lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
			for (i = clientrc.left; i < clientrc.right; i ++) {
				*lppix = SavedScreen[(j * width + i) * 3 + 2];
				*(lppix + 1) = SavedScreen[(j * width + i) * 3 + 1];
				*(lppix + 2) = SavedScreen[(j * width + i) * 3];
				lppix += 3;
			}
		}

		// write BMP
		CFile file;
		if (filename) {
			if (file.Open(LeftImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
				img.Write(&file);
				file.Close();
			}
		}

		// save frame to AVI file
		if (AVIFile) {
			AVIFile->AddFrame(img.m_hBitmap);
		}

		// if in stereo, do the right image
		if (m_bStereoEnabled && filename) {
			// do right image
			glReadBuffer(GL_FRONT_RIGHT);

			// get image form OpenGl buffer
			// scale for full window...uses ortho scaling for 2d drawing
			glPushMatrix();
				glMatrixMode(GL_PROJECTION);
   				glLoadIdentity();

				// set viewport to entire window
				glViewport(0, 0, width + 1, height + 1);
				gluOrtho2D(0.0, 1.0, 0.0, 1.0);
				
				glMatrixMode(GL_MODELVIEW);
   				glLoadIdentity();

				// get image
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, SavedScreen);
			glPopMatrix();

			// move image to BMP
			for (j = clientrc.top; j < clientrc.bottom; j ++) {
				lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
				for (i = clientrc.left; i < clientrc.right; i ++) {
					*lppix = SavedScreen[(j * width + i) * 3 + 2];
					*(lppix + 1) = SavedScreen[(j * width + i) * 3 + 1];
					*(lppix + 2) = SavedScreen[(j * width + i) * 3];
					lppix += 3;
				}
			}

			// write BMP
			if (file.Open(RightImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
				img.Write(&file);
				file.Close();
			}
		}
		delete [] SavedScreen;
	}

	EndGLCommands();

	return(retcode);
}

BOOL COpenGLWnd::Old_SaveScreenImage(LPCTSTR filename)
{
	// convert the saved openGL screen to a device independent bitmap
	GLubyte* SavedScreen;
	BOOL retcode = TRUE;

	CRect clientrc;
	GetClientRect(clientrc);
	int iwidth = clientrc.right - clientrc.left;

	int width = clientrc.right - clientrc.left;
	int height = clientrc.bottom - clientrc.top;

	CSize isz(width, height);
	CImage img;
	img.CreateImage(isz, 24);

	// fix file names
	CString LeftImageFileName(filename);
	CString RightImageFileName;

	if (m_bStereoEnabled) {
		CFileSpec fs(LeftImageFileName);
		CString FileNameOnly = fs.FileTitle();
		fs.SetTitle(FileNameOnly + "_left");
		LeftImageFileName = fs.GetFullSpec();
		fs.SetTitle(FileNameOnly + "_right");
		RightImageFileName = fs.GetFullSpec();
	}

	// do left image
	BeginGLCommands();
	glReadBuffer(GL_FRONT_LEFT);

	// get image form OpenGl buffer
	// scale for full window...uses ortho scaling for 2d drawing
	glPushMatrix();
		glMatrixMode(GL_PROJECTION);
   		glLoadIdentity();

		// set viewport to entire window
		glViewport(0, 0, width + 1, height + 1);
		gluOrtho2D(0.0, 1.0, 0.0, 1.0);
		
		glMatrixMode(GL_MODELVIEW);
   		glLoadIdentity();

		// set up memory for screen image
		SavedScreen = new GLubyte [(width) * (height) * 3];

		if (!SavedScreen)
			retcode = FALSE;

		// get image
		if (retcode) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, SavedScreen);
		}
	glPopMatrix();

	// move image to BMP
	BYTE* lppix;
	int i, j;
	if (retcode) {
		for (j = clientrc.top; j < clientrc.bottom; j ++) {
			lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
			for (i = clientrc.left; i < clientrc.right; i ++) {
				*lppix = SavedScreen[(j * width + i) * 3 + 2];
				*(lppix + 1) = SavedScreen[(j * width + i) * 3 + 1];
				*(lppix + 2) = SavedScreen[(j * width + i) * 3];
				lppix += 3;
			}
		}

		// write BMP
		CFile file;
		if (file.Open(LeftImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
			img.Write(&file);
			file.Close();
		}

		// if in stereo, do the right image
		if (m_bStereoEnabled) {
			// do right image
			glReadBuffer(GL_FRONT_RIGHT);

			// get image form OpenGl buffer
			// scale for full window...uses ortho scaling for 2d drawing
			glPushMatrix();
				glMatrixMode(GL_PROJECTION);
   				glLoadIdentity();

				// set viewport to entire window
				glViewport(0, 0, width + 1, height + 1);
				gluOrtho2D(0.0, 1.0, 0.0, 1.0);
				
				glMatrixMode(GL_MODELVIEW);
   				glLoadIdentity();

				// get image
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, SavedScreen);
			glPopMatrix();

			// move image to BMP
			for (j = clientrc.top; j < clientrc.bottom; j ++) {
				lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
				for (i = clientrc.left; i < clientrc.right; i ++) {
					*lppix = SavedScreen[(j * width + i) * 3 + 2];
					*(lppix + 1) = SavedScreen[(j * width + i) * 3 + 1];
					*(lppix + 2) = SavedScreen[(j * width + i) * 3];
					lppix += 3;
				}
			}

			// write BMP
			if (file.Open(RightImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
				img.Write(&file);
				file.Close();
			}
		}
		delete [] SavedScreen;
	}

	EndGLCommands();

	return(retcode);
}

BOOL COpenGLWnd::SaveDepthImage(LPCTSTR filename, double Minx, double Miny, double Minz, double Maxx, double Maxy, double Maxz, BOOL SaveInfo)
{
	// convert the saved openGL screen to a device independent bitmap
	GLfloat* SavedScreen;
	BOOL retcode = TRUE;

	CRect clientrc;
	GetClientRect(clientrc);
	int iwidth = clientrc.right - clientrc.left;

	int width = clientrc.right - clientrc.left;
	int height = clientrc.bottom - clientrc.top;

	CSize isz(width, height);
	CImage img;
	img.CreateImage(isz, 24);

	// fix file names
	CString LeftImageFileName(filename);
	CString RightImageFileName;

	if (m_bStereoEnabled) {
		CFileSpec fs(LeftImageFileName);
		CString FileNameOnly = fs.FileTitle();
		fs.SetTitle(FileNameOnly + "_left");
		LeftImageFileName = fs.GetFullSpec();
		fs.SetTitle(FileNameOnly + "_right");
		RightImageFileName = fs.GetFullSpec();
	}

	// do left image
	BeginGLCommands();
	glReadBuffer(GL_FRONT_LEFT);

	// get image form OpenGl buffer
	// scale for full window...uses ortho scaling for 2d drawing
	glPushMatrix();
		glMatrixMode(GL_PROJECTION);
   		glLoadIdentity();

		// set viewport to entire window
		glViewport(0, 0, width + 1, height + 1);
		gluOrtho2D(0.0, 1.0, 0.0, 1.0);
		
		glMatrixMode(GL_MODELVIEW);
   		glLoadIdentity();

		// set up memory for screen image
		SavedScreen = new GLfloat [(width) * (height)];

		if (!SavedScreen)
			retcode = FALSE;

		// get image
		if (retcode) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, SavedScreen);
		}
	glPopMatrix();

	// move image to BMP
	BYTE* lppix;
	int i, j;
	if (retcode) {
		// get min/max values in depth array
		double mindepth = 999999999.0;
		double maxdepth = -999999999.0;
		for (j = clientrc.top; j < clientrc.bottom; j ++) {
			for (i = clientrc.left; i < clientrc.right; i ++) {
				if ((double) SavedScreen[(j * width + i)] < mindepth)
					mindepth = (double) SavedScreen[(j * width + i)];
				if ((double) SavedScreen[(j * width + i)] > maxdepth && SavedScreen[(j * width + i)] != 1.0f)
					maxdepth = (double) SavedScreen[(j * width + i)];
			}
		}

		// move depth info to image
		unsigned char intensity;
		for (j = clientrc.top; j < clientrc.bottom; j ++) {
			lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
			for (i = clientrc.left; i < clientrc.right; i ++) {
				if (SavedScreen[(j * width + i)] == 1.0f)
					intensity = 0;
				else
					intensity = 255 - (unsigned char) (((double) SavedScreen[(j * width + i)] - mindepth) / (maxdepth - mindepth) * 255.0);
				*lppix = intensity;
				*(lppix + 1) = intensity;
				*(lppix + 2)= intensity;
				lppix += 3;
			}
		}

		// write BMP
		CFile file;
		if (file.Open(LeftImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
			img.Write(&file);
			file.Close();
		}

		if (SaveInfo) {
			CFileSpec InfoFS(LeftImageFileName);
			InfoFS.SetExt(".csv");
			FILE* f = fopen(InfoFS.GetFullSpec(), "wt");
			if (f) {
				fprintf(f, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", mindepth, maxdepth, Minx, Miny, Minz, Maxx, Maxy, Maxz);
				fclose(f);
			}
		}

		// if in stereo, do the right image
		if (m_bStereoEnabled) {
			// do right image
			glReadBuffer(GL_FRONT_RIGHT);

			// get image form OpenGl buffer
			// scale for full window...uses ortho scaling for 2d drawing
			glPushMatrix();
				glMatrixMode(GL_PROJECTION);
   				glLoadIdentity();

				// set viewport to entire window
				glViewport(0, 0, width + 1, height + 1);
				gluOrtho2D(0.0, 1.0, 0.0, 1.0);
				
				glMatrixMode(GL_MODELVIEW);
   				glLoadIdentity();

				// get image
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, SavedScreen);
			glPopMatrix();

			// get min/max values in depth array
			mindepth = 999999999.0;
			maxdepth = -999999999.0;
			for (j = clientrc.top; j < clientrc.bottom; j ++) {
				for (i = clientrc.left; i < clientrc.right; i ++) {
					if ((double) SavedScreen[(j * width + i)] < mindepth)
						mindepth = (double) SavedScreen[(j * width + i)];
					if ((double) SavedScreen[(j * width + i)] > maxdepth && SavedScreen[(j * width + i)] != 1.0f)
						maxdepth = (double) SavedScreen[(j * width + i)];
				}
			}

			// move depth info to image
			unsigned char intensity;
			for (j = clientrc.top; j < clientrc.bottom; j ++) {
				lppix = img.m_lpImage + (j - clientrc.top) * img.m_dwBytesPerScanLine;
				for (i = clientrc.left; i < clientrc.right; i ++) {
					if (SavedScreen[(j * width + i)] == 1.0f)
						intensity = 0;
					else
						intensity = 255 - (unsigned char) (((double) SavedScreen[(j * width + i)] - mindepth) / (maxdepth - mindepth) * 255.0);
					*lppix = intensity;
					*(lppix + 1) = intensity;
					*(lppix + 2)= intensity;
					lppix += 3;
				}
			}

			// write BMP
			if (file.Open(RightImageFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
				img.Write(&file);
				file.Close();
			}
		}
		delete [] SavedScreen;
	}

	EndGLCommands();

	return(retcode);
}

void COpenGLWnd::MoveToEyePosition(int Eye, int OffsetIndex)
{
	CPoint3D r;
	double ratio,radians,wd2,ndfl;
	double left,right,top,bottom;

	if (m_bUseSplitScreen)
		ratio  = ((double) m_Camera.screenwidth / 2.0) / (double) m_Camera.screenheight;		// use half of width for spit screen stereo
	else
		ratio  = (double) m_Camera.screenwidth / (double) m_Camera.screenheight;

	radians = DTOR * m_Camera.aperture / 2.0f;
	wd2     = m_Camera.nearplane * tan(radians);
	ndfl    = m_Camera.nearplane / m_Camera.focallength;
	if (m_MotionController == FLYBALL) {
		// for roaming, the near plane is set to a very small value resulting in a very small eye separation...no 3D effect
		// use an artificial value for the near plane distance
		ndfl    = 0.1 / m_Camera.focallength;
	}
	m_Camera.eyesep = ndfl * m_EyeSeparationMultiplier;


	// calculate stuff
	CROSSPROD(m_Camera.vd,m_Camera.vu,r);
	r.Normalize();
	r.x *= m_Camera.eyesep / 2.0;
	r.y *= m_Camera.eyesep / 2.0;
	r.z *= m_Camera.eyesep / 2.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch (Eye) {
	case LEFT:
		left  = - ratio * wd2 + 0.5 * m_Camera.eyesep * ndfl;
		right =   ratio * wd2 + 0.5 * m_Camera.eyesep * ndfl;
		top    =   wd2;
		bottom = - wd2;
		if (m_bUseSplitScreen) {
			if (m_Alignment == PARALLEL)
				glViewport(0, 0, m_ClientRect.right / 2, m_ClientRect.bottom);							// left side of screen...use for parallel viewing
			else
				glViewport(m_ClientRect.right / 2, 0, m_ClientRect.right / 2, m_ClientRect.bottom);		// right side of screen...use for X-eyed
		}
		Frustum(left, right, bottom, top, m_Camera.nearplane, m_Camera.farplane, jarray(8)[OffsetIndex][0], jarray(8)[OffsetIndex][1]);

		glMatrixMode(GL_MODELVIEW);
		if (m_bDoubleBufferEnabled)
			glDrawBuffer(GL_BACK_LEFT);
		else
			glDrawBuffer(GL_LEFT);
		glLoadIdentity();
		gluLookAt(m_Camera.vp.x - r.x, m_Camera.vp.y - r.y, m_Camera.vp.z - r.z,
				  m_Camera.vp.x - r.x + m_Camera.vd.x, m_Camera.vp.y - r.y + m_Camera.vd.y, m_Camera.vp.z - r.z + m_Camera.vd.z,
				  m_Camera.vu.x, m_Camera.vu.y, m_Camera.vu.z);
		break;
	case RIGHT:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left  = - ratio * wd2 - 0.5 * m_Camera.eyesep * ndfl;
		right =   ratio * wd2 - 0.5 * m_Camera.eyesep * ndfl;
		top    =   wd2;
		bottom = - wd2;
		if (m_bUseSplitScreen) {
			if (m_Alignment == PARALLEL)
				glViewport(m_ClientRect.right / 2, 0, m_ClientRect.right / 2, m_ClientRect.bottom);		// right side of screen...use for parallel viewing
			else
				glViewport(0, 0, m_ClientRect.right / 2, m_ClientRect.bottom);							// left side of screen...use for X-eyed
		}
		Frustum(left, right, bottom, top, m_Camera.nearplane, m_Camera.farplane, jarray(8)[OffsetIndex][0], jarray(8)[OffsetIndex][1]);

		glMatrixMode(GL_MODELVIEW);
		if (m_bDoubleBufferEnabled)
			glDrawBuffer(GL_BACK_RIGHT);
		else
			glDrawBuffer(GL_RIGHT);
		glLoadIdentity();
		gluLookAt(m_Camera.vp.x + r.x, m_Camera.vp.y + r.y, m_Camera.vp.z + r.z,
				  m_Camera.vp.x + r.x + m_Camera.vd.x, m_Camera.vp.y + r.y + m_Camera.vd.y, m_Camera.vp.z + r.z + m_Camera.vd.z,
				  m_Camera.vu.x, m_Camera.vu.y, m_Camera.vu.z);
		break;
	case MIDDLE:
	default:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left  = - ratio * wd2;
		right =   ratio * wd2;
		top    =   wd2;
		bottom = - wd2;
		glViewport(0, 0, m_ClientRect.right, m_ClientRect.bottom);
		Frustum(left, right, bottom, top, m_Camera.nearplane, m_Camera.farplane, jarray(8)[OffsetIndex][0], jarray(8)[OffsetIndex][1]);

		glMatrixMode(GL_MODELVIEW);
		if (m_bDoubleBufferEnabled)
			glDrawBuffer(GL_BACK);
		else
			glDrawBuffer(GL_FRONT);
		glLoadIdentity();
		gluLookAt(m_Camera.vp.x, m_Camera.vp.y, m_Camera.vp.z,
				  m_Camera.vp.x + m_Camera.vd.x, m_Camera.vp.y + m_Camera.vd.y, m_Camera.vp.z + m_Camera.vd.z,
				  m_Camera.vu.x, m_Camera.vu.y, m_Camera.vu.z);
		break;
	}
	
	m_CurrentEyePosition = Eye;
}

void COpenGLWnd::Frustum(double left, double right, double bottom, double top, double oglnear, double oglfar, double pixdx, double pixdy, double eyedx, double eyedy, double focus)
{
	GLdouble xwsize, ywsize;
	GLdouble dx, dy;
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	xwsize = right - left;
	ywsize = top - bottom;
	dx = -(pixdx * xwsize / (GLdouble) viewport[2] + eyedx * oglnear / focus);
	dy = -(pixdy * ywsize / (GLdouble) viewport[3] + eyedy * oglnear / focus);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (m_ProjectionType == PERSPECTIVE)
		glFrustum(left + dx, right + dx, bottom + dy, top + dy, oglnear, oglfar);
	else
		glOrtho(left + dx, right + dx, bottom + dy, top + dy, oglnear, oglfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(-eyedx, -eyedy, 0.0);
}

void COpenGLWnd::Perspective(double fovy, double aspect, double oglnear, double oglfar, double pixdx, double pixdy, double eyedx, double eyedy, double focus)
{
	GLdouble fov2, left, right, bottom, top;
	fov2 = ((fovy * PI_) / 180.0) / 2.0;

	top = oglnear / (cos(fov2) / sin(fov2));
	bottom = -top;
	right = top * aspect;
	left = -right;

	Frustum(left, right, bottom, top, oglnear, oglfar, pixdx, pixdy, eyedx, eyedy, focus);
}

void COpenGLWnd::AADraw(int eye)
{
	// hard-coded for 8 passes
	for (int jitter = 1; jitter <= AAPASSES; jitter ++) {
		MoveToEyePosition(eye, jitter);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (m_MotionController == TRACKBALL)
			m_TrackBall.IssueGLrotation();
		else {
			ApplyTranslationControl();

			// allow apps to modify the camera
			ModifyCameraLocation();
		}

		OnDrawGL();
		// do load on first pass to avoid having to clear the accumulation buffer
		if (jitter == 1)
			glAccum(GL_LOAD, AAWEIGHT);
		else
			glAccum(GL_ACCUM, AAWEIGHT);
		// display progressive images...start out dark due to weighting
		//	glAccum(GL_RETURN, AAWEIGHT * (double) jitter);
		//	SwapBuffers(m_pCDC->GetSafeHdc());
	}
	glAccum(GL_RETURN, 1.0);
	glFlush();
}

BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style |= CS_OWNDC;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void COpenGLWnd::OnDrawGLMask()
{
	// do nothing
	// override in derived class to draw a mask over the GL view
}

void COpenGLWnd::OnUpdateGLState()
{
	// do nothing in base implementation
	// override in derived class to update variables that control the current state of the "model"
	// must do this outside the drawing loop in the derived class so left and right views are drawn using the same data
}

void COpenGLWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL KeyWasUsed = FALSE;

	CString csFilter("Bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*|");
	CString csExt("bmp");

	if (m_MotionController == TRACKBALL) {
		KeyWasUsed = m_TrackBall.Key(nChar);
	}
	else if (m_MotionController == FLYBALL) {
		KeyWasUsed = m_TranslationController.Key(nChar);
	}

	if (KeyWasUsed)
		Invalidate();
	else {
		// do some special key processing
		switch (nChar) {
		case VK_ESCAPE:
			m_TrackBall.ClearLastRotation();
			break;
		case 'm':
		case 'M':
			m_DoContinuousDrawing = m_DoContinuousDrawing ? FALSE : TRUE;
			m_TrackBall.ClearLastRotation();
			break;
		case 0x65:			// e
		case 0x45:			// E
			if (::GetKeyState(VK_SHIFT) & 0x80) {
				if (::GetKeyState(VK_CONTROL) & 0x80)
					m_EyeSeparationMultiplier = 1.0;
				else
					m_EyeSeparationMultiplier += 0.1;
			}
			else {
				if (m_EyeSeparationMultiplier > 0.2)
					m_EyeSeparationMultiplier -= 0.1;
			}
			Invalidate();
			break;
		case 0x61:			// a
		case 0x41:			// A
			m_bUseSplitScreen = FALSE;
			m_bUseAnaglyph = m_bUseAnaglyph ? FALSE : TRUE;
			Invalidate();
			break;
		case 0x74:			// t
		case 0x54:			// T
//			if (::GetKeyState(VK_SHIFT) & 0x80 && ::GetKeyState(VK_CONTROL) & 0x80) {
//				CFileDialog fd(FALSE, csExt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter);
//				if (fd.DoModal() == IDOK) {
//					CFileSpec fs(fd.GetPathName());
//					CString BaseName = fd.GetFileTitle();
//					BaseName += "_left";
//					fs.SetTitle(BaseName);
//					RenderToMemoryAndSaveImage(fs.GetFullSpec(), LEFT);
//					BaseName = fd.GetFileTitle();
//					BaseName += "_left";
//					fs.SetTitle(BaseName);
//					RenderToMemoryAndSaveImage(fs.GetFullSpec(), RIGHT);
//				}
//			}
//			else if (::GetKeyState(VK_CONTROL) & 0x80) {
			if (::GetKeyState(VK_CONTROL) & 0x80) {
				CFileDialog fd(FALSE, csExt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter);
				if (fd.DoModal() == IDOK) {
					RedrawWindow();
//					RenderToMemoryAndSaveImage(fd.GetPathName(), MIDDLE);
					SaveScreenImage(fd.GetPathName(), NULL);
				}
			}
			break;
		case 0x73:			// s
		case 0x53:			// S
			m_bUseAnaglyph = FALSE;
			m_bUseSplitScreen = m_bUseSplitScreen ? FALSE : TRUE;
			Invalidate();
			break;
		case 'x':			// x
		case 'X':			// X
			if (IsStereo() || m_bUseSplitScreen) {
				m_Alignment = ((m_Alignment == PARALLEL) ? XEYED : PARALLEL);
				Invalidate();
			}
			break;
		default:
			CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

BOOL COpenGLWnd::PrepareForMemoryRendering(int Width, int Height)
{
	// set DIB size for rendering
//	CSize szDIB(4096, 3072);
	CSize szDIB(Width, Height);
	
	// create DIB Section
	m_DIBSectionForRendering.CreateImage(szDIB, 24);

	// create memory DC, and associate it with the DIB
	if (!m_MemoryDCForRendering.CreateCompatibleDC(NULL)) {
		m_DIBSectionForRendering.Empty();
		return(FALSE);
	}
	::SelectObject(m_MemoryDCForRendering.GetSafeHdc(), m_DIBSectionForRendering.m_hBitmap);

	// 4. Setup memory DC's pixel format.
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 24;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 0;
	pixelDesc.cAccumRedBits = 0;
	pixelDesc.cAccumGreenBits = 0;
	pixelDesc.cAccumBlueBits = 0;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 0;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int nPixelIndex = ::ChoosePixelFormat(m_MemoryDCForRendering.GetSafeHdc(), &pixelDesc);
	if (nPixelIndex == 0) { // Choose default
		nPixelIndex = 1;
		if (::DescribePixelFormat(m_MemoryDCForRendering.GetSafeHdc(), nPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0) {
			m_MemoryDCForRendering.DeleteDC();
			m_DIBSectionForRendering.Empty();
			return(FALSE);
		}
	}

	if (!::SetPixelFormat(m_MemoryDCForRendering.GetSafeHdc(), nPixelIndex, &pixelDesc)) {
		m_MemoryDCForRendering.DeleteDC();
		m_DIBSectionForRendering.Empty();
		return(FALSE);
	}

	// create memory RC
	m_hMemoryRCForRendering = ::wglCreateContext(m_MemoryDCForRendering.GetSafeHdc());
	if (!m_hMemoryRCForRendering) {
		m_MemoryDCForRendering.DeleteDC();
		m_DIBSectionForRendering.Empty();
		return(FALSE);
	}

	// store old DC and RC...should be associated with screen rendering
	m_hOldDC = ::wglGetCurrentDC();
	m_hOldRC = ::wglGetCurrentContext(); 

	// save old RC size
	m_OldWidth = m_ClientRect.right;
	m_OldHeight = m_ClientRect.bottom;
	m_OldSplitScreenSetting = m_bUseSplitScreen;

	// make the memory RC current
	::wglMakeCurrent(m_MemoryDCForRendering.GetSafeHdc(), m_hMemoryRCForRendering);

	// do initial setup
	m_ClientRect.right = szDIB.cx;
	m_ClientRect.bottom = szDIB.cy;
	m_Camera.screenwidth = szDIB.cx;
	m_Camera.screenheight = szDIB.cy;
	m_bUseSplitScreen = FALSE;

	OnCreateGL();

	// ready to render
	return(TRUE);
}

BOOL COpenGLWnd::SaveMemoryRendering(LPCTSTR filename) 
{
	BOOL retcode = FALSE;

	// should have an image is in the DIB...we don't need the memory RC anymore
	// save the image to file
	OnDestroyGL();

	// release memory RC, and restore the old DC and RC
	::wglMakeCurrent(NULL, NULL);	
	::wglDeleteContext(m_hMemoryRCForRendering);

	// Restore last DC and RC
	::wglMakeCurrent(m_hOldDC, m_hOldRC);	
	m_ClientRect.right = m_OldWidth;
	m_ClientRect.bottom = m_OldHeight;
	m_Camera.screenwidth = m_OldWidth;
	m_Camera.screenheight = m_OldHeight;
	m_bUseSplitScreen = m_OldSplitScreenSetting;

	// reinitialize previous context
	OnCreateGL();

	// save image
	CFile file;
	if (file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		m_DIBSectionForRendering.Write(&file);
		file.Close();
		retcode = TRUE;
	}

	// clean up
	m_DIBSectionForRendering.Empty();
	m_MemoryDCForRendering.DeleteDC();
	m_hOldDC = NULL;

	return(retcode);
}

void COpenGLWnd::OnDrawHeadsUpDisplay()
{
//	TRACE("In COpenGLWnd::OnDrawHeadsUpDisplay\n");	
	
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();

	// set viewport to entire window
	glViewport(0, 0, m_ClientRect.right, m_ClientRect.bottom);
	gluOrtho2D(0.0, (double) m_ClientRect.right, 0.0, (double) m_ClientRect.bottom);
	
	glMatrixMode(GL_MODELVIEW);
   	glLoadIdentity();
	
	glDrawBuffer(GL_BACK);
	glDepthFunc(GL_ALWAYS);

	// draw stuff
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawHeadsUpInfo(10);			// does actual drawing

	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void COpenGLWnd::ModifyCameraLocation()
{
//	TRACE("In COpenGLWnd::ModifyCameraLocation\n");	
	// change camera position after trackball or flyball controls have been applied
	// used by derived to provide scene-specific modifications
}

void COpenGLWnd::DrawHeadsUpInfo(int UpdateInterval)
{
	// default implementation displays frame rate info along top of GL window
	// override to add more interesting stuff...can still call base implementation to provide frame rate info

	// UpdateInterval is the frequency of updates to the heads up information...can be ignored in derived class implementation
	// heads-up info is drawn each time but only updated every UpdateInterval frames

	// window has been set up using screen pixel coord system with origin in lower left corner
	// to use m_ClientRect remember to reverse meaning of bottom and top parameters
	static CString csFPS;
	if (m_FramesDrawn) {
		if (m_FramesDrawn % UpdateInterval == 0)		// update string every 10 frames
			csFPS.Format("FPS: %-6.1lf  %s", m_FrameRate, m_BannerMessage);
//			csFPS.Format("FPS: %-6.1lf  UpdateTime: %.4lfms  RenderTime: %.4lfms  DelayTime: %.4lfms", m_FrameRate, m_TimeToUpdateFrameInfo * 1000.0, m_TimeToDrawFrame * 1000.0, m_TimeBetweenFrames * 1000.0);
	}
	else
		csFPS.Format("Elapsed time: %-6.1lf", m_TotalTime.GetElapsed());

	// get width and height of string
	SIZE sz;
	GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csFPS, csFPS.GetLength(), &sz);

	// draw background
	glColor4d(0.7, 0.7, 0.7, 0.25);
	glBegin(GL_QUADS);
	glVertex2d(0.0, (double) m_ClientRect.bottom);
	glVertex2d((double) m_ClientRect.right, (double) m_ClientRect.bottom);
	glVertex2d((double) m_ClientRect.right, (double) m_ClientRect.bottom - (double) sz.cy - 2.0);
	glVertex2d(0.0, (double) m_ClientRect.bottom - (double) sz.cy - 2.0);
	glEnd();
	
	glColor4d(1.0, 0.0, 0.0, 1.0);		// red
//	glColor4d(1.0, 1.0, 0.0, 1.0);		// yellow
	glRasterPos2d((double) 0.0, (double) m_ClientRect.bottom - (double) sz.cy + 3.0);
// centered	glRasterPos2d((double) m_ClientRect.right / 2.0 - (double) sz.cx / 2.0, (double) m_ClientRect.bottom - (double) sz.cy + 3.0);
	PrintString(csFPS);

	// display stereo mode in upper right corner
	CString csMode;
	if (m_bStereoEnabled)
		csMode = _T("True stereo");
	else if (m_bUseAnaglyph)
		csMode = _T("Anaglyph");
	else if (m_bUseSplitScreen) {
		if (m_Alignment == PARALLEL)
			csMode = _T("Split screen ||");
		else
			csMode = _T("Split screen X");
	}
	else {
		if (m_MotionController == TRACKBALL) 
			csMode = _T("Trackball--Mono");
		else
			csMode = _T("Flyball--Mono");
	}

	GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csMode, csMode.GetLength(), &sz);
	glRasterPos2d((double) m_ClientRect.right - ((double) sz.cx + 3.0), (double) m_ClientRect.bottom - (double) sz.cy + 3.0);
	PrintString(csMode);
}

int COpenGLWnd::DecideOnPixelFormat(HDC hDC, LPPIXELFORMATDESCRIPTOR Currentpfd, BOOL PrintInfo)
{
	int i, maxpf;
	CString csTemp;
	int PixFormat = 0;

    PIXELFORMATDESCRIPTOR pfd;
    PIXELFORMATDESCRIPTOR Desiredpfd;
	memcpy(&Desiredpfd, Currentpfd, sizeof(PIXELFORMATDESCRIPTOR));

	// calling DescribePixelFormat() with NULL args return maximum number of pixel formats
	maxpf = DescribePixelFormat(hDC, 0, 0, NULL);

	// print the table header
	if (PrintInfo) {
		csTemp.Format("-----------------------------------------------------------------------\r\n");
		TRACE0(csTemp);
		csTemp.Format("   visual  a  bf lv rg d st  r  g  b a  ax dp st accum buffs  ms \r\n");
		TRACE0(csTemp);
		csTemp.Format(" id dep cl cc sz l  ci b ro sz sz sz sz bf th cl  r  g  b  a ns b score\r\n");
		TRACE0(csTemp);
		csTemp.Format("-----------------------------------------------------------------------\r\n");
		TRACE0(csTemp);
	}

	// loop through all the pixel formats
	int maxscore = 0;
	int maxFormat = 0;
	int score;
	for(i = 1; i <= maxpf; i++) {
		DescribePixelFormat(hDC, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		// only describe this format if it supports OpenGL
		if ((pfd.dwFlags & PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL) == (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) {
			// make sure we got the correct double-buffer setting
			if (((pfd.dwFlags & PFD_DOUBLEBUFFER) && (Desiredpfd.dwFlags & PFD_DOUBLEBUFFER)) || (!(pfd.dwFlags & PFD_DOUBLEBUFFER) && !(Desiredpfd.dwFlags & PFD_DOUBLEBUFFER))) {
				// make sure we have the minimum color depth and depth-buffer resolution...also have to have RGB(A) not color indexed mode
				if (pfd.cDepthBits >= Desiredpfd.cDepthBits && pfd.cColorBits >= Desiredpfd.cColorBits && pfd.iPixelType == PFD_TYPE_RGBA) {
					// compute score...start with 1 since we met basic criteria that was requested
					score = 1;

					if (pfd.dwFlags & PFD_GENERIC_ACCELERATED || !(pfd.dwFlags & PFD_GENERIC_FORMAT))
						score += 127;

					if (pfd.cDepthBits > 24)
						score += 63;

					if (pfd.cAlphaBits >= 8)
						score += 31;

					if (pfd.dwFlags & PFD_STEREO && Desiredpfd.dwFlags & PFD_STEREO)
						score += 15;

					// compare score to maxscore...testing > yields first instance of high score, >= yields last instance of high score
					if (score > maxscore) {
						maxscore = score;
						maxFormat = i;
					}

					if (PrintInfo) {
						// print out the information for this pixel format
						csTemp.Format(" %3i ", i);
						TRACE0(csTemp);

						csTemp.Format("%2d ", pfd.cColorBits);
						TRACE0(csTemp);

						if (pfd.dwFlags & PFD_DRAW_TO_WINDOW && pfd.dwFlags & PFD_DRAW_TO_BITMAP)
							csTemp.Format("wb ");
						else if (pfd.dwFlags & PFD_DRAW_TO_WINDOW)
							csTemp.Format("wn ");
						else if (pfd.dwFlags & PFD_DRAW_TO_BITMAP)
							csTemp.Format("bm ");
						else
							csTemp.Format(".  ");
						TRACE0(csTemp);

						// hardware acceleration
						if (pfd.dwFlags & PFD_GENERIC_ACCELERATED || !(pfd.dwFlags & PFD_GENERIC_FORMAT))
							csTemp.Format(" y ");
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						csTemp.Format("%2d ", pfd.cColorBits);
						TRACE0(csTemp);

						// bReserved field indicates number of over/underlays
						if (pfd.bReserved)
							csTemp.Format(" %d ", pfd.bReserved);
						else
							csTemp.Format(" . "); 
						TRACE0(csTemp);

						csTemp.Format(" %c ", pfd.iPixelType == PFD_TYPE_RGBA ? 'r' : 'c');
						TRACE0(csTemp);

						csTemp.Format("%c ", pfd.dwFlags & PFD_DOUBLEBUFFER ? 'y' : '.');
						TRACE0(csTemp);

						csTemp.Format(" %c ", pfd.dwFlags & PFD_STEREO ? 'y' : '.');
						TRACE0(csTemp);

						if (pfd.cRedBits)
							csTemp.Format("%2d ", pfd.cRedBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cGreenBits)
							csTemp.Format("%2d ", pfd.cGreenBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cBlueBits)
							csTemp.Format("%2d ", pfd.cBlueBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAlphaBits)
							csTemp.Format("%2d ", pfd.cAlphaBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAuxBuffers)
							csTemp.Format("%2d ", pfd.cAuxBuffers);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cDepthBits)
							csTemp.Format("%2d ", pfd.cDepthBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cStencilBits)
							csTemp.Format("%2d ", pfd.cStencilBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAccumRedBits)
							csTemp.Format("%2d ", pfd.cAccumRedBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAccumGreenBits)
							csTemp.Format("%2d ", pfd.cAccumGreenBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAccumBlueBits)
							csTemp.Format("%2d ", pfd.cAccumBlueBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						if (pfd.cAccumAlphaBits)
							csTemp.Format("%2d ", pfd.cAccumAlphaBits);
						else
							csTemp.Format(" . ");
						TRACE0(csTemp);

						// no multisample in Win32
						csTemp.Format(" . .");
						TRACE0(csTemp);

						// print score
						csTemp.Format("  %4i\r\n", score);
						TRACE0(csTemp);
					}
				}
			}
		}
	}

	if (maxscore > 0)
		PixFormat = maxFormat;

	if (PrintInfo) {
		// print table footer
		csTemp.Format("-----------------------------------------------------------------------\r\n");
		TRACE0(csTemp);
		csTemp.Format("   visual  a  bf lv rg d st  r  g  b a  ax dp st accum buffs  ms  score\r\n");
		TRACE0(csTemp);
		csTemp.Format(" id dep cl cc sz l  ci b ro sz sz sz sz bf th cl  r  g  b  a ns b\r\n");
		TRACE0(csTemp);
		csTemp.Format("-----------------------------------------------------------------------\r\n");
		TRACE0(csTemp);
	}

	return(PixFormat);
}

void COpenGLWnd::SetAnaglyphSensitiveColor(float r, float g, float b, float a)
{
	if (m_bUseAnaglyph) {
		float i;
		i = r * 0.2126f + g * 0.7152f + b * 0.0722f;		// calculate intensity
//		i = 1.0f;		// force white
		glColor4f(i, i, i, a);
	}
	else
		glColor4f(r, g, b, a);
}

void COpenGLWnd::SetAnaglyphSensitiveColor(COLORREF color, float a)
{
	float r = float(GetRValue(color)) / 255;
	float g = float(GetGValue(color)) / 255;
	float b = float(GetBValue(color)) / 255;
	if (m_bUseAnaglyph) {
		SetAnaglyphSensitiveColor(r, g, b, a);
	}
	else
		glColor4f(r, g, b, a);
}

void COpenGLWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_MotionController == TRACKBALL)
		m_TrackBall.MouseDown(point);
	else
		m_TranslationController.MouseDown(point);

	SetCapture();

	m_InMotion = TRUE;
}

void COpenGLWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_MotionController == TRACKBALL)
		m_TrackBall.MouseUp(point);
	else
		m_TranslationController.MouseUp(point);

	ReleaseCapture();
	Invalidate();

	m_InMotion = FALSE;
}

void COpenGLWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_InMotion) {
		if (m_MotionController == TRACKBALL)
			m_TrackBall.MouseMove(point);
		else {
			m_TranslationController.MouseMove(point);
		}

		Invalidate();
	}
}

void COpenGLWnd::ApplyTranslationControl()
{
	double x_comp, y_comp, z_comp, length;
	double angle, fangle;


	// we need to modify the camera location directly so the manipulation needs to be in COpenGLWnd instead of the CTranslationController
	if (m_TranslationController.m_TypeOfMovement == MOTION) {
		// move along the line-of-sight
//		m_Camera.vp.Translate(m_TranslationController.m_MovementX / 10.0, m_TranslationController.m_MovementY / 10.0, 0.0);
//		m_Camera.pr.Translate(m_TranslationController.m_MovementX / 10.0, m_TranslationController.m_MovementY / 10.0, 0.0);

		/* FORWARD (+) AND REVERSE (-) MOVEMENT */

		x_comp = m_Camera.pr.x - m_Camera.vp.x;
		y_comp = m_Camera.pr.y - m_Camera.vp.y;

		length = hypot(x_comp, y_comp);

		m_Camera.vp.x += (m_TranslationController.m_MovementY * MOVEMENT_SENSITIVITY) / length * x_comp;
		m_Camera.vp.y += (m_TranslationController.m_MovementY * MOVEMENT_SENSITIVITY) / length * y_comp; 

		m_Camera.pr.x += (m_TranslationController.m_MovementY * MOVEMENT_SENSITIVITY) / length * x_comp;
		m_Camera.pr.y += (m_TranslationController.m_MovementY * MOVEMENT_SENSITIVITY) / length * y_comp; 

		angle = atan2(m_Camera.pr.y - m_Camera.vp.y, m_Camera.pr.x - m_Camera.vp.x);
		fangle = D2R * (ANGLE_STEP) * m_TranslationController.m_MovementX;
		length = hypot(m_Camera.pr.x - m_Camera.vp.x, m_Camera.pr.y - m_Camera.vp.y);

		m_Camera.pr.x = m_Camera.vp.x + length * cos(angle - fangle);
		m_Camera.pr.y = m_Camera.vp.y + length * sin(angle - fangle);
	}
	else if (m_TranslationController.m_TypeOfMovement == TRANSLATE) {
		// get vector perpendicular to line-of-sight
		CPoint3D r;
		CROSSPROD(m_Camera.vd,m_Camera.vu,r);
		r.Normalize();
		r.x *= m_TranslationController.m_MovementX * MOVEMENT_SENSITIVITY;
		r.y *= m_TranslationController.m_MovementX * MOVEMENT_SENSITIVITY;
		r.z *= m_TranslationController.m_MovementX * MOVEMENT_SENSITIVITY;

		m_Camera.vp.x += r.x;
		m_Camera.vp.y += r.y;
		m_Camera.vp.z += r.z;
		m_Camera.pr.x += r.x;
		m_Camera.pr.y += r.y;
		m_Camera.pr.z += r.z;
	}
	else if (m_TranslationController.m_TypeOfMovement == TILT_HEAD) {
		z_comp = m_Camera.pr.z - m_Camera.vp.z;

		m_Camera.pr.z += (m_TranslationController.m_MovementY * MOVEMENT_SENSITIVITY);
	}

	// recalculate the view direction vector
	m_Camera.vd.x = m_Camera.pr.x - m_Camera.vp.x;
	m_Camera.vd.y = m_Camera.pr.y - m_Camera.vp.y;
	m_Camera.vd.z = m_Camera.pr.z - m_Camera.vp.z;
	m_Camera.vd.Normalize();

//	m_TranslationController.DoGLTranslation();
}

void COpenGLWnd::CheckGLError(const char *file, int line)
{
	static CString error;
	GLenum err (glGetError());

	while (err != GL_NO_ERROR) {

		switch(err) {
			case GL_INVALID_OPERATION:
				error="GL_INVALID_OPERATION";
				break;
			case GL_INVALID_ENUM:
				error="GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error="GL_INVALID_VALUE";
				break;
			case GL_OUT_OF_MEMORY:
				error="GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_OVERFLOW:
				error="GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				error="GL_STACK_UNDERFLOW";
				break;
		}

		error.Format("%s--%s: %i\n", error, file, line);
		TRACE(error);	

		err = glGetError();
	}

}
