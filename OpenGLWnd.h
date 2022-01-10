#if !defined(AFX_OPENGLWND_H__455CD4BE_C24C_4CF0_8289_73173B606EE5__INCLUDED_)
#define AFX_OPENGLWND_H__455CD4BE_C24C_4CF0_8289_73173B606EE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLWnd.h : header file
//
// MS openGL libraries (link with OPENGL32.LIB and GLU32.LIB)
#include "..\scatter\LDV\writeavi.h"
#include "gl\gl.h"
#include "gl\glu.h"
#include "resource.h"		// needed for CImage class to get progress dialog ID
#include "image.h"	// Added by ClassView
#include "BallController.h"	// Added by ClassView
#include "FULLWINDOW\TranslationController.h"	// Added by ClassView

#define		LEFT		0
#define		RIGHT		1
#define		MIDDLE		2

#define		PARALLEL	0
#define		XEYED		1

#define		PERSPECTIVE		0
#define		ORTHOGRAPHIC	1

#define		TRACKBALL	0
#define		FLYBALL		1

#define	PI_		3.14159265358979323846
#define DTOR            0.0174532925
#define RTOD            57.2957795
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

//#define GLdouble double
/////////////////////////////////////////////////////////////////////////////
// Global type definitions
	enum InfoField {VENDOR,RENDERER,VERSION,ACCELERATION,EXTENSIONS};
	enum ColorsNumber{INDEXED,THOUSANDS,MILLIONS,MILLIONS_WITH_TRANSPARENCY};
	enum ZAccuracy{NORMAL,ACCURATE};

/////////////////////////////////////////////////////////////////////////////
// general 3D point classes
class CPoint3D
{
public:
	double x, y, z;
	CPoint3D () { x=y=z=0.0; }
	CPoint3D (float c1, float c2, float c3) {
		x = (double) c1;
		y = (double) c2;
		z = (double) c3;
	}
	CPoint3D (double c1, double c2, double c3) {
		x = c1;
		y = c2;
		z = c3;
	}
	CPoint3D& operator=(const CPoint3D& pt) {
		x = pt.x;
		z = pt.z;
		y = pt.y;
	return *this;
	}
	CPoint3D (const CPoint3D& pt) {
		*this = pt;
	}
	void Translate(double cx, double cy, double cz) {
		x += cx;
		y += cy;
		z += cz;
	}
	void Normalize() {
		double length;

		length = sqrt(x * x + y * y + z * z);
		if (length != 0) {
			x /= length;
			y /= length;
			z /= length;
		} 
		else {
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// performance timer class...gives access to very high resolution timer
class CPerformanceTimer
{
public:
	CPerformanceTimer( CString msg ) {
		_msg = msg;
		m_Running = TRUE;
		QueryPerformanceCounter( (LARGE_INTEGER*)&_starttime );
		QueryPerformanceFrequency( (LARGE_INTEGER*)&_frequency );
		_LoResStartTime = GetTickCount();
	}
	CPerformanceTimer() {
		_msg = _T("");
		m_Running = TRUE;
		QueryPerformanceCounter( (LARGE_INTEGER*)&_starttime );
		QueryPerformanceFrequency( (LARGE_INTEGER*)&_frequency );
		_LoResStartTime = GetTickCount();
	}
	~CPerformanceTimer() {
		if (m_Running) {
			StopAndReport();
		}
	}
	void Start() {
		m_Running = TRUE;
		QueryPerformanceCounter( (LARGE_INTEGER*)&_starttime );
	}
	double Stop() {
		if (m_Running) {
			QueryPerformanceCounter( (LARGE_INTEGER*)&_stoptime );
			_elapsedtime = _stoptime - _starttime;
			m_Running = FALSE;
			return(((double)(unsigned long)_elapsedtime)/((double)(unsigned long)_frequency));
		}
		return(1000000.0);
	}
	double GetElapsed() {
		if (m_Running) {
			QueryPerformanceCounter( (LARGE_INTEGER*)&_stoptime );
			_intermediate_elapsedtime = _stoptime - _starttime;
			return(((double)(unsigned long)_intermediate_elapsedtime)/((double)(unsigned long)_frequency));
		}
		return(1000000.0);
//		// return time since start (creation of object) in seconds...not precise
//		if (m_Running) {
//			DWORD elapsedtime = GetTickCount() - _LoResStartTime;
//			return((double) elapsedtime / 1000.0);
//		}
//		return(1000000.0);
	}
	void StopAndReport() {
		if (m_Running) {
			double  sec;
			QueryPerformanceCounter( (LARGE_INTEGER*)&_stoptime );
			_elapsedtime = _stoptime - _starttime;
			sec   = ((double)(long)_elapsedtime)/((double)(long)_frequency);
//			TRACE( _T("Elapsed time %s: %I64d (%f s)\n"), _msg, _elapsedtime, sec );
			m_Running = FALSE;
		}
	}

protected:
	__int64 _elapsedtime;
	__int64 _intermediate_elapsedtime;
	__int64 _starttime;
	__int64 _stoptime;
	__int64 _frequency;
	DWORD _LoResStartTime;
	CString _msg;
	BOOL m_Running;
};

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd window

class COpenGLWnd : public CWnd
{
	// Construction
public:
	// structure to maintain camera variables
	typedef struct {
	   CPoint3D vp;									// View position
	   CPoint3D vd;									// View direction vector
	   CPoint3D vu;									// View up direction
	   CPoint3D pr;									// Point to rotate about
	   double focallength;							// Focal Length along vd
	   double aperture;								// Camera aperture
	   double eyesep;								// Eye separation
	   double nearplane;							// distance to near clipping plane
	   double farplane;								// distance to far clipping plane
	   int screenwidth,screenheight;				// display screen size
	   double filmwidth, filmheight;				// "film" size
	} CAMERA;

	// Implementation
public:
	BOOL m_InRedraw;								// "semaphore" to prevent attempts to use GL context from multiple code segments
	BOOL m_OnCreateGLComplete;						// flag indicating OnCreateGL() has been called
	BOOL m_InMotion;								// flag indicating model is in motion (usually due to mouse interaction)
	BOOL m_bAntiAlias;								// flag indicaing anit-aliasing is enabled
	BOOL m_bUseAnaglyph;							// flag indicating anaglyph mode is enabled
	BOOL m_bStereoEnabled;							// flag indicating quad-buffered stereo is enabled
	BOOL m_bDoubleBufferEnabled;					// flag indicating double buffering is enabled
	BOOL m_bUseSplitScreen;							// flag indicating split-screen stereo is enabled
	BOOL m_DoContinuousDrawing;						// flag controlling ability to "throw" the 3D model
	COLORREF m_BackgroundColor;						// background color for GL window...clear color
	unsigned int m_FramesDrawn;						// count of the total number of frames drawn
	CPerformanceTimer m_TotalTime;					// timer for total time program has been running
	CPerformanceTimer m_FrameTimer;					// timer for frame related stuff...render, update, and between frame delay
	double m_FrameRate;								// frame rate for LAST rendered frame
	double m_TimeBetweenFrames;						// elapsed time since end of last frame
	double m_TimeToDrawFrame;						// actual time for drawing
	double m_TimeToUpdateFrameInfo;					// time to update model state information
	int m_CurrentEyePosition;						// which eys is being used to draw (LEFT, MIDDLE, RIGHT)
	int  m_Alignment;								// alignment used in split-screen stereo mode (PARALLEL, XEYED)
	int m_ProjectionType;							// projection method (PERSPECTIVE or ORTHOGRAPHIC)
	CRect m_ClientRect;								// client area size
	CAMERA m_Camera;								// camera
	CBallController m_TrackBall;					// trackball control
	CTranslationController m_TranslationController;	// translation controller
	int m_MotionController;							// which controller is active

public:
	COpenGLWnd();
	virtual ~COpenGLWnd();
	void SetClearCol(COLORREF rgb);
	void SetAnaglyphSensitiveColor(float r, float g, float b, float a = 1.0f);
	void SetAnaglyphSensitiveColor(COLORREF color, float a = 1.0f);
	void SetMouseCursor(HCURSOR mcursor=NULL);			// Mouse cursor function

	const CString GetInformation(InfoField type);		// Information retrieval function
	double GetAspectRatio() {return m_dAspectRatio;};	// Attribute retrieval function
	BOOL IsStereo();
	BOOL IsDoubleBuffered();

	void CopyToClipboard();
	BOOL RenderToMemoryAndSaveImage(LPCTSTR filename, int Eye = MIDDLE);
	BOOL SaveDepthImage(LPCTSTR filename, double Minx, double Miny, double Minz, double Maxx, double Maxy, double Maxz, BOOL SaveInfo = TRUE);
	BOOL Old_SaveScreenImage(LPCTSTR filename);
	BOOL SaveScreenImage(LPCTSTR filename, CAVIFile* AVIFile);

	void DrawBox(CPoint3D org, double x, double y, double z);
	
	void BeginGLCommands();								// use to issue GL commands outside Overridables
	void EndGLCommands();								// i.e: in menu event handlers, button events handler etc.

	void MakeFont();									// create font for use in GL window
	void PrintString(const char* str);					// print string in GL window

	// CGLDispList
	// DESC:-this is an helper class which let you create "display list objects",
	//	   use these objects to define the key elements in your scene (a disp.
	//	   list is faster than the corresponding GL commands).
	//	  -Through the class members functions you have total control on a
	//	   single display list.
	//	  -An isolated display list save OGL parameters before execution
	//	   (so it's not affected by preceding transformations or settings).
	class CGLDispList {
		friend class COpenGLWnd;
		private:
			BOOL m_bIsolated;
			int m_glListId;
		public:
			void DeleteList();
			BOOL IsValid();
			CGLDispList();  // constructor
			~CGLDispList(); // destructor
			void StartDef(BOOL bImmediateExec=FALSE);// enclose a disp.list def.
			void EndDef();
			void Draw();// execute disp list GL commands 
			void SetIsolation(BOOL bValue) {m_bIsolated=bValue;}; // set isolation property
	};

	// Operations
public:
	void CheckGLError(const char *file = __FILE__, int line = __LINE__);
	void ApplyTranslationControl();
	virtual void OnDrawGLBeforeTrackball();
	void NewCopyToClipboard();
	CString m_BannerMessage;
	// Stock Display lists functions
	//	DESC.: these display lists are internally organized in a vector (20 max),
	//	   you have control on definition and redrawing only. 
	//	   use them for background elements which are to be drawn everytime
	//	   all together.
	// NOTE: between BeginStockDispList and EndStockDispList should be present 
	// OpenGL calls only (see documentation for which are allowed and how are them treated)
	void StartStockDListDef();	// allocates a new stock display list entry and opens a display list definition
	void EndStockListDef();		// closes a stock display list definition
	void DrawStockDispLists();	// executes all the stock display lists
	void ClearStockDispLists(); // deletes all the stock display lists

	// Overridables
	virtual void OnCreateGL(); // override to set bg color, activate z-buffer, and other global settings
	virtual void OnDestroyGL(); // override to clean up
	virtual void OnSizeGL(int cx, int cy); // override to adapt the viewport to the window
	virtual void OnDrawGL(); // override to issue drawing functions
	virtual void OnUpdateGLState(); // override to update derived class variables prior to redrawing
	virtual void OnDrawGLMask();	// override to draw mask that shows up in both images
	virtual void OnDrawGDI(CPaintDC *pDC); // override to issue GDI drawing functions
	virtual void VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf, BOOL &stereo, BOOL &anaglyph, BOOL &split_screen, int &alignment, int &projection_type); // override to specify some video mode parameters
	virtual void ResetCamera();
	virtual void DrawHeadsUpInfo(int UpdateInterval = 10);
	virtual void ModifyCameraLocation();	// change camera position after trackball or flyball controls have been applied
	// Overrides
	// NOTE: these have been declared private because they shouldn't be
	//		 overridden, use the provided virtual functions instead.
private:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenGLWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void OnDrawHeadsUpDisplay();

	// Generated message map functions
	// NOTE: these have been declared private because they shouldn't be
	//		 overridden, use the provided virtual functions instead.
private:
	//{{AFX_MSG(COpenGLWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// member variables
protected:
	CDC* m_pCDC;							// WinGDI Device Context

private:
	CImage m_DIBSectionForRendering;		// DIB section used for rendering images to save to disk
	double m_EyeSeparationMultiplier;		// controls separation of stereo images
	int m_OldWidth;							// old width of GL context...needed for memory rendering
	int m_OldHeight;						// old height of GL context...needed for memory rendering
	int m_OldSplitScreenSetting;			// old split screen setting...needed for memory rendering
	HDC			m_hOldDC;					// old device context associated with screen GL context...needed for memory rendering
	HGLRC		m_hOldRC;					// old GL context used for screen rendering...needed for memory rendering
	CDC m_MemoryDCForRendering;				// WinGDI Device Context
	HGLRC m_hMemoryRCForRendering;			// OpenGL Rendering Context
	HGLRC m_hRC;							// OpenGL Rendering Context
	HCURSOR m_hMouseCursor;					// mouse cursor handle for the view
	CPalette m_CurrentPalette;				// palettes
	CPalette* m_pOldPalette;				// palettes
	CFont m_Font;							// font for window
	BOOL m_bGotFont;						// flag indicating font set is ready for use
	double m_dAspectRatio;					// aspect ration of display window w/h
	int m_DispListVector[20];				// Internal stock display list vector
	BOOL m_bInsideDispList;					// Disp List definition semaphore
	BOOL m_bExternGLCall;
	BOOL m_bExternDispListCall;
	GLuint m_FontListBase;					// index of first display list in font set

	// initialization helper functions
	BOOL bSetupPixelFormat();
	int DecideOnPixelFormat(HDC hDC, LPPIXELFORMATDESCRIPTOR Currentpfd, BOOL PrintInfo = FALSE);
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	void CreateRGBPalette();
	void AADraw(int eye = MIDDLE);
	void Frustum(double left, double right, double bottom, double top, double oglnear, double oglfar, double pixdx, double pixdy, double eyedx = 0.0, double eyedy = 0.0, double focus = 1.0);
	void Perspective(double fovy, double aspect, double oglnear, double oglfar, double pixdx, double pixdy, double eyedx = 0.0, double eyedy = 0.0, double focus = 0.0);
	void MoveToEyePosition(int Eye = MIDDLE, int OffsetIndex = 0);
	BOOL SaveMemoryRendering(LPCTSTR filename);
	BOOL PrepareForMemoryRendering(int Width = 800, int Height = 600);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLWND_H__455CD4BE_C24C_4CF0_8289_73173B606EE5__INCLUDED_)
