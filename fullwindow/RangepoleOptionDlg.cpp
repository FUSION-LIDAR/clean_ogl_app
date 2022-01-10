// RangepoleOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fullwindow.h"
#include "RangepoleOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RangepoleOptionDlg dialog


RangepoleOptionDlg::RangepoleOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RangepoleOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(RangepoleOptionDlg)
	m_DrawPoleLL = FALSE;
	m_DrawPoleLR = FALSE;
	m_DrawPoleMB = FALSE;
	m_DrawPoleML = FALSE;
	m_DrawPoleMR = FALSE;
	m_DrawPoleMT = FALSE;
	m_DrawPoleUL = FALSE;
	m_DrawPoleUR = FALSE;
	m_DrawPoleCenter = FALSE;
	m_RangepoleTotalHeight = 0.0;
	m_RangepoleSegmentHeight = 0.0;
	m_ScaleRangepoleToData = FALSE;
	m_UseRangepoleHeightToScaleData = FALSE;
	//}}AFX_DATA_INIT
}


void RangepoleOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RangepoleOptionDlg)
	DDX_Check(pDX, IDC_LOWERLEFT, m_DrawPoleLL);
	DDX_Check(pDX, IDC_LOWERRIGHT, m_DrawPoleLR);
	DDX_Check(pDX, IDC_MIDDLEBOTTOM, m_DrawPoleMB);
	DDX_Check(pDX, IDC_MIDDLELEFT, m_DrawPoleML);
	DDX_Check(pDX, IDC_MIDDLERIGHT, m_DrawPoleMR);
	DDX_Check(pDX, IDC_MIDDLETOP, m_DrawPoleMT);
	DDX_Check(pDX, IDC_UPPERLEFT, m_DrawPoleUL);
	DDX_Check(pDX, IDC_UPPERRIGHT, m_DrawPoleUR);
	DDX_Check(pDX, IDC_CENTER, m_DrawPoleCenter);
	DDX_Text(pDX, IDC_TOTALHEIGHT, m_RangepoleTotalHeight);
	DDV_MinMaxDouble(pDX, m_RangepoleTotalHeight, 0., 10000.);
	DDX_Text(pDX, IDC_SEGMENTHEIGHT, m_RangepoleSegmentHeight);
	DDV_MinMaxDouble(pDX, m_RangepoleSegmentHeight, 0., 10000.);
	DDX_Check(pDX, IDC_SCALETODATA, m_ScaleRangepoleToData);
	DDX_Check(pDX, IDC_USERANGEPOLETOSCALESDATA, m_UseRangepoleHeightToScaleData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RangepoleOptionDlg, CDialog)
	//{{AFX_MSG_MAP(RangepoleOptionDlg)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_BN_CLICKED(IDC_CENTERONLY, OnCenteronly)
	ON_BN_CLICKED(IDC_CORNERONLY, OnCorneronly)
	ON_BN_CLICKED(IDC_SIDESONLY, OnSidesonly)
	ON_BN_CLICKED(IDC_SCALETODATA, OnScaletodata)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RangepoleOptionDlg message handlers

void RangepoleOptionDlg::OnAll() 
{
	UpdateData();

	m_DrawPoleLL = TRUE;
	m_DrawPoleUL = TRUE;
	m_DrawPoleUR = TRUE;
	m_DrawPoleLR = TRUE;
	m_DrawPoleML = TRUE;
	m_DrawPoleMT = TRUE;
	m_DrawPoleMR = TRUE;
	m_DrawPoleMB = TRUE;
	m_DrawPoleCenter = TRUE;

	UpdateData(FALSE);
}

void RangepoleOptionDlg::OnCenteronly() 
{
	UpdateData();

	m_DrawPoleLL = FALSE;
	m_DrawPoleUL = FALSE;
	m_DrawPoleUR = FALSE;
	m_DrawPoleLR = FALSE;
	m_DrawPoleML = FALSE;
	m_DrawPoleMT = FALSE;
	m_DrawPoleMR = FALSE;
	m_DrawPoleMB = FALSE;
	m_DrawPoleCenter = TRUE;

	UpdateData(FALSE);
}

void RangepoleOptionDlg::OnCorneronly() 
{
	UpdateData();

	m_DrawPoleLL = TRUE;
	m_DrawPoleUL = TRUE;
	m_DrawPoleUR = TRUE;
	m_DrawPoleLR = TRUE;
	m_DrawPoleML = FALSE;
	m_DrawPoleMT = FALSE;
	m_DrawPoleMR = FALSE;
	m_DrawPoleMB = FALSE;
	m_DrawPoleCenter = FALSE;

	UpdateData(FALSE);
}

void RangepoleOptionDlg::OnSidesonly() 
{
	UpdateData();

	m_DrawPoleLL = FALSE;
	m_DrawPoleUL = FALSE;
	m_DrawPoleUR = FALSE;
	m_DrawPoleLR = FALSE;
	m_DrawPoleML = TRUE;
	m_DrawPoleMT = TRUE;
	m_DrawPoleMR = TRUE;
	m_DrawPoleMB = TRUE;
	m_DrawPoleCenter = FALSE;

	UpdateData(FALSE);
}

void RangepoleOptionDlg::OnScaletodata() 
{
	UpdateData();

	// set status of height text box
	if (m_ScaleRangepoleToData) {
		GetDlgItem(IDC_TOTALHEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USERANGEPOLETOSCALESDATA)->EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_TOTALHEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USERANGEPOLETOSCALESDATA)->EnableWindow(TRUE);
	}
}

void RangepoleOptionDlg::OnNone() 
{
	UpdateData();

	m_DrawPoleLL = FALSE;
	m_DrawPoleUL = FALSE;
	m_DrawPoleUR = FALSE;
	m_DrawPoleLR = FALSE;
	m_DrawPoleML = FALSE;
	m_DrawPoleMT = FALSE;
	m_DrawPoleMR = FALSE;
	m_DrawPoleMB = FALSE;
	m_DrawPoleCenter = FALSE;

	UpdateData(FALSE);
}

BOOL RangepoleOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnScaletodata();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
