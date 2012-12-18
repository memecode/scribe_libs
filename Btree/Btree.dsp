# Microsoft Developer Studio Project File - Name="Btree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Btree - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Btree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Btree.mak" CFG="Btree - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Btree - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Btree - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Btree - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /GX /O1 /I "../Lgi/include/win32" /I "../Lgi/include/common" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Btree - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /Zi /Od /I "../Lgi/include/win32" /I "../Lgi/include/common" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Btree - Win32 Release"
# Name "Btree - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\btree.c
# End Source File
# Begin Source File

SOURCE=.\btree_delete.c
# End Source File
# Begin Source File

SOURCE=.\btree_header.c
# End Source File
# Begin Source File

SOURCE=.\btree_insert.c
# End Source File
# Begin Source File

SOURCE=.\btree_lock.c
# End Source File
# Begin Source File

SOURCE=.\btree_node.c
# End Source File
# Begin Source File

SOURCE=.\btree_search.c
# End Source File
# Begin Source File

SOURCE=.\btree_traverse.c
# End Source File
# Begin Source File

SOURCE=.\config.c
# End Source File
# Begin Source File

SOURCE=.\db.c
# End Source File
# Begin Source File

SOURCE=.\db_blocklist.c
# End Source File
# Begin Source File

SOURCE=.\db_blocks.c
# End Source File
# Begin Source File

SOURCE=.\db_cache.c
# End Source File
# Begin Source File

SOURCE=.\db_header.c
# End Source File
# Begin Source File

SOURCE=.\db_lock.c
# End Source File
# Begin Source File

SOURCE=.\db_utils.c
# End Source File
# Begin Source File

SOURCE=.\hashtable.c
# End Source File
# Begin Source File

SOURCE=.\offsetlist.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\btree.h
# End Source File
# Begin Source File

SOURCE=.\btree_header.h
# End Source File
# Begin Source File

SOURCE=.\btree_lock.h
# End Source File
# Begin Source File

SOURCE=.\btree_node.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\db.h
# End Source File
# Begin Source File

SOURCE=.\db_blocklist.h
# End Source File
# Begin Source File

SOURCE=.\db_blocks.h
# End Source File
# Begin Source File

SOURCE=.\db_cache.h
# End Source File
# Begin Source File

SOURCE=.\db_header.h
# End Source File
# Begin Source File

SOURCE=.\db_internal.h
# End Source File
# Begin Source File

SOURCE=.\db_lock.h
# End Source File
# Begin Source File

SOURCE=.\db_types.h
# End Source File
# Begin Source File

SOURCE=.\db_utils.h
# End Source File
# Begin Source File

SOURCE=.\hashtable.h
# End Source File
# Begin Source File

SOURCE=.\offsetlist.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# End Group
# Begin Group "C++"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gbtree.cpp
# End Source File
# Begin Source File

SOURCE=.\gbtree.h
# End Source File
# End Group
# End Target
# End Project
