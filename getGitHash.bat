@ECHO OFF
REM This script generates a .cpp file containing the current git hash string

REM This is the file to which output will be written
SET outputPath="%1src/application/"
SET outputFile=gitHash.cpp

SET texPath="%1manual/"
SET texFile=gitHash.sty

FOR /f %%i IN ('git describe --tags --abbrev^=0') DO SET gitTag=%%i
FOR /f %%i IN ('git rev-parse --short HEAD') DO SET gitHash=%%i

REM ECHO outputPath = %outputPath%
REM ECHO outputFile = %outputFile%
REM ECHO texPath = %texPath%
REM ECHO texFile = %texFile%
ECHO version = %gitTag%
ECHO gitHash = %gitHash%

REM Build source file
@ECHO /*=============================================================================> %outputPath%%outputFile%
@ECHO                                      VVASE>> %outputPath%%outputFile%
@ECHO                         Copyright Kerry R. Loux 2007-2017>> %outputPath%%outputFile%
@ECHO =============================================================================*/>> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO // File:  %outputFile%>> %outputPath%%outputFile%
@ECHO // Date:  [last build date]>> %outputPath%%outputFile%
@ECHO // Auth:  getGitHash.bat>> %outputPath%%outputFile%
@ECHO // Desc:  Automatically generated file containing latest git hash and version info.>> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO // Local headers>> %outputPath%%outputFile%
@ECHO #include "gitHash.h">> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO const wxString vvaseVersion = _T("%gitTag%");>> %outputPath%%outputFile%
@ECHO const wxString vvaseGitHash = _T("%gitHash%");>> %outputPath%%outputFile%

REM Build TeX file
@ECHO %% %texFile%> %texPath%%texFile%
@ECHO \ProvidesPackage{gitHash}[6/9/2016 for VVASE]>> %texPath%%texFile%
@ECHO.>> %texPath%%texFile%
@ECHO %% Common commands>> %texPath%%texFile%
@ECHO \newcommand{\version}{%gitTag%}>> %texPath%%texFile%
@ECHO \newcommand{\gitHash}{%gitHash%}>> %texPath%%texFile%
