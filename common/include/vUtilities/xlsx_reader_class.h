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
//	11/7/2011	- Corrected camelCase, K. Loux.XlsxReader

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
class XlsxReader
{
public:
	// Constructor
	XlsxReader(const wxString &_pathAndFileName);

	// Destructor
	~XlsxReader();

	// For checking to make sure it opened OK
	bool IsOk(void) const { return isOkFlag; };

	// Functions for accessing file data
	unsigned int GetNumberOfSheets(void) const { return sheets.size(); };
	unsigned int GetNumberOfColumns(const unsigned int &sheet) const;
	unsigned int GetNumberOfRows(const unsigned int &sheet) const;
	wxString GetSheetName(const unsigned int &sheet) const;
	unsigned int GetSelectedSheet(void) const;
	wxString GetCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;
	double GetNumericCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;
	bool CellIsNumeric(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;

private:
	// Path and file name
	wxString pathAndFileName;

	// Objects for initial access and unzipping of the files
	wxZipInputStream *OpenFile(void) const;
	wxZipEntry *GetEntry(wxZipInputStream &zipStream, const wxString &entryString) const;
	bool LoadSheet(const unsigned int &sheet);

	// Functions and objects required for object initialization
	bool Initialize(void);
	wxString GetDimensionString(const wxXmlDocument &sheetDocument) const;
	bool SheetIsSelected(const wxXmlDocument &sheetDocument) const;
	unsigned int ParseForRowCount(const wxString &dimensionString) const;
	unsigned int ParseForColumnCount(const wxString &dimensionString) const;
	wxString GetSharedString(const unsigned int &index) const;

	unsigned int ColumnNumberFromString(const wxString &column) const;
	wxString GetCellAddressString(const unsigned int &row, const unsigned int column,
		const wxString &referenceCell) const;
	wxString GetReferenceCell(const wxString &dimensionString) const;

	// Objects for storing the XML documents and related information
	wxXmlDocument *workbook;
	wxXmlDocument *sharedStrings;
	ManagedList<wxXmlDocument> worksheets;
	std::vector<std::pair<wxString, unsigned int> > sheets;

	// Flag indicating status of this object
	bool isOkFlag;
};

#endif// _XLS_READER_CLASS_H_