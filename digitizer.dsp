# Microsoft Developer Studio Project File - Name="Engauge Digitizer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Engauge Digitizer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Digitizer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Digitizer.mak" CFG="Engauge Digitizer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engauge Digitizer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Engauge Digitizer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin"
# PROP BASE Intermediate_Dir "objs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "$(FFTW_HOME)\api" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "UNICODE" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 bin\libfftw3-3.lib imm32.lib $(QTDIR)\lib\qt-mt.lib $(QTDIR)\lib\qtmain.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib wsock32.lib /nologo /stack:0x180000 /subsystem:windows /machine:I386 /out:"bin\engauge.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin"
# PROP BASE Intermediate_Dir "objs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "$(FFTW_HOME)\api" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "UNICODE" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 bin\libfftw3-3.lib imm32.lib $(QTDIR)\lib\qt-mt.lib $(QTDIR)\lib\qtmain.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"bin\engauge.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Engauge Digitizer - Win32 Release"
# Name "Engauge Digitizer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=src\axispointdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\clipboard.cpp
# End Source File
# Begin Source File

SOURCE=src\colorchooser.cpp
# End Source File
# Begin Source File

SOURCE=src\coordsysdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\curvecmb.cpp
# End Source File
# Begin Source File

SOURCE=src\defaultsettings.cpp
# End Source File
# Begin Source File

SOURCE=src\digitdebug.cpp
# End Source File
# Begin Source File

SOURCE=src\digitdef.cpp
# End Source File
# Begin Source File

SOURCE=src\digitdoc.cpp
# End Source File
# Begin Source File

SOURCE=src\digitizer.rc
# End Source File
# Begin Source File

SOURCE=src\digitmain.cpp
# End Source File
# Begin Source File

SOURCE=src\digittool.cpp
# End Source File
# Begin Source File

SOURCE=src\digitview.cpp
# End Source File
# Begin Source File

SOURCE=src\discretize.cpp
# End Source File
# Begin Source File

SOURCE=src\discretizedlg.cpp
# End Source File
# Begin Source File

SOURCE=src\exportdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\geometrydlg.cpp
# End Source File
# Begin Source File

SOURCE=src\geometrywindowitem.cpp
# End Source File
# Begin Source File

SOURCE=src\grid.cpp
# End Source File
# Begin Source File

SOURCE=src\gridmesh.cpp
# End Source File
# Begin Source File

SOURCE=src\gridmeshdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\gridremoval.cpp
# End Source File
# Begin Source File

SOURCE=src\gridremovaldlg.cpp
# End Source File
# Begin Source File

SOURCE=src\helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=src\highlight.cpp
# End Source File
# Begin Source File

SOURCE=src\main.cpp
# End Source File
# Begin Source File

SOURCE=src\match.cpp
# End Source File
# Begin Source File

SOURCE=src\matchset.cpp
# End Source File
# Begin Source File

SOURCE=src\measurecmb.cpp
# End Source File
# Begin Source File

SOURCE=src\mmsubs.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_axispointdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_colorchooser.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_coordsysdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_curvecmb.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_digitdoc.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_digitmain.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_digitview.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_discretizedlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_exportdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_geometrydlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_gridmeshdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_gridremovaldlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_measurecmb.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_mrudocuments.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_pointmatchdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_pointsetdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_pointsetnamedlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_pointsetsdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_scalebardlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_segmentdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_sessionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_transform.cpp
# End Source File
# Begin Source File

SOURCE=src\moc_zoomcmb.cpp
# End Source File
# Begin Source File

SOURCE=src\mrudocuments.cpp
# End Source File
# Begin Source File

SOURCE=src\osxsubs.cpp
# End Source File
# Begin Source File

SOURCE=src\point.cpp
# End Source File
# Begin Source File

SOURCE=src\pointmatch.cpp
# End Source File
# Begin Source File

SOURCE=src\pointmatchdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\pointmatchthread.cpp
# End Source File
# Begin Source File

SOURCE=src\pointset.cpp
# End Source File
# Begin Source File

SOURCE=src\pointsetdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\pointsetnamedlg.cpp
# End Source File
# Begin Source File

SOURCE=src\pointsets.cpp
# End Source File
# Begin Source File

SOURCE=src\pointsetsdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\pointsetstyles.cpp
# End Source File
# Begin Source File

SOURCE=src\scalebardlg.cpp
# End Source File
# Begin Source File

SOURCE=src\scanforaxes.cpp
# End Source File
# Begin Source File

SOURCE=src\segment.cpp
# End Source File
# Begin Source File

SOURCE=src\segmentdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\segments.cpp
# End Source File
# Begin Source File

SOURCE=src\sessionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=src\transform.cpp
# End Source File
# Begin Source File

SOURCE=src\zoomcmb.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=src\axispointdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=axispointdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=axispointdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\clipboard.h
# End Source File
# Begin Source File

SOURCE=src\colorchooser.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=colorchooser

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=colorchooser

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\coordsysdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=coordsysdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=coordsysdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\curvecmb.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=curvecmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=curvecmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\defaultsettings.h
# End Source File
# Begin Source File

SOURCE=src\digitdebug.h
# End Source File
# Begin Source File

SOURCE=src\digitdef.h
# End Source File
# Begin Source File

SOURCE=src\digitdoc.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitdoc

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitdoc

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\digitmain.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitmain

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitmain

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\digittool.h
# End Source File
# Begin Source File

SOURCE=src\digitview.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitview

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=digitview

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\discretize.h
# End Source File
# Begin Source File

SOURCE=src\discretizedlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=discretizedlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=discretizedlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\exportdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=exportdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=exportdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\geometrydlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=geometrydlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=geometrydlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\geometrywindowitem.h
# End Source File
# Begin Source File

SOURCE=src\grid.h
# End Source File
# Begin Source File

SOURCE=src\gridmesh.h
# End Source File
# Begin Source File

SOURCE=src\gridmeshdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=gridmeshdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=gridmeshdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\gridremoval.h
# End Source File
# Begin Source File

SOURCE=src\gridremovaldlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=gridremovaldlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=gridremovaldlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\helpwindow.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=helpwindow

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=helpwindow

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\highlight.h
# End Source File
# Begin Source File

SOURCE=src\main.h
# End Source File
# Begin Source File

SOURCE=src\match.h
# End Source File
# Begin Source File

SOURCE=src\matchset.h
# End Source File
# Begin Source File

SOURCE=src\measurecmb.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=measurecmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=measurecmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\mmsubs.h
# End Source File
# Begin Source File

SOURCE=src\mrudocuments.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=mrudocuments

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=mrudocuments

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\osxsubs.h
# End Source File
# Begin Source File

SOURCE=src\point.h
# End Source File
# Begin Source File

SOURCE=src\pointmatch.h
# End Source File
# Begin Source File

SOURCE=src\pointmatchdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointmatchdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointmatchdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\pointmatchthread.h
# End Source File
# Begin Source File

SOURCE=src\pointset.h
# End Source File
# Begin Source File

SOURCE=src\pointsetdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\pointsetnamedlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetnamedlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetnamedlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\pointsets.h
# End Source File
# Begin Source File

SOURCE=src\pointsetsdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetsdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=pointsetsdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\pointsetstyles.h
# End Source File
# Begin Source File

SOURCE=src\scalebardlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=scalebardlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=scalebardlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\scanforaxes.h
# End Source File
# Begin Source File

SOURCE=src\segment.h
# End Source File
# Begin Source File

SOURCE=src\segmentdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=segmentdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=segmentdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\segments.h
# End Source File
# Begin Source File

SOURCE=src\sessionsdlg.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=sessionsdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=sessionsdlg

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\transform.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=transform

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=transform

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=src\zoomcmb.h

!IF  "$(CFG)" == "Engauge Digitizer - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=zoomcmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Engauge Digitizer - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - qt moc on $(InputName).h
InputPath=src
InputName=zoomcmb

"src\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe -p . src\$(InputName).h -o src\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=src\img\digitizer.ico
# End Source File
# End Group
# Begin Source File

SOURCE=src\digitaxis.xpm
# End Source File
# Begin Source File

SOURCE=src\digitcurve.xpm
# End Source File
# Begin Source File

SOURCE=src\digitmeasure.xpm
# End Source File
# Begin Source File

SOURCE=src\digitsegment.xpm
# End Source File
# Begin Source File

SOURCE=src\digitselect.xpm
# End Source File
# Begin Source File

SOURCE=src\fileimport.xpm
# End Source File
# Begin Source File

SOURCE=src\fileopen.xpm
# End Source File
# Begin Source File

SOURCE=src\filesave.xpm
# End Source File
# Begin Source File

SOURCE=src\whatsthis.xpm
# End Source File
# End Target
# End Project
