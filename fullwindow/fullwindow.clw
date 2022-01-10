; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFullwindowApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "fullwindow.h"
LastPage=0

ClassCount=5
Class1=CChildView
Class2=CAboutDlg
Class3=CFullwindowApp
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=RangepoleOptionDlg
Resource3=IDD_RANGEPOLE_DLG

[CLS:CChildView]
Type=0
BaseClass=COpenGLWnd 
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
LastObject=ID_CLONEPDQ
Filter=W
VirtualFilter=WC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=fullwindow.cpp
ImplementationFile=fullwindow.cpp
Filter=D
VirtualFilter=dWC
LastObject=CAboutDlg

[CLS:CFullwindowApp]
Type=0
BaseClass=CWinApp
HeaderFile=fullwindow.h
ImplementationFile=fullwindow.cpp
LastObject=CFullwindowApp
Filter=N
VirtualFilter=AC

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=ID_CLONEPDQ
Filter=T
VirtualFilter=fWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=12
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1342177283
Control3=IDC_VERSIONSTRING,static,1342308480
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_KEYSTROKES,edit,1353779332
Control8=IDC_STATIC,static,1342308352
Control9=IDC_RETURNCOLORS,button,1342193675
Control10=IDC_STATIC,static,1342308352
Control11=IDC_LASCODES,edit,1353779332
Control12=IDC_LASCOLORS,button,1342193675

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FILE_PRINT
Command5=ID_APP_ABOUT
CommandCount=5

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_RANGEPOLEOPTIONS
Command3=ID_APP_EXIT
Command4=ID_EDIT_COPY
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_VIEW_RANGEPOLES
Command8=ID_VIEW_POINTS
Command9=ID_VIEW_AXES
Command10=ID_CLONEPDQ
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[DLG:IDD_RANGEPOLE_DLG]
Type=1
Class=RangepoleOptionDlg
ControlCount=24
Control1=IDC_SCALETODATA,button,1342242819
Control2=IDC_STATIC,static,1342308866
Control3=IDC_TOTALHEIGHT,edit,1350631552
Control4=IDC_STATIC,static,1342308866
Control5=IDC_SEGMENTHEIGHT,edit,1350631552
Control6=IDC_USERANGEPOLETOSCALESDATA,button,1342251011
Control7=IDC_UPPERLEFT,button,1342242851
Control8=IDC_STATIC,static,1342308352
Control9=IDC_MIDDLETOP,button,1342243587
Control10=IDC_UPPERRIGHT,button,1342242819
Control11=IDC_MIDDLERIGHT,button,1342242819
Control12=IDC_LOWERRIGHT,button,1342242819
Control13=IDC_STATIC,static,1342308352
Control14=IDC_MIDDLEBOTTOM,button,1342243587
Control15=IDC_LOWERLEFT,button,1342242851
Control16=IDC_MIDDLELEFT,button,1342242851
Control17=IDC_CENTER,button,1342242819
Control18=IDC_ALL,button,1342242816
Control19=IDC_NONE,button,1342242816
Control20=IDC_CORNERONLY,button,1342242816
Control21=IDC_CENTERONLY,button,1342242816
Control22=IDC_SIDESONLY,button,1342242816
Control23=IDOK,button,1342242817
Control24=IDCANCEL,button,1342242816

[CLS:RangepoleOptionDlg]
Type=0
HeaderFile=RangepoleOptionDlg.h
ImplementationFile=RangepoleOptionDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=RangepoleOptionDlg
VirtualFilter=dWC

