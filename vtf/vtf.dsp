# Microsoft Developer Studio Project File - Name="vtf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vtf - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vtf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vtf.mak" CFG="vtf - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vtf - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vtf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "vtf"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vtf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W4 /Ox /Ot /Ow /Og /Oi /Op /Gf /Gy /I "..\..\src\common" /I "..\..\src\public" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D fopen=dont_use_fopen /D "_WIN32" /FD /c
# ADD CPP /nologo /G6 /W4 /Z7 /Ox /Ot /Ow /Og /Oi /Op /Gf /Gy /I "..\common" /I "..\public" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D fopen=dont_use_fopen /D "_WIN32" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Publishing to target directory (..\lib\public\)...
TargetDir=.\Release
TargetPath=.\Release\vtf.lib
InputPath=.\Release\vtf.lib
SOURCE="$(InputPath)"

"..\lib\public\vtf.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\lib\public\vtf.lib attrib -r ..\lib\public\vtf.lib 
	copy $(TargetPath) ..\lib\public\vtf.lib 
	if exist $(TargetDir)\vtf.map copy $(TargetDir)\vtf.map ..\lib\public\vtf.map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "vtf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W4 /Gm /Zi /Od /Op /I "..\..\src\common" /I "..\..\src\public" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D fopen=dont_use_fopen /D "_WIN32" /FR /FD /GZ /c
# ADD CPP /nologo /G6 /W4 /Z7 /Od /Op /I "..\common" /I "..\public" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D fopen=dont_use_fopen /D "_WIN32" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Publishing to target directory (..\lib\public\)...
TargetDir=.\Debug
TargetPath=.\Debug\vtf.lib
InputPath=.\Debug\vtf.lib
SOURCE="$(InputPath)"

"..\lib\public\vtf.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\lib\public\vtf.lib attrib -r ..\lib\public\vtf.lib 
	copy $(TargetPath) ..\lib\public\vtf.lib 
	if exist $(TargetDir)\vtf.map copy $(TargetDir)\vtf.map ..\lib\public\vtf.map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "vtf - Win32 Release"
# Name "vtf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Public\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=.\vtf.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Public\vtf\vtf.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\common\s3tc.lib
# End Source File
# End Target
# End Project
