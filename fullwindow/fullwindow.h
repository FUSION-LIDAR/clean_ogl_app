// fullwindow.h : main header file for the FULLWINDOW application
//
// V1.7 5/15/2009
// increased maximum zoom level by a factor of 2
// added scanning capability for checking DTM quality
// changed program banner to drop "UW"
//
// 11/25/2009 PDQ V1.8
//	Added "I" key to toggle display of axes
//
// 3/30/2010 PDQ V1.81
//	Added very small marker at origin on the axes to help with data orientation
//
// 6/4/2010 PDQ V1.82
//	Added ability to color using intensity values. Use "N" to toggle between
//	elevation and intensity coloring
//
// 8/26/2010 PDQ V1/83
//	Added support for LAS files with RGB information for each point. PDQ will
//	use the RGB color to color the points. The intensity (N) and LAS classification
//	coloring methods will override the RGB colors when toggled on.
//
// 11/2/2010 PDQ V1.84
//	Minor change in the description of keyboard commands in the Help...About dialog. "N" toggles
//	between using the intensity data to color returns. Previous versions indicated that "N"
//	toggled between RGB using values and point elevation for coloring points.
//
// 12/9/2010 PDQ V1.90
//	Added optional rangepoles to help establish scale in point cloud and surface displays. Rangepoles can be a scaled to the data or
//	a fixed size. The rangepole height can optionally be considered when computing the scaling for the data display. This allows you 
//	to compare various point clouds or produce a series of images that can be directly compared since the display scaling is controlled
//	by the rangepole height.
//
//	Rangepole display is controlled on the View menu and the options are on the File...Rangepole options... menu
//
//	Using the rangepole height to scale the data view works best when you load samples that cover the same size footprint on the ground.
//	If you load different size samples, the XY size may override the Z size for the scaling. In addition if the height of the rangepole
//	is less than the Z range in the data, the scaling will be controlled by the data and not the rangepole height.
//
// 4/13/2011 PDQ V2.0...didn't increment version number for code so this V2.00 was never released
//	Added /m command line option to allow multiple instances of PDQ and /s command line option to synchronize multiple copies. To use these
//	options, you must start PDQ from a DOS prompt or batch file. Synchronization includes to viewing geometry, point size, and focal length.
//
//	Also added "Clone PDQ" to menu to launch another instance of PDQ that is syncronized with the current instance. The role of a particular
//	instance of PDQ is displayed in the lower right corner of the heads-up information. The "master" role will control all other running 
//	instances of PDQ and the "slave" role only listens and will not send viewing updates to other instances of PDQ. It is possible to have 
//	several instances of PDQ in the "master" role.
//
//	Also added code to use the correct icon for the window's title bar
//
// 3/28/2012 PDQ V 1.91
//	Modified logic that assigns point colors using LAS RGB information to correctly scale the color components. Previous versions
//	assumed the color components ranged from 0-255. Modifications scan the RGB colors and figure out if the range is 0-255 or 0 - 65025.
//	The LAS standard does not specify the component range but allows a 16-bit value for each of the red, green, and blue components.
//
// 8/16/2012 PDQ V2.00
//	Changed LAS reading code to use LASlib and modified some of the VC++ project options (use MFC in static library and runtime as static library).
//	Initial tests show that the LASlib code is about 3x faster than the original code to read LAS data and the change adds support for zipped LAS files.
//	Non-LAS format files are still read using the original code.
//
// 6/7/2013 PDQ V2.10
// Added support for roaming through the data using a translation controller. This is something that I tried to add
// to the base OpenGL classes in the beginning but could never get it to work correctly. The new code allows you to 
// switch between viewing modes using the "V" key. The view is reset each time you switch viewing modes. This isn't 
// necessarily the most desirable behavior but it was easier to set up to reset. In the future I may want to add a second
// camera so you can maintain the settings for each viewing mode as you switch between modes.
//
// Also added surface following when viewing a surface. The offset from the surface is fixed at 10 units...may want to change
// this at some point to be user defined.
//
// Cloned views don't work when roaming.
//
//	8/15/2013 PDQ V2.20
//	Added support for the LASzip.dll developed by Martin Isenburg to read and write LAS and LAZ files. To use the library you simply need to copy 
//	it to the same folder where FUSION is installed. If the library is present, it will be used. If it is not present, the older code that does not support
//	compressed LAS data will be used. Use of the /nolaszipdll option in any program will suppress loading of the library and will remove support
//	for compressed LAS data for that instance of the program.
//
// 5/7/2014 PDQ 2.30
//	Added support for ASCII raster files. Drawing style is a flat plate with the cells rendered using a standard color ramp. This doesn't provide the
//	best way to view the data but it does allow a quick check for raster outputs from FUSION programs.
//
// 9/23/14 PDQ 2.40
//	Improved handling of large DTM files so that they re displayed as wireframe meshes when PDQ cannot render them as a shaded surface. Also fixed some
//	memory alloction problems related to large DTMs.
//
// 10/15/2014 PDQ v2.41
//	Added color by return option (j). Also cleaned up some of the information in the About dialog and added a legend for the return colors. Current support 
//	is for 5 returns with returns 0 and >5 set to magenta
//
// 2/2/2017 PDQ v2.42
//	Made some changes to accomodate LAS V1.4 files and the new point records for this format. Mainly changed the code to use RGB to color points. Added support
//	for coloring using false-color NIR ("f" to activate)
//
// 9/29/2017 PDQ V2.43
//	Modified the behavior when cloning PDQ instances so that the window size of the master window is used to size the slave windows. If you change the size of
//	the master window. the slave windows will also be resized. In addition, I changed the behavior of the slave windows so they cannot clone additional
//	instances of PDQ.
//
// 5/15/2018 PDQ V2.44
//	Added and option to prevent continuous drawing (/l) to help in situations where PDQ is running from a server. This dramatically reduces the CPU load
//	associated with PDQ.
//
// 6/4/2018 PDQ V2.45
//	Added new colors to support up to 15 returns per pulse and changed they way the color bars are drawn in the about dialog.
//
// 8/10/2018 PDQ V2.46
//	Modified the logic used when loading and rendering large DTM files. PDQ can now handle much larger files/grids. When the model is too large to render as
//	a shaded surface, it is rendered as a colored wireframe. It is still possible to crash PDQ with really large models but this behavior depends on the type and 
//	memory available on the computer's graphics card. The error seems to happen in a card related dll and the behavior varies depending on the type of card.
//	Overall the improvements result in much more robust handling of large models.
//
//	Also made some changes to the motion controls when flying through data or over ground models to make the control behavior more useful. Previous versions
//	tended to look into the ground when moving uphill. Use [V] to toggle this mode.
//
//	Also changed the way ASCII raster files are loaded so that the points are divided into subsets. This allows for larger grids and helps drawing performance.
//	When rendering grids, point anti-aliasing is turned off. You may need to change the point size to achieve a good image of smaller grids.
//
//	If you have a large DTM loaded and try to load another DTM, you may get the message that there wasn't enough memory for the normal vectors. When this 
//	happens, you may be able to load the same model into a new instance of PDQ (exit the running copy and launch again with the new model or drop the
//	new model onto a new running copy of PDQ. This appears to be a memory management issue but I'm not sure if the problem is in my code or something in 
//	the way windows manages and releases memory.
//
//	9/4/2018 PDQ V2.50
//	Major code changes to move to "modern" development environment (MS Visual Studio 2017). Lots of small code changes needed to change environments and prepare for 
//	64-bit version.
//
//	9/17/2018 PDQ V2.50
//	Also changes to the translation-mode controller (fly through data) to use a rough ground surface to set the viewpoint elevation when within the point data. Added
//	depth cueing (fog) to help differentiate near and far points and added controls (+ and -) to raise and lower the viewpoint relative the rough ground surface. The 
//	rough ground surface is built using the lowest points in the cell and the surface is always 64 by 64 cells. There can be cases where that are no points on the 
//	ground within the cell resulting in a jump in the surface. Hopefully these cases will be rare. There is no smoothing or hole-filling for the rough surface.
//
//	1/28/2019 PDQ V2.51
//	Fixed a problem with the color used to show withheld points when coloring according the LAS classification values. Previous versions used color 12 instead of 
//	color 14 and the withheld flag was not being correctly detected for LAS 1.4 format files that used point record formats 6 and higher.
//
//	Also reworked the colors used when coloring by classification code to use 32 colors and added a dialog to interactively control which returns are colored. This 
//	revision allows control over 32 classes. I also implemented a dialog that allows control over all possible classes (256) but it gets a bit busy and I suspect that
//	almost all users are only interested in the first 32 classes. Dialog is accessed by pressing "L" (not case sensitive) when viewing a point file.
//
//	6/17/2019 PDQ V2.52
//	Updated logic to modify the program name shown in the title bar to reflect 32- or 64-bit builds. Name will be "PDQ" for 32-bit build and "PDQ64" for 64-bit build.
//
//	8/28/2019 PDQ V2.53
//	Minor update to clean up reporting of LASzip.dll in the about box.
//
//	3/16/2020 PDQ V2.54
//	Minor revision to change message when skipping cells when drawing surfaces so no message is displayed when all cells are drawn (full resolution used for drawing).
//	Previous versions would display "Skipping every 1 cells"...I thought this was misleading.
//
//
//
//
//
//
//
//
//
//
//
//
//	8/16/2018
//	I spent some time trying to figure out why you can load a large DTM when PDQ is not running but when you try to load the same DTM into a running copy of PDQ
//	that aleady has a model loaded, you can't render as a shaded surface and have to drop back to a wireframe. There seems to be a memory "leak" so that not all
//	memory used with a DTM is being "freed". I added memory status code to the headsup display but things didn't really make sense. Memory would drop for several
//	DTM loads and then start increasing. It seemed to matter if you were loading a model smaller than the one currently loaded or larger. In the end (after 6-10 hours)
//	I didn't figure anything out. I added a message to the change log telling users to try loading a large model from a new instance of PDQ when they get the 
//	wireframe message.
//
#if !defined(AFX_FULLWINDOW_H__14F875B0_C68E_4FB1_8E22_EEEF1ED9EDE9__INCLUDED_)
#define AFX_FULLWINDOW_H__14F875B0_C68E_4FB1_8E22_EEEF1ED9EDE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define		PROGRAM_VERSION		2.54

#include "resource.h"       // main symbols

class CCustomCommandLineInfo : public CCommandLineInfo
{
public:
	CCustomCommandLineInfo() {
		m_RunMultipleCopy = m_SynchronizeCopies = m_NoConstantRedraw = FALSE;		// default is single copy with data reload
	}

	// flags for command line switches
	BOOL m_RunMultipleCopy;			// for /m
	BOOL m_SynchronizeCopies;		// for /s
	BOOL m_NoConstantRedraw;		// for /l

	//public methods for checking these.
public:
	BOOL IsMultipleCopy() { return m_RunMultipleCopy; };
	BOOL IsSynchronize() { return m_SynchronizeCopies; };
	BOOL IsNoConstantRedraw() { return m_NoConstantRedraw; };

	virtual void ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast) {
		if (_stricmp(pszParam, "m") == 0) {
			m_RunMultipleCopy = TRUE;
		} 
		else if (_stricmp(pszParam, "s") == 0) {
			m_SynchronizeCopies = TRUE;
		} 
		else if (_stricmp(pszParam, "l") == 0) {
			m_NoConstantRedraw = TRUE;
		} 
	}
};

/////////////////////////////////////////////////////////////////////////////
// CFullwindowApp:
// See fullwindow.cpp for the implementation of this class
//

class CFullwindowApp : public CWinApp
{
public:
	CFullwindowApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFullwindowApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL IsIdleMessage( MSG* pMsg );
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	CCustomCommandLineInfo oInfo;
	BOOL LASLIBIsLoaded;

// Implementation

public:
	//{{AFX_MSG(CFullwindowApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FULLWINDOW_H__14F875B0_C68E_4FB1_8E22_EEEF1ED9EDE9__INCLUDED_)
