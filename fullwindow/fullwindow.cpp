// fullwindow.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "fullwindow.h"
#include "filespec.h"

#include "MainFrm.h"

#include "laszip_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_HelpInfo;
	CString	m_VersionIdentifier;
	CString	m_LASClassificationLabels;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_HelpInfo = _T("");
	m_VersionIdentifier = _T("");
	m_LASClassificationLabels = _T("");
	//}}AFX_DATA_INIT
	char* helps[] = {
		"Mouse\r\n",
		"Left & motion\trotate data cloud\r\n",
		"Wheel\t\tzoom in/out\r\n",
		"\r\n",
		"Keyboard\r\n",
		"Escape\t\tstop data rotation\r\n",
		"A\t\ttoggle anaglyph mode\r\n",
		"B\t\tset background color to black\r\n",
		"C\t\tcolor points using RGB color in LAS file\r\n",
		"E\t\tdecrease eye separation in split-screen stereo mode\r\n",
		"shift E\t\tincrease eye separation in split-screen stereo mode\r\n",
		"shift ctrl E\t\treset eye separation in split-screen stereo mode\r\n",
		"F\t\tcolor points using NIR-RG color values in LAS file\r\n",
		"H\t\tcolor points using height\r\n",
		"I\t\ttoggle display of axes (wireframe cube)\r\n",
		"J\t\tcolor points by return number\r\n",
		"L\t\tcolor points using LAS classification value\r\n",
		"M\t\ttoggle continuous rotation mode\r\n",
		"N\t\tcolor points using intensity data from LAS files (if available)\r\n",
		"O\t\treset orientation (overhead view)\r\n",
		"P\t\ttoggle points on/off\r\n",
		"Q\t\ttoggle low/high resolution terrain surface\r\n",
		"R\t\tbegin/end recording to AVI file\r\n",
		"S\t\ttoggle split-screen stereo mode\r\n",
		"ctrl T\t\tcapture screen image\r\n",
//		"shift ctrl T\t\tcapture stereo pair\r\n",
		"V\t\ttoggle between trackball and translation motion controller\r\n",
		"W\t\tset background color to white\r\n",
		"X\t\ttoggle x-eyed/parallel-eyed viewing in split-screen mode\r\n",
		"Z\t\tlower DTM while in scanning mode\r\n",
		"shift-Z\t\traise DTM while in scanning mode\r\n",
		"\r\n",
		"ctrl +\t\tincrease symbol size\r\n",
		"ctrl -\t\tdecrease symbol size\r\n",
		"\r\n",
		"F5\t\ttoggle scanning mode for DTM evaluation\r\n",
		"\t\tuse + and - to move model\r\n",
		"",
		""
	};
	char* labels[] = {
		"LAS versions <= 1.3 and V1.4: point records 0-5\r\n",
		"0\tCreated, never classified\r\n",
		"1\tUnclassified\r\n",
		"2\tGround\r\n",
		"3\tLow vegetation\r\n",
		"4\tMedium vegetation\r\n",
		"5\tHigh vegetation\r\n",
		"6\tBuilding\r\n",
		"7\tLow point (noise)\r\n",
		"8\tModel key-point (mass point)\r\n",
		"9\tWater\r\n",
		"10\tReserved\r\n",
		"11\tReserved\r\n",
		"12\tOverlap points\r\n",
		"13-31\tReserved\r\n",
		"W\tWithheld\r\n",
		"------------------------------------\r\n",
		"LAS version 1.4 point records 6-10\r\n",
		"0\tCreated, never classified\r\n",
		"1\tUnclassified\r\n",
		"2\tGround\r\n",
		"3\tLow vegetation\r\n",
		"4\tMedium vegetation\r\n",
		"5\tHigh vegetation\r\n",
		"6\tBuilding\r\n",
		"7\tLow point (noise)\r\n",
		"8\tReserved\r\n",
		"9\tWater\r\n",
		"10\tRail\r\n",
		"11\tRoad surface\r\n",
		"12\tReserved\r\n",
		"13\tWire-Guard (Shield)\r\n",
		"14\tWire-Conductor (Phase)\r\n",
		"15\tTransmission tower\r\n",
		"16\tWire-structure connector\r\n",
		"17\tBridge deck\r\n",
		"18\tHigh noise\r\n",
		"19-63\tReserved\r\n",
		"64-255\tUser definable\r\n",
		"",
		""
	};
	int i = 0;
	while (helps[i][0] != '\0') {
		m_HelpInfo += _T(helps[i]);
		i ++;
	}
	i = 0;
	while (labels[i][0] != '\0') {
		m_LASClassificationLabels += _T(labels[i]);
		i ++;
	}
#ifdef _DEBUG
#ifdef USE_LASLIB
	m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASlib) -- Debug build", PROGRAM_VERSION);
#else
#if defined(_M_X64)
	if (GetModuleHandle("LASzip64.dll") != NULL) {
#else
	if (GetModuleHandle("LASzip.dll") != NULL) {
#endif
		// get version of LASzip DLL
		laszip_U8 version_major;
		laszip_U8 version_minor;
		laszip_U16 version_revision;
		laszip_U32 version_build;
		CString Version;

		if (!laszip_get_version(&version_major, &version_minor, &version_revision, &version_build)) {
			Version.Format("V%d.%d r%d (build %d)", (int)version_major, (int)version_minor, (int)version_revision, (int)version_build);

#if defined(_M_X64)
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip64.dll %s) -- Debug build", PROGRAM_VERSION, Version);
#else
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip.dll %s) -- Debug build", PROGRAM_VERSION, Version);
#endif
		}
		else {
#if defined(_M_X64)
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip64.dll NO VERSION INFO) -- Debug build", PROGRAM_VERSION);
#else
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip.dll NO VERSION INFO) -- Debug build", PROGRAM_VERSION);
#endif
		}
	}
	else
		m_VersionIdentifier.Format("PDQ Data Viewer V%.2f -- Debug build", PROGRAM_VERSION);
#endif
#else
#ifdef USE_LASLIB
	m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASlib) -- Release build", PROGRAM_VERSION);
#else
#if defined(_M_X64)
	if (GetModuleHandle("LASzip64.dll") != NULL) {
#else
	if (GetModuleHandle("LASzip.dll") != NULL) {
#endif
		// get version of LASzip DLL
		laszip_U8 version_major;
		laszip_U8 version_minor;
		laszip_U16 version_revision;
		laszip_U32 version_build;
		CString Version;

		if (!laszip_get_version(&version_major, &version_minor, &version_revision, &version_build)) {
			Version.Format("V%d.%d r%d (build %d)", (int)version_major, (int)version_minor, (int)version_revision, (int)version_build);

#if defined(_M_X64)
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip64.dll %s) -- Release build", PROGRAM_VERSION, Version);
#else
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip.dll %s) -- Release build", PROGRAM_VERSION, Version);
#endif
		}
		else {
#if defined(_M_X64)
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip64.dll NO VERSION INFO) -- Debug build", PROGRAM_VERSION);
#else
			m_VersionIdentifier.Format("PDQ Data Viewer V%.2f (LASzip.dll NO VERSION INFO) -- Debug build", PROGRAM_VERSION);
#endif
		}
	}
	else
		m_VersionIdentifier.Format("PDQ Data Viewer V%.2f -- Release build", PROGRAM_VERSION);
#endif
#endif
#if defined(_M_X64)
	m_VersionIdentifier += _T("-- 64-bit");
#else
	m_VersionIdentifier += _T("-- 32-bit");
#endif
	}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_KEYSTROKES, m_HelpInfo);
	DDX_Text(pDX, IDC_VERSIONSTRING, m_VersionIdentifier);
	DDX_Text(pDX, IDC_LASCODES, m_LASClassificationLabels);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp

BEGIN_MESSAGE_MAP(CFullwindowApp, CWinApp)
	//{{AFX_MSG_MAP(CFullwindowApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp construction

CFullwindowApp::CFullwindowApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	LASLIBIsLoaded = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFullwindowApp object

CFullwindowApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp initialization

BOOL CFullwindowApp::InitInstance()
{
	ParseCommandLine(oInfo);

//	if (oInfo.IsMultipleCopy()) {
//		AfxMessageBox("Have switch");
//	}

 	// prevent multiple instances
	HWND oldapp = ::FindWindow(NULL, "PDQ");
	// check for 64 bit version...changes the window title
	if (!oldapp)
		oldapp = ::FindWindow(NULL, "PDQ64");
	if (oldapp != NULL && !oInfo.IsMultipleCopy()) {
		// tell running instance to load new data
		CFileSpec fs(CFileSpec::FS_APPDIR);
		fs.SetFileNameEx("_Tfer_.dat");
		FILE* f = fopen(fs.GetFullSpec(), "wt");
		fprintf(f, "%s\n", m_lpCmdLine);
		fclose(f);
		::PostMessage(oldapp, ::RegisterWindowMessage("OpenData"), 0, 0);
		return(FALSE);
	}

	// try to load LASlib
	if (laszip_load_dll() == 0)
		LASLIBIsLoaded = TRUE;
	else
		LASLIBIsLoaded = FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("USDA Forest Service -- Pacific Northwest Research Station"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	// need to load a different icon if we are 64-bit

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	pFrame->DragAcceptFiles();

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp message handlers
void CFullwindowApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp message handlers


BOOL CFullwindowApp::OnIdle(LONG lCount) 
{
	// trigger a frame update...comment this line out to prevent continuous drawing or use the /l command line option
	if (!oInfo.m_NoConstantRedraw)
		AfxGetApp()->GetMainWnd()->Invalidate();

	return(0);
}

BOOL CFullwindowApp::IsIdleMessage( MSG* pMsg )
{
	// enable idle time processing
	CWinApp::IsIdleMessage(pMsg);

	return TRUE;
}

void CFullwindowApp::OnFinalRelease() 
{
	// try to unload LASlib
	if (LASLIBIsLoaded)
		laszip_unload_dll();
	
	CWinApp::OnFinalRelease();
}

void CAboutDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int i;
	CWnd* ParentWnd = GetParent();

	if (nIDCtl == IDC_RETURNCOLORS && !IsIconic()) {
		CDC dc;
		COLORREF color;
		dc.Attach(lpDrawItemStruct->hDC);

		COLORREF m_LASReturnColors[15];
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
		m_LASReturnColors[13] = RGB(0, 0, 128);		// navy blue return 14
		m_LASReturnColors[14] = RGB(128, 0, 0);		// maroon return 15
		int NumColors = 15;

		// divide the rectangle into blocks for each color...colors are pre-defined
		int blockwidth = (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / NumColors;

		for (i = 0; i < NumColors; i ++) {
			color = m_LASReturnColors[i];
			dc.FillSolidRect(lpDrawItemStruct->rcItem.left + (i * blockwidth), lpDrawItemStruct->rcItem.top, blockwidth, lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top, color);
		}

		// draw text labels
		CFont font;
		font.CreateFont(
		  18,                        // nHeight
		  0,                         // nWidth
		  0,                         // nEscapement
		  0,                         // nOrientation
		  FW_BOLD,                 // nWeight
		  FALSE,                     // bItalic
		  FALSE,                     // bUnderline
		  0,                         // cStrikeOut
		  ANSI_CHARSET,              // nCharSet
		  OUT_DEFAULT_PRECIS,        // nOutPrecision
		  CLIP_DEFAULT_PRECIS,       // nClipPrecision
		  DEFAULT_QUALITY,           // nQuality
		  DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		  _T("Arial"));              // lpszFacename

		dc.SelectObject(font);

		CRect labelrc(lpDrawItemStruct->rcItem);
		CString LabelString;
		double intensity;

		labelrc.top = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / 4;

		for (i = 0; i < NumColors; i++) {
			labelrc.left = lpDrawItemStruct->rcItem.left + blockwidth * i;
			labelrc.right = labelrc.left + blockwidth;

			color = m_LASReturnColors[i];

			intensity = ((0.21 * (double)GetRValue(color)) + (0.72 * (double)GetGValue(color)) + (0.07 * (double)GetBValue(color))) / 255.0;
			if (intensity < 0.5)
				dc.SetTextColor(RGB(255, 255, 255));
			else
				dc.SetTextColor(RGB(0, 0, 0));

			dc.SetBkColor(color);
			if (i == 14) {
				labelrc.left -= 2;
				LabelString.Format("%i+", i + 1);
			}
			else
				LabelString.Format("%i", i + 1);
			dc.DrawText(LabelString, labelrc, DT_CENTER);
		}

		dc.Detach();
	}
	else if (nIDCtl == IDC_LASCOLORS && !IsIconic()) {
		CDC dc;
		COLORREF color;
		dc.Attach(lpDrawItemStruct->hDC);

		COLORREF m_LASClassColors[32];
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
		
		/*
		m_LASClassColors[0] = RGB(128, 128, 128);		// created, never classified
		m_LASClassColors[1] = RGB(192, 192, 192);		// unclassified
		m_LASClassColors[2] = RGB(128, 64, 0);			// ground
		m_LASClassColors[3] = RGB(0, 128, 0);			// low vegetation
		m_LASClassColors[4] = RGB(0, 192, 0);			// medium vegetation
		m_LASClassColors[5] = RGB(0, 255, 0);			// high vegetation
		m_LASClassColors[6] = RGB(0, 128, 192);			// building
		m_LASClassColors[7] = RGB(255, 0, 0);			// low point (noise)
		m_LASClassColors[8] = RGB(255, 255, 0);			// model key-point (mass point)
		m_LASClassColors[9] = RGB(0, 0, 255);			// water
		m_LASClassColors[10] = RGB(255, 128, 0);		// reserved
		m_LASClassColors[11] = RGB(255, 128, 0);		// reserved
		m_LASClassColors[12] = RGB(255, 0, 255);		// overlap points
		m_LASClassColors[13] = RGB(255, 128, 0);		// reserved
		m_LASClassColors[14] = RGB(0, 255, 255);		// withheld point
		*/
		int NumColors = 32;

		// divide the rectangle into blocks for each color...colors are pre-defined
		int blockwidth = (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / (NumColors / 2);
		int blockheight = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / 2;

		for (i = 0; i < NumColors / 2; i++) {
			color = m_LASClassColors[i];
			dc.FillSolidRect(lpDrawItemStruct->rcItem.left + (i * blockwidth), lpDrawItemStruct->rcItem.top, blockwidth, blockheight, color);
		}
		for (i = NumColors / 2; i < NumColors; i++) {
			color = m_LASClassColors[i];
			dc.FillSolidRect(lpDrawItemStruct->rcItem.left + ((i - NumColors / 2) * blockwidth), lpDrawItemStruct->rcItem.top + blockheight, blockwidth, blockheight, color);
		}

		// draw text labels
		CFont font;
		font.CreateFont(
		  18,                        // nHeight
		  0,                         // nWidth
		  0,                         // nEscapement
		  0,                         // nOrientation
		  FW_BOLD,                 // nWeight
		  FALSE,                     // bItalic
		  FALSE,                     // bUnderline
		  0,                         // cStrikeOut
		  ANSI_CHARSET,              // nCharSet
		  OUT_DEFAULT_PRECIS,        // nOutPrecision
		  CLIP_DEFAULT_PRECIS,       // nClipPrecision
		  DEFAULT_QUALITY,           // nQuality
		  DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		  _T("Arial"));              // lpszFacename

		dc.SelectObject(font);

		CRect labelrc(lpDrawItemStruct->rcItem);
		CString LabelString;
		double intensity;

		labelrc.top = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / 8;
		labelrc.bottom = labelrc.top + blockheight;

		for (i = 0; i < NumColors / 2; i ++) {
			labelrc.left = lpDrawItemStruct->rcItem.left + blockwidth * i;
			labelrc.right = labelrc.left + blockwidth;

			color = m_LASClassColors[i];

			intensity = ((0.21 * (double) GetRValue(color)) + (0.72 * (double) GetGValue(color)) + (0.07 * (double) GetBValue(color))) / 255.0;
			if (intensity < 0.5)
				dc.SetTextColor(RGB(255, 255, 255));
			else
				dc.SetTextColor(RGB(0, 0, 0));

			dc.SetBkColor(color);
			LabelString.Format("%i", i);

			dc.DrawText(LabelString, labelrc, DT_CENTER);
		}

		labelrc.top += blockheight;
		labelrc.bottom = labelrc.top + blockheight;

		for (i = NumColors / 2; i < NumColors; i++) {
			labelrc.left = lpDrawItemStruct->rcItem.left + blockwidth * (i - (NumColors / 2));
			labelrc.right = labelrc.left + blockwidth;

			color = m_LASClassColors[i];

			intensity = ((0.21 * (double)GetRValue(color)) + (0.72 * (double)GetGValue(color)) + (0.07 * (double)GetBValue(color))) / 255.0;
			if (intensity < 0.5)
				dc.SetTextColor(RGB(255, 255, 255));
			else
				dc.SetTextColor(RGB(0, 0, 0));

			dc.SetBkColor(color);
			LabelString.Format("%i", i);

			dc.DrawText(LabelString, labelrc, DT_CENTER);
		}

		dc.Detach();
	}
	else {
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}
