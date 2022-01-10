// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__D03D2335_1D6B_413F_BFFE_12375AC02D77__INCLUDED_)
#define AFX_CHILDVIEW_H__D03D2335_1D6B_413F_BFFE_12375AC02D77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\openglwnd.h"

#include "3DSModel.h"
#include "3DSModelObject.h"
#include "CClassificationCodeDlg32colors.h"
#include "datafile.h"
#include "TextureMap.h"	// Added by ClassView
#include "..\..\COMMON\FUSION_UTIL\plansdtm.h"	// Added by ClassView

//#include "..\..\SCATTER\LDV\WriteAVI.h"	// Added by ClassView

#define		POINTDATA	1
#define		SURFACE		2
#define		MODEL		4
#define		PANORAMA	8
#define		IMAGEPLATE	16
#define		JACK		32
#define		CLOCK		64

// color modes
#define		COLORBYHEIGHT			0
#define		COLORBYINTENSITY		1
#define		COLORBYCLASSIFICATION	2
#define		COLORBYRGB				3
#define		COLORBYRETURN			4
#define		COLORBYNIR				5

/////////////////////////////////////////////////////////////////////////////
// CChildView window

#define	DATALAYERS	24

typedef struct {
	double x;
	double y;
	double z;
	double w;
	double focallength;
	float pointsize;
	HWND hwnd;
	int window_width;
	int window_height;
} Sync_orientation;

class CChildView : public COpenGLWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	typedef struct {
		double x;
		double y;
		double z;
		COLORREF color;
	} COLOREDPOINT;
	COLOREDPOINT* m_PointList;

	BOOL m_LASClassState[256];
	BOOL m_LASSynthetic;
	BOOL m_LASOverlap;
	BOOL m_LASKeypoint;
	BOOL m_LASWithheld;
	BOOL m_ShowOnlyPointsWithFlags;

	// flags to tell if data has various LAS classes and classification flags
	BOOL m_DataHasClass[256];
	BOOL m_DataHasSynthetic;
	BOOL m_DataHasOverlap;
	BOOL m_DataHasKeypoint;
	BOOL m_DataHasWithheld;

#ifdef _DEBUG
	CMemoryState oldMemState, newMemState, diffMemState;
#endif

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	double m_FlyballOffset;
	CString m_HighResolutionMessage;
	CString m_LowResolutionMessage;
	BOOL m_SurfaceIsWireframe;
	void DrawImagePlate();
	void CreateImagePlate(LPCTSTR FileName);
	void CreateImagePlateAsPointLayers(LPCTSTR FileName);
	void CreateImagePlateAsInterleavedPointLayers(LPCTSTR FileName);
	PlansDTM m_TerrainForFlyball;
	BOOL m_HaveTerrainForFlyball;
	BOOL m_ShowHeadsUpInfo;
	BOOL m_RespondingToSync;
	BOOL m_InZoom;
	void SendSync();
	BOOL m_NeedRescale;
	BOOL m_UseRangepoleHeightToScaleData;
	double m_ScaledVerticalSpan;
	double m_ActualVerticalSpan;
	void OnDrawGLBeforeTrackball();
	void DrawRangePole();
	void CreateRangePole(double PoleHeight, double HeightStep, double PoleDiameter);
	GLuint m_MarkerTextureName;
	CTextureMap m_MarkerTexture;
	BOOL m_LoadingData;
	int m_ScanPositionIndex;
	BOOL m_QAModelScan;
	double m_ScanOffsetX;
	double m_ScanOffsetY;
	double m_ScanOffsetZ;
	int m_PointFileFormat;
	CString m_PointFileName;
	CString m_SurfaceFileName;
	COLORREF m_LASClassColors[32];
	COLORREF m_LASReturnColors[15];
	CString m_AVIFileName;
	BOOL m_RecordFramesToAVI;
	CAVIFile m_AVIFile;
	BOOL m_DisplayLowResSurface;
	double m_ScaledMinX, m_ScaledMinY, m_ScaledMinZ;
	double m_ScaledMaxX, m_ScaledMaxY, m_ScaledMaxZ;
	BOOL m_SBETViewer;
	double m_Roll, m_Pitch, m_TrueHeading;
	int m_SBETRecordCount, m_SBETRecords, m_SBETSpeedStep;
	double m_SBETMinLat, m_SBETMaxLat, m_SBETMinLong, m_SBETMaxLong, m_SBETMinEllHt, m_SBETMaxEllHt;
	double m_GPSTime, m_Lat, m_Long, m_EllHt;
	char m_SBETLineBuffer[1024];
	CDataFile m_SBETDataFile;
	void ResetCamera();
	void DeleteDataObject();
	void CreateModel(LPCTSTR FileName);
	void DrawModel();
	void Create3DFigure();
	void DrawSurface();
	void DrawDataObject();
	void CreateSurface(LPCTSTR FileName);
	void CreateDataObject(LPCTSTR FileName);
	void DrawHeadsUpInfo(int UpdateInterval=10);
	int m_PointCount;
	int m_DrawnPointCount;
	float m_PointSize;
	void DrawPointCloud();
	void DrawASCIIPointCloud();
	void CreatePointCloud(LPCTSTR FileName = "", int Format = 0);
	void DrawCylinder();
	void DrawClockface();
	void CreateCylinder(LPCTSTR FileName, int Format);
	void ReadPointCloud(LPCTSTR FileName);
	double m_CurrentTime;
	void OnUpdateGLState();
	int m_StartTime;
	void OnCreateGL();
	void OnSizeGL(int cx, int cy); // override to adapt the viewport to the window
	double m_PoleBaseZ;
//	CGLDispList m_RangePole;
	CGLDispList m_RangePoleRedStripes;
	CGLDispList m_RangePoleWhiteStripes;
	CGLDispList m_Object;
	CGLDispList m_SecondHand;
	CGLDispList m_MinuteHand;
	CGLDispList m_HourHand;
	CGLDispList m_PointLayer[DATALAYERS];
	CGLDispList m_SurfaceLayer;
	CGLDispList m_LORESSurfaceLayer;
	CGLDispList m_ImagePlate;
	C3DSModel m_ObjectModel;
	void OnDrawGL();
	void ModifyCameraLocation();
	void VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf, BOOL &stereo, BOOL &anaglyph, BOOL &splitscreen, int &alignment, int &projection_type);
	virtual ~CChildView();

	void ClassificationCodeDlgFeedback(BOOL Redraw = TRUE);
	void OpenClassificationCodeDlg();
	void ClassificationCodeDlgDone();
	CClassificationCodeDlg* m_ClassificationCodeDlg;

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnViewAxes();
	afx_msg void OnUpdateViewAxes(CCmdUI* pCmdUI);
	afx_msg void OnViewPoints();
	afx_msg void OnUpdateViewPoints(CCmdUI* pCmdUI);
	afx_msg void OnFileRangepoleoptions();
	afx_msg void OnViewRangepoles();
	afx_msg void OnUpdateViewRangepoles(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
//	LRESULT CChildView::OnJoy1ButtonDown(WPARAM wParam, LPARAM lParam);
//	LRESULT CChildView::OnJoy1Move(WPARAM wParam, LPARAM lParam);
//	LRESULT CChildView::OnJoy1ButtonUp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_DrawAxes;
	BOOL m_DrawPointData;
	BOOL m_DrawRangePoles;
	BOOL m_DrawPoleCenter;
	BOOL m_DrawPoleLL;
	BOOL m_DrawPoleUL;
	BOOL m_DrawPoleUR;
	BOOL m_DrawPoleLR;
	BOOL m_DrawPoleML;
	BOOL m_DrawPoleMT;
	BOOL m_DrawPoleMR;
	BOOL m_DrawPoleMB;
	double m_RangepoleSegmentHeight;
	double m_RangepoleTotalHeight;
	BOOL m_ScaleRangepoleToData;
	int m_ColorMode;
	int m_DataType;
	int m_LayersDrawnInLastFrame;
	void DrawPointList();
	void CreateJack();
	void CreateAxes();
	void CreateClockFace();
public:
	void SetupFog(bool Enable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__D03D2335_1D6B_413F_BFFE_12375AC02D77__INCLUDED_)
