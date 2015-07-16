/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  xlsxReader.h
// Created:  10/21/2010
// Author:  K. Loux
// Description:  Class for reading from Excel's XML-based files.  Contains functions
//				 intended for extracting columns of data.
// History:
//	11/7/2011	- Corrected camelCase, K. Loux.XlsxReader

#ifndef XLSX_READER_H_
#define XLSX_READER_H_

// Standard C++ headers
#include <utility>
#include <vector>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/xml/xml.h>

// Local headers
#include "vUtilities/managedList.h"

// wxWidgets forward declarations
class wxZipInputStream;
class wxZipEntry;

// Main class declaration
class XlsxReader
{
public:
	// Constructor
	XlsxReader(const wxString &pathAndFileName);

	// Destructor
	~XlsxReader();

	// For checking to make sure it opened OK
	bool IsOk() const { return isOkFlag; }

	// Functions for accessing file data
	unsigned int GetNumberOfSheets() const { return sheets.size(); }
	unsigned int GetNumberOfColumns(const unsigned int &sheet) const;
	unsigned int GetNumberOfRows(const unsigned int &sheet) const;
	wxString GetSheetName(const unsigned int &sheet) const;
	unsigned int GetSelectedSheet() const;
	wxString GetCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;
	double GetNumericCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;
	bool CellIsNumeric(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const;

private:
	wxString pathAndFileName;

	// Objects for initial access and unzipping of the files
	wxZipInputStream *OpenFile() const;
	wxZipEntry *GetEntry(wxZipInputStream &zipStream, const wxString &entryString) const;
	bool LoadSheet(const unsigned int &sheet);

	// Functions and objects required for object initialization
	bool Initialize();
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

	bool isOkFlag;
};

#endif// XLS_READER_H_