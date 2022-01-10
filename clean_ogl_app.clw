; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=COpenGLWnd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "clean_ogl_app.h"
LastPage=0

ClassCount=6
Class1=COGLapp
Class2=CCleanGLWnd
Class3=CImageProgress
Class4=CAboutDlg
Class5=COGLappDlg
Class6=COpenGLWnd

ResourceCount=3
Resource1=IDD_CLEAN_OGL_APP_DIALOG
Resource2=IDD_ABOUTBOX
Resource3=IDD_IMAGELOADPROGRESS

[CLS:COGLapp]
Type=0
BaseClass=CWinApp
HeaderFile=clean_ogl_app.h
ImplementationFile=clean_ogl_app.cpp

[CLS:CCleanGLWnd]
Type=0
BaseClass=COpenGLWnd
HeaderFile=CleanGlWnd.h
ImplementationFile=CleanGlWnd.cpp

[CLS:CImageProgress]
Type=0
BaseClass=CDialog
HeaderFile=image.h
ImplementationFile=Image.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=oglappDlg.cpp
ImplementationFile=oglappDlg.cpp
LastObject=CAboutDlg

[CLS:COGLappDlg]
Type=0
BaseClass=CDialog
HeaderFile=oglappDlg.h
ImplementationFile=oglappDlg.cpp

[CLS:COpenGLWnd]
Type=0
BaseClass=CWnd
HeaderFile=OpenGLWnd.h
ImplementationFile=OpenGLWnd.cpp
LastObject=COpenGLWnd

[DLG:IDD_IMAGELOADPROGRESS]
Type=1
Class=CImageProgress
ControlCount=5
Control1=IDC_STATIC,static,1342308352
Control2=IDC_PROGRESS_FILENAME,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PROGRESS_FILEFORMAT,static,1342308352
Control5=IDC_PROGRESS_BAR,msctls_progress32,1350565889

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CLEAN_OGL_APP_DIALOG]
Type=1
Class=COGLappDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1208025088
Control3=IDC_DISPLAYPLACEHOLDER,static,1073741831

