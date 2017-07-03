#!/bin/bash
# This file automatically generates a source file containing git version
# and hash information.

outputPath="$1src/application/"
outputFile=gitHash.cpp

texPath="$1manual/"
texFile=gitHash.sty

gitTag=$(git describe --tags --abbrev=0)
gitHash=$(git rev-parse --short HEAD)

#echo outputPath = $outputPath
#echo outputFile = $outputFile
#echo texPath = $texPath
#echo texFile = $texFile
echo version = $gitTag
echo gitHash = $gitHash

# Build source file
echo /*=============================================================================> $outputPath$outputFile
echo                                      VVASE>> $outputPath$outputFile
echo                         Copyright Kerry R. Loux 2007-2017>> $outputPath$outputFile
echo =============================================================================*/>> $outputPath$outputFile
echo>> $outputPath$outputFile
echo // File:  $outputFile> $outputPath$outputFile
echo // Auth:  getGitHash.sh>> $outputPath$outputFile
echo // Desc:  Automatically generated file containing latest git hash and version info.>> $outputPath$outputFile
echo>> $outputPath$outputFile
echo // Local headers>> $outputPath$outputFile
echo \#include \"application/gitHash.h\">> $outputPath$outputFile
echo>> $outputPath$outputFile
echo const wxString vvaseVersion = _T\(\"$gitTag\"\)\;>> $outputPath$outputFile
echo const wxString vvaseGitHash = _T\(\"$gitHash\"\)\;>> $outputPath$outputFile


# Build TeX style file
echo %% $texFile> $texPath$texFile
echo \\ProvidesPackage{gitHash}[6/9/2016 for VVASE]>> $texPath$texFile
echo>> $texPath$texFile
echo %% Common commands>> $texPath$texFile
echo \\newcommand{\\version}{$gitTag}>> $texPath$texFile
echo \\newcommand{\\gitHash}{$gitHash}>> $texPath$texFile
