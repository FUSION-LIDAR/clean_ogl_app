// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "fullwindow.h"
#include "filespec.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
const UINT    wm_RemoteOpen = RegisterWindowMessage("OpenData");
const UINT    wm_Synchronize = RegisterWindowMessage("PDQSynchView");

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_CLONEPDQ, OnClonepdq)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(wm_RemoteOpen, OnRemoteOpen)
	ON_REGISTERED_MESSAGE(wm_Synchronize, OnSynchronize)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0, 0, 0, hIcon);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMainFrame::OnRemoteOpen(WPARAM wParam, LPARAM lParam)
{
	char buf[1025];
	CFileSpec fs(CFileSpec::FS_APPDIR);
	fs.SetFileNameEx("_Tfer_.dat");
	FILE* f = fopen(fs.GetFullSpec(), "rt");
	fgets(buf, 1024, f);
	fclose(f);
	DeleteFile(fs.GetFullSpec());
	CString NewFile = _T(buf);
	NewFile.TrimRight();
	NewFile.Replace("\"", "");
	if (!NewFile.IsEmpty()) {
		m_wndView.m_LoadingData = TRUE;
		m_wndView.RedrawWindow();

		m_wndView.BeginGLCommands();
		m_wndView.CreateDataObject(NewFile);
		m_wndView.EndGLCommands();

		m_wndView.m_LoadingData = FALSE;
		return(TRUE);
	}
	else
		return(FALSE);
}

LRESULT CMainFrame::OnSynchronize(WPARAM wParam, LPARAM lParam)
{
	// not sure is we want every instance to respond of only when /s is on the command line...
	// ((CFullwindowApp*) AfxGetApp())->oInfo.IsSynchronize()
	if (!m_wndView.m_InMotion) {
		// get data from clipboard
		OpenClipboard(); 
		HANDLE handle = GetClipboardData(RegisterClipboardFormat("Sync_orientation")); 

		if (handle) {
			Sync_orientation* so = (Sync_orientation *) ::GlobalLock((HGLOBAL) handle); 
			if (so) {
				if (so->hwnd != m_wndView.GetSafeHwnd()) {
					vector v(so->x, so->y, so->z);
					quaternion q(so->w, v);
					unitquaternion uq(q);
					m_wndView.m_TrackBall.SetBallOrientation(uq);

					// Unlock 
					::GlobalUnlock((HGLOBAL) handle); 

					m_wndView.m_Camera.focallength = so->focallength;
					m_wndView.m_PointSize = so->pointsize;

					m_wndView.m_RespondingToSync = TRUE;

					// resize window
					SetWindowPos(NULL, 0, 0, so->window_width, so->window_height, SWP_NOMOVE | SWP_NOZORDER);

					m_wndView.Invalidate();
				}
			}
		}
		CloseClipboard();
	}

	return(FALSE);
}

void CMainFrame::OnFileOpen() 
{
	CString csFilter;
	CString csExt;

	csFilter.LoadString(IDS_ALLFILES);
	csExt = _T("");

	CFileDialog fd(TRUE, csExt, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, csFilter, (CWnd*) this);

	if (fd.DoModal() == IDOK) {
		CString DataFileName = fd.GetPathName();

		m_wndView.m_LoadingData = TRUE;
		m_wndView.RedrawWindow();

		m_wndView.BeginGLCommands();
		m_wndView.CreateDataObject(DataFileName);
		m_wndView.EndGLCommands();

		m_wndView.m_LoadingData = FALSE;

		Invalidate();
	}
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char namebuf[1025];
	if (DragQueryFile(hDropInfo, 0xFFFFFFFF, namebuf, 1024) > 1) {
		AfxMessageBox("Drag and drop is not supported for multiple files");
	}
	else {
		DragQueryFile(hDropInfo, 0, namebuf, 1024);

		m_wndView.m_LoadingData = TRUE;
		m_wndView.RedrawWindow();

		m_wndView.BeginGLCommands();
		m_wndView.CreateDataObject(namebuf);
		m_wndView.EndGLCommands();


		m_wndView.m_LoadingData = FALSE;
		Invalidate();
	}

	DragFinish(hDropInfo);
}


void CMainFrame::OnClonepdq() 
{
	if (((CFullwindowApp*) AfxGetApp())->oInfo.m_RunMultipleCopy == FALSE) {
		// get module path and executable name
		CFileSpec afs(CFileSpec::FS_BUILTINS::FS_APP);

		// turn on syncronization for this instance
		((CFullwindowApp*) AfxGetApp())->oInfo.m_SynchronizeCopies = TRUE;

		// open new instance with /m option...don't need /s unless you want the new instance to be able to control this instance
		CString csCLine;
		csCLine.Format("%s /m", afs.GetFullSpec());

		WinExec(csCLine, SW_SHOW);

		m_wndView.SendSync();
	}
}


void CMainFrame::SetAppName(LPCTSTR Title)
{
	m_strTitle = Title;
}
