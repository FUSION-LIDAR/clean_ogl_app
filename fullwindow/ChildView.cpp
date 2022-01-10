// ChildView.cpp : implementation of the CChildView class
//
//
#include "stdafx.h"
#include "fullwindow.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "RangepoleOptionDlg.h"
#include "lidardata_LASLIB.h"
#include "..\..\fusion\ASCIIGridFile.h"
#include "ColorRamp.h"

#include "filespec.h"
#include "dtm3d.h"
#include "float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		TARGETFRAMERATE		30.0

#define		UNKNOWNFORMAT		0
#define		ASCII_XYZRGB		1
#define		LIDARDATA			2
#define		BINARY_LDVD			3
#define		ASCII_IDXYZ			4
#define		ASCII_SBET			5

#define	PI	3.14159265358979
#define	D2R (PI / 180.0)

#define MAXSKIP 32

#define SETGLCOLOR(a) glColor3ub(GetRValue((a)), GetGValue((a)), GetBValue((a)))

/*
JOYINFO  joyInfo;
bool testWithMouse = true;
int StartX, StartY; // Declare at the top of the form’s unit
CString strFormatting;
bool ButtonDown = false;
static bool capture = false;
*/

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	m_BackgroundColor = RGB(0, 0, 0);

	CTime ct = CTime::GetCurrentTime();
	m_StartTime = ct.GetSecond() + ct.GetMinute() * 60 + ct.GetHour() * 60 * 60;

	m_PointSize = 2.0;
	m_PointCount = 0;
	m_DrawnPointCount = 0;

	m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit", PROGRAM_VERSION);

	m_DoContinuousDrawing = FALSE;

	m_ShowHeadsUpInfo = TRUE;

	m_RespondingToSync = FALSE;

	m_InZoom = FALSE;

	m_SurfaceIsWireframe = FALSE;

	m_PointList = NULL;

	m_DataType = 0;

	m_DrawAxes = TRUE;
	m_DrawPointData = TRUE;

	m_DrawRangePoles = FALSE;

	m_RangepoleTotalHeight = 100.0;
	m_RangepoleSegmentHeight = 5.0;
	m_ScaleRangepoleToData = TRUE;

	m_DrawPoleCenter = TRUE;
	m_DrawPoleLL = TRUE;
	m_DrawPoleUL = TRUE;
	m_DrawPoleUR = TRUE;
	m_DrawPoleLR = TRUE;
	m_DrawPoleML = TRUE;
	m_DrawPoleMT = TRUE;
	m_DrawPoleMR = TRUE;
	m_DrawPoleMB = TRUE;

	m_PointFileFormat = UNKNOWNFORMAT;
	m_PointFileName.Empty();

	m_SurfaceFileName.Empty();

	m_SBETSpeedStep = 25;
	m_SBETViewer = FALSE;
	m_DisplayLowResSurface = TRUE;

	m_FlyballOffset = 10.0;

	m_RecordFramesToAVI = FALSE;
	m_AVIFileName.Empty();

	m_ColorMode = COLORBYHEIGHT;
													// LAS V1.0-1.3						// LAS V1.4 prf 6+ (only differences from earlier formats)
	m_LASClassColors[0] = RGB(128, 128, 128);		// created, never classified
	m_LASClassColors[1] = RGB(192, 192, 192);		// unclassified
	m_LASClassColors[2] = RGB(128, 64, 0);			// ground
	m_LASClassColors[3] = RGB(0, 128, 0);			// low vegetation
	m_LASClassColors[4] = RGB(0, 192, 0);			// medium vegetation
	m_LASClassColors[5] = RGB(0, 255, 0);			// high vegetation
	m_LASClassColors[6] = RGB(0, 128, 192);			// building
	m_LASClassColors[7] = RGB(192, 0, 0);			// low point (noise)																dull red
	m_LASClassColors[8] = RGB(255, 255, 0);			// model key-point (mass point)		reserved
	m_LASClassColors[9] = RGB(0, 0, 255);			// water
	m_LASClassColors[10] = RGB(245, 130, 48);		// reserved							rail											orange
	m_LASClassColors[11] = RGB(210, 245, 60);		// reserved							road surface									lime
	m_LASClassColors[12] = RGB(255, 0, 255);		// overlap points					reserved
	m_LASClassColors[13] = RGB(250, 190, 190);		// reserved							wire-guard (shield)								pink
	m_LASClassColors[14] = RGB(228, 192, 228);		// reserved							wire-conductor (phase)							magenta
	m_LASClassColors[15] = RGB(170, 255, 195);		// reserved							transmission tower								mint
	m_LASClassColors[16] = RGB(230, 190, 255);		// reserved							wire-structure connector (e.g. insulator)		lavender
	m_LASClassColors[17] = RGB(145, 30, 180);		// reserved							bridge deck										purple
	m_LASClassColors[18] = RGB(255, 0, 0);			// reserved							high noise										bright red
	m_LASClassColors[19] = RGB(0, 128, 128);		// reserved							reserved										teal
	m_LASClassColors[20] = RGB(0, 0, 128);			// reserved							reserved										navy blue
	m_LASClassColors[21] = RGB(128, 128, 0);		// reserved							reserved										olive
	m_LASClassColors[22] = RGB(255, 215, 180);		// reserved							reserved										coral
	m_LASClassColors[23] = RGB(170, 110, 40);		// reserved							reserved										light brown
	m_LASClassColors[24] = RGB(255, 250, 200);		// reserved							reserved										beige
	m_LASClassColors[25] = RGB(60, 180, 75);		// reserved							reserved										green
	m_LASClassColors[26] = RGB(0, 192, 192);		// reserved							reserved										blue
	m_LASClassColors[27] = RGB(70, 240, 240);		// reserved							reserved										cyan
	m_LASClassColors[28] = RGB(255, 225, 25);		// reserved							reserved										yellow
	m_LASClassColors[29] = RGB(99, 255, 172);		// reserved							reserved										dark mint
	m_LASClassColors[30] = RGB(64, 64, 64);			// reserved							reserved										dark gray
	m_LASClassColors[31] = RGB(0, 255, 255);		// reserved							reserved (19-63 reserved, 64-255 user-defined)	bright cyan

	m_LASReturnColors[0] = RGB(255, 0, 0);			// red return 1
	m_LASReturnColors[1] = RGB(255, 255, 0);		// yellow return 2
	m_LASReturnColors[2] = RGB(0, 255, 0);			// green return 3
	m_LASReturnColors[3] = RGB(0, 255, 255);		// cyan return 4
	m_LASReturnColors[4] = RGB(0, 0, 255);			// blue return 5
	m_LASReturnColors[5] = RGB(255, 0, 255);		// magenta return 6
	m_LASReturnColors[6] = RGB(210, 245, 60);		// lime return 7
	m_LASReturnColors[7] = RGB(250, 190, 190);		// pink return 8
	m_LASReturnColors[8] = RGB(0, 128, 128);		// teal return 9
	m_LASReturnColors[9] = RGB(245, 130, 48);		// orange return 10
	m_LASReturnColors[10] = RGB(230, 190, 255);		// lavender return 11
	m_LASReturnColors[11] = RGB(170, 110, 40);		// brown return 12
	m_LASReturnColors[12] = RGB(170, 255, 195);		// mint return 13
	m_LASReturnColors[13] = RGB(0, 0, 128);			// navy blue return 14
	m_LASReturnColors[14] = RGB(128, 0, 0);			// maroon return 15

	// DTM scanning stuff
	m_ScanPositionIndex = 0;
	m_QAModelScan = FALSE;
	m_ScanOffsetX = 0.0;
	m_ScanOffsetY = 0.0;
	m_ScanOffsetZ = 0.0;

	m_LoadingData = FALSE;

	m_HaveTerrainForFlyball = FALSE;

	m_ClassificationCodeDlg = NULL;

	for (int i = 0; i < 256; i++) {
		m_LASClassState[i] = TRUE;
		m_DataHasClass[i] = FALSE;
	}

	m_DataHasSynthetic = FALSE;
	m_DataHasOverlap = FALSE;
	m_DataHasKeypoint = FALSE;
	m_DataHasWithheld = FALSE;

	m_LASSynthetic = FALSE;
	m_LASOverlap = FALSE;
	m_LASKeypoint = FALSE;
	m_LASWithheld = FALSE;
	m_ShowOnlyPointsWithFlags = FALSE;
}

CChildView::~CChildView()
{
	DeleteDataObject();

//	// release joystick
//	joyReleaseCapture(JOYSTICKID1);
//	capture = false;
}


BEGIN_MESSAGE_MAP(CChildView, COpenGLWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_VIEW_AXES, OnViewAxes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AXES, OnUpdateViewAxes)
	ON_COMMAND(ID_VIEW_POINTS, OnViewPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POINTS, OnUpdateViewPoints)
	ON_COMMAND(ID_FILE_RANGEPOLEOPTIONS, OnFileRangepoleoptions)
	ON_COMMAND(ID_VIEW_RANGEPOLES, OnViewRangepoles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RANGEPOLES, OnUpdateViewRangepoles)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
//	ON_MESSAGE(MM_JOY1BUTTONDOWN, OnJoy1ButtonDown)
//	ON_MESSAGE(MM_JOY1BUTTONUP, OnJoy1ButtonUp)
//	ON_MESSAGE(MM_JOY1MOVE, OnJoy1Move)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers


void CChildView::VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf, BOOL &stereo, BOOL &anaglyph, BOOL &splitscreen, int &alignment, int &projection_type)
{
	// set default videomode
	c = MILLIONS;
	z = ACCURATE;
	dbuf = TRUE;

	// support stereo display if available
	stereo = TRUE;
	anaglyph = FALSE;
	splitscreen = FALSE;
	alignment = XEYED;

	projection_type = PERSPECTIVE;
}

void CChildView::OnUpdateGLState()
{
	m_CurrentTime = m_TotalTime.GetElapsed() + m_StartTime;
	return;
}

void CChildView::ModifyCameraLocation()
{
	TRACE("In CChildView::ModifyCameraLocation\n");	

	double elev, diff, X, Y;
	// change camera position after trackball or flyball controls have been applied

	// see if wea are viewing a surface model using the FLYBALL interface and we have a surface for interpolation
	if ((m_DataType == SURFACE || m_DataType == POINTDATA) && m_MotionController == FLYBALL && m_HaveTerrainForFlyball) {
		// reverse scale to get true camera location
		X = m_ScaledMinX + (m_ScaledMaxX - m_ScaledMinX) * (m_Camera.vp.x + 0.5);
		Y = m_ScaledMinY + (m_ScaledMaxY - m_ScaledMinY) * (m_Camera.vp.y + 0.5);

		// get elevation for camera location
		elev = m_TerrainForFlyball.InterpolateElev(X, Y);
		if (elev >= 0.0) {
			// get difference between head and focus elevations
			diff = m_Camera.pr.z - m_Camera.vp.z;
//			diff = 0.0;		// temp

			// scale elevation to data coordinates and adjust viewpoint elevation
			m_Camera.vp.z = ((elev + m_FlyballOffset) - m_ScaledMinZ) / (m_ScaledMaxZ - m_ScaledMinZ) - 0.5;

			X = m_ScaledMinX + (m_ScaledMaxX - m_ScaledMinX) * (m_Camera.pr.x + 0.5);
			Y = m_ScaledMinY + (m_ScaledMaxY - m_ScaledMinY) * (m_Camera.pr.y + 0.5);

			// get elevation for camera location
			elev = m_TerrainForFlyball.InterpolateElev(X, Y);
			if (elev >= 0.0) {
				// adjust focus point
				m_Camera.pr.z = ((elev + m_FlyballOffset / 2.0) - m_ScaledMinZ) / (m_ScaledMaxZ - m_ScaledMinZ) - 0.5;
			}
			else {
				m_Camera.pr.z = m_Camera.vp.z + diff;
			}

			// recalculate the view direction vector
			m_Camera.vd.x = m_Camera.pr.x - m_Camera.vp.x;
			m_Camera.vd.y = m_Camera.pr.y - m_Camera.vp.y;
			m_Camera.vd.z = m_Camera.pr.z - m_Camera.vp.z;
			m_Camera.vd.Normalize();
		}
		else {
			// set head and focus points so we can see pretty much the entire model
			m_Camera.vp.z = ((m_TerrainForFlyball.MaxElev()) - m_ScaledMinZ) / (m_ScaledMaxZ - m_ScaledMinZ) - 0.5;

			// adjust focus point
			m_Camera.pr.z = ((m_TerrainForFlyball.MinElev() + (m_TerrainForFlyball.MaxElev() - m_TerrainForFlyball.MinElev()) / 2.0) - m_ScaledMinZ) / (m_ScaledMaxZ - m_ScaledMinZ) - 0.5;
		}
	}
}

void CChildView::OnDrawGL()
{
//	COpenGLWnd::OnDrawGL();
//	return;

/*	if (m_LoadingData) {
		m_Object.Draw();

		// draw clock elements
		// hour hand
		glPushMatrix();
			glRotated((m_CurrentTime / 3600.0) / 12.0 * 360.0, 0.0, 0.0, 1.0);
			m_HourHand.Draw();
		glPopMatrix();

		// minute hand
		glPushMatrix();
			glRotated((m_CurrentTime / 60.0) / 60.0 * 360.0, 0.0, 0.0, 1.0);
			m_MinuteHand.Draw();
		glPopMatrix();

		// second hand
		glPushMatrix();
			glRotated(((double) ((int) fmod(m_CurrentTime, 60.0)) / 60.0) * 360.0, 0.0, 0.0, 1.0);
			m_SecondHand.Draw();
		glPopMatrix();

		return;
	}
*/
	// send message to other instances to synchronize trackballs
	if (((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize() && (m_DoContinuousDrawing || m_InMotion || m_InZoom) && !m_RespondingToSync) {
		SendSync();
	}
	m_RespondingToSync = FALSE;

	// main drawing function...called anytime window needs painting
	DrawDataObject();

	if (m_DrawAxes) {
		glPushMatrix();
		// set line color for axes depending on background color
		float intensity = (float) GetRValue(m_BackgroundColor) * 0.2126f + (float) GetGValue(m_BackgroundColor) * 0.7152f + (float) GetBValue(m_BackgroundColor) * 0.0722f;		// calculate intensity
		if (intensity > 0.5)
			SetAnaglyphSensitiveColor(RGB(0, 0, 0));
		else
			SetAnaglyphSensitiveColor(RGB(255, 255, 255));

		glDisable(GL_LIGHTING);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2.0);
		glTranslated(-0.5, -0.5, -0.5);
		DrawBox(CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0, 1.0);

//		// draw origin indicator
//		SetAnaglyphSensitiveColor(RGB(255, 0, 0));
//		glTranslated(-0.005, -0.0005, -0.0005);
//		DrawBox(CPoint3D(0.0, 0.0, 0.0), 0.001, 0.001, 0.001);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	// draw rangepoles
	if ((m_DataType & SURFACE || m_DataType & POINTDATA) && m_DrawRangePoles) {
		glDisable(GL_CULL_FACE);
		if (m_DrawPoleCenter) {
			glPushMatrix();
			glTranslated(0.0, 0.0, m_PoleBaseZ);		// center of data space
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleLL) {
			glPushMatrix();
			glTranslated(-0.5, -0.5, m_PoleBaseZ);		// LL XY
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleUL) {
			glPushMatrix();
			glTranslated(-0.5, 0.5, m_PoleBaseZ);		// UL XY
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleUR) {
			glPushMatrix();
			glTranslated(0.5, 0.5, m_PoleBaseZ);		// UR XY
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleLR) {
			glPushMatrix();
			glTranslated(0.5, -0.5, m_PoleBaseZ);		// LR XY
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleMB) {
			glPushMatrix();
			glTranslated(0.0, -0.5, m_PoleBaseZ);		// middle of bottom edge
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleMT) {
			glPushMatrix();
			glTranslated(0.0, 0.5, m_PoleBaseZ);		// middle of top edge
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleML) {
			glPushMatrix();
			glTranslated(-0.5, 0.0, m_PoleBaseZ);		// middle of left edge
			DrawRangePole();
			glPopMatrix();
		}

		if (m_DrawPoleMR) {
			glPushMatrix();
			glTranslated(0.5, 0.0, m_PoleBaseZ);		// middle of right edge
			DrawRangePole();
			glPopMatrix();
		}

		glEnable(GL_CULL_FACE);
	}

	// see if we are saving AVI file
	if (m_RecordFramesToAVI) {
		OnDrawHeadsUpDisplay();

		SaveScreenImage(NULL, &m_AVIFile);
	}

	m_InZoom = FALSE;

//	if (!m_PointLayer[0].IsValid() && !m_SurfaceLayer.IsValid())
//		m_Object.Draw();
	
/*	// draw clock elements
	// hour hand
	glPushMatrix();
		glRotated((m_CurrentTime / 3600.0) / 12.0 * 360.0, 0.0, 0.0, 1.0);
		m_HourHand.Draw();
	glPopMatrix();

	// minute hand
	glPushMatrix();
		glRotated((m_CurrentTime / 60.0) / 60.0 * 360.0, 0.0, 0.0, 1.0);
		m_MinuteHand.Draw();
	glPopMatrix();

	// second hand
	glPushMatrix();
		glRotated(((double) ((int) fmod(m_CurrentTime, 60.0)) / 60.0) * 360.0, 0.0, 0.0, 1.0);
		m_SecondHand.Draw();
	glPopMatrix();
*/


/*	for (int i = 0; i < 90; i += 3) {
		glPushMatrix();
		glRotated((double) i, 0.0, 0.0, 1.0);
		m_Object.Draw();
		glPopMatrix();
	}
*/
	// draw box 1 by 1 by 1
/*	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(3.0);
	SetAnaglyphSensitiveColor(1.0, 1.0, 1.0);
	DrawBox(CPoint3D(-0.5, -0.5, -0.5), 1.0, 1.0, 1.0);
	glEnable(GL_LIGHTING);
*/
}

void CChildView::DrawRangePole()
{
//	glPushMatrix();
//	m_RangePole.Draw();
//	glPopMatrix();

	glPushMatrix();
	SetAnaglyphSensitiveColor(RGB(255, 0, 0));
	m_RangePoleRedStripes.Draw();
	glPopMatrix();

	glPushMatrix();
	SetAnaglyphSensitiveColor(RGB(223, 223, 223));
	m_RangePoleWhiteStripes.Draw();
	glPopMatrix();
}

void CChildView::OnCreateGL()
{
	COpenGLWnd::OnCreateGL();

	if (AfxGetApp()->m_lpCmdLine[0] != '\0') {
		CString cmd = _T(AfxGetApp()->m_lpCmdLine);
		// remove quotation marks
		cmd.Remove('\"');

		// remove multi-copy switch...not very elegant but since we only have a few command line options it works
		cmd.Replace(" /m", "");
		cmd.Replace(" /M", "");
		cmd.Replace("/m", "");
		cmd.Replace("/M", "");

		// remove synchronize switch...not very elegant but since we only have a few command line options it works
		cmd.Replace(" /s", "");
		cmd.Replace(" /S", "");
		cmd.Replace("/s", "");
		cmd.Replace("/S", "");

		// remove synchronize switch...not very elegant but since we only have a few command line options it works
		cmd.Replace(" /l", "");
		cmd.Replace(" /L", "");
		cmd.Replace("/l", "");
		cmd.Replace("/L", "");

		CreateDataObject(cmd);
	}
//	else
//		CreateDataObject();

//	m_MotionController = TRANSLATOR;
//	ResetCamera();

	// all of these work...
//	CreateJack();
//	CreateClockFace();
//	Create3DFigure();

	// set up second light below data cube
	GLfloat normal_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat bottomlight_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat normal_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
	GLfloat toplight_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, normal_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, toplight_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, bottomlight_diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, normal_ambient);

//	GLfloat position[] = {0.5f, 0.5f, 4.0f, 0.0f};
	GLfloat position[] = {0.5f, 0.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	position[2] = -4.0f;
	glLightfv(GL_LIGHT1, GL_POSITION, position);

	// material properties
//	GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat shininess[] = {50.0f};
	GLfloat mat_nospecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat noshininess[] = {0.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, normal_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, normal_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_nospecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, noshininess);

//	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	if (((CFullwindowApp*) AfxGetApp())->oInfo.m_RunMultipleCopy == TRUE) {
		// disable menu option the clone a new instance
		EnableMenuItem(AfxGetMainWnd()->GetMenu()->GetSafeHmenu(), ID_CLONEPDQ, MF_GRAYED);
	}

	// change application name to reflect 32- or 64-bit
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
#ifdef _M_X64
	pFrame->SetAppName("PDQ64");
#else
	pFrame->SetAppName("PDQ");
#endif
	pFrame->OnUpdateFrameTitle(TRUE);

/*
	// try joystick
	int error;
    if (!capture) {
		// Start the capture
		error = joySetCapture(GetSafeHwnd(), JOYSTICKID1, NULL, FALSE);
        switch (error) {
			case MMSYSERR_NODRIVER :
				MessageBox("The joystick driver is not present", "Error when capturing", MB_ICONERROR);
				break;
			case MMSYSERR_INVALPARAM :
				MessageBox("Windows 95/98/Me: Invalid joystick ID or hwnd is NULL.", "Error when capturing", MB_ICONERROR);
				break;
			case JOYERR_NOCANDO :
				MessageBox("Cannot capture joystick input because a required service (such as a Windows timer) is unavailable.", "Error when capturing", MB_ICONERROR);
				break;
			case JOYERR_UNPLUGGED :
				MessageBox("The specified joystick is not connected to the system. ", "Error when capturing", MB_ICONERROR);
				break;
			default :
				MessageBox("The joystick has been captured");
				capture = true;
		}
	}
*/
}

void CChildView::OnSizeGL(int cx, int cy)
{
	COpenGLWnd::OnSizeGL(cx, cy);

	if (((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize()) {
		SendSync();
	}
}

void CChildView::CreateRangePole(double PoleHeight, double HeightStep, double PoleDiameter)
{
	int i;
	int slices = (int) (PoleHeight / HeightStep);

	GLUquadricObj* quad = gluNewQuadric();

//	m_RangePole.DeleteList();
	m_RangePoleRedStripes.DeleteList();
	m_RangePoleWhiteStripes.DeleteList();
/*
	// define a single pole with red/white stripes
	glPushMatrix();
	m_RangePole.StartDef();
	for (i = 0; i < slices; i ++) {
		if (i % 2 == 0)
			SetAnaglyphSensitiveColor(RGB(255, 0, 0));		// red
		else
			SetAnaglyphSensitiveColor(RGB(223, 223, 223));		// white

		gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, HeightStep, 16, 1);
		glTranslated(0.0, 0.0, HeightStep);
	}
	// see if we need a last section
	if ((double) slices * HeightStep < PoleHeight) {
		if (slices % 2 == 0)
			SetAnaglyphSensitiveColor(RGB(255, 0, 0));		// red
		else
			SetAnaglyphSensitiveColor(RGB(223, 223, 223));		// white
		gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, PoleHeight - ((double) slices * HeightStep), 16, 1);
	}
	m_RangePole.EndDef();
	glPopMatrix();
*/
	// define red and white stripes
	glPushMatrix();
	m_RangePoleRedStripes.StartDef();
	for (i = 0; i < slices; i ++) {
		if (i % 2 == 0)
			gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, HeightStep, 16, 1);
	
		glTranslated(0.0, 0.0, HeightStep);
	}
	// see if we need a last section
	if ((double) slices * HeightStep < PoleHeight) {
		if (slices % 2 == 0)
			gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, PoleHeight - ((double) slices * HeightStep), 16, 1);
	}
	m_RangePoleRedStripes.EndDef();
	glPopMatrix();

	// define a single pole with red/white stripes
	glPushMatrix();
	m_RangePoleWhiteStripes.StartDef();
	for (i = 0; i < slices; i ++) {
		if (i % 2 != 0)
			gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, HeightStep, 16, 1);
	
		glTranslated(0.0, 0.0, HeightStep);
	}
	// see if we need a last section
	if ((double) slices * HeightStep < PoleHeight) {
		if (slices % 2 != 0)
			gluCylinder(quad, PoleDiameter / 2.0, PoleDiameter / 2.0, PoleHeight - ((double) slices * HeightStep), 16, 1);
	}
	m_RangePoleWhiteStripes.EndDef();
	glPopMatrix();

	gluDeleteQuadric(quad);
}

void CChildView::CreateClockFace()
{

	// create the "object" to draw
	GLUquadricObj* quad = gluNewQuadric();
	m_Object.StartDef();
		// center ball
		glColor3d(0.0, 0.0, 1.0);
		gluSphere(quad, 0.025, 16, 32);

		// draw outer rim
		glColor3d(1.0, 0.0, 0.0);
		glTranslated(0.0, 0.0, -0.05);
		gluCylinder(quad, 0.5, 0.5, 0.1, 120, 1);
		gluQuadricOrientation(quad, GLU_INSIDE);
		gluCylinder(quad, 0.475, 0.5, 0.1, 120, 1);

		gluDisk(quad, 0.475, 0.5, 120, 2);
		
		glTranslated(0.0, 0.0, 0.049);

		// face
		glColor3d(0.7, 0.7, 0.7);
		gluDisk(quad, 0.0, 0.5, 120, 2);
		glTranslated(0.0, 0.0, 0.002);
		gluQuadricOrientation(quad, GLU_OUTSIDE);
		gluDisk(quad, 0.0, 0.5, 120, 2);

		// top of rim
		glColor3d(1.0, 0.0, 0.0);
		glTranslated(0.0, 0.0, 0.049);
		gluDisk(quad, 0.475, 0.5, 120, 2);
		// draw balls at hour marks

		glTranslated(0.0, 0.0, -0.05);
		glColor3d(1.0, 1.0, 0.0);
		for (int i = 0; i < 12; i ++) {
			glPushMatrix();
			glRotated((double) i * 30.0, 0.0, 0.0, 1.0);
			glTranslated(0.425, 0.0, 0.0);
			if (i == 9)
				glColor3d(1.0, 0.0, 0.0);
			else
				glColor3d(1.0, 1.0, 0.0);
			gluSphere(quad, 0.025, 16, 32);
			glPopMatrix();
		}
	m_Object.EndDef();

	// second hand
	m_SecondHand.StartDef();
		glColor3d(1.0, 0.0, 0.0);
		glPushMatrix();
		glRotated(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quad, 0.005, 0.0, 0.4, 16, 1);
		glPopMatrix();
	m_SecondHand.EndDef();

	// minute hand
	m_MinuteHand.StartDef();
		glColor3d(0.0, 0.0, 0.0);
		glPushMatrix();
		glRotated(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quad, 0.015, 0.0, 0.4, 16, 1);
		glPopMatrix();
	m_MinuteHand.EndDef();

	// hour hand
	m_HourHand.StartDef();
		glColor3d(0.0, 0.0, 0.0);
		glPushMatrix();
		glRotated(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quad, 0.015, 0.0, 0.3, 16, 1);
		glPopMatrix();
	m_HourHand.EndDef();

	gluDeleteQuadric(quad);

	m_DataType = CLOCK;
}

void CChildView::CreateAxes()
{
	double pts[8][3] = {
		{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{1.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
		{1.0, 0.0, 1.0},
		{1.0, 1.0, 1.0},
		{0.0, 1.0, 1.0}
	};
	double clr[8][3] = {
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 1.0, 1.0},
		{0.0, 1.0, 1.0},
		{0.0, 0.0, 1.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0}
	};

	// create the "object" to draw
	GLUquadricObj* quad = gluNewQuadric();
	m_Object.StartDef();
		// draw balls
		glTranslated(-0.5, -0.5, -0.5);
		for (int i = 0; i < 8; i ++) {
			glPushMatrix();
			glTranslated(pts[i][0], pts[i][1], pts[i][2]);
			glColor3d(clr[i][0], clr[i][1], clr[i][2]);
			gluSphere(quad, 0.05, 8, 16);
			glPopMatrix();
		}

		glPushMatrix();
			// Y axis
			glColor3d(0.0, 0.0, 1.0);
			gluCylinder(quad, 0.01, 0.01, 1.0, 16, 8);
		glPopMatrix();
		glPushMatrix();
			// x axis
			glColor3d(1.0, 0.0, 0.0);
			glRotated(90.0, 0.0, 1.0, 0.0);
			gluCylinder(quad, 0.01, 0.01, 1.0, 16, 8);
		glPopMatrix();
		glPushMatrix();
			glColor3d(0.0, 1.0, 0.0);
			glRotated(-90.0, 1.0, 0.0, 0.0);
			gluCylinder(quad, 0.01, 0.01, 1.0, 16, 8);
		glPopMatrix();
	m_Object.EndDef();

	gluDeleteQuadric(quad);
}

void CChildView::CreateJack()
{
	double pts[8][3] = {
		{ 0.0,  0.0,  0.0},
		{ 0.5,  0.0,  0.0},
		{-0.5,  0.0,  0.0},
		{ 0.0,  0.5,  0.0},
		{ 0.0, -0.5,  0.0},
		{ 0.0,  0.0,  0.5},
		{ 0.0,  0.0, -0.5},
	};
	double clr[8][3] = {
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 1.0},
		{1.0, 0.0, 1.0},
	};

	int slices = 32;
	int rings =33;

	// create the "object" to draw
	GLUquadricObj* quad = gluNewQuadric();
	m_Object.StartDef();
		// draw balls
		for (int i = 0; i < 7; i ++) {
			glPushMatrix();
			glTranslated(pts[i][0], pts[i][1], pts[i][2]);
			glColor3d(clr[i][0], clr[i][1], clr[i][2]);
			if (i == 0)
				gluSphere(quad, 0.125, slices, rings);
			else
				gluSphere(quad, 0.1, slices, rings);
			glPopMatrix();
		}

		glColor3d(1.0, 1.0, 0.0);
		glPushMatrix();
			// z axis cones
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
			glRotated(180.0, 0.0, 1.0, 0.0);
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
		glPopMatrix();
		glPushMatrix();
			// x axis
			glRotated(90.0, 0.0, 1.0, 0.0);
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
			glRotated(180.0, 0.0, 1.0, 0.0);
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
		glPopMatrix();
		glPushMatrix();
			glRotated(90.0, 1.0, 0.0, 0.0);
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
			glRotated(180.0, 0.0, 1.0, 0.0);
			gluCylinder(quad, 0.075, 0.025, 0.5, slices, 1);
		glPopMatrix();

	m_Object.EndDef();

	gluDeleteQuadric(quad);

	m_DataType = JACK;
}

void CChildView::CreateCylinder(LPCTSTR FileName, int Format)
{
	int slices = 36;

	// create the "object" to draw
	m_MarkerTexture.LoadTextureMap(FileName, FALSE);
	glBindTexture(GL_TEXTURE_2D, m_MarkerTextureName);
	m_MarkerTexture.ActivateTextureMap(GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR, TRUE);
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);

	m_Object.StartDef();
		glColor3d(1.0, 1.0, 1.0);
		glPushMatrix();
			glBegin(GL_QUAD_STRIP);
			for (int i = 0; i <= slices; i ++) {
				glTexCoord2d((double) i * 1.0 / (double) slices, 0.0);
				glVertex3d(0.5 * sin((double) i * 10 * D2R), 0.5 * cos((double) i * 10.0 * D2R), -0.25);
				glTexCoord2d((double) i * 1.0 / (double) slices, 1.0);
				glVertex3d(0.5 * sin((double) i * 10 * D2R), 0.5 * cos((double) i * 10.0 * D2R), 0.25);
			}
			glEnd();

			// z axis cylinder...texture wraps around the outside so it ends up looking backwards
//			gluCylinder(quad, 0.5, 0.5, 0.5, slices, 1);
		glPopMatrix();

	m_Object.EndDef();

	gluDeleteQuadric(quad);

}

void CChildView::CreatePointCloud(LPCTSTR FileName, int Format)
{
//	ReadPointCloud("");
//	return;

	CFileSpec fs(CFileSpec::FS_APPDIR);
	CString PointFileName = _T(FileName);
	if (PointFileName.IsEmpty()) {
		PointFileName = _T("tempdat.xyz");

		// use directory for executable and default file name
		fs.SetFileNameEx(PointFileName);
	}
	else {
		PointFileName.Remove(34);
		fs.SetFullSpec(PointFileName);

		if (!fs.Exists()) {
			fs.SetFullSpec(CFileSpec::FS_APPDIR);
			fs.SetFileNameEx(PointFileName);
		}
	}

	if (!fs.Exists()) {
		// can't open file...display message and simply return...current data will be intact
		CString prompt;
		prompt.Format("Could not open data file: \n%s\n", fs.GetFullSpec());
		AfxMessageBox(prompt);

		return;
	}

	m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- %s", PROGRAM_VERSION, fs.GetFileNameEx());

	BeginWaitCursor();

	// format options...
	//	ASCII X Y Z R G B
	//	ASCII LIDARDAT
	//	binary LDVD
	//	binary LDA/LAS
	int DataFormat = Format;
	CDataFile asciidat;
	FILE* f;
	char line[1025];
	int DatCount = 1;
	long dataoffset;
	float version;
	int ptcnt = 0;
	int asciifldcnt = 0;
	char* c;

	// try for known LIDAR format
	LIDARRETURN pt;
	CLidarData ldat(fs.GetFullSpec());
	if (ldat.IsValid()) {
		DataFormat = LIDARDATA;
	}

	if (DataFormat == UNKNOWNFORMAT) {
		// see if data file is binary LDVD
		f = fopen(fs.GetFullSpec(), "rb");
		if (f) {
			if (fread(line, sizeof(char), 4, f) == 4) {
				line[4] = '\0';
				if (strcmp(line, "LDVD") == 0) {
					// read version and number of points
					fread(&version, sizeof(float), 1, f);
					fread(&ptcnt, sizeof(int), 1, f);

					dataoffset = ftell(f);

					// set flag
					DataFormat = BINARY_LDVD;
				}
			}
			fclose(f);
		}
	}

	if (DataFormat == UNKNOWNFORMAT) {
		// read a line of the file and try to parse XYZ values
		asciidat.Open(fs.GetFullSpec());
		asciifldcnt = 0;
		if (asciidat.IsValid()) {
			asciidat.ReadDataLine(line, ";%!#\"");
			c = strtok(line, " ,\t");
			if (c)
				asciifldcnt ++;
			c = strtok(NULL, " ,\t");
			if (c)
				asciifldcnt ++;
			c = strtok(NULL, " ,\t");
			if (c)
				asciifldcnt ++;
			c = strtok(NULL, " ,\t");
			if (c)
				asciifldcnt ++;
			c = strtok(NULL, " ,\t");
			if (c)
				asciifldcnt ++;
			c = strtok(NULL, " ,\t");
			if (c)
				asciifldcnt ++;

			asciidat.Close();
		}

		// assume ASCII XYZRGB
		DataFormat = ASCII_XYZRGB;

		// if there are 4 fields, assume ID,X,Y,Z
		if (asciifldcnt == 4)
			DataFormat = ASCII_IDXYZ;

		// make sure we got at least 3 fields
		if (asciifldcnt < 3) {
			CString prompt;
			prompt.Format("Data file does not appear to contain XYZ data: \n%s\n", fs.GetFullSpec());
			AfxMessageBox(prompt);

			return;
		}
	}

	// open file...for LIDARDATA, the file is already opened in the CLidarData object
	if (DataFormat == BINARY_LDVD)
		f = fopen(fs.GetFullSpec(), "rb");
	else if (DataFormat == ASCII_XYZRGB || DataFormat == ASCII_IDXYZ || DataFormat == ASCII_SBET)
		asciidat.Open(fs.GetFullSpec());

	// seek past header if reading binary data
	if (DataFormat == BINARY_LDVD) {
		fseek(f, dataoffset, SEEK_SET);
	}

	double x, y, z;
	char ID[256];
	COLORREF color;
	float attribute;
//	int r, g, b;
	double minx = 999999999.9;
	double miny = 999999999.9;
	double minz = 999999999.9;
	double maxx = -999999999.9;
	double maxy = -999999999.9;
	double maxz = -999999999.9;
	unsigned short maxcolorcomponent = 0;
	double actual_minz;
	int i;
	BOOL DrawPoint;

	if (DataFormat) {
		if (DataFormat == BINARY_LDVD) {
			m_ColorMode = COLORBYHEIGHT;
			for (i = 0; i < ptcnt; i ++) {
				// read binary data points
				fread(&x, sizeof(double), 1, f);
				fread(&y, sizeof(double), 1, f);
				fread(&z, sizeof(double), 1, f);
				fread(&color, sizeof(COLORREF), 1, f);
				fread(&attribute, sizeof(float), 1, f);

				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
				if (z < minz)
					minz = z;
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (z > maxz)
					maxz = z;
			}
		}
		else if (DataFormat == LIDARDATA) {
			if (ldat.GetFileFormat() == LASDATA) {
#ifdef USE_LASLIB
				ptcnt = (int) ldat.lasreader->npoints;
				minx = ldat.lasreader->get_min_x();
				miny = ldat.lasreader->get_min_y();
				minz = ldat.lasreader->get_min_z();
				maxx = ldat.lasreader->get_max_x();
				maxy = ldat.lasreader->get_max_y();
				maxz = ldat.lasreader->get_max_z();
#else
				if (ldat.m_HaveLASZIP_DLL) {
					ptcnt = (int) ldat.lasdll_header->number_of_point_records;
					minx = ldat.lasdll_header->min_x;
					miny = ldat.lasdll_header->min_y;
					minz = ldat.lasdll_header->min_z;
					maxx = ldat.lasdll_header->max_x;
					maxy = ldat.lasdll_header->max_y;
					maxz = ldat.lasdll_header->max_z;
				}
				else {
					ptcnt = (int) ldat.m_LASFile.Header.NumberOfPointRecords;
					minx = ldat.m_LASFile.Header.MinX;
					miny = ldat.m_LASFile.Header.MinY;
					minz = ldat.m_LASFile.Header.MinZ;
					maxx = ldat.m_LASFile.Header.MaxX;
					maxy = ldat.m_LASFile.Header.MaxY;
					maxz = ldat.m_LASFile.Header.MaxZ;
				}
#endif
				// if point records have RGB info, look for max value...format allows 16-bit color values so we have
				// to know how to scale them to create actual colors
#ifdef USE_LASLIB
				if (ldat.lasreader->point.have_rgb) {
					while (ldat.ReadNextRecord(&pt)) {
						maxcolorcomponent = max(maxcolorcomponent, ldat.lasreader->point.rgb[0]);
						maxcolorcomponent = max(maxcolorcomponent, ldat.lasreader->point.rgb[1]);
						maxcolorcomponent = max(maxcolorcomponent, ldat.lasreader->point.rgb[2]);
					}
				}
#else
				if (ldat.m_HaveLASZIP_DLL) {
					if (ldat.lasdll_header->point_data_format == 2 || ldat.lasdll_header->point_data_format == 3 || ldat.lasdll_header->point_data_format == 5 || ldat.lasdll_header->point_data_format == 7 || ldat.lasdll_header->point_data_format == 8 || ldat.lasdll_header->point_data_format == 10) {
						while (ldat.ReadNextRecord(&pt)) {
							maxcolorcomponent = max(maxcolorcomponent, ldat.lasdll_point->rgb[0]);
							maxcolorcomponent = max(maxcolorcomponent, ldat.lasdll_point->rgb[1]);
							maxcolorcomponent = max(maxcolorcomponent, ldat.lasdll_point->rgb[2]);
						}
					}
				}
				else {
					if (ldat.m_LASFile.Header.PointDataFormatID == 2 || ldat.m_LASFile.Header.PointDataFormatID == 3 || ldat.m_LASFile.Header.PointDataFormatID == 5 || ldat.m_LASFile.Header.PointDataFormatID == 7 || ldat.m_LASFile.Header.PointDataFormatID == 8 || ldat.m_LASFile.Header.PointDataFormatID == 10) {
						while (ldat.ReadNextRecord(&pt)) {
							maxcolorcomponent = max(maxcolorcomponent, ldat.m_LASFile.PointRecord.Red);
							maxcolorcomponent = max(maxcolorcomponent, ldat.m_LASFile.PointRecord.Green);
							maxcolorcomponent = max(maxcolorcomponent, ldat.m_LASFile.PointRecord.Blue);
						}
					}
				}
#endif
			}
			else {
				m_ColorMode = COLORBYHEIGHT;
				while (ldat.ReadNextRecord(&pt)) {
					ptcnt ++;
					if (pt.X < minx)
						minx = pt.X;
					if (pt.Y < miny)
						miny = pt.Y;
					if (pt.Elevation < minz)
						minz = pt.Elevation;
					if (pt.X > maxx)
						maxx = pt.X;
					if (pt.Y > maxy)
						maxy = pt.Y;
					if (pt.Elevation > maxz)
						maxz = pt.Elevation;
				}
			}
		}
		else if (DataFormat == ASCII_XYZRGB) {
			m_ColorMode = COLORBYHEIGHT;
			while (asciidat.ReadDataLine(line, ";%!#\"")) {
				x = atof(strtok(line, " ,\t"));
				y = atof(strtok(NULL, " ,\t"));
				z = atof(strtok(NULL, " ,\t"));
				ptcnt ++;
				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
				if (z < minz)
					minz = z;
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (z > maxz)
					maxz = z;
			}
		}
		else if (DataFormat == ASCII_IDXYZ) {
			m_ColorMode = COLORBYHEIGHT;
			while (asciidat.ReadDataLine(line, ";%!#\"")) {
				strcpy(ID, strtok(line, " ,\t"));
				x = atof(strtok(NULL, " ,\t"));
				y = atof(strtok(NULL, " ,\t"));
				z = atof(strtok(NULL, " ,\t"));
				ptcnt ++;
				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
				if (z < minz)
					minz = z;
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (z > maxz)
					maxz = z;
			}
		}
		else if (DataFormat == ASCII_SBET) {
			m_ColorMode = COLORBYHEIGHT;
			while (asciidat.ReadDataLine(line, ";%!#\"")) {
				strcpy(ID, strtok(line, " ,\t"));
				y = atof(strtok(NULL, " ,\t"));
				x = atof(strtok(NULL, " ,\t"));
				z = atof(strtok(NULL, " ,\t"));

				ptcnt ++;
//				if (x < minx)
//					minx = x;
//				if (y < miny)
//					miny = y;
//				if (z < minz)
//					minz = z;
//				if (x > maxx)
//					maxx = x;
//				if (y > maxy)
//					maxy = y;
//				if (z > maxz)
//					maxz = z;
			}
		}

		actual_minz = minz;

		// use min elevation of 0 for SBET files
		if (DataFormat == ASCII_SBET) {
//			minz = 0.0;
			minx = m_SBETMinLong;
			miny = m_SBETMinLat;
			minz = m_SBETMinEllHt;
			maxx = m_SBETMaxLong;
			maxy = m_SBETMaxLat;
			maxz = m_SBETMaxEllHt;
		}

		// see if we are using the rangepole to scale the vertical dimension...if so only use if the pole height is greater than the verical
		// span of the data
		if (m_UseRangepoleHeightToScaleData && !m_ScaleRangepoleToData && (m_RangepoleTotalHeight > (maxz - minz)))
			maxz = minz + m_RangepoleTotalHeight;

		// figure out scaling
		double xspan = maxx - minx;
		double yspan = maxy - miny;
		double zspan = maxz - minz;

		// set up for color scaling...used when incoming data does not contain color information
		double ColorMinZ = minz;
		double ColorMaxZ = maxz;
		double ColorRange = zspan;
		int BaseR = 128;
		int BaseG = 64;
		int BaseB = 0;
		int TopR = 0;
		int TopG = 255;
		int TopB = 0;

		// color scaling value for LAS RGB color components
		unsigned short ColorScale = 255;
		if (maxcolorcomponent >= 255)
			ColorScale *= 255;

		// special color rules for SBET files
		if (DataFormat == ASCII_SBET) {
			ColorMinZ = 0.0;
			ColorMaxZ = 15.0;
			ColorRange = 15.0;
			BaseR = 0;
			BaseG = 255;
			BaseB = 0;
			TopR = 255;
			TopG = 0;
			TopB = 0;
		}

		int SpreadR = TopR - BaseR;
		int SpreadG = TopG - BaseG;
		int SpreadB = TopB - BaseB;

		// get max dimension
		double maxspan = max(xspan, yspan);
		maxspan = max(maxspan, zspan);

		m_ScaledVerticalSpan = maxspan;
		m_ActualVerticalSpan = maxz - minz;

		// adjust scaling
		if (DataFormat != ASCII_SBET) {
			minx -= (maxspan - xspan) / 2.0;
			maxx += (maxspan - xspan) / 2.0;
			miny -= (maxspan - yspan) / 2.0;
			maxy += (maxspan - yspan) / 2.0;
			minz -= (maxspan - zspan) / 2.0;
			maxz += (maxspan - zspan) / 2.0;
		}

		m_PoleBaseZ = (actual_minz - minz) / maxspan - 0.5;

		// rewind file
		if (DataFormat == BINARY_LDVD)
			rewind(f);
		else if (DataFormat == ASCII_XYZRGB || DataFormat == ASCII_IDXYZ || DataFormat == ASCII_SBET)
			asciidat.Rewind();
		else
			ldat.Rewind();

		// seek past header if reading binary data
		if (DataFormat == BINARY_LDVD) {
			fseek(f, dataoffset, SEEK_SET);
		}

		// allocate memory for point data
		BOOL errflag = FALSE;
		double pitchangle;
		double* pt_x;
		double* pt_y;
		double* pt_z;
		unsigned char* pt_r;
		unsigned char* pt_g;
		unsigned char* pt_b;
		int AllocationLevel = 0;

		try {
			pt_x = new double[ptcnt];
			if (pt_x) {
				AllocationLevel = 1;
				pt_y = new double[ptcnt];
				if (pt_y) {
					AllocationLevel = 2;
					pt_z = new double[ptcnt];
					if (pt_z) {
						AllocationLevel = 3;
						pt_r = new unsigned char[ptcnt];
						if (pt_r) {
							AllocationLevel = 4;
							pt_g = new unsigned char[ptcnt];
							if (pt_g) {
								AllocationLevel = 5;
								pt_b = new unsigned char[ptcnt];
								if (!pt_b) {
									delete[] pt_x;
									delete[] pt_y;
									delete[] pt_z;
									delete[] pt_r;
									delete[] pt_g;
									errflag = TRUE;
								}
							}
							else {
								delete[] pt_x;
								delete[] pt_y;
								delete[] pt_z;
								delete[] pt_r;
								errflag = TRUE;
							}
						}
						else {
							delete[] pt_x;
							delete[] pt_y;
							delete[] pt_z;
							errflag = TRUE;
						}
					}
					else {
						delete[] pt_x;
						delete[] pt_y;
						errflag = TRUE;
					}
				}
				else {
					delete[] pt_x;
					errflag = TRUE;
				}
			}
			else
				errflag = TRUE;
		}
		catch (const CMemoryException *e) {
			// allocation failed at some point...use AllocationLevel to clean up anything that may have been successfully allocated
			if (AllocationLevel >= 5)
				delete[] pt_g;
			if (AllocationLevel >= 4)
				delete[] pt_r;
			if (AllocationLevel >= 3)
				delete[] pt_z;
			if (AllocationLevel >= 2)
				delete[] pt_y;
			if (AllocationLevel >= 1)
				delete[] pt_x;

//			AfxMessageBox("Memory allocation for point data failed");

			errflag = TRUE;
		}

		if (errflag) {
			CString msg;
			msg.Format("Not enough memory to load data...%i points", ptcnt);
			AfxMessageBox(msg);
			return;
		}

		// have memory...delete old display lists
		if (m_PointCount) {
			for (int layer = 0; layer < DATALAYERS; layer ++)
				m_PointLayer[layer].DeleteList();
		}

		for (int i = 0; i < 256; i++) {
			m_DataHasClass[i] = FALSE;
		}

		m_DataHasSynthetic = FALSE;
		m_DataHasOverlap = FALSE;
		m_DataHasKeypoint = FALSE;
		m_DataHasWithheld = FALSE;

		// read data
		BOOL HavePointColor = TRUE;
		int LASClass;

//		if ((DataFormat == ASCII_XYZRGB && asciifldcnt < 6) || DataFormat == LIDARDATA)
//			HavePointColor = FALSE;

		for (i = 0; i < ptcnt; i ++) {
			if (DataFormat == BINARY_LDVD) {
				// read binary data points
				fread(&pt_x[i], sizeof(double), 1, f);
				fread(&pt_y[i], sizeof(double), 1, f);
				fread(&pt_z[i], sizeof(double), 1, f);
				fread(&color, sizeof(COLORREF), 1, f);
				fread(&attribute, sizeof(float), 1, f);

				pt_r[i] = (unsigned char) (GetRValue(color));
				pt_g[i] = (unsigned char) (GetGValue(color));
				pt_b[i] = (unsigned char) (GetBValue(color));
			}
			else if (DataFormat == LIDARDATA) {
				ldat.ReadNextRecord(&pt);
				pt_x[i] = pt.X;
				pt_y[i] = pt.Y;
				pt_z[i] = pt.Elevation;
				
				// set point color
				if (m_ColorMode == COLORBYCLASSIFICATION) {
#ifdef USE_LASLIB
					LASClass = ldat.lasreader->point.classification;
#else
					if (ldat.m_HaveLASZIP_DLL) {
						LASClass = ldat.lasdll_point->classification;
					}
					else {
						LASClass = ldat.m_LASFile.PointRecord.Classification;
					}
#endif

					// 1/29/2019 added dialog to give control over the points displayed according to their classification value and
					// the classification flags. This presents problems since we have counted ALL the points and allocated memory for
					// drawing data. For the first implementation, use a special color value to indicate points that will not be drawn.
					// 2/20/2019: Made a few changes to the dialog so you can display all points or only points with a classification flag set

//					// see if point should be drawn based on class
//					DrawPoint = m_LASClassState[LASClass];

					// check flags...point flag needs to match checkbox state
					if (m_ShowOnlyPointsWithFlags) {
						DrawPoint = FALSE;
						if (ldat.LastPointIsKeypoint() && m_LASKeypoint)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsSynthetic() && m_LASSynthetic)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsOverlap() && m_LASOverlap)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsWithheld() && m_LASWithheld)
							DrawPoint = m_LASClassState[LASClass];
					}
					else {
						DrawPoint = FALSE;
						if (ldat.LastPointIsKeypoint() && m_LASKeypoint)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsSynthetic() && m_LASSynthetic)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsOverlap() && m_LASOverlap)
							DrawPoint = m_LASClassState[LASClass];
						if (ldat.LastPointIsWithheld() && m_LASWithheld)
							DrawPoint = m_LASClassState[LASClass];

						if (!ldat.LastPointIsKeypoint() && !ldat.LastPointIsSynthetic() && !ldat.LastPointIsOverlap() && !ldat.LastPointIsWithheld())
							DrawPoint = m_LASClassState[LASClass];
					}
					
					m_DataHasClass[LASClass] = TRUE;

					if (ldat.LastPointIsSynthetic())
						m_DataHasSynthetic = TRUE;
					if (ldat.LastPointIsOverlap())
						m_DataHasOverlap = TRUE;
					if (ldat.LastPointIsKeypoint())
						m_DataHasKeypoint = TRUE;
					if (ldat.LastPointIsWithheld())
						m_DataHasWithheld = TRUE;

					if (DrawPoint) {
						// original code below

						// limit the class value to 13...this is a carry-over from pre-V1.4 formats...
						// fixed this in Jan 2019...expanded the number of class colors to 32...now limited to 31
						// point record formats 0-5 allow 4 bits so values of 0-31
						// format 6+ allows 8 bits so values of 0-255
						LASClass = min(LASClass, 31);
						LASClass = max(LASClass, 0);

//						if (ldat.LastPointIsWithheld()) {
//							pt_r[i] = GetRValue(m_LASClassColors[14]);
//							pt_g[i] = GetGValue(m_LASClassColors[14]);
//							pt_b[i] = GetBValue(m_LASClassColors[14]);
//						}
//						else {
							pt_r[i] = GetRValue(m_LASClassColors[LASClass]);
							pt_g[i] = GetGValue(m_LASClassColors[LASClass]);
							pt_b[i] = GetBValue(m_LASClassColors[LASClass]);
//						}

						// original code above
					}
					else {
						pt_r[i] = 0;
						pt_g[i] = 1;
						pt_b[i] = 2;
					}
				}
				else if (m_ColorMode == COLORBYRETURN) {
					if (pt.ReturnNumber > 0 && pt.ReturnNumber < 15) {
						// set color by return number
						pt_r[i] = GetRValue(m_LASReturnColors[pt.ReturnNumber - 1]);
						pt_g[i] = GetGValue(m_LASReturnColors[pt.ReturnNumber - 1]);
						pt_b[i] = GetBValue(m_LASReturnColors[pt.ReturnNumber - 1]);
					}
					else {
						// default color
						pt_r[i] = GetRValue(m_LASReturnColors[14]);
						pt_g[i] = GetGValue(m_LASReturnColors[14]);
						pt_b[i] = GetBValue(m_LASReturnColors[14]);
					}
				}
				else if (m_ColorMode == COLORBYINTENSITY) {
					pt.Intensity = min(pt.Intensity, 255.0f);
					pt.Intensity = max(pt.Intensity, 0.0f);
					pt_r[i] = (unsigned char) ((double) BaseR + ((double) pt.Intensity / 255.0) * (double) SpreadR);
					pt_g[i] = (unsigned char) ((double) BaseG + ((double) pt.Intensity / 255.0) * (double) SpreadG);
					pt_b[i] = (unsigned char) ((double) BaseB + ((double) pt.Intensity / 255.0) * (double) SpreadB);
				}
				else if (m_ColorMode == COLORBYHEIGHT) {
					pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
					pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
					pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
				}
				else {		// m_ColorMode == COLORBYRGB && m_ColorMode == COLORBYNIR
					// read RGB and scale to 8 bit values
#ifdef USE_LASLIB
					if (ldat.GetFileFormat() == LASDATA) {
						if (ldat.lasreader->point.have_rgb) {
							pt_r[i] = (unsigned char) (((double) ldat.lasreader->point.rgb[0] / (double) ColorScale) * 256.0);
							pt_g[i] = (unsigned char) (((double) ldat.lasreader->point.rgb[1] / (double) ColorScale) * 256.0);
							pt_b[i] = (unsigned char) (((double) ldat.lasreader->point.rgb[2] / (double) ColorScale) * 256.0);
						}
						else {
							pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
							pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
							pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
						}
					}
					else {		// LDA format
						pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
						pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
						pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
					}
#else
					if (ldat.m_HaveLASZIP_DLL) {
						if (ldat.GetFileFormat() == LASDATA) {
							if (ldat.lasdll_header->point_data_format == 2 || ldat.lasdll_header->point_data_format == 3 || ldat.lasdll_header->point_data_format == 5 || ldat.lasdll_header->point_data_format == 7 || ldat.lasdll_header->point_data_format == 8 || ldat.lasdll_header->point_data_format == 10) {
								if (m_ColorMode == COLORBYNIR && (ldat.lasdll_header->point_data_format == 8 || ldat.lasdll_header->point_data_format == 10)) {
									pt_r[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[3] / (double) ColorScale) * 256.0);
									pt_g[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[0] / (double) ColorScale) * 256.0);
									pt_b[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[1] / (double) ColorScale) * 256.0);
								}
								else {
									pt_r[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[0] / (double) ColorScale) * 256.0);
									pt_g[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[1] / (double) ColorScale) * 256.0);
									pt_b[i] = (unsigned char) (((double) ldat.lasdll_point->rgb[2] / (double) ColorScale) * 256.0);
								}
							}
							else {
								pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
								pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
								pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
							}
						}
						else {		// LDA format
							pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
							pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
							pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
						}
					}
					else {
						if (ldat.GetFileFormat() == LASDATA) {
							if (ldat.m_LASFile.Header.PointDataFormatID == 2 || ldat.m_LASFile.Header.PointDataFormatID == 3 || ldat.m_LASFile.Header.PointDataFormatID == 5 || ldat.m_LASFile.Header.PointDataFormatID == 7 || ldat.m_LASFile.Header.PointDataFormatID == 8 || ldat.m_LASFile.Header.PointDataFormatID == 10) {
								if (m_ColorMode == COLORBYNIR && (ldat.m_LASFile.Header.PointDataFormatID == 8 || ldat.m_LASFile.Header.PointDataFormatID == 10)) {
									pt_r[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.NIR / (double) ColorScale) * 256.0);
									pt_g[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.Red / (double) ColorScale) * 256.0);
									pt_b[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.Green / (double) ColorScale) * 256.0);
								}
								else {
									pt_r[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.Red / (double) ColorScale) * 256.0);
									pt_g[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.Green / (double) ColorScale) * 256.0);
									pt_b[i] = (unsigned char) (((double) ldat.m_LASFile.PointRecord.Blue / (double) ColorScale) * 256.0);
								}
							}
							else {
								pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
								pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
								pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
							}
						}
						else {		// LDA format
							pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
							pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
							pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
						}
					}
#endif
				}
			}
			else if (DataFormat == ASCII_XYZRGB) {
				asciidat.ReadDataLine(line, ";%!#\"");
				pt_x[i] = atof(strtok(line, " ,\t"));
				pt_y[i] = atof(strtok(NULL, " ,\t"));
				pt_z[i] = atof(strtok(NULL, " ,\t"));
				if (asciifldcnt >= 6) {
					pt_r[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
					pt_g[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
					pt_b[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
				}
				else {
					pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
					pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
					pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
				}
//				if (sscanf(line, "%lf %lf %lf %i %i %i", &pt_x[i], &pt_y[i], &pt_z[i], &r, &g, &b) == 6) {
//					pt_r[i] = (unsigned char) r;
//					pt_g[i] = (unsigned char) g;
//					pt_b[i] = (unsigned char) b;
//				}
//				else {
//					HavePointColor = FALSE;
//				}
			}
			else if (DataFormat == ASCII_IDXYZ) {
				asciidat.ReadDataLine(line, ";%!#\"");
				strcpy(ID, strtok(line, " ,\t"));
				pt_x[i] = atof(strtok(NULL, " ,\t"));
				pt_y[i] = atof(strtok(NULL, " ,\t"));
				pt_z[i] = atof(strtok(NULL, " ,\t"));
				if (asciifldcnt > 6) {
					pt_r[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
					pt_g[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
					pt_b[i] = (unsigned char) atoi(strtok(NULL, " ,\t"));
				}
				else {
					pt_r[i] = (unsigned char) ((double) BaseR + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadR);
					pt_g[i] = (unsigned char) ((double) BaseG + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadG);
					pt_b[i] = (unsigned char) ((double) BaseB + ((pt_z[i] - ColorMinZ) / ColorRange) * (double) SpreadB);
				}
			}
			else if (DataFormat == ASCII_SBET) {
				asciidat.ReadDataLine(line, ";%!#\"");
				strcpy(ID, strtok(line, " ,\t"));
				pt_y[i] = atof(strtok(NULL, " ,\t"));
				pt_x[i] = atof(strtok(NULL, " ,\t"));
				pt_z[i] = atof(strtok(NULL, " ,\t"));

				pitchangle = atof(strtok(NULL, " ,\t"));
				pitchangle = fabs(atof(strtok(NULL, " ,\t")));
				pt_r[i] = (unsigned char) ((double) BaseR + ((pitchangle - ColorMinZ) / ColorRange) * (double) SpreadR);
				pt_g[i] = (unsigned char) ((double) BaseG + ((pitchangle - ColorMinZ) / ColorRange) * (double) SpreadG);
				pt_b[i] = (unsigned char) ((double) BaseB + ((pitchangle - ColorMinZ) / ColorRange) * (double) SpreadB);
			}
		}

		if (DataFormat == BINARY_LDVD)
			fclose(f);

		// build display lists
		int lastr, lastg, lastb;
		lastr = lastg = lastb = -1;
		m_DrawnPointCount = 0;

		for (int layer = 0; layer < DATALAYERS; layer ++) {
			lastr = lastg = lastb = -1;
			m_PointLayer[layer].StartDef();
			if (!HavePointColor || DataFormat == ASCII_SBET)
					glColor3ub(255, 255, 0);

			glBegin(GL_POINTS);
//				glBegin(GL_LINE_STRIP);
				for (i = layer; i < ptcnt; i += DATALAYERS) {
					// see if we are drawing the point...rgb of 0,1,2 is the flag to not draw a point
					if (pt_r[i] == 0 && pt_g[i] == 1 && pt_b[i] == 2)
						continue;

					x = (pt_x[i] - minx) / maxspan;
					y = (pt_y[i] - miny) / maxspan;
					z = (pt_z[i] - minz) / maxspan;

					if ((pt_r[i] != lastr || pt_g[i] != lastg || pt_b[i] != lastb) && HavePointColor && DataFormat != ASCII_SBET) {
						glColor3ub(pt_r[i], pt_g[i], pt_b[i]);
						lastr = pt_r[i];
						lastg = pt_g[i];
						lastb = pt_b[i];
					}
					glVertex3d(x, y, z);
					m_DrawnPointCount ++;
				}
			glEnd();
			m_PointLayer[layer].EndDef();
		}
#define FLYGRIDSIZE		64
		// create a surface to use for flyball control using lowest point in cells
		// 1/10/2020...there are some scaling issues that I didn't do correctly when the fly grid was first implemented
		// @@@@@@@ using the maxspan in the calculation for col and row spacing seems to fix the XY position but the elevation may still be wrong as you can still go below ground
		// @@@@@@@ I think the problem is in the viewpoint positioning...DTM should be correct
		int row, col;
		double ptelev;
		double minelev = DBL_MAX;
		double maxelev = -DBL_MAX;
		double colspacing = maxspan / ((double)(FLYGRIDSIZE)-1.0);
		double rowspacing = maxspan / ((double)(FLYGRIDSIZE)-1.0);
		double elev;
		m_TerrainForFlyball.CreateNewModel(minx, miny, FLYGRIDSIZE, FLYGRIDSIZE, colspacing, rowspacing, 0, 0, 2, 0, 0, 0, 0, 100000000);
		if (m_TerrainForFlyball.IsValid()) {
			for (i = 0; i < ptcnt; i++) {
				col = (int)((pt_x[i] - minx) / colspacing);
				row = (int)((pt_y[i] - miny) / rowspacing);

				ptelev = pt_z[i];

				if (ptelev < m_TerrainForFlyball.GetGridElevation(col, row)) {
					m_TerrainForFlyball.SetInternalElevationValue(col, row, ptelev);
				}
			}

			// scan for min/max value
			for (row = 0; row < FLYGRIDSIZE; row ++) {
				for (col = 0; col < FLYGRIDSIZE; col ++) {
					elev = m_TerrainForFlyball.GetGridElevation(col, row);
					if (elev >= 99999999.0)
						m_TerrainForFlyball.SetInternalElevationValue(col, row, -1.0);
					else if (elev >= 0.0) {
						minelev = min(elev, minelev);
						maxelev = max(elev, maxelev);
					}
				}
			}

			m_TerrainForFlyball.ChangeMinMaxElevation(minelev, maxelev);

//			m_TerrainForFlyball.WriteModel("g:\\fusion_demo\\flyball.dtm");
			m_HaveTerrainForFlyball = TRUE;
			m_ScaledMinX = minx;
			m_ScaledMinY = miny;
			m_ScaledMinZ = minz;
			m_ScaledMaxX = maxx;
			m_ScaledMaxY = maxy;
			m_ScaledMaxZ = maxz;
		}
		delete [] pt_x;
		delete [] pt_y;
		delete [] pt_z;
		delete [] pt_r;
		delete [] pt_g;
		delete [] pt_b;

		// save point file name and format
		m_PointFileName = fs.GetFullSpec();
		m_PointFileFormat = DataFormat;

		// define a single rangepole
		if (m_ScaleRangepoleToData)
			CreateRangePole(m_ActualVerticalSpan / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);
		else
			CreateRangePole(m_RangepoleTotalHeight / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);
//		CreateRangePole(zspan / (maxz - minz), (5.0 / maxspan), 1.0 / maxspan);
//		CreateRangePole(100.0 / (maxz - minz), (5.0 / maxspan), 1.0 / maxspan);
	}
	else
		AfxMessageBox("Could not open data file");

	m_PointCount = ptcnt;

	if (m_ClassificationCodeDlg) {
		m_ClassificationCodeDlg->UpdateControls();
	}

	EndWaitCursor();
}

void CChildView::DrawPointCloud()
{
//	DrawPointList();
//	return;

	static int LayersInLeftEye = DATALAYERS;
	static int LayersInLeftEyeWhileMoving = DATALAYERS;
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(m_PointSize);

	glPushMatrix();
	glTranslated(-0.5, -0.5, -0.5);
	
	if (m_MotionController == FLYBALL)
		SetupFog(TRUE);

	double framerate;
	int LayersDrawn = 0;
	int layer;

	if (m_QAModelScan) {
		m_ScanOffsetX = (0.2 * (double) (m_ScanPositionIndex % 6)) - 0.5;
		m_ScanOffsetY = (0.2 * (double) (m_ScanPositionIndex / 6)) - 0.5;

		if ((m_ScanPositionIndex / 6) % 2 == 1)
			glTranslated(m_ScanOffsetX, m_ScanOffsetY, 0.0);
		else
			glTranslated(-m_ScanOffsetX, m_ScanOffsetY, 0.0);
	}

	SetAnaglyphSensitiveColor(RGB(255, 0, 0));
	glDisable(GL_LIGHTING);
	for (layer = 0; layer < DATALAYERS; layer ++) {
		m_PointLayer[layer].Draw();
		LayersDrawn ++;

		glFinish ();
		framerate = 1.0 / (m_FrameTimer.GetElapsed() + m_TimeBetweenFrames + m_TimeToUpdateFrameInfo);
		if (m_InMotion) {
			if (IsStereo() || m_bUseAnaglyph || m_bUseSplitScreen) {
				if (m_CurrentEyePosition == LEFT && framerate < TARGETFRAMERATE * 2.0) {
					LayersInLeftEyeWhileMoving = layer;
					break;
				}
				else if (m_CurrentEyePosition == RIGHT && layer >= LayersInLeftEyeWhileMoving)
					break;
			}
			else if (framerate < TARGETFRAMERATE)
				break;
		}
	}
	glEnable(GL_LIGHTING);

	SetupFog(FALSE);

	glPopMatrix();

	m_LayersDrawnInLastFrame = LayersDrawn;
}

void CChildView::DrawASCIIPointCloud()
{
	// this draw point layers with no blending. If blending is turned on, you get stripes and wierd patterns of light/dark points due to the blending and 
	// use of depth buffer for the transparent frame around the round point. The behavior seems to be dependent on the graphics card and drivers. Anti-aliased
	// points is one of the things that is not fully enforced in the OpenGL implementations so vendors can do different things. Draing without blending gives us
	// square points with even color...not a bad thing and different from point clouds.

//	DrawPointList();
//	return;

	static int LayersInLeftEye = DATALAYERS;
	static int LayersInLeftEyeWhileMoving = DATALAYERS;
	glPointSize(m_PointSize);

	glPushMatrix();
	glTranslated(-0.5, -0.5, -0.5);

	double framerate;
	int LayersDrawn = 0;
	int layer;

	if (m_QAModelScan) {
		m_ScanOffsetX = (0.2 * (double) (m_ScanPositionIndex % 6)) - 0.5;
		m_ScanOffsetY = (0.2 * (double) (m_ScanPositionIndex / 6)) - 0.5;

		if ((m_ScanPositionIndex / 6) % 2 == 1)
			glTranslated(m_ScanOffsetX, m_ScanOffsetY, 0.0);
		else
			glTranslated(-m_ScanOffsetX, m_ScanOffsetY, 0.0);
	}

	SetAnaglyphSensitiveColor(RGB(255, 0, 0));
	glDisable(GL_LIGHTING);
	for (layer = 0; layer < DATALAYERS; layer ++) {
		m_PointLayer[layer].Draw();
		LayersDrawn ++;

		glFinish ();
		framerate = 1.0 / (m_FrameTimer.GetElapsed() + m_TimeBetweenFrames + m_TimeToUpdateFrameInfo);
		if (m_InMotion) {
			if (IsStereo() || m_bUseAnaglyph || m_bUseSplitScreen) {
				if (m_CurrentEyePosition == LEFT && framerate < TARGETFRAMERATE * 2.0) {
					LayersInLeftEyeWhileMoving = layer;
					break;
				}
				else if (m_CurrentEyePosition == RIGHT && layer >= LayersInLeftEyeWhileMoving)
					break;
			}
			else if (framerate < TARGETFRAMERATE)
				break;
		}
	}
	glEnable(GL_LIGHTING);
	glPopMatrix();

	m_LayersDrawnInLastFrame = LayersDrawn;
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_SUBTRACT:
		if (::GetKeyState(VK_CONTROL) & 0x80) {
			if (m_PointSize > 1.0) {
				m_PointSize = m_PointSize - 1.0f;

				// send message to other instances to synchronize trackballs
				if (((CFullwindowApp*)AfxGetApp())->oInfo.IsSynchronize()) {
					SendSync();
				}

				Invalidate();
			}
		}
		else if (::GetKeyState(VK_SHIFT) & 0x80) {
			double step = -1.0;
			m_Camera.focallength += step * 10.0;
			m_Camera.focallength = min(m_Camera.focallength, 1000.0);
			m_Camera.focallength = max(m_Camera.focallength, 20.0);

			m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
			m_Camera.eyesep = m_Camera.focallength / 1300.0;

			m_InZoom = TRUE;

			Invalidate();
		}
		else if (m_SBETViewer) {
			m_SBETSpeedStep--;
			m_SBETSpeedStep = max(m_SBETSpeedStep, 0);
		}
		else if (m_QAModelScan) {
			m_ScanPositionIndex--;
			if (m_ScanPositionIndex <= 0)
				m_ScanPositionIndex = 0;

			Invalidate();
		}
		else {
			if (m_MotionController == FLYBALL) {
				m_FlyballOffset -= 1.0;
				//m_Camera.vp.z -= 0.01;

				//m_Camera.pr.z -= 0.01;

				//m_Camera.vd.x = m_Camera.pr.x - m_Camera.vp.x;
				//m_Camera.vd.y = m_Camera.pr.y - m_Camera.vp.y;
				//m_Camera.vd.z = m_Camera.pr.z - m_Camera.vp.z;

//				Invalidate();
			}
		}

		break;
	case VK_ADD:
		if (::GetKeyState(VK_CONTROL) & 0x80) {
			if (m_PointSize < 16.0) {
				m_PointSize = m_PointSize + 1.0f;

				// send message to other instances to synchronize trackballs
				if (((CFullwindowApp*)AfxGetApp())->oInfo.IsSynchronize()) {
					SendSync();
				}

				Invalidate();
			}
		}
		else if (::GetKeyState(VK_SHIFT) & 0x80) {
			double step = 1.0;
			m_Camera.focallength += step * 10.0;
			m_Camera.focallength = min(m_Camera.focallength, 1000.0);
			m_Camera.focallength = max(m_Camera.focallength, 20.0);

			m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
			m_Camera.eyesep = m_Camera.focallength / 1300.0;

			m_InZoom = TRUE;

			Invalidate();
		}
		else if (m_SBETViewer) {
			m_SBETSpeedStep++;
			m_SBETSpeedStep = min(m_SBETSpeedStep, 500);
		}
		else if (m_QAModelScan) {
			m_ScanPositionIndex++;
			if (m_ScanPositionIndex >= 35)
				m_ScanPositionIndex = 35;

			Invalidate();
		}
		else {
			if (m_MotionController == FLYBALL) {
				m_FlyballOffset += 1.0;
				//m_Camera.vp.z += 0.01;
				//m_Camera.pr.z += 0.01;

				//m_Camera.vd.x = m_Camera.pr.x - m_Camera.vp.x;
				//m_Camera.vd.y = m_Camera.pr.y - m_Camera.vp.y;
				//m_Camera.vd.z = m_Camera.pr.z - m_Camera.vp.z;

//				Invalidate();
			}
		}

		break;
	case 'z':
	case 'Z':
		if (m_QAModelScan) {
			if (::GetKeyState(VK_SHIFT) & 0x80) {
				m_ScanOffsetZ += 0.05;

				if (m_ScanOffsetZ >= 0.5)
					m_ScanOffsetZ = 0.5;
			}
			else {
				m_ScanOffsetZ -= 0.05;

				if (m_ScanOffsetZ <= -0.5)
					m_ScanOffsetZ = -0.5;
			}

			Invalidate();
		}
		break;

	case VK_F10:
#ifdef _DEBUG
		oldMemState.Checkpoint();
#endif

		break;

	case VK_F9:
	{
		// clear existing data objects
		DeleteDataObject();

		// get SBET file name
		CString csFilter;
		CString csTextFilter;
		CString csDTMFilter;
		CString csExt;

		csFilter.LoadString(IDS_ALLFILES);
		csTextFilter.LoadString(IDS_TEXTFILES);
		csDTMFilter.LoadString(IDS_DTMFILES);
		csExt = _T("");

		CFileDialog fd(TRUE, csExt, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, csTextFilter, (CWnd*)this);

		fd.m_ofn.lpstrTitle = "SBET file";
		if (fd.DoModal() == IDOK) {
			CString DTMFileName;
			CString DataFileName = fd.GetPathName();

			// prompt for DTM
			CFileDialog dfd(TRUE, csExt, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, csDTMFilter, (CWnd*)this);

			dfd.m_ofn.lpstrTitle = "DTM file";
			DTMFileName.Empty();
			if (dfd.DoModal() == IDOK) {
				DTMFileName = dfd.GetPathName();

				m_SBETViewer = TRUE;

				BeginWaitCursor();

				// read entire file to get extent and number of records
				m_SBETDataFile.Open(DataFileName);
				m_SBETRecordCount = 0;

				m_SBETMinLat = m_SBETMinLong = 999999999.9;
				m_SBETMaxLat = m_SBETMaxLong = m_SBETMaxEllHt = -999999999.9;
				m_SBETMinEllHt = 0.0;
				//CString TempFileName = DataFileName + _T(".new");
				//FILE* f = fopen(TempFileName, "wt");
				while (m_SBETDataFile.ReadDataLine(m_SBETLineBuffer, SKIPCOMMENTS)) {
					// parse
//					sscanf(m_SBETLineBuffer, "%lf %lf %lf %lf", &m_GPSTime, &m_Lat, &m_Long, &m_EllHt);
					sscanf(m_SBETLineBuffer, "%lf %lf %lf %lf %lf %lf %lf", &m_GPSTime, &m_Lat, &m_Long, &m_EllHt, &m_Roll, &m_Pitch, &m_TrueHeading);
					//					fprintf(f, "%lf %lf %lf %lf %lf %lf %lf\n", m_GPSTime, m_Lat, -m_Long, m_EllHt, m_Roll, m_Pitch, m_TrueHeading);					
					//					m_Lat *= 20000.0;
					//					m_Long *= -20000.0;

										// check extent
					m_SBETMinLat = min(m_SBETMinLat, m_Lat);
					m_SBETMaxLat = max(m_SBETMaxLat, m_Lat);
					m_SBETMinLong = min(m_SBETMinLong, m_Long);
					m_SBETMaxLong = max(m_SBETMaxLong, m_Long);
					//					m_SBETMinEllHt = min(m_SBETMinEllHt, m_EllHt);
					m_SBETMaxEllHt = max(m_SBETMaxEllHt, m_EllHt);

					// count records
					m_SBETRecordCount++;
				}
				//fclose(f);
				m_SBETRecords = m_SBETRecordCount;

				// rewind file
				m_SBETDataFile.Rewind();
				m_SBETRecordCount = 0;

				// figure out scaling
				double xspan = m_SBETMaxLong - m_SBETMinLong;
				double yspan = m_SBETMaxLat - m_SBETMinLat;
				double zspan = m_SBETMaxEllHt - m_SBETMinEllHt;

				// get max dimension
				double maxspan = max(xspan, yspan);
				maxspan = max(maxspan, zspan);

				// adjust scaling
				m_SBETMinLong -= (maxspan - xspan) / 2.0;
				m_SBETMaxLong += (maxspan - xspan) / 2.0;
				m_SBETMinLat -= (maxspan - yspan) / 2.0;
				m_SBETMaxLat += (maxspan - yspan) / 2.0;
				m_SBETMinEllHt -= (maxspan - zspan) / 2.0;
				m_SBETMaxEllHt += (maxspan - zspan) / 2.0;

				xspan = m_SBETMaxLong - m_SBETMinLong;
				yspan = m_SBETMaxLat - m_SBETMinLat;
				zspan = m_SBETMaxEllHt - m_SBETMinEllHt;

				BeginGLCommands();

				// load object model
				CFileSpec objfile(CFileSpec::FS_APPDIR);
				objfile.SetFileNameEx("cessna.3ds");
				CreateModel(objfile.GetFullSpec());
				m_DataType = MODEL;

				if (!DTMFileName.IsEmpty()) {
					CreateSurface(DTMFileName);
					m_DataType |= SURFACE;
				}

				// create point cloud
				CreatePointCloud(DataFileName, ASCII_SBET);
				m_DataType |= POINTDATA;

				EndGLCommands();

				m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- Experimental SBET file playback", PROGRAM_VERSION);

				EndWaitCursor();
			}
		}
	}
	break;
	case VK_F5:
		// start/stop QA scanning
		if (m_QAModelScan)
			m_QAModelScan = FALSE;
		else {
			m_QAModelScan = TRUE;
			m_TrackBall.ResetOrientation(0.0);
			Invalidate();
		}

		break;
	case 'b':
	case 'B':
		m_BackgroundColor = RGB(0, 0, 0);
		Invalidate();
		break;
	case 'i':
	case 'I':
		//	case 'a':
		//	case 'A':
		if (m_DrawAxes)
			m_DrawAxes = FALSE;
		else
			m_DrawAxes = TRUE;
		Invalidate();
		break;
	case 'c':
	case 'C':
		m_ColorMode = COLORBYRGB;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'f':
	case 'F':
		m_ColorMode = COLORBYNIR;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'h':
	case 'H':
		m_ColorMode = COLORBYHEIGHT;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'j':
	case 'J':
		m_ColorMode = COLORBYRETURN;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'l':
	case 'L':
		m_ColorMode = COLORBYCLASSIFICATION;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			OpenClassificationCodeDlg();

			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'n':		// color by intensity
	case 'N':
		m_ColorMode = COLORBYINTENSITY;

		// re-open the point file
		if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
			BeginGLCommands();
			CreatePointCloud(m_PointFileName, m_PointFileFormat);
			EndGLCommands();
		}

		break;
	case 'o':
	case 'O':
		ResetCamera();

		// reset orientation to overhead
		m_TrackBall.ResetOrientation(0.0);
		Invalidate();

		break;
	case 'p':
	case 'P':
		if (m_DrawPointData)
			m_DrawPointData = FALSE;
		else
			m_DrawPointData = TRUE;
		Invalidate();
		break;
	case 'q':
	case 'Q':
		// toggle low/high resolution terrain surface
//		if (m_SBETViewer) {
		if (m_DisplayLowResSurface)
			m_DisplayLowResSurface = FALSE;
		else
			m_DisplayLowResSurface = TRUE;
		//		}
		break;
	case 'r':
	case 'R':
		// AVI file recording
		if (m_RecordFramesToAVI) {
			// close current AVI file
			m_AVIFile.Close();

			m_RecordFramesToAVI = FALSE;
		}
		else {
			// prompt for AVI filename
			CString csFilter;
			CString csExt;

			csFilter.LoadString(IDS_AVIFILEFILTER);
			csExt.LoadString(IDS_AVIFILEEXT);

			CFileDialog fd(FALSE, csExt, NULL, OFN_OVERWRITEPROMPT, csFilter, (CWnd*)this);

			if (fd.DoModal() == IDOK) {
				m_AVIFileName = fd.GetPathName();
				m_AVIFile.Create(m_AVIFileName, m_ClientRect.Width(), m_ClientRect.Height());

				// set flag to record frames
				m_RecordFramesToAVI = TRUE;

				// trigger a redraw to start recording
				Invalidate();
			}
		}
		break;
	case 'v':
	case 'V':
		if (m_MotionController == TRACKBALL) {
			// turn off motion updating...not used for flyball interface
			m_DoContinuousDrawing = FALSE;

			m_MotionController = FLYBALL;

			ResetCamera();

			// set the focal length so we are zoomed out as far as we can go
			m_Camera.focallength = 10.0;

			m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
			m_Camera.eyesep = m_Camera.focallength / 1300.0;

			m_InZoom = TRUE;
		}
		else {
			m_MotionController = TRACKBALL;

			// reset orientation to overhead
			m_TrackBall.ResetOrientation(0.0);

			ResetCamera();
		}

		Invalidate();

		break;
	case 'w':
	case 'W':
		m_BackgroundColor = RGB(255, 255, 255);
		Invalidate();
		break;
	default:
		COpenGLWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}

	if (m_ColorMode != COLORBYCLASSIFICATION) {
		if (m_ClassificationCodeDlg)
			m_ClassificationCodeDlg->CloseDialog();
	}
}

void CChildView::DrawHeadsUpInfo(int UpdateInterval)
{
	// don't show anything if we are saving frames
	if (m_RecordFramesToAVI || !m_ShowHeadsUpInfo)
		return;

	COpenGLWnd::DrawHeadsUpInfo(UpdateInterval);

	CString csTemp;
	CString csStatus;
	if (m_DataType & SURFACE && !(m_DataType & POINTDATA)) {
		if (m_InMotion) {
			csTemp = m_LowResolutionMessage;
		}
		else {
			csTemp = m_HighResolutionMessage;
		}
	}
	else {
		for (int i = 0; i < DATALAYERS; i ++) {
			if (i < m_LayersDrawnInLastFrame)
				csStatus += "+";
			else
				csStatus += "=";
		}
		csTemp.Format("Points: %i (drawn: %i)  [%s]", m_PointCount, m_DrawnPointCount, csStatus);
	}

	//MEMORYSTATUS stat;
	//GlobalMemoryStatus(&stat);
	//csStatus.Format(" %10ld bytes free virtual memory", stat.dwAvailVirtual);
	//csTemp += csStatus;

	// get width and height of string
	SIZE sz;
	GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csTemp, csTemp.GetLength(), &sz);

	// draw background
	glColor4d(0.7, 0.7, 0.7, 0.25);
	glBegin(GL_QUADS);
	glVertex2d(0.0, (double) m_ClientRect.top);
	glVertex2d((double) m_ClientRect.right, (double) m_ClientRect.top);
	glVertex2d((double) m_ClientRect.right, (double) m_ClientRect.top + (double) sz.cy + 2.0);
	glVertex2d(0.0, (double) m_ClientRect.top + (double) sz.cy + 2.0);
	glEnd();

	if (m_DataType & PANORAMA) {
		glColor4d(1.0, 1.0, 0.0, 0.25);
		glBegin(GL_LINES);
		glVertex2d((double) m_ClientRect.right * 0.5, (double) m_ClientRect.bottom);
		glVertex2d((double) m_ClientRect.right * 0.5, (double) m_ClientRect.top);

		glVertex2d(0.0, (double) (m_ClientRect.bottom - m_ClientRect.top) * 0.5);
		glVertex2d((double) m_ClientRect.right, (double) (m_ClientRect.bottom - m_ClientRect.top) * 0.5);
		glEnd();
	}
	
	glColor4d(1.0, 0.0, 0.0, 1.0);		// red
//	glColor4d(1.0, 1.0, 0.0, 1.0);		// yellow
	glRasterPos2d((double) 0.0, 3.0);
	PrintString(csTemp);

	csTemp.Format("%iw by %ih", m_ClientRect.right, m_ClientRect.bottom);

	if (((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize()) {
		csTemp.Format("%iw by %ih -- Master", m_ClientRect.right, m_ClientRect.bottom);
	}
	else {
		csTemp.Format("%iw by %ih -- Slave ", m_ClientRect.right, m_ClientRect.bottom);
	}

	GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csTemp, csTemp.GetLength(), &sz);
	glRasterPos2d((double) (m_ClientRect.right - sz.cx), 3.0);
	PrintString(csTemp);

	if (m_DataType == POINTDATA) {
		if (m_ColorMode == COLORBYCLASSIFICATION) {
			csTemp.Format("Color by LAS classification");
		}
		else if (m_ColorMode == COLORBYRETURN) {
			csTemp.Format("Color by Return");
		}
		else if (m_ColorMode == COLORBYINTENSITY) {
			csTemp.Format("Color by Intensity");
		}
		else if (m_ColorMode == COLORBYHEIGHT) {
			csTemp.Format("Color by Height");
		}
		else if (m_ColorMode == COLORBYRGB) {
			csTemp.Format("Color using point RGB");
		}
		else if (m_ColorMode == COLORBYNIR) {
			csTemp.Format("Color using point NIR-RG");
		}
		else {
			csTemp.Empty();
		}

		GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csTemp, csTemp.GetLength(), &sz);
		glRasterPos2d((double)(m_ClientRect.right / 2.0 - sz.cx / 2.0), 3.0);
		PrintString(csTemp);
	}

	if (m_QAModelScan) {
		csTemp.Format("DTM scanning mode for quality assessment: +/- move surface, Z/z raise/lower surface");
		GetTextExtentPoint32(m_pCDC->GetSafeHdc(), csTemp, csTemp.GetLength(), &sz);
		glRasterPos2d((double) (m_ClientRect.right / 2.0 - sz.cx / 2.0), 3.0);
		PrintString(csTemp);
	}

	if (m_SBETViewer) {
		csTemp.Format("Roll: %6.2lf     Pitch: %6.2lf     True Heading: %6.2lf", m_Roll, m_Pitch, m_TrueHeading);
		glRasterPos2d((double) 0.0, 3.0 + sz.cy);
		PrintString(csTemp);
		csTemp.Format("Height above ellipsoid: %.2lf", m_EllHt);
		glRasterPos2d((double) 0.0, 3.0 + (sz.cy * 2));
		PrintString(csTemp);
		csTemp.Format("SBET record %i of %i at %.1lf", m_SBETRecordCount, m_SBETRecords, m_GPSTime);
		glRasterPos2d((double) 0.0, 3.0 + (sz.cy * 4));
		PrintString(csTemp);
		glRasterPos2d((double) 0.0, 3.0 + (sz.cy * 6));
		if (!m_DisplayLowResSurface)
			PrintString("Terrain model quality: HIGH (Q to toggle)");
		else
			PrintString("Terrain model quality: LOW (Q to toggle)");
	}
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	double step = -((double) zDelta / 120.0);
	m_Camera.focallength += step * 10.0;
	m_Camera.focallength = min(m_Camera.focallength, 1500.0);
	m_Camera.focallength = max(m_Camera.focallength, 10.0);

	m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;
	m_Camera.eyesep = m_Camera.focallength / 1300.0;

	m_InZoom = TRUE;

	Invalidate();

	// send message to other instances to synchronize trackballs
//	if (((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize()) {
//		SendSync();
//	}

	return(0);
}

void CChildView::ReadPointCloud(LPCTSTR FileName)
{
	CString PointFileName = _T(FileName);
	if (PointFileName.IsEmpty())
		PointFileName = _T("tempdat.xyz");

	BOOL HaveBinaryData = FALSE;

	// see if data file is binary
	FILE* f = fopen(PointFileName, "rb");
	char line[1000];
	int DatCount = 1;
	long dataoffset;
	float version;
	int ptcnt = 0;
	if (f) {
		if (fread(line, sizeof(char), 4, f) == 4) {
			line[4] = '\0';
			if (strcmp(line, "LDVD") == 0) {
				// read version and number of points
				fread(&version, sizeof(float), 1, f);
				fread(&ptcnt, sizeof(int), 1, f);

				dataoffset = ftell(f);

				// set flag
				HaveBinaryData = TRUE;
			}
		}
		fclose(f);
	}
	else {
		AfxMessageBox("Could not open data file");
		return;
	}

	if (HaveBinaryData)
		f = fopen(PointFileName, "rb");
	else
		f = fopen(PointFileName, "rt");

	// seek past header if reading binary data
	if (HaveBinaryData) {
		fseek(f, dataoffset, SEEK_SET);
	}

	double x, y, z;
	COLORREF color;
	float attribute;
	int r, g, b;
	double minx = 999999999.9;
	double miny = 999999999.9;
	double minz = 999999999.9;
	double maxx = -999999999.9;
	double maxy = -999999999.9;
	double maxz = -999999999.9;
	int i;
	if (f) {
		if (HaveBinaryData) {
			for (i = 0; i < ptcnt; i ++) {
				// read binary data points
				fread(&x, sizeof(double), 1, f);
				fread(&y, sizeof(double), 1, f);
				fread(&z, sizeof(double), 1, f);
				fread(&color, sizeof(COLORREF), 1, f);
				fread(&attribute, sizeof(float), 1, f);

				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
				if (z < minz)
					minz = z;
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (z > maxz)
					maxz = z;
			}
		}
		else {
			while (fscanf(f, "%lf %lf %lf %i %i %i\n", &x, &y, &z, &r, &g, &b) == 6) {
				ptcnt ++;
				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
				if (z < minz)
					minz = z;
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (z > maxz)
					maxz = z;
			}
		}

		// figure out scaling
		double xspan = maxx - minx;
		double yspan = maxy - miny;
		double zspan = maxz - minz;

		// get max dimension
		double maxspan = max(xspan, yspan);
		maxspan = max(maxspan, zspan);

		// adjust scaling
		minx -= (maxspan - xspan) / 2.0;
		maxx += (maxspan - xspan) / 2.0;
		miny -= (maxspan - yspan) / 2.0;
		maxy += (maxspan - yspan) / 2.0;
		minz -= (maxspan - zspan) / 2.0;
		maxz += (maxspan - zspan) / 2.0;

		// rewind file
		rewind(f);

		// seek past header if reading binary data
		if (HaveBinaryData) {
			fseek(f, dataoffset, SEEK_SET);
		}

		// allocate memory for point data
		m_PointList = new COLOREDPOINT[ptcnt];

		// read data
		for (i = 0; i < ptcnt; i ++) {
			if (HaveBinaryData) {
				// read binary data points
				fread(&m_PointList[i].x, sizeof(double), 1, f);
				fread(&m_PointList[i].y, sizeof(double), 1, f);
				fread(&m_PointList[i].z, sizeof(double), 1, f);
				fread(&m_PointList[i].color, sizeof(COLORREF), 1, f);
				fread(&attribute, sizeof(float), 1, f);
			}
			else {
				fscanf(f, "%lf %lf %lf %i %i %i\n", &m_PointList[i].x, &m_PointList[i].y, &m_PointList[i].z, &r, &g, &b);
				m_PointList[i].color = RGB(r, g, b);
			}
			m_PointList[i].x = (m_PointList[i].x - minx) / maxspan;
			m_PointList[i].y = (m_PointList[i].y - miny) / maxspan;
			m_PointList[i].z = (m_PointList[i].z - minz) / maxspan;
		}

		fclose(f);
	}
	else
		AfxMessageBox("Could not open data file");

	m_PointCount = ptcnt;
}

void CChildView::DrawPointList()
{
	if (m_PointList && m_PointCount > 0) {
		COLORREF lastcolor = 0xFFFFFFFF;
		int i;

		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPointSize(m_PointSize);
		
		glTranslated(-0.5, -0.5, -0.5);

		glDisable(GL_LIGHTING);
		lastcolor = 0xFFFFFFFF;
		glBegin(GL_POINTS);
			for (i = 0; i < m_PointCount; i ++) {
				if (m_PointList[i].color != lastcolor) {
					SetAnaglyphSensitiveColor(m_PointList[i].color);
					lastcolor = m_PointList[i].color;
				}
				glVertex3d(m_PointList[i].x, m_PointList[i].y, m_PointList[i].z);
			}
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void CChildView::DeleteDataObject()
{
	// delete the existing display objects
	if (m_PointList) {
		delete [] m_PointList;
		m_PointList = NULL;
	}

	if (m_ImagePlate.IsValid())
		m_ImagePlate.DeleteList();

	if (m_SurfaceLayer.IsValid())
		m_SurfaceLayer.DeleteList();

	if (m_LORESSurfaceLayer.IsValid())
		m_LORESSurfaceLayer.DeleteList();

	if (m_PointCount) {
		if (m_PointLayer[0].IsValid()) {
			for (int layer = 0; layer < DATALAYERS; layer ++)
				m_PointLayer[layer].DeleteList();
		}
	}
	m_PointCount = 0;
	m_DrawnPointCount = 0;

	if (m_ObjectModel.IsValid()) {
		m_ObjectModel.Destroy();
	}

	if (m_MarkerTexture.IsValid()) {
		glDeleteTextures(1, &m_MarkerTextureName);
		m_MarkerTexture.Empty();
	}

	if (m_SBETDataFile.IsValid())
		m_SBETDataFile.Close();

	m_SBETViewer = FALSE;
	m_DisplayLowResSurface = FALSE;

	// close DTM for flyball movement
	if (m_HaveTerrainForFlyball) {
		m_TerrainForFlyball.Destroy();
		m_HaveTerrainForFlyball = FALSE;
	}

	// delete rangepoles
	if (m_RangePoleRedStripes.IsValid())
		m_RangePoleRedStripes.DeleteList();
	if (m_RangePoleWhiteStripes.IsValid())
		m_RangePoleWhiteStripes.DeleteList();
}

void CChildView::CreateDataObject(LPCTSTR FileName)
{
	// delete existing objects
	DeleteDataObject();

	// check format
	if (FileName[0] != '\0') {
		CFileSpec fs(FileName);
		if (fs.Extension().CompareNoCase(".3ds") == 0) {
			CreateModel(FileName);
			m_DataType = MODEL;
		}
		else if (fs.Extension().CompareNoCase(".dtm") == 0) {
			CreateSurface(FileName);
			m_DataType = SURFACE;
		}
		else if (fs.Extension().CompareNoCase(".jpg") == 0 || fs.Extension().CompareNoCase(".tif") == 0) {
			CreateCylinder(FileName, 1);
			m_DataType = PANORAMA;
			ResetCamera();
		}
		else if (fs.Extension().CompareNoCase(".asc") == 0) {
			CreateImagePlateAsInterleavedPointLayers(FileName);
//			CreateImagePlateAsPointLayers(FileName);
//			CreateImagePlate(FileName);
			m_DataType = IMAGEPLATE;
//			m_DataType = POINTDATA;
		}
		else {		// assume point cloud
			CreatePointCloud(FileName);
			m_DataType = POINTDATA;
		}
	}
	
	if (m_DataType != POINTDATA) {
		m_ColorMode = COLORBYHEIGHT;

		if (m_ClassificationCodeDlg)
			m_ClassificationCodeDlg->CloseDialog();
	}
	//#ifdef _DEBUG
//    newMemState.Checkpoint();
//	oldMemState.DumpAllObjectsSince();
//#endif
}

void CChildView::CreateImagePlate(LPCTSTR FileName)
{
	CFileSpec fs(CFileSpec::FS_APPDIR);
	CString GRIDFileName = _T(FileName);
	if (!GRIDFileName.IsEmpty()) {
		GRIDFileName.Remove(34);
		fs.SetFullSpec(GRIDFileName);
	}

	if (!fs.Exists()) {
		// can't open file...display message and simply return...current data will be intact
		CString prompt;
		prompt.Format("Could not open ASCII raster file: \n%s\n", fs.GetFullSpec());
		AfxMessageBox(prompt);

		return;
	}

	m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- %s", PROGRAM_VERSION, fs.GetFileNameEx());

	BeginWaitCursor();

	// read the grid file header
	CASCIIGridFile agrid(fs.GetFullSpec());
	if (agrid.IsValid()) {
		int i, j;
		double MinGridValue = DBL_MAX;
		double MaxGridValue = -DBL_MAX;
		double GridRange, GridMidpoint;

		// read data values to get range
		for (j = 0; j < agrid.m_Rows; j ++) {
			if (agrid.LoadNextRow()) {
				for (i = 0; i < agrid.m_Columns; i ++) {
					if (agrid.m_RowValues[i] != agrid.m_NODATA) {
						MinGridValue = min(MinGridValue, (double) agrid.m_RowValues[i]);
						MaxGridValue = max(MaxGridValue, (double) agrid.m_RowValues[i]);
					}
				}
			}
		}
		GridRange = MaxGridValue - MinGridValue;
		GridMidpoint = (MaxGridValue + MinGridValue) / 2.0;

		// we need to scale the image plate to a 1.0 by 1.0 space with the Z set to 0.0
		double cellwidth, cellheight;
		double celloriginx, celloriginy;
		if (agrid.m_Rows > agrid.m_Columns) {
			cellheight = 1.0 / (double) agrid.m_Rows;
			cellwidth = cellheight;

			celloriginx = ((double) (agrid.m_Rows - agrid.m_Columns) / 2.0) * cellwidth;
			celloriginy = 0.0;
		}
		else {
			cellwidth = 1.0 / (double) agrid.m_Columns;
			cellheight = cellwidth;

			celloriginx = 0.0;
			celloriginy = ((double) (agrid.m_Columns - agrid.m_Rows) / 2.0) * cellheight;
		}

		// rewind file and read header to get back to start of data
		agrid.Rewind();
		agrid.ReadHeader();

		double ptx, pty;
		COLORREF rgb;

		// set up the color ramp
		CColorRamp Ramp;
//		Ramp.CreateRamp(HSVColor, 255, 0, 0, 0, 0, 255);
		Ramp.CreateRamp(RandomColor, 255, 0, 0, 0, 0, 255);
		Ramp.SetEndpoints(MinGridValue, MaxGridValue);

		// start display list..."image" will be rendered as a set of points with each point centered on the grid cell
		//
		// it would be possible to create the grid points using the same layering logic as a point cloud but my ASCII grid class
		// can only read data sequentially so the only option would be to have each point layer be a sequential group of rows from the grid.
		// This would work but you would only see a part of the data when you rotate the data. A better approach would be to assign a 
		// random set of grid points to each layer so you still see the entire extent of the data when you rotate. However, this would
		// require random access to the ASCII grid or reading in the entire grid and then building the point layers.
		//
		// There could be some additional option to use the grid values to provide the Z for each point. This would allow viewing a surface
		// in raster format that might be too large to render as a shaded surface.
		m_ImagePlate.StartDef();
		glBegin(GL_POINTS);
			pty = celloriginy + (double) agrid.m_Rows * cellheight - cellheight / 2.0;
			for (j = 0; j < agrid.m_Rows; j ++) {
				if (agrid.LoadNextRow()) {
					ptx = celloriginx + cellwidth / 2.0;
					for (i = 0; i < agrid.m_Columns; i ++) {
						if (agrid.m_RowValues[i] != agrid.m_NODATA) {
							// set the point color
							rgb = Ramp.GetColorForValue((double) agrid.m_RowValues[i]);
							glColor3ub(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));

							glVertex3d(ptx, pty, 0.0);
//							glVertex3d(ptx, pty, ((double) agrid.m_RowValues[i] - GridMidpoint) / (5.0 * GridRange));
						}

						ptx += cellwidth;
					}
				}
				pty -= cellheight;
			}
		glEnd();

		m_ImagePlate.EndDef();

		m_PointCount = agrid.m_Columns * agrid.m_Rows;
		m_DrawnPointCount = m_PointCount;
	}

	EndWaitCursor();
}

void CChildView::CreateImagePlateAsPointLayers(LPCTSTR FileName)
{
	// 28 colors from blue to yellow to red
	COLORREF colors[] = {0xFF8000, 0xE28E1C, 0xC69C38, 0xAAAA55, 0x8DB871, 0x71C68D, 0x55D4AA, 0x38E2C6, 0x1CF0E2, 0x00FFFF, 0x00F0FF, 0x00E2FF, 0x00D4FF, 0x00C6FF, 
						 0x00B8FF, 0x00AAFF, 0x009CFF, 0x008EFF, 0x0080FF, 0x0071FF, 0x0063FF, 0x0055FF, 0x0047FF, 0x0038FF, 0x002AFF, 0x001CFF, 0x000EFF, 0x0000FF};

	CFileSpec fs(CFileSpec::FS_APPDIR);
	CString GRIDFileName = _T(FileName);
	if (!GRIDFileName.IsEmpty()) {
		GRIDFileName.Remove(34);
		fs.SetFullSpec(GRIDFileName);
	}

	if (!fs.Exists()) {
		// can't open file...display message and simply return...current data will be intact
		CString prompt;
		prompt.Format("Could not open ASCII raster file: \n%s\n", fs.GetFullSpec());
		AfxMessageBox(prompt);

		return;
	}

	m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- %s", PROGRAM_VERSION, fs.GetFileNameEx());

	BeginWaitCursor();

	// have memory...delete old display lists
	if (m_PointCount) {
		for (int layer = 0; layer < DATALAYERS; layer ++)
			m_PointLayer[layer].DeleteList();
	}

	// read the grid file header
	CASCIIGridFile agrid(fs.GetFullSpec());
	if (agrid.IsValid()) {
		int i, j, k;
		double MinGridValue = DBL_MAX;
		double MaxGridValue = -DBL_MAX;
		double GridRange, GridMidpoint;

		// read data values to get range
		for (j = 0; j < agrid.m_Rows; j ++) {
			if (agrid.LoadNextRow()) {
				for (i = 0; i < agrid.m_Columns; i ++) {
					if (agrid.m_RowValues[i] != agrid.m_NODATA) {
						MinGridValue = min(MinGridValue, (double) agrid.m_RowValues[i]);
						MaxGridValue = max(MaxGridValue, (double) agrid.m_RowValues[i]);
					}
				}
			}
		}
		GridRange = MaxGridValue - MinGridValue;
		GridMidpoint = (MaxGridValue + MinGridValue) / 2.0;

		// we need to scale the image plate to a 1.0 by 1.0 space with the Z set to 0.0
		double cellwidth, cellheight;
		double celloriginx, celloriginy;
		if (agrid.m_Rows > agrid.m_Columns) {
			cellheight = 1.0 / (double) agrid.m_Rows;
			cellwidth = cellheight;

			celloriginx = ((double) (agrid.m_Rows - agrid.m_Columns) / 2.0) * cellwidth;
			celloriginy = 0.0;
		}
		else {
			cellwidth = 1.0 / (double) agrid.m_Columns;
			cellheight = cellwidth;

			celloriginx = 0.0;
			celloriginy = ((double) (agrid.m_Columns - agrid.m_Rows) / 2.0) * cellheight;
		}

		// rewind file and read header to get back to start of data
		agrid.Rewind();
		agrid.ReadHeader();

		double ptx, pty;
		long ptcnt = 0;

		// set up the color ramp
		CColorRamp Ramp;
//		Ramp.CreateRamp(HSVColor, 255, 0, 0, 0, 0, 255);
		Ramp.CreateRamp(RandomColor, 255, 0, 0, 0, 0, 255);
		Ramp.SetEndpoints(MinGridValue, MaxGridValue);

		// start display list..."image" will be rendered as a set of points with each point centered on the grid cell
		// using point layers so we can fit more points
		int RowStep = agrid.m_Rows / DATALAYERS + 1;
		int layer = 0;

		pty = celloriginy + (double) agrid.m_Rows * cellheight - cellheight / 2.0;
		for (k = 0; k < agrid.m_Rows; k += RowStep) {
			m_PointLayer[layer].StartDef();
			glBegin(GL_POINTS);
				for (j = k; j < k + RowStep; j ++) {
					if (j > (agrid.m_Rows - 1))
						break;

					if (agrid.LoadNextRow()) {
						ptx = celloriginx + cellwidth / 2.0;
						for (i = 0; i < agrid.m_Columns; i ++) {
							if (agrid.m_RowValues[i] != agrid.m_NODATA) {
								// set the point color
								SETGLCOLOR(colors[(int) (((double) agrid.m_RowValues[i] - MinGridValue) / ((MaxGridValue - MinGridValue) + 1.0) * 28.0)]);
//								rgb = Ramp.GetColorForValue((double) agrid.m_RowValues[i]);
//								glColor3ub(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));

								glVertex3d(ptx, pty, 0.5);
//								glVertex3d(ptx, pty, ((double) agrid.m_RowValues[i] - GridMidpoint) / (5.0 * GridRange) + 0.5);

								ptcnt ++;
							}

							ptx += cellwidth;
						}
					}
					pty -= cellheight;
				}
			glEnd();

			m_PointLayer[layer].EndDef();
			layer ++;
		}

		m_PointCount = ptcnt;
		m_DrawnPointCount = m_PointCount;
	}

	EndWaitCursor();
}

void CChildView::CreateImagePlateAsInterleavedPointLayers(LPCTSTR FileName)
{
	// 28 colors from blue to yellow to red
	COLORREF colors[] = {0xFF8000, 0xE28E1C, 0xC69C38, 0xAAAA55, 0x8DB871, 0x71C68D, 0x55D4AA, 0x38E2C6, 0x1CF0E2, 0x00FFFF, 0x00F0FF, 0x00E2FF, 0x00D4FF, 0x00C6FF, 
						 0x00B8FF, 0x00AAFF, 0x009CFF, 0x008EFF, 0x0080FF, 0x0071FF, 0x0063FF, 0x0055FF, 0x0047FF, 0x0038FF, 0x002AFF, 0x001CFF, 0x000EFF, 0x0000FF};
	COLORREF NODATA_color = 0x000000;		// black

	CFileSpec fs(CFileSpec::FS_APPDIR);
	CString GRIDFileName = _T(FileName);
	if (!GRIDFileName.IsEmpty()) {
		GRIDFileName.Remove(34);
		fs.SetFullSpec(GRIDFileName);
	}

	if (!fs.Exists()) {
		// can't open file...display message and simply return...current data will be intact
		CString prompt;
		prompt.Format("Could not open ASCII raster file: \n%s\n", fs.GetFullSpec());
		AfxMessageBox(prompt);

		return;
	}

	m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- %s", PROGRAM_VERSION, fs.GetFileNameEx());

	BeginWaitCursor();

	// have memory...delete old display lists
	if (m_PointCount) {
		for (int layer = 0; layer < DATALAYERS; layer ++)
			m_PointLayer[layer].DeleteList();
	}

	// basic idea is to read all the cell values and then create a set of point display lists. the problem is that the rendering order matters when
	// rendering point clouds. ideally you would sort the points from back to front before rendering but this adds lots of time. with interleaved sets of points
	// the amount of "bleed" from the surround for each point rendered against a blank screen onto points rendered later is minimized.

	// read the grid file header
	CASCIIGridFile agrid(fs.GetFullSpec());
	if (agrid.IsValid()) {
		int i, j;
		double* cell_val;
		double MinGridValue = DBL_MAX;
		double MaxGridValue = -DBL_MAX;
		double GridRange, GridMidpoint;
		long ptcnt = agrid.m_Rows * agrid.m_Columns;

		// allocate memory for the values...we should be able to compute XY on the fly and do the color math on the fly
		cell_val = new double[ptcnt];
		if (cell_val) {
			// read data values into memory and get the range
			for (j = 0; j < agrid.m_Rows; j ++) {
				if (agrid.LoadNextRow()) {
					for (i = 0; i < agrid.m_Columns; i ++) {
						cell_val[j * agrid.m_Columns + i] = (double) agrid.m_RowValues[i];

						if (agrid.m_RowValues[i] != agrid.m_NODATA) {
							MinGridValue = min(MinGridValue, (double) agrid.m_RowValues[i]);
							MaxGridValue = max(MaxGridValue, (double) agrid.m_RowValues[i]);
						}
					}
				}
			}
			GridRange = MaxGridValue - MinGridValue;
			GridMidpoint = (MaxGridValue + MinGridValue) / 2.0;

			// we need to scale the image plate to a 1.0 by 1.0 space with the Z set to 0.0
			double cellwidth, cellheight;
			double celloriginx, celloriginy;
			if (agrid.m_Rows > agrid.m_Columns) {
				cellheight = 1.0 / (double) agrid.m_Rows;
				cellwidth = cellheight;

				celloriginx = ((double) (agrid.m_Rows - agrid.m_Columns) / 2.0) * cellwidth;
				celloriginy = 0.0;
			}
			else {
				cellwidth = 1.0 / (double) agrid.m_Columns;
				cellheight = cellwidth;

				celloriginx = 0.0;
				celloriginy = ((double) (agrid.m_Columns - agrid.m_Rows) / 2.0) * cellheight;
			}

			double ptx, pty;

			for (int layer = 0; layer < DATALAYERS; layer ++) {
				m_PointLayer[layer].StartDef();

				glBegin(GL_POINTS);
					for (i = layer; i < ptcnt; i += DATALAYERS) {
						ptx = celloriginx + (double) (i % agrid.m_Columns) * cellwidth  + cellwidth / 2.0;
						pty = (celloriginy + ((double) agrid.m_Rows * cellheight)) - (double) (i / agrid.m_Columns) * cellheight - cellheight / 2.0;

						if (cell_val[i] != agrid.m_NODATA)
							SETGLCOLOR(colors[(int) ((cell_val[i] - MinGridValue) / ((MaxGridValue - MinGridValue) + 1.0) * 28.0)]);
						else
							SETGLCOLOR(NODATA_color);

						glVertex3d(ptx, pty, 0.5);
//						glVertex3d(ptx, pty, (cell_val[i] - GridMidpoint) / (5.0 * GridRange) + 0.5);
					}
				glEnd();
				m_PointLayer[layer].EndDef();
			}
			delete [] cell_val;
		}
		else {
			// not enough memory for cell values
			CString msg;
			msg.Format("Not enough memory to load cell values...%i cells", ptcnt);
			AfxMessageBox(msg);
			return;
		}

		m_PointCount = ptcnt;
		m_DrawnPointCount = m_PointCount;
	}

	EndWaitCursor();
}

void CChildView::CreateSurface(LPCTSTR FileName)
{
	// 28 colors from blue to yellow to red
	COLORREF colors[] = {0xFF8000, 0xE28E1C, 0xC69C38, 0xAAAA55, 0x8DB871, 0x71C68D, 0x55D4AA, 0x38E2C6, 0x1CF0E2, 0x00FFFF, 0x00F0FF, 0x00E2FF, 0x00D4FF, 0x00C6FF, 
						 0x00B8FF, 0x00AAFF, 0x009CFF, 0x008EFF, 0x0080FF, 0x0071FF, 0x0063FF, 0x0055FF, 0x0047FF, 0x0038FF, 0x002AFF, 0x001CFF, 0x000EFF, 0x0000FF};

	CFileSpec fs(CFileSpec::FS_APPDIR);
	CString DTMFileName = _T(FileName);
	if (!DTMFileName.IsEmpty()) {
		DTMFileName.Remove(34);
		fs.SetFullSpec(DTMFileName);
	}

	if (!fs.Exists()) {
		// can't open file...display message and simply return...current data will be intact
		CString prompt;
		prompt.Format("DTM file doesn't exist: \n%s\n", fs.GetFullSpec());
		AfxMessageBox(prompt);

		return;
	}

	BeginWaitCursor();

	DTM3D dtm(fs.GetFullSpec(), TRUE, FALSE, FALSE);
//	DTM3D dtm(FileName, TRUE, FALSE, TRUE);
	if (dtm.IsValid()) {
		m_SurfaceFileName = fs.GetFullSpec();

//		int skip;
		int HRskip, LRskip;
		double minx = dtm.OriginX();
		double miny = dtm.OriginY();
		double minz = dtm.MinElev();
		double maxx = minx + dtm.Width();
		double maxy = miny + dtm.Height();
		double maxz = dtm.MaxElev();
		double actual_minz = minz;

		// see if we are using the rangepole to scale the vertical dimension...if so only use if the pole height is greater than the verical
		// span of the data
		if (m_UseRangepoleHeightToScaleData && !m_ScaleRangepoleToData && (m_RangepoleTotalHeight > (maxz - minz)))
			maxz = minz + m_RangepoleTotalHeight;

		// figure out scaling
		double xspan = maxx - minx;
		double yspan = maxy - miny;
		double zspan = maxz - minz;

		// get max dimension
		double maxspan = max(xspan, yspan);
		maxspan = max(maxspan, zspan);

		m_ScaledVerticalSpan = maxspan;
		m_ActualVerticalSpan = maxz - minz;

		// adjust scaling
		minx -= (maxspan - xspan) / 2.0;
		maxx += (maxspan - xspan) / 2.0;
		miny -= (maxspan - yspan) / 2.0;
		maxy += (maxspan - yspan) / 2.0;
		minz -= (maxspan - zspan) / 2.0;
		maxz += (maxspan - zspan) / 2.0;

		if (m_SBETViewer) {
			m_SBETMinLong = minx;
			m_SBETMinLat = miny;
			m_SBETMinEllHt = minz;
			m_SBETMaxLong = maxx;
			m_SBETMaxLat = maxy;
			m_SBETMaxEllHt = maxz;
		}

		// set scaling parameters
		if (m_SBETViewer)
			dtm.SetScalingForRendering(m_SBETMinLong, m_SBETMinLat, m_SBETMinEllHt, m_SBETMaxLong, m_SBETMaxLat, m_SBETMaxEllHt);
		else
			dtm.SetScalingForRendering(minx, miny, minz, maxx, maxy, maxz);

		m_PoleBaseZ = (actual_minz - minz) / maxspan - 0.5;

		// figure out the skip factor for the lo-res surface...clamped to range of 1-32
//		skip = (int) (max((double) dtm.Columns(), (double) dtm.Rows()) / 400.0);
//		skip = min(MAXSKIP, skip);
//		skip = max(1, skip);

#if defined(_M_X64)
		HRskip = (int) sqrt((double) dtm.Columns() * (double) dtm.Rows() / 1000000000.0);
#else
		HRskip = (int)sqrt((double)dtm.Columns() * (double)dtm.Rows() / 10000000.0);
#endif

		HRskip += 1;
		HRskip = max(1, HRskip);
		
#if defined(_M_X64)
		LRskip = (int)sqrt((double)dtm.Columns() * (double)dtm.Rows() / 1000000.0);
#else
		LRskip = (int)sqrt((double)dtm.Columns() * (double)dtm.Rows() / 100000.0);
#endif
		LRskip += 1;
		LRskip = max(1, LRskip);

		m_BannerMessage.Format("PDQ Data Viewer -- v%.2f -- PNW LIDAR Toolkit -- %s", PROGRAM_VERSION, fs.GetFileNameEx());

		m_LowResolutionMessage.Empty();
		m_HighResolutionMessage.Empty();

		if (LRskip > 1) m_LowResolutionMessage.Format("Skipping every %i cells", LRskip);
		if (HRskip > 1) m_HighResolutionMessage.Format("Skipping every %i cells", HRskip);

		dtm.CalculateSmoothNormals();

		// make sure we have normals
		if (dtm.AreNormalsAvailable()) {
			m_SurfaceLayer.StartDef();

//			if (skip >= MAXSKIP || !dtm.AreNormalsAvailable())
//				dtm.DrawGroundFast(skip, FALSE, 1.0);
//			else {
/*				// calculate slope colors
				unsigned char** ptcolor;
				COLORREF colors[255];
				// allocate memory for color indices
				ptcolor = new unsigned char* [dtm.Columns()];

				for (int ti = 0; ti < dtm.Columns(); ti ++) {
					ptcolor[ti] = new unsigned char [dtm.Rows()];
					for (int tj = 0; tj < dtm.Rows(); tj ++)
						ptcolor[ti][tj] = 0;
				}

				// calculate slope...the multiplier when setting the color controls the range...128 gives slopes from 0 to 200%
				double GTerm, HTerm;
				double Slope;
				for (int cols = 1; cols < dtm.Columns() - 1; cols ++) {
					for (int rows = 1; rows < dtm.Rows() - 1; rows ++) {
						GTerm = (dtm.GetGridElevation(cols + 1, rows) - dtm.GetGridElevation(cols - 1, rows)) / dtm.ColumnSpacing() * 2.0;
						HTerm = (dtm.GetGridElevation(cols, rows + 1) - dtm.GetGridElevation(cols, rows - 1)) / dtm.PointSpacing() * 2.0;
						Slope = sqrt(GTerm * GTerm + HTerm * HTerm);
						ptcolor[cols][rows] = (unsigned char) (Slope * 64.0);
					}
				}

				// build color ramp that starts at white and becomes pure red...higher slopes, more red
				for (ti = 0; ti < 127; ti ++)
					colors[ti] = RGB(ti + 32, ti + 32, ti + 32);
				for (ti = 128; ti < 255; ti ++)
					colors[ti] = RGB(ti, 255 - ti, 255 - ti);

				dtm.DrawGroundSurfaceWithColor(colors, ptcolor, 1.0, FALSE);

				for (ti = 0; ti < dtm.Columns(); ti ++)
					delete [] ptcolor[ti];

				delete [] ptcolor;
*/

/*				// calculate elevation colors
				double scale;
				int gsvalue;
				unsigned char** ptcolor;
				COLORREF colors[255];
				// allocate memory for color indices
				ptcolor = new unsigned char* [dtm.Columns()];

				for (int ti = 0; ti < dtm.Columns(); ti ++) {
					ptcolor[ti] = new unsigned char [dtm.Rows()];
					for (int tj = 0; tj < dtm.Rows(); tj ++)
						ptcolor[ti][tj] = 0;
				}

				// calculate slope...the multiplier when setting the color controls the range...128 gives slopes from 0 to 200%
				double Range = dtm.MaxElev() - dtm.MinElev();
				for (int cols = 1; cols < dtm.Columns() - 1; cols ++) {
					for (int rows = 1; rows < dtm.Rows() - 1; rows ++) {
						scale = (dtm.GetGridElevation(cols, rows) - dtm.MinElev()) / Range;
						ptcolor[cols][rows] = (unsigned char) (scale * 255.0);
					}
				}

				// build color ramp that starts at light gray and goes to medium gray
				for (ti = 0; ti < 256; ti ++) {
					scale = (double ) ti / 255.0;
	//				gsvalue = (int) (32.0 + scale * 127.0);
					gsvalue = (int) (159.0 - scale * 127.0);
					colors[ti] = RGB(gsvalue, gsvalue, gsvalue);
				}

				dtm.DrawGroundSurfaceWithColor(colors, ptcolor, 1.0, FALSE);

				for (ti = 0; ti < dtm.Columns(); ti ++)
					delete [] ptcolor[ti];

				delete [] ptcolor;
*/
				// drawing with QUADS seems to give a slightly smoother surface with fewer jagged edges
				dtm.DrawGroundSurfaceFast(HRskip, 1.0, TRUE);
//				dtm.DrawGroundSurfaceWithColorRamp(colors, 28, HRskip, 1.0, TRUE);
//			}

			m_SurfaceLayer.EndDef();

			// create lo-resolution surface
			m_LORESSurfaceLayer.StartDef();

			dtm.DrawGroundSurfaceFast(LRskip);

			m_LORESSurfaceLayer.EndDef();

			m_SurfaceIsWireframe = FALSE;
		}
		else {
			EndGLCommands();
			// showing the message dialog seems to mess up the GL context so I added the calls to EndGLCommands() and BeginGLCommands() to 
			// "reset" the context
			AfxMessageBox("Ground model too large to compute normal vectors...too many cells!\r\nRendering as wireframe with elevation gradient.");
			BeginGLCommands();

			HRskip = (int) sqrt((double) dtm.Columns() * (double) dtm.Rows() / 1000000.0);
			HRskip += 1;
			HRskip = max(1, HRskip);

			m_LowResolutionMessage.Empty();
			m_HighResolutionMessage.Empty();

			if (LRskip > 1) m_LowResolutionMessage.Format("Skipping every %i cells", LRskip);
			if (HRskip > 1) m_HighResolutionMessage.Format("Skipping every %i cells", HRskip);

			// draw surface as wireframe
			m_SurfaceLayer.StartDef();

			// should be able to render a higher-resolution wireframe but I don't know how much higher...skip/2 is a guess
			// when the rendering fails, the program crashes and goes away. It looks like this happens in the OGL library from the graphics card drivers
			dtm.DrawGroundFastWithColorRamp(colors, 28, HRskip, FALSE, 1.0);
//			dtm.DrawGroundSurfaceWithColorRamp(colors, 28, HRskip, 1.0, TRUE);

			m_SurfaceLayer.EndDef();

			// create lo-resolution surface
			m_LORESSurfaceLayer.StartDef();

			dtm.DrawGroundFastWithColorRamp(colors, 28, LRskip, FALSE, 1.0);
//			dtm.DrawGroundSurfaceWithColorRamp(colors, 28, LRskip, 1.0, TRUE);

			m_LORESSurfaceLayer.EndDef();

//			m_SurfaceIsWireframe = FALSE;
			m_SurfaceIsWireframe = TRUE;
		}

		dtm.Empty();
		dtm.Destroy();


		// define rangepole
		if (m_ScaleRangepoleToData)
			CreateRangePole(m_ActualVerticalSpan / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);
		else
			CreateRangePole(m_RangepoleTotalHeight / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);

		// open DTM for flyball movement
		m_TerrainForFlyball.LoadElevations(fs.GetFullSpec());
		if (m_TerrainForFlyball.IsValid()) {
			m_HaveTerrainForFlyball = TRUE;
			m_ScaledMinX = minx;
			m_ScaledMinY = miny;
			m_ScaledMinZ = minz;
			m_ScaledMaxX = maxx;
			m_ScaledMaxY = maxy;
			m_ScaledMaxZ = maxz;
		}
	}
	else {
		AfxMessageBox("Ground model too large to load into memory...too many cells!");
	}

	EndWaitCursor();
}

void CChildView::DrawDataObject()
{
	if (m_DataType & PANORAMA || m_DataType & JACK)
		DrawCylinder();
	if (m_DataType & SURFACE)
		DrawSurface();
	if (m_DataType & POINTDATA && m_DrawPointData)
		DrawPointCloud();
	if (m_DataType & MODEL)
		DrawModel();
	if (m_DataType & IMAGEPLATE)
		DrawASCIIPointCloud();
//		DrawImagePlate();
	if (m_DataType & CLOCK)
		DrawClockface();
}

void CChildView::DrawClockface()
{
	glPushMatrix();

//	glTranslated(0.0, 0.0, -0.25);

		glRotated(180.0, 1.0, 0.0, 0.0);
		m_Object.Draw();

		// draw clock elements
		// hour hand
		glPushMatrix();
			glRotated((m_CurrentTime / 3600.0) / 12.0 * 360.0, 0.0, 0.0, 1.0);
			m_HourHand.Draw();
		glPopMatrix();

		// minute hand
		glPushMatrix();
			glRotated((m_CurrentTime / 60.0) / 60.0 * 360.0, 0.0, 0.0, 1.0);
			m_MinuteHand.Draw();
		glPopMatrix();

		// second hand
		glPushMatrix();
			glRotated(((double) ((int) fmod(m_CurrentTime, 60.0)) / 60.0) * 360.0, 0.0, 0.0, 1.0);
			m_SecondHand.Draw();
		glPopMatrix();
	glFinish ();

	glPopMatrix();
}

void CChildView::DrawCylinder()
{
	glPushMatrix();

//	glTranslated(0.0, 0.0, -0.25);

	SetAnaglyphSensitiveColor(RGB(255, 255, 255));
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	// draw the surface display list
	m_Object.Draw();

	glFinish ();
//	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void CChildView::DrawImagePlate()
{
	glPointSize(m_PointSize);
	
	glPushMatrix();
	glTranslated(-0.5, -0.5, 0.0);

	if (m_QAModelScan) {
		m_ScanOffsetX = (0.2 * (double) (m_ScanPositionIndex % 6)) - 0.5;
		m_ScanOffsetY = (0.2 * (double) (m_ScanPositionIndex / 6)) - 0.5;

		if ((m_ScanPositionIndex / 6) % 2 == 1)
			glTranslated(m_ScanOffsetX, m_ScanOffsetY, 0.0);
		else
			glTranslated(-m_ScanOffsetX, m_ScanOffsetY, 0.0);
	}

	m_ImagePlate.Draw();

	glFinish ();

	glPopMatrix();
}

void CChildView::DrawSurface()
{
	glPushMatrix();
	glTranslated(-0.5, -0.5, -0.5);

	if (m_SurfaceIsWireframe) {
		SetAnaglyphSensitiveColor(RGB(192, 192, 192));
		glDisable(GL_LIGHTING);
	}
	else  {
		SetAnaglyphSensitiveColor(RGB(128, 128, 128));
		glEnable(GL_COLOR_MATERIAL);
	}

	glDisable(GL_CULL_FACE);
//	glEnable(GL_LIGHTING);

	// DTM scanning stuff
	if (m_QAModelScan) {
		m_ScanOffsetX = (0.2 * (double) (m_ScanPositionIndex % 6)) - 0.5;
		m_ScanOffsetY = (0.2 * (double) (m_ScanPositionIndex / 6)) - 0.5;

		if ((m_ScanPositionIndex / 6) % 2 == 1)
			glTranslated(m_ScanOffsetX, m_ScanOffsetY, 0.0);
		else
			glTranslated(-m_ScanOffsetX, m_ScanOffsetY, 0.0);

		glTranslated(0.0, 0.0, m_ScanOffsetZ);
	}

	// draw the surface display list
	// use full resolution when in FLYBALL mode
	if ((m_InMotion || m_DisplayLowResSurface) && m_MotionController == TRACKBALL)
		m_LORESSurfaceLayer.Draw();
	else
		m_SurfaceLayer.Draw();

	if (glGetError()) {
		TRACE("Error in DrawSurface\n");	
	}

	glFinish ();
//	glDisable(GL_LIGHTING);

	glPopMatrix();
}

void CChildView::Create3DFigure()
{
	double pts[8][3] = {
		{0.3, 0.1, 0.0},
		{0.5, 0.5, 0.0},
		{0.7, 0.9, 0.0},
		{0.9, 0.3, 0.0},
		{0.3, 0.1, 0.1},
		{0.5, 0.5, 0.2},
		{0.7, 0.9, 0.3},
		{0.9, 0.3, 0.1}
	};

	// create the "object" to draw
	m_Object.StartDef();
		// draw balls
//		glTranslated(-0.5, -0.5, -0.5);
		glBegin(GL_LINES);

		glVertex3dv(pts[0]);
		glVertex3dv(pts[1]);
		glVertex3dv(pts[1]);
		glVertex3dv(pts[2]);
		glVertex3dv(pts[3]);
		glVertex3dv(pts[1]);

		glVertex3dv(pts[4]);
		glVertex3dv(pts[5]);
		glVertex3dv(pts[5]);
		glVertex3dv(pts[6]);
		glVertex3dv(pts[7]);
		glVertex3dv(pts[5]);

		glEnd();
	m_Object.EndDef();

	m_DataType = JACK;			// just need to draw the m_Object display list
}

void CChildView::DrawModel()
{
	static int ferr;
	static int i;
	static double posx, posy, posz;

	glPushMatrix();
	glTranslated(0.0, 0.0, -0.5);

	glPushMatrix();

	// if using SBET file, read next line and parse aircraft attitude
	if (m_SBETDataFile.IsValid() && m_SBETViewer) {
		for (i = 0; i < m_SBETSpeedStep; i ++) {
			if (!m_SBETDataFile.ReadDataLine(m_SBETLineBuffer, SKIPCOMMENTS)) {
				// rewind file
				m_SBETDataFile.Rewind();

				m_SBETRecordCount = 0;

				break;
			}
			m_SBETRecordCount ++;
		}

		if (m_SBETDataFile.ReadDataLine(m_SBETLineBuffer, SKIPCOMMENTS)) {
			ferr = sscanf(m_SBETLineBuffer, "%lf %lf %lf %lf %lf %lf %lf", &m_GPSTime, &m_Lat, &m_Long, &m_EllHt, &m_Roll, &m_Pitch, &m_TrueHeading);
			if (ferr == 7) {
//				m_Lat *= 20000.0;
//				m_Long *= -20000.0;

				// move model to lat, long position
				posx = (m_Long - m_SBETMinLong) / (m_SBETMaxLong - m_SBETMinLong) - 0.5;
				posy = (m_Lat - m_SBETMinLat) / (m_SBETMaxLat - m_SBETMinLat) - 0.5;
				posz = (m_EllHt - m_SBETMinEllHt) / (m_SBETMaxEllHt - m_SBETMinEllHt);

				glTranslated(posx, posy, posz);

				m_TrueHeading += 180.0;
				if (m_TrueHeading > 360.0)
					m_TrueHeading -= 360.0;

				// rotate for aircraft attitude
				glRotated(-m_TrueHeading, 0, 0, 1);// yaw
				glRotated(m_Roll, 0, 1, 0);		// roll
				glRotated(m_Pitch, 1, 0, 0);		// pitch

				m_SBETRecordCount ++;
			}
			else if (ferr == EOF) {
				// rewind file
				m_SBETDataFile.Rewind();

				m_SBETRecordCount = 0;
			}
		}
	}
	else {
		// move model to center of cube
		glTranslated(0.0, 0.0, 0.5);
	}

	glPushMatrix();

	// scale model to fit into cube...wingspan limited
	m_ObjectModel.ScaleToUnitCube();
	glRotated(90.0, 1, 0, 0);		// cessna 3ds model stands on its nose by default
	glRotated(180.0, 0, 1, 0);

	// further reduce size
	glScaled(0.025, 0.025, 0.025);

	// draw the model
	m_ObjectModel.Render();

	glPopMatrix();

	// draw line from object center down...reflect 14 degree scan angle
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glColor4d(1.0, 0.0, 0.0, 0.3);
	if (m_SBETViewer) {
//		posz = (m_EllHt - m_SBETMinEllHt) / (m_SBETMaxEllHt - m_SBETMinEllHt);
		posz = (m_EllHt + 200.0) / (m_SBETMaxEllHt - m_SBETMinEllHt);
		posx = (0.249328 * m_EllHt) / (m_SBETMaxLong - m_SBETMinLong);
		glBegin(GL_TRIANGLES);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(-posx, 0.0, -(posz));
			glVertex3d(posx, 0.0, -(posz));
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(-0.25, 0.0, -0.5);
			glVertex3d(0.25, 0.0, -0.5);
		glEnd();
	}


	glPopMatrix();

/*	// draw flat plane to represent the ellipsoid height
	glColor3d(0.6, 0.6, 0.6);
	if (m_SBETViewer) {
		posz = (0.0 - m_SBETMinEllHt) / (m_SBETMaxEllHt - m_SBETMinEllHt);
		glBegin(GL_QUADS);
			glVertex3d(-0.5, -0.5, posz);
			glVertex3d(-0.5, 0.5, posz);
			glVertex3d(0.5, 0.5, posz);
			glVertex3d(0.5, -0.5, posz);
		glEnd();
	}
	else {
		glBegin(GL_QUADS);
			glVertex3d(-0.5, -0.5, 0.0);
			glVertex3d(-0.5, 0.5, 0.0);
			glVertex3d(0.5, 0.5, 0.0);
			glVertex3d(0.5, -0.5, 0.0);
		glEnd();
	}
*/	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	glFinish ();
}

void CChildView::CreateModel(LPCTSTR FileName)
{
	m_ObjectModel.Load(FileName, TRUE);
}

void CChildView::OnEditCopy() 
{
	NewCopyToClipboard();
}

void CChildView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CChildView::ResetCamera()
{
//	COpenGLWnd::ResetCamera();
//	return;

	TRACE("In CChildView::ResetCamera\n");	

	m_Camera.filmwidth = 36.0;
	m_Camera.filmheight = 24.0;

	m_Camera.focallength = 150;
	m_Camera.focallength = 100;
	m_Camera.aperture = 2.0 * (atan2(m_Camera.filmheight / 2.0, m_Camera.focallength)) / DTOR;

	m_FlyballOffset = 10.0;

	if (m_MotionController == FLYBALL) {
		// viewpoint
		m_Camera.vp.x = 0.0;
		m_Camera.vp.y = -1.0;
		m_Camera.vp.z = 0.0;

		// view direction
		m_Camera.vd.x = 0.0; 
		m_Camera.vd.y = 1.0; 
		m_Camera.vd.z = 0.0;

		// focus point
		m_Camera.pr.x = 0.0;
		m_Camera.pr.y = 0.0;		// original position at center of data space
		m_Camera.pr.y = -0.5;		// gives better feel in flyball mode
		m_Camera.pr.z = 0.0;

		// up vector
		m_Camera.vu.x = 0;  
		m_Camera.vu.y = 0; 
		m_Camera.vu.z = 1;

		m_Camera.nearplane = 0.001;
		m_Camera.farplane = 3.0;
	}
	else {
		// viewpoint
		m_Camera.vp.x = 0.0;
		m_Camera.vp.y = 0.0;
		if (m_DataType == PANORAMA)
			m_Camera.vp.z = 0.1;
		else {
			m_Camera.vp.z = 4.0;
		}


		// view direction
		m_Camera.vd.x = 0.0; 
		m_Camera.vd.y = 0.0; 
		if (m_DataType == PANORAMA)
			m_Camera.vd.z = -0.1;
		else {
			m_Camera.vd.z = -4.0;
		}

		// focus point
		m_Camera.pr.x = 0.0;
		m_Camera.pr.y = 0.0;
		m_Camera.pr.z = 0.0;

		// up vector
		m_Camera.vu.x = 0;  
		m_Camera.vu.y = 1; 
		m_Camera.vu.z = 0;

		if (m_DataType == PANORAMA)
			m_Camera.nearplane = 0.05;
		else
			m_Camera.nearplane = 3.0;
		m_Camera.farplane = 5.0;
	}
}

void CChildView::OnViewAxes() 
{
	if (m_DrawAxes)
		m_DrawAxes = FALSE;
	else
		m_DrawAxes = TRUE;
	Invalidate();
}

void CChildView::OnUpdateViewAxes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
	pCmdUI->SetCheck(m_DrawAxes);
}

void CChildView::OnViewPoints() 
{
	if (m_DrawPointData)
		m_DrawPointData = FALSE;
	else
		m_DrawPointData = TRUE;
	Invalidate();
}

void CChildView::OnUpdateViewPoints(CCmdUI* pCmdUI) 
{
	if (m_DataType & POINTDATA)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);

	pCmdUI->SetCheck(m_DrawPointData);
}

void CChildView::OnDrawGLBeforeTrackball()
{
	return;
	glPushMatrix();

	// default orientation is looking down the Z axis...same as overhead view

	// rotate 90 degrees around the X axis
	glRotated(90.0, 1.0, 0.0, 0.0);

	// draw rangepoles
	if ((m_DataType & SURFACE || m_DataType & POINTDATA) && m_DrawRangePoles) {
		glDisable(GL_CULL_FACE);
		glPushMatrix();
		glTranslated(0.0, 0.0, m_PoleBaseZ);		// center of data space
		DrawRangePole();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.5, -0.5, m_PoleBaseZ);		// LL XY
		DrawRangePole();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.5, 0.5, m_PoleBaseZ);		// UL XY
		DrawRangePole();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.5, 0.5, m_PoleBaseZ);		// UR XY
		DrawRangePole();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.5, -0.5, m_PoleBaseZ);		// LR XY
		DrawRangePole();
		glPopMatrix();
		glEnable(GL_CULL_FACE);
	}
	glPopMatrix();
}

void CChildView::OnFileRangepoleoptions() 
{
	RangepoleOptionDlg dlg;

	dlg.m_DrawPoleCenter = m_DrawPoleCenter;
	dlg.m_DrawPoleLL = m_DrawPoleLL;
	dlg.m_DrawPoleUL = m_DrawPoleUL;
	dlg.m_DrawPoleUR = m_DrawPoleUR;
	dlg.m_DrawPoleLR = m_DrawPoleLR;
	dlg.m_DrawPoleML = m_DrawPoleML;
	dlg.m_DrawPoleMT = m_DrawPoleMT;
	dlg.m_DrawPoleMR = m_DrawPoleMR;
	dlg.m_DrawPoleMB = m_DrawPoleMB;

	dlg.m_RangepoleTotalHeight = m_RangepoleTotalHeight;
	dlg.m_RangepoleSegmentHeight = m_RangepoleSegmentHeight;
	dlg.m_ScaleRangepoleToData = m_ScaleRangepoleToData;
	dlg.m_UseRangepoleHeightToScaleData = m_UseRangepoleHeightToScaleData;

	if (dlg.DoModal() == IDOK) {
		// update rangepole info
		m_DrawPoleCenter = dlg.m_DrawPoleCenter;
		m_DrawPoleLL = dlg.m_DrawPoleLL;
		m_DrawPoleUL = dlg.m_DrawPoleUL;
		m_DrawPoleUR = dlg.m_DrawPoleUR;
		m_DrawPoleLR = dlg.m_DrawPoleLR;
		m_DrawPoleML = dlg.m_DrawPoleML;
		m_DrawPoleMT = dlg.m_DrawPoleMT;
		m_DrawPoleMR = dlg.m_DrawPoleMR;
		m_DrawPoleMB = dlg.m_DrawPoleMB;

		m_RangepoleTotalHeight = dlg.m_RangepoleTotalHeight;
		m_RangepoleSegmentHeight = dlg.m_RangepoleSegmentHeight;
		m_ScaleRangepoleToData = dlg.m_ScaleRangepoleToData;

		// see if we need to rescale the data space
		if (dlg.m_UseRangepoleHeightToScaleData != m_UseRangepoleHeightToScaleData) {
			m_UseRangepoleHeightToScaleData = dlg.m_UseRangepoleHeightToScaleData;

			// re-open the point file
			if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
				DeleteDataObject();

				BeginGLCommands();
				CreatePointCloud(m_PointFileName, m_PointFileFormat);
				EndGLCommands();
			}
			else if (m_DataType & SURFACE && !m_SurfaceFileName.IsEmpty()) {
				DeleteDataObject();

				BeginGLCommands();
				CreateSurface(m_SurfaceFileName);
				EndGLCommands();
			}

			m_NeedRescale = TRUE;
		}

		m_UseRangepoleHeightToScaleData = dlg.m_UseRangepoleHeightToScaleData;

		if ((m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) || (m_DataType & SURFACE && !m_SurfaceFileName.IsEmpty())) {
			BeginGLCommands();
				if (m_ScaleRangepoleToData)
					CreateRangePole(m_ActualVerticalSpan / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);
				else
					CreateRangePole(m_RangepoleTotalHeight / m_ScaledVerticalSpan, m_RangepoleSegmentHeight / m_ScaledVerticalSpan, 0.01);
			EndGLCommands();
		}

		Invalidate();
	}
}

void CChildView::OnViewRangepoles() 
{
	if (m_DrawRangePoles)
		m_DrawRangePoles = FALSE;
	else
		m_DrawRangePoles = TRUE;
	Invalidate();
}

void CChildView::OnUpdateViewRangepoles(CCmdUI* pCmdUI) 
{
	if (m_DataType & SURFACE || m_DataType & POINTDATA)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);

	pCmdUI->SetCheck(m_DrawRangePoles);
}
/*
LRESULT CChildView::OnJoy1ButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam & JOY_BUTTON1)
		MessageBox("The joystick Button 1 Pressed");
	else if (wParam & JOY_BUTTON2)
		MessageBox("The joystick Button 2 Pressed");
	else if (wParam & JOY_BUTTON3)
		MessageBox("The joystick Button 3 Pressed");
	else if (wParam & JOY_BUTTON4)
		MessageBox("The joystick Button 4 Pressed");
	return 0;
}

LRESULT CChildView::OnJoy1Move(WPARAM wParam, LPARAM lParam)
{
	int errorVal; // Error returned from the joyGetPos call

	errorVal = joyGetPos(JOYSTICKID1, &joyInfo);
	switch (errorVal) {
		case MMSYSERR_NODRIVER:
			MessageBox("The joystick driver is not present");
			break;
		case MMSYSERR_INVALPARAM:
			MessageBox("An invalid parameter was passed");
			break;
		case JOYERR_UNPLUGGED:
			MessageBox("The joystick is not connected");
			break;
//		default: // Update the joysticks x,y,z position
//			CString ts;
//			ts.Format("X:%d   Y:%d   Z:%d", joyInfo.wXpos, joyInfo.wYpos, joyInfo.wZpos);
//			MessageBox(ts);
	}

	return 0;
}

LRESULT CChildView::OnJoy1ButtonUp(WPARAM wParam, LPARAM lParam)
{
	// This procedure will annotate the joystrick button that has been released.
	if (wParam & JOY_BUTTON1CHG) {
		MessageBox("The joystick Button 1 Released");
	}
	else if (wParam & JOY_BUTTON2CHG) {
		MessageBox("The joystick Button 2 Released");
	}
	else if (wParam & JOY_BUTTON3CHG) {
		MessageBox("The joystick Button 3 Released");
	}
	else if (wParam & JOY_BUTTON4CHG) {
		MessageBox("The joystick Button 4 Released");
	}

	return 0;
}
*/

void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// call the base object's OnMouseMove to process the movement
	COpenGLWnd ::OnMouseMove(nFlags, point);

	// send message to other instances to synchronize trackballs
//	if (((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize() && m_InMotion) {
//		SendSync();
//	}
}

void CChildView::SendSync()
{
	// allocate memory on heap for trackball orientation
	unitquaternion uq = m_TrackBall.GetBallOrientation();
	Sync_orientation so;

	// Generate handle 
	HANDLE handle = (HANDLE)::GlobalAlloc (GHND, sizeof(Sync_orientation)); 
	if (handle != NULL) { 
		// copy values to Sync_orientation structure
		so.x = uq.x();
		so.y = uq.y();
		so.z = uq.z();
		so.w = uq.w();
		so.pointsize = m_PointSize;
		so.hwnd = GetSafeHwnd();
		so.focallength = m_Camera.focallength;

		RECT rc;
		GetParent()->GetWindowRect(&rc);
		so.window_height = rc.bottom - rc.top;
		so.window_width = rc.right - rc.left;

		// Lock handle 
		char *pData = (char *) ::GlobalLock((HGLOBAL) handle); 

		// Copy header and data 
		memcpy(pData, &so, sizeof(Sync_orientation));
		
		// Unlock 
		::GlobalUnlock((HGLOBAL) handle); 

		// Push data in clipboard 
		OpenClipboard(); 
		EmptyClipboard(); 
		SetClipboardData(RegisterClipboardFormat("Sync_orientation"), handle); 
		CloseClipboard(); 
	
		::PostMessage(HWND_BROADCAST, RegisterWindowMessage("PDQSynchView"), 0, 0);
	} 
}



void CChildView::SetupFog(bool Enable)
{
	if (Enable) {
		GLfloat fcolor[4] = { (float)GetRValue(m_BackgroundColor) / 255.0f, (float)GetGValue(m_BackgroundColor) / 255.0f, (float)GetBValue(m_BackgroundColor) / 255.0f, 1.0f };
		glFogfv(GL_FOG_COLOR, fcolor);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 0.0);			// only used with fog mode GL_LINEAR
		glFogf(GL_FOG_END, 1.0);			// only used with fog mode GL_LINEAR

//		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 2.0);		// only used with fog modes GL_EXP and GL_EXP2
		glEnable(GL_FOG);
	}
	else
		glDisable(GL_FOG);
}

void CChildView::ClassificationCodeDlgDone()
{
	m_ClassificationCodeDlg = NULL;

	m_ColorMode = COLORBYHEIGHT;

	// re-open the point file
	if (m_DataType & POINTDATA && !m_PointFileName.IsEmpty()) {
		BeginGLCommands();
		CreatePointCloud(m_PointFileName, m_PointFileFormat);
		EndGLCommands();
	}
}

void CChildView::OpenClassificationCodeDlg()
{
	if (m_ClassificationCodeDlg == NULL) {
		m_ClassificationCodeDlg = new CClassificationCodeDlg(this);
		m_ClassificationCodeDlg->Create();
		
		// set controls
		for (int i = 0; i < 32; i++)
			m_ClassificationCodeDlg->m_ShowClass[i] = m_LASClassState[i];
		m_ClassificationCodeDlg->m_Synthetic = m_LASSynthetic;
		m_ClassificationCodeDlg->m_Overlap = m_LASOverlap;
		m_ClassificationCodeDlg->m_Keypoint = m_LASKeypoint;
		m_ClassificationCodeDlg->m_Withheld = m_LASWithheld;
		m_ClassificationCodeDlg->m_ShowOnlyPointsWithFlags = m_ShowOnlyPointsWithFlags;
	}
	else {
		m_ClassificationCodeDlg->SetActiveWindow();
//		m_ClassificationCodeDlg->UpdateInfo(NOTHING);
	}

	AfxGetApp()->GetMainWnd()->SetForegroundWindow();
}

void CChildView::ClassificationCodeDlgFeedback(BOOL Redraw)
{
	AfxGetApp()->GetMainWnd()->SetForegroundWindow();

	// read dialog controls
	if (m_ClassificationCodeDlg) {
		for (int i = 0; i < 32; i++)
			m_LASClassState[i] = m_ClassificationCodeDlg->m_ShowClass[i];
		m_LASSynthetic = m_ClassificationCodeDlg->m_Synthetic;
		m_LASOverlap = m_ClassificationCodeDlg->m_Overlap;
		m_LASKeypoint = m_ClassificationCodeDlg->m_Keypoint;
		m_LASWithheld = m_ClassificationCodeDlg->m_Withheld;
		m_ShowOnlyPointsWithFlags = m_ClassificationCodeDlg->m_ShowOnlyPointsWithFlags;
	}

	if (Redraw) {
		BeginGLCommands();
		CreatePointCloud(m_PointFileName, m_PointFileFormat);
		EndGLCommands();
	}
}

