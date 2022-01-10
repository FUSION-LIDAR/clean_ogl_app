// CClassificationCodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fullwindow.h"
#include "ChildView.h"
#include "CClassificationCodeDlg.h"
#include "afxdialogex.h"


// CClassificationCodeDlg dialog

IMPLEMENT_DYNAMIC(CClassificationCodeDlg, CDialog)

CClassificationCodeDlg::CClassificationCodeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CLASS_LIST, pParent)
	, m_Synthetic(FALSE)
	, m_Overlap(FALSE)
	, m_Keypoint(FALSE)
	, m_Withheld(FALSE)
{
	m_pParent = pParent;
	m_nID = IDD_CLASS_LIST;

	for (int i = 0; i < 8; i++)
		m_ColumnChecks[i] = TRUE;

	for (int i = 0; i < 256; i++)
		m_ShowClass[i] = TRUE;
}

CClassificationCodeDlg::~CClassificationCodeDlg()
{
}

void CClassificationCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	for (int i = 0; i < 8; i++)
		DDX_Check(pDX, IDC_ALL_COLUMN1 + i, m_ColumnChecks[i]);

	for (int i = 0; i < 256; i++)
		DDX_Check(pDX, IDC_CLASS0 + i, m_ShowClass[i]);

	DDX_Check(pDX, IDC_SYNTHETIC, m_Synthetic);
	DDX_Check(pDX, IDC_OVERLAP, m_Overlap);
	DDX_Check(pDX, IDC_KEYPOINT, m_Keypoint);
	DDX_Check(pDX, IDC_WITHHELD, m_Withheld);
}


BEGIN_MESSAGE_MAP(CClassificationCodeDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CClassificationCodeDlg::OnBnClickedCancel)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_APPLY, &CClassificationCodeDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_ALL_COLUMN1, &CClassificationCodeDlg::OnClickedAllColumn1)
	ON_BN_CLICKED(IDC_ALL_COLUMN2, &CClassificationCodeDlg::OnClickedAllColumn2)
	ON_BN_CLICKED(IDC_ALL_COLUMN3, &CClassificationCodeDlg::OnClickedAllColumn3)
	ON_BN_CLICKED(IDC_ALL_COLUMN4, &CClassificationCodeDlg::OnClickedAllColumn4)
	ON_BN_CLICKED(IDC_ALL_COLUMN5, &CClassificationCodeDlg::OnClickedAllColumn5)
	ON_BN_CLICKED(IDC_ALL_COLUMN6, &CClassificationCodeDlg::OnClickedAllColumn6)
	ON_BN_CLICKED(IDC_ALL_COLUMN7, &CClassificationCodeDlg::OnClickedAllColumn7)
	ON_BN_CLICKED(IDC_ALL_COLUMN8, &CClassificationCodeDlg::OnClickedAllColumn8)
END_MESSAGE_MAP()


// CClassificationCodeDlg message handlers


void CClassificationCodeDlg::PostNcDestroy()
{
	// delete the dlg
	delete this;
}


void CClassificationCodeDlg::OnOK()
{
	// notify parent the dialog is gone so it can clean up its pointer
	((CChildView*)m_pParent)->ClassificationCodeDlgDone();


	CDialog::OnOK();
}


BOOL CClassificationCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CClassificationCodeDlg::Create()
{
	BOOL retcode = CDialog::Create(m_nID, m_pParent);
	if (retcode) {
	}

	return(retcode);
}


void CClassificationCodeDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CClassificationCodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
		OnOK();
	else
		CDialog::OnSysCommand(nID, lParam);
}


void CClassificationCodeDlg::OnBnClickedApply()
{
	UpdateData();

	((CChildView*)m_pParent)->ClassificationCodeDlgFeedback();
}


void CClassificationCodeDlg::OnClickedAllColumn1()
{
	UpdateData();

	int Column = 0;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i ++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn2()
{
	UpdateData();

	int Column = 1;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn3()
{
	UpdateData();

	int Column = 2;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn4()
{
	UpdateData();

	int Column = 3;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn5()
{
	UpdateData();

	int Column = 4;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn6()
{
	UpdateData();

	int Column = 5;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn7()
{
	UpdateData();

	int Column = 6;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}


void CClassificationCodeDlg::OnClickedAllColumn8()
{
	UpdateData();

	int Column = 7;
	int BaseClass = Column * 32;

	if (m_ColumnChecks[Column]) {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = TRUE;
	}
	else {
		for (int i = BaseClass; i < BaseClass + 32; i++)
			m_ShowClass[i] = FALSE;
	}

	UpdateData(FALSE);
}
