# Microsoft Developer Studio Generated NMAKE File, Based on pyx.dsp
!IF "$(CFG)" == ""
CFG=example_SAXPyx - Win32 Debug
!MESSAGE No configuration specified. Defaulting to example_SAXPyx - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "example_SAXPyx - Win32 Release" && "$(CFG)" != "example_SAXPyx - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pyx.mak" CFG="example_SAXPyx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "example_SAXPyx - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "example_SAXPyx - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "example_SAXPyx - Win32 Release"

OUTDIR=.\..\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\pyx.exe"

!ELSE 

ALL : "ArabicaLib - Win32 Release" "$(OUTDIR)\pyx.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ArabicaLib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pyx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pyx.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\pyx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pyx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\pyx.pdb" /machine:I386 /out:"$(OUTDIR)\pyx.exe" 
LINK32_OBJS= \
	"$(INTDIR)\pyx.obj" \
	"..\..\lib\Arabica.lib"

"$(OUTDIR)\pyx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "example_SAXPyx - Win32 Debug"

OUTDIR=.\..\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\pyx.exe"

!ELSE 

ALL : "ArabicaLib - Win32 Debug" "$(OUTDIR)\pyx.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ArabicaLib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pyx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pyx.exe"
	-@erase "$(OUTDIR)\pyx.ilk"
	-@erase "$(OUTDIR)\pyx.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GR /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\pyx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pyx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\pyx.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pyx.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\pyx.obj" \
	"..\..\lib\Arabica.lib"

"$(OUTDIR)\pyx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("pyx.dep")
!INCLUDE "pyx.dep"
!ELSE 
!MESSAGE Warning: cannot find "pyx.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "example_SAXPyx - Win32 Release" || "$(CFG)" == "example_SAXPyx - Win32 Debug"
SOURCE=.\pyx.cpp

"$(INTDIR)\pyx.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "example_SAXPyx - Win32 Release"

"ArabicaLib - Win32 Release" : 
   cd "\work\JezUK\Arabica\src\SAX"
   $(MAKE) /$(MAKEFLAGS) /F .\SAX.mak CFG="ArabicaLib - Win32 Release" 
   cd "..\examples\SAX"

"ArabicaLib - Win32 ReleaseCLEAN" : 
   cd "\work\JezUK\Arabica\src\SAX"
   $(MAKE) /$(MAKEFLAGS) /F .\SAX.mak CFG="ArabicaLib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\examples\SAX"

!ELSEIF  "$(CFG)" == "example_SAXPyx - Win32 Debug"

"ArabicaLib - Win32 Debug" : 
   cd "\work\JezUK\Arabica\src\SAX"
   $(MAKE) /$(MAKEFLAGS) /F .\SAX.mak CFG="ArabicaLib - Win32 Debug" 
   cd "..\examples\SAX"

"ArabicaLib - Win32 DebugCLEAN" : 
   cd "\work\JezUK\Arabica\src\SAX"
   $(MAKE) /$(MAKEFLAGS) /F .\SAX.mak CFG="ArabicaLib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\examples\SAX"

!ENDIF 


!ENDIF 

