# Microsoft Developer Studio Project File - Name="fullwindow" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=fullwindow - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fullwindow.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fullwindow.mak" CFG="fullwindow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fullwindow - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "fullwindow - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fullwindow - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "\Bob's Stuff\Code\common\FUSION_util" /I "\Bob's Stuff\Code from other people\lastools_latest" /I "\Bob's Stuff\Code\FUSION" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib ..\..\common\jpeg-7\libjpeg.lib ..\..\common\image\tlib_RELEASE_MT.lib vfw32.lib Winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"LIBCMTD.LIB" /nodefaultlib:"MSVCRTD.lib" /out:"\fusion_demo\pdq.exe" /VERBOSE:LIB
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "fullwindow - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\Bob's Stuff\Code\common\FUSION_util" /I "\Bob's Stuff\Code from other people\lastools_latest" /I "\Bob's Stuff\Code\FUSION" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib ..\..\common\jpeg-7\libjpeg.lib ..\..\common\image\tlib.lib vfw32.lib Winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\fusion_demo\pdq.exe" /pdbtype:sept /VERBOSE:LIB
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "fullwindow - Win32 Release"
# Name "fullwindow - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\3dmath.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSModel.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSModelObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fusion\ASCIIGridFile.cpp
# End Source File
# Begin Source File

SOURCE=..\BallController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\cdib.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\ColorRamp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\DataFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\DataIndex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\dtm3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\filespec.cpp
# End Source File
# Begin Source File

SOURCE=.\fullwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\fullwindow.rc
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\LASFormatFile.cpp
# End Source File
# Begin Source File

SOURCE="..\..\..\Code from other people\lastools_latest\laszip_api.c"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\LidarData_LASlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\LPP\LPPTrajectoryFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\OpenGLWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\plansdtm.cpp
# End Source File
# Begin Source File

SOURCE=.\RangepoleOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TextureMap.cpp
# End Source File
# Begin Source File

SOURCE=.\TranslationController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\scatter\LDV\WRITEAVI.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\3dmath.h
# End Source File
# Begin Source File

SOURCE=.\3DSModel.h
# End Source File
# Begin Source File

SOURCE=.\3DSModelObject.h
# End Source File
# Begin Source File

SOURCE=..\..\fusion\ASCIIGridFile.h
# End Source File
# Begin Source File

SOURCE=..\BallController.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\cdib.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\ColorRamp.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\DataFile.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\DataIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\dtm3d.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\filespec.h
# End Source File
# Begin Source File

SOURCE=.\fullwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\image.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\LASFormatFile.h
# End Source File
# Begin Source File

SOURCE="..\..\..\Code from other people\lastools_latest\laszip_api.h"
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\LidarData_LASlib.h
# End Source File
# Begin Source File

SOURCE=..\..\common\LPP\LPPTrajectoryFile.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\OpenGLWnd.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FUSION_util\plansdtm.h
# End Source File
# Begin Source File

SOURCE=.\RangepoleOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextureMap.h
# End Source File
# Begin Source File

SOURCE=.\TranslationController.h
# End Source File
# Begin Source File

SOURCE=..\..\scatter\LDV\WriteAVI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\fullwindow.ico
# End Source File
# Begin Source File

SOURCE=.\res\fullwindow.rc2
# End Source File
# Begin Source File

SOURCE=.\res\lascolor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\returnco.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
