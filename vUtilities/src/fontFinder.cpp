/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fontFinder.cpp
// Created:  11/17/2011
// Author:  K. Loux
// Description:  Cross-platform methods for working with fonts.  Builds on wxWidgets
//				 objects.
// History:

// Standard C++ headers
#include <cstdlib>
#include <fstream>

/*#ifdef __WXGTK__
// *nix headers
#include <sys/types.h>
#include <unistd.h>
#endif*/// FIXME:  Not sure if these are needed

// wxWidgets headers
#include <wx/wx.h>
#include <wx/fontenum.h>

// Local headers
#include "vUtilities/fontFinder.h"

//==========================================================================
// Class:			FontFinder
// Function:		GetFontPathAndFileName
//
// Description:		Searches the local hard drive (intelligently) and returns
//					the path and file name for a preferred font.
//
// Input Arguments:
//		fontName	= const std::string& name of the desired font
//
// Output Arguments:
//		None
//
// Return Value:
//		std::string containing the path to the font file, or an empty string
//		if the font could not be located
//
//==========================================================================
wxString FontFinder::GetFontPathAndFileName(const wxString &fontName)
{
	// We will need to search through all the font files on the system, open
	// each one and get the name from file to know if we have the correct file
	wxString path;

#ifdef __WXMSW__
	// For Windows, search through the fonts directory, checking font names
	// until we find the one we're looking for
	// FIXME:  Implement this
	path = wxGetOSDirectory() + _T("\\fonts\\arial.ttf");

#elif defined __WXGTK__
	// FIXME:  Implement this
	//path = _T("/usr/share/fonts/dejavu/DejaVuSans.ttf");// Fedora 13
	path = _T("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");// Ubuntu 11.10

	// Code from ttf_name_extractor.cpp:
	/*
	// Set up pipes to catch system output
	pid_t child;
	int pipes[2];
	if (pipe(pipes) != 0)
	{
		std::cerr << "Error - could not create pipes" << std::endl;
		exit(1);
	}

	// Fork a new process
	child = fork();
	if (child < 0)
	{
		std::cerr << "Error - process failed to fork" << std::endl;
		exit(1);
	}
	else if (child == 0)// Child process
	{
		// Redirect stdout to pipes[1]
		dup2(pipes[1], 1);
		close(pipes[0]);
		if (execl("/bin/find", "find", "/", "-iname", "*.ttf", NULL) == -1)
		{
			std::cerr << "Error launching find command" << std::endl;
			return 1;
		}
	}
	else// Parent process
	{
		dup2(pipes[0], 0);
		close(pipes[1]);
		setvbuf(stdout,(char*)NULL,_IONBF,0);
		const int MaxLineLen(512);
		char line[MaxLineLen];
		std::string Path;
		while (fgets(line, MaxLineLen, stdin) != NULL)
		{
			// Get the font name, display it, and reset for the next font
			Path.assign(line);
			Path = Path.substr(0, Path.length() - 1);// Remove newline at end of line
			std::cout << GetFontName(Path) << "\t" << Path << std::endl;;
		}
		wait();
	}
	*/

#else
	// Unknown platform - warn the user
#	warning "Unrecognized platform - unable to locate font files!"
	path = wxEmptyString
#endif

	return path;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetFontFaceName
//
// Description:		Returns the best match for a font on the system when given
//					a list of acceptable fonts.
//
// Input Arguments:
//		encoding		= wxFontEncoding
//		preferredFonts	= const wxArrayString& list of preferred font faces
//		fixedWidth		= const bool&
//
// Output Arguments:
//		fontName		= wxString& containing the name of the best match
//
// Return Value:
//		bool, true for found a match
//
//==========================================================================
bool FontFinder::GetFontFaceName(wxFontEncoding encoding, const wxArrayString &preferredFonts,
	const bool &fixedWidth, wxString &fontName)
{
	// Get a list of the fonts found on the system
	wxArrayString fontList = wxFontEnumerator::GetFacenames(encoding, fixedWidth);

	// See if any of them are in our preferred fonts list
	// Assume list is organized with most desired fonts first
	unsigned int i, j;
	for (i = 0; i < preferredFonts.GetCount(); i++)
	{
		for (j = 0; j < fontList.GetCount(); j++)
		{
			if (preferredFonts[i].CmpNoCase(fontList[j]) == 0)
			{
				fontName = fontList[j];
				return true;
			}
		}
	}

	// If no fonts were found with the enumerator, return an empty string
	if (fontList.GetCount() > 0)
		fontName = fontList[0];
	else
		fontName.Empty();

	return false;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetFontName
//
// Description:		Returns name of the font for the specified .ttf file.
//
// Input Arguments:
//		fontFile	= const wxString& specifying the file location
//
// Output Arguments:
//		fontName	= wxString& containing the name of the font
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool FontFinder::GetFontName(const wxString &fontFile, wxString &fontName)
{
	
	// Open the font file
	std::ifstream fontStream(fontFile.c_str(), std::ios::in | std::ios::binary);

	// Make sure it opened OK
	if (!fontStream.good())
		return false;

	// Read the offset table
	TT_OFFSET_TABLE ttOffsetTable;
	fontStream.read((char*)&ttOffsetTable, sizeof(TT_OFFSET_TABLE));

	// Rearrange the bytes for the important fields
	SwapEndian(ttOffsetTable.numOfTables);
	SwapEndian(ttOffsetTable.majorVersion);
	SwapEndian(ttOffsetTable.minorVersion);

	// Make sure this is a version 1.0 TrueType Font
	if(ttOffsetTable.majorVersion != 1 || ttOffsetTable.minorVersion != 0)
		return false;

	TT_TABLE_DIRECTORY tblDir;
	tblDir.offset = 0;// To avoid MSVC++ Warning C4701
	bool found = false;
	wxString tempTagString;
	wxString tableName("name");

	int i;
	for (i = 0; i < ttOffsetTable.numOfTables; i++)
	{
		fontStream.read((char*)&tblDir, sizeof(TT_TABLE_DIRECTORY));
		tempTagString.clear();

		// Table's tag is <= 4 characters
		tempTagString.assign(tblDir.tag);
		tempTagString.resize(4);
		if(tempTagString.CmpNoCase(tableName) == 0)
		{
			// Found the table, make sure to swap the bytes for the offset and length
			found = true;
			SwapEndian(tblDir.length);
			SwapEndian(tblDir.offset);
			break;
		}
	}

	// If we didnt' find the name, stop now
	if (!found)
		return false;

	// Go to the offset we found above
	fontStream.seekg(tblDir.offset, std::ios_base::beg);
	TT_NAME_TABLE_HEADER ttNTHeader;
	fontStream.read((char*)&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));

	// Swap the bytes again
	SwapEndian(ttNTHeader.nrCount);
	SwapEndian(ttNTHeader.storageOffset);

	TT_NAME_RECORD ttRecord;
	found = false;

	for (i = 0; i < ttNTHeader.nrCount; i++)
	{
		fontStream.read((char*)&ttRecord, sizeof(TT_NAME_RECORD));
		SwapEndian(ttRecord.nameID);

		// Name ID == 1 indicates font name
		if (ttRecord.nameID == 1)
		{
			SwapEndian(ttRecord.stringLength);
			SwapEndian(ttRecord.stringOffset);

			// Save file position, so we can return to continue with search
			int nPos = fontStream.tellg();
			fontStream.seekg(tblDir.offset + ttRecord.stringOffset +
				ttNTHeader.storageOffset, std::ios_base::beg);

			char *nameBuffer = new char[ttRecord.stringLength];
			fontStream.read(nameBuffer, ttRecord.stringLength);
			fontName.assign(nameBuffer);
			fontName.resize(ttRecord.stringLength);
			delete [] nameBuffer;

			// Check to make sure the name isn't empty - if it is, continue searching
			if (!fontName.Trim().IsEmpty())// FIXME:  This isn't working!
				break;

			fontStream.seekg(nPos, std::ios_base::beg);
		}
	}

	if (fontName.Length() > 0)
		return true;

	return false;
}