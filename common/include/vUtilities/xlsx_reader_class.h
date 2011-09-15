/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  xlsx_reader_class.h
// Created:  10/21/2010
// Author:  K. Loux
// Description:  Class for reading from Excel's XML-based files.  Contains functions
//				 intended for extracting columns of data.
// History:

#ifndef _XLSX_READER_CLASS_H_
#define _XLSX_READER_CLASS_H_

// Standard C++ headers
#include <utility>
#include <vector>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/xml/xml.h>

// Local headers
#include "vUtilities/managed_list_class.h"

// wxWidgets forward declarations
class wxZipInputStream;
class wxZipEntry;

// Main class declaration
class XLSX_READER
{
public:
	// Constructor
	XLSX_READER(const wxString &_PathAndFileName);

	// Destructor
	~XLSX_READER();

	// For checking to make sure it opened OK
	bool IsOk(void) const { return IsOkFlag; };

	// Functions for accessing file data
	unsigned int GetNumberOfSheets(void) const { return Sheets.size(); };
	unsigned int GetNumberOfColumns(const unsigned int &Sheet) const;
	unsigned int GetNumberOfRows(const unsigned int &Sheet) const;
	wxString GetSheetName(const unsigned int &Sheet) const;
	unsigned int GetSelectedSheet(void) const;
	wxString GetCellData(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const;
	double GetNumericCellData(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const;
	bool CellIsNumeric(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const;

private:
	// Path and file name
	wxString PathAndFileName;

	// Objects for initial access and unzipping of the files
	wxZipInputStream *OpenFile(void) const;
	wxZipEntry *GetEntry(wxZipInputStream &ZipStream, const wxString &EntryString) const;
	bool LoadSheet(const unsigned int &Sheet);

	// Functions and objects required for object initialization
	bool Initialize(void);
	wxString GetDimensionString(const wxXmlDocument &SheetDocument) const;
	bool SheetIsSelected(const wxXmlDocument &SheetDocument) const;
	unsigned int ParseForRowCount(const wxString &DimensionString) const;
	unsigned int ParseForColumnCount(const wxString &DimensionString) const;
	wxString GetSharedString(const unsigned int &Index) const;

	unsigned int ColumnNumberFromString(const wxString &Column) const;
	wxString GetCellAddressString(const unsigned int &Row, const unsigned int Column,
		const wxString &ReferenceCell) const;
	wxString GetReferenceCell(const wxString &DimensionString) const;

	// Objects for storing the XML documents and related information
	wxXmlDocument *Workbook;
	wxXmlDocument *SharedStrings;
	MANAGED_LIST<wxXmlDocument> Worksheets;
	std::vector<std::pair<wxString, unsigned int> > Sheets;

	// Flag indicating status of this object
	bool IsOkFlag;
};

#endif// _XLS_READER_CLASS_H_