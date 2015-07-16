/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  xlsxReader.cpp
// Created:  10/21/2010
// Author:  K. Loux
// Description:  Class for reading from Excel's XML-based files.  Contains functions
//				 intended for extracting columns of data.
// History:
//	11/7/2011	- Corrected camelCase, K. Loux.XlsxReader

// Standard C++ headers
#include <limits>

// wxWidgets headers
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

// Local headers
#include "vUtilities/xlsxReader.h"

//==========================================================================
// Class:			XlsxReader
// Function:		XlsxReader
//
// Description:		Constructor for the XlsxReader class.
//
// Input Arguments:
//		pathAndFileName	= wxString describing of the .xlsx file to read.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
XlsxReader::XlsxReader(const wxString &pathAndFileName)
{
	isOkFlag = false;
	this->pathAndFileName = pathAndFileName;
	workbook = NULL;
	sharedStrings = NULL;

	if (!wxFileExists(pathAndFileName))
		return;

	if (Initialize())
		isOkFlag =  true;
}

//==========================================================================
// Class:			XlsxReader
// Function:		~XlsxReader
//
// Description:		Destructor for the XlsxReader class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
XlsxReader::~XlsxReader()
{
	// Delete dynamic memory
	worksheets.Clear();

	delete workbook;
	workbook = NULL;

	delete sharedStrings;
	sharedStrings = NULL;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetNumberOfColumns
//
// Description:		Returns the numer of columns for the specified sheet.
//
// Input Arguments:
//		sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of columns
//
//==========================================================================
unsigned int XlsxReader::GetNumberOfColumns(const unsigned int &sheet) const
{
	assert(sheet < sheets.size());

	// Get the number of columns from the specified sheet
	return ParseForColumnCount(GetDimensionString(*worksheets[sheet]));
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetNumberOfRows
//
// Description:		Returns the number of rows for the specified sheet.
//
// Input Arguments:
//		sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of rows
//
//==========================================================================
unsigned int XlsxReader::GetNumberOfRows(const unsigned int &sheet) const
{
	assert(sheet < sheets.size());

	// Get the number of rows from the specified sheet
	return ParseForRowCount(GetDimensionString(*worksheets[sheet]));
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetSheetName
//
// Description:		Returns the name of the specified sheet.
//
// Input Arguments:
//		sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the sheet name
//
//==========================================================================
wxString XlsxReader::GetSheetName(const unsigned int &sheet) const
{
	assert(sheet < sheets.size());

	return sheets[sheet].first;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetSelectedSheet
//
// Description:		Returns the index of the selected tab
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the selected tab
//
//==========================================================================
unsigned int XlsxReader::GetSelectedSheet() const
{
	// Check each sheet until we find the selected one
	unsigned int i;
	for (i = 0; i < worksheets.GetCount(); i++)
	{
		if (SheetIsSelected(*worksheets[i]))
			return i;
	}

	// If we can't find the selected sheet, return an invalid number
	return worksheets.GetCount();
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetEntry
//
// Description:		Locates the wxZipEntry in the wxZipInputStream with the
//					specified name.
//
// Input Arguments:
//		zipStream	= wxZipEntryStream& to look in
//		entryString	= const wxString& containing the name of the entry we're
//					  looking for
//
// Output Arguments:
//		None
//
// Return Value:
//		wxZipEntry* pointing to the part of the stream where the specified
//		entry is located
//
//==========================================================================
wxZipEntry *XlsxReader::GetEntry(wxZipInputStream &zipStream, const wxString &entryString) const
{
	// Parse the file and record relevant information
	wxZipEntry *zipPointer = NULL;
	while (zipPointer = zipStream.GetNextEntry(), zipPointer != NULL)
	{
		// Look for the workbook pointer
		if (zipPointer->GetInternalName().CompareTo(wxZipEntry::GetInternalName(entryString)) == 0)
			return zipPointer;

		delete zipPointer;
		zipPointer = NULL;
	}

	return NULL;
}

//==========================================================================
// Class:			XlsxReader
// Function:		OpenFile
//
// Description:		Opens and decompresses the associated .xlsx stream.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxZipInputStream* for the associated file
//
//==========================================================================
wxZipInputStream *XlsxReader::OpenFile() const
{
	// Open the archive
	wxFFileInputStream *fileInput = new wxFFileInputStream(pathAndFileName);
	if (fileInput->IsOk())
	{
		wxZipInputStream *sipStream = new wxZipInputStream(fileInput);
		if (sipStream->IsOk())
			return sipStream;
	}

	return NULL;
}

//==========================================================================
// Class:			XlsxReader
// Function:		Initialize
//
// Description:		Initializes all class data for the associated file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool XlsxReader::Initialize()
{
	// Open the archive
	wxZipInputStream *zipStream = OpenFile();

	// Locate the workbook entry
	wxZipEntry *zipPointer = GetEntry(*zipStream, _T("xl\\workbook.xml"));
	if (zipPointer == NULL)
		return false;

	// Open the workbook stream as an XML document
	workbook = new wxXmlDocument(*zipStream);
	if (!workbook->IsOk())
		return false;

	// Delete the ZipPointer
	delete zipPointer;
	zipPointer = NULL;

	// Locate the shared strings entry
	// FIXME:  This only works because sharedStrings.xml is AFTER workbook.xml
	// (reverse alphabetical order?) - will this always work, or should the file
	// be closed and re-opened to ensure it is found?
	zipPointer = GetEntry(*zipStream, _T("xl\\sharedStrings.xml"));
	if (zipPointer == NULL)
		return false;

	// Open the shared strings stream
	sharedStrings = new wxXmlDocument(*zipStream);
	if (!sharedStrings->IsOk())
		return false;

	// Delete the ZipPointer
	delete zipPointer;
	zipPointer = NULL;

	// Locate the sheets
	wxXmlNode *node = workbook->GetRoot()->GetChildren();
	wxString tempName;
	while (node)
	{
		// Does this node contain sheet information?
		if (node->GetName().CompareTo(_T("sheets")) == 0)
		{
			wxXmlNode *sheetNode = node->GetChildren();
			unsigned long sheetId;
			wxString idString;
			while (sheetNode)
			{
				idString = sheetNode->GetAttribute(_T("r:id"), _T("-1"));
				idString.Mid(3, idString.Length() - 3).ToULong(&sheetId);
				std::pair<wxString, unsigned int> sheetData(sheetNode->GetAttribute(_T("name")), sheetId);
				sheets.push_back(sheetData);

				// Get the next sheet
				sheetNode = sheetNode->GetNext();
			}
		}

		// Get the next child
		node = node->GetNext();
	}

	// Close the file
	delete zipStream;
	zipStream = NULL;

	// Now that we know how many sheets we're looking for, load the data from each sheet
	unsigned int sheet;
	for (sheet = 0; sheet < sheets.size(); sheet++)
	{
		if (!LoadSheet(sheet))
			return false;
	}

	return true;
}

//==========================================================================
// Class:			XlsxReader
// Function:		LoadSheet
//
// Description:		Loads the XML content of the worksheets and stores the
//					document objects.
//
// Input Arguments:
//		sheet	= const unsigned int& specifying the sheet to load
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool XlsxReader::LoadSheet(const unsigned int &sheet)
{
	// Open the stream
	wxZipInputStream *zipStream = OpenFile();

	// Locate the relevant entry point
	wxString sheetString;
	sheetString.Printf("xl\\worksheets\\sheet%i.xml", sheets[sheet].second);
	wxZipEntry *zipPointer = GetEntry(*zipStream, sheetString);

	// Open the stream as an XML file
	worksheets.Add(new wxXmlDocument(*zipStream));
	if (!worksheets[sheet]->IsOk())
		return false;

	// Delete the ZipPointer
	delete zipPointer;
	zipPointer = NULL;

	// Close the file
	delete zipStream;
	zipStream = NULL;

	return true;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetDimensionString
//
// Description:		Retrieves the string describing the range of this document
//					from the passed wxXmlDocument
//
// Input Arguments:
//		sheetDocument	= const wxXmlDocument&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the range of the document
//
//==========================================================================
wxString XlsxReader::GetDimensionString(const wxXmlDocument &sheetDocument) const
{
	wxXmlNode *node = sheetDocument.GetRoot()->GetChildren();
	while (node)
	{
		// Does this node contain sheet information?
		if (node->GetName().CompareTo(_T("dimension")) == 0)
			return node->GetAttribute(_T("ref"));

		// Get the next child
		node = node->GetNext();
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			XlsxReader
// Function:		SheetsSelected
//
// Description:		Looks for nodes and properties indicating that this sheet
//					was the active sheet when the document was saved.
//
// Input Arguments:
//		sheetDocument	= const wxXmlDocument&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for is selected, false otherwise
//
//==========================================================================
bool XlsxReader::SheetIsSelected(const wxXmlDocument &sheetDocument) const
{
	// Locate the sheet view properties
	wxXmlNode *node = sheetDocument.GetRoot()->GetChildren();
	while (node)
	{
		// Does this node contain sheet information?
		if (node->GetName().CompareTo(_T("sheetViews")) == 0)
		{
			wxXmlNode *viewNode = node->GetChildren();
			while (viewNode)
			{
				if (viewNode->GetName().CompareTo(_T("sheetView")) == 0)
				{
					if (viewNode->HasAttribute(_T("tabSelected")))
					{
						long Selected = 0;
						viewNode->GetAttribute(_T("tabSelected")).ToLong(&Selected);
						return Selected == 1;
					}
				}

				// Get the next view node
				viewNode = viewNode->GetNext();
			}
		}

		// Get the next child
		node = node->GetNext();
	}

	return false;
}

//==========================================================================
// Class:			XlsxReader
// Function:		ParseForRowCount
//
// Description:		Parses a string describing a cell range to determine the
//					number of rows contained within the range.
//
// Input Arguments:
//		dimensionString	= const wxString& describing the range (i.e. "B4:ED56")
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of rows in the range
//
//==========================================================================
unsigned int XlsxReader::ParseForRowCount(const wxString &dimensionString) const
{
	// If there is no :, the range consists of only one row
	int colonLocation = dimensionString.Find(':');
	if (colonLocation == wxNOT_FOUND)
		return 1;

	// Search for the first numeric character before the colon
	unsigned int i;
	for (i = 0; i < dimensionString.Length(); i++)
	{
		if (dimensionString.Mid(i, 1).IsNumber())
			break;
	}

	// Determine the first row containing data
	long firstRow;
	dimensionString.Mid(i, colonLocation - 1).ToLong(&firstRow);

	// Find the last row containing data
	for (i = colonLocation + 1; i < dimensionString.Length(); i++)
	{
		if (dimensionString.Mid(i, 1).IsNumber())
		{
			long lastRow;
			dimensionString.Mid(i, dimensionString.Length() - i).ToLong(&lastRow);
			
			// Compute the number of rows containing data
			return int(lastRow - firstRow) + 1;
		}
	}

	return 0;
}

//==========================================================================
// Class:			XlsxReader
// Function:		ParseForColumnCount
//
// Description:		Parses a string describing a cell range to determine the
//					number of columns contained within the range.
//
// Input Arguments:
//		dimensionString	= const wxString& describing the range (i.e. "B4:ED56")
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of columns in the range
//
//==========================================================================
unsigned int XlsxReader::ParseForColumnCount(const wxString &dimensionString) const
{
	// If there is no :, the range consists of only one column
	int colonLocation = dimensionString.Find(':');
	if (colonLocation == wxNOT_FOUND)
		return 1;

	// Search for the first numeric character before the colon
	unsigned int i;
	for (i = 0; i < dimensionString.Length(); i++)
	{
		if (dimensionString.Mid(i, 1).IsNumber())
			break;
	}

	// Determine the first column containing data
	long firstColumn = ColumnNumberFromString(dimensionString.Mid(0, i));

	// Find the last column containing data
	for (i = colonLocation + 1; i < dimensionString.Length(); i++)
	{
		if (dimensionString.Mid(i, 1).IsNumber())
			// Compute the number of columns containing data
			return ColumnNumberFromString(dimensionString.Mid(
			colonLocation + 1, i - colonLocation - 1)) - firstColumn + 1;
	}

	return 0;
}

//==========================================================================
// Class:			XlsxReader
// Function:		ColumnNumberFromString
//
// Description:		Converts from Excel's column identifier string (i.e. 'A')
//					to a number representing the column location (starting
//					with column 'A' = 0).
//
// Input Arguments:
//		column	= const wxString& containing the column identifier
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int representing the 0-based column location
//
//==========================================================================
unsigned int XlsxReader::ColumnNumberFromString(const wxString &column) const
{
	unsigned int i, columnNumber = 0;
	unsigned int factor = 1;
	for (i = column.Length(); i >= 1; i--)
	{
		columnNumber += ((unsigned int)*column.Mid(i - 1, 1).c_str() - 64) * factor;
		factor *= 26;
	}

	return columnNumber - 1;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetCellData
//
// Description:		Reads the data from the specified cell and returns a
//					wxString with the contents.
//
// Input Arguments:
//		sheet	= const unsigned int&
//		row		= const unsigned int&
//		column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString with the contents of the specified cell
//
//==========================================================================
wxString XlsxReader::GetCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const
{
	assert(sheet < (unsigned int)worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *sheetDataNode = worksheets[sheet]->GetRoot()->GetChildren();
	wxString cellString = GetCellAddressString(row, column,
		GetReferenceCell(GetDimensionString(*worksheets[sheet])));
	wxString rowString;
	rowString.Printf("%i", row + 1);
	while (sheetDataNode)
	{
		// Is this the sheet data?
		if (sheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *rowNode = sheetDataNode->GetChildren();
			while (rowNode)
			{
				// Is this a row?
				if (rowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (rowNode->GetAttribute(_T("r")).CompareTo(rowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *columnNode = rowNode->GetChildren();

						while (columnNode)
						{
							// Is this the column we're looking for?
							if (columnNode->GetAttribute(_T("r")).CompareTo(cellString) == 0)
							{
								// Is the value numeric?
								if (columnNode->HasAttribute(_T("t")))
								{
									// Check to see if we know what kind of data this is
									if (columnNode->GetAttribute(_T("t")).CompareTo(_T("s")) == 0)// String
									{
										// The cell is numeric, but we can still handle this - return a string containing the value
										wxXmlNode *valueNode = columnNode->GetChildren();
										while (valueNode)
										{
											// Make sure we're looking at the correct node, then return the content
											if (valueNode->GetName().CompareTo(_T("v")) == 0)
											{
												long index;
												valueNode->GetNodeContent().ToLong(&index);
												return GetSharedString(index);
											}

											// Get the next node
											valueNode = valueNode->GetNext();
										}
									}
									else
										return _T("Unknown data type");// FIXME!!!
								}
								else
								{
									// The cell is numeric, but we can still handle this - return a string containing the value
									wxXmlNode *valueNode = columnNode->GetChildren();
									while (valueNode)
									{
										// Make sure we're looking at the correct node, then return the content
										if (valueNode->GetName().CompareTo(_T("v")) == 0)
											return valueNode->GetNodeContent();

										// Get the next node
										valueNode = valueNode->GetNext();
									}
								}
							}

							// Get the next column
							columnNode = columnNode->GetNext();
						}
					}
				}

				// Get the next node
				rowNode = rowNode->GetNext();
			}
		}

		// Get the next child
		sheetDataNode = sheetDataNode->GetNext();
	}

	// If the cell wasn't found, return an empty string
	return wxEmptyString;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetCellData
//
// Description:		Returns the numeric contents of the specified cell.
//
// Input Arguments:
//		sheet	= const unsigned int&
//		row		= const unsigned int&
//		column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double equal to the cell value
//
//==========================================================================
double XlsxReader::GetNumericCellData(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const
{
	assert(sheet < (unsigned int)worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *sheetDataNode = worksheets[sheet]->GetRoot()->GetChildren();
	wxString cellString = GetCellAddressString(row, column,
		GetReferenceCell(GetDimensionString(*worksheets[sheet])));
	wxString rowString;
	rowString.Printf("%i", row + 1);
	while (sheetDataNode)
	{
		// Is this the sheet data?
		if (sheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *rowNode = sheetDataNode->GetChildren();
			while (rowNode)
			{
				// Is this a row?
				if (rowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (rowNode->GetAttribute(_T("r")).CompareTo(rowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *columnNode = rowNode->GetChildren();

						while (columnNode)
						{
							// Is this the column we're looking for?
							if (columnNode->GetAttribute(_T("r")).CompareTo(cellString) == 0)
							{
								// Is the value numeric?
								if (columnNode->HasAttribute(_T("t")))
									// Value is NOT numeric - return QNaN
									return std::numeric_limits<double>::quiet_NaN();

								// Return the value of this cell
								wxXmlNode *valueNode = columnNode->GetChildren();
								while (valueNode)
								{
									// Make sure we're looking at the correct node, then return the content
									if (valueNode->GetName().CompareTo(_T("v")) == 0)
									{
										double number;
										if (valueNode->GetNodeContent().ToDouble(&number))
											return number;

										// Conversion did not occur successfully - return QNaN
										return std::numeric_limits<double>::quiet_NaN();
									}

									// Get the next node
									valueNode = valueNode->GetNext();
								}
							}

							// Get the next column
							columnNode = columnNode->GetNext();
						}
					}
				}

				// Get the next node
				rowNode = rowNode->GetNext();
			}
		}

		// Get the next child
		sheetDataNode = sheetDataNode->GetNext();
	}

	// If the cell wasn't found, return zero
	return 0.0;
}

//==========================================================================
// Class:			XlsxReader
// Function:		CellIsNumeric
//
// Description:		Checks to see if a cell's content is numeric.
//
// Input Arguments:
//		sheet	= const unsigned int&
//		row		= const unsigned int&
//		column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the value is a number, false otherwise
//
//==========================================================================
bool XlsxReader::CellIsNumeric(const unsigned int &sheet,
		const unsigned int &row, const unsigned int &column) const
{
	assert(sheet < (unsigned int)worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *sheetDataNode = worksheets[sheet]->GetRoot()->GetChildren();
	wxString cellString = GetCellAddressString(row, column,
		GetReferenceCell(GetDimensionString(*worksheets[sheet])));
	wxString rowString;
	rowString.Printf("%i", row + 1);
	while (sheetDataNode)
	{
		// Is this the sheet data?
		if (sheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *rowNode = sheetDataNode->GetChildren();
			while (rowNode)
			{
				// Is this a row?
				if (rowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (rowNode->GetAttribute(_T("r")).CompareTo(rowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *columnNode = rowNode->GetChildren();

						while (columnNode)
						{
							// Is this the column we're looking for?
							if (columnNode->GetAttribute(_T("r")).CompareTo(cellString) == 0)
							{
								// Is the value numeric?
								if (columnNode->HasAttribute(_T("t")))
									return false;
								else
									return true;
							}

							// Get the next column
							columnNode = columnNode->GetNext();
						}
					}
				}

				// Get the next node
				rowNode = rowNode->GetNext();
			}
		}

		// Get the next child
		sheetDataNode = sheetDataNode->GetNext();
	}

	// If the cell wasn't found, return true
	return true;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetCellAddressString
//
// Description:		Returns the cell's Excel-style address from the row and
//					column indecies.  For example, calling with (0, 0, "A1")
//					would return "A1".
//
// Input Arguments:
//		row		= const unsigned int&
//		column	= const unsigned int&
//		referenceCell	= const wxString& identifying the first cell with any
//						  content in the worksheet (may not necessarily be A1)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the cell's address string
//
//==========================================================================
wxString XlsxReader::GetCellAddressString(const unsigned int &row, const unsigned int column,
	const wxString &referenceCell) const
{
	// Get the absolute location of the cell (including the reference offset)
	unsigned int i;
	for (i = 0; i < referenceCell.Length(); i++)
	{
		if (referenceCell.Mid(i, 1).IsNumber())
			break;
	}

	long referenceRow;
	referenceCell.Mid(i, referenceCell.Length() - i).ToLong(&referenceRow);
	unsigned int absoluteRow = row + referenceRow;
	// +1 here because ColumnNumberFromString() returns 0-based and we want 1-based index
	unsigned int absoluteColumn = column + ColumnNumberFromString(referenceCell.Mid(0, i)) + 1;

	// FIXME:  This only works for two-character or less addresses
	wxString columnString;
	if (absoluteColumn > 26)
	{
		columnString = wxChar(wxChar((absoluteColumn - 1) / 26) + 64);
		columnString.Append(wxChar((absoluteColumn - 1) % 26 + 65));
	}
	else
		columnString = wxChar((absoluteColumn - 1) % 26 + 65);

	wxString address;
	address.Printf("%i", absoluteRow);
	address.Prepend(columnString);

	return address;
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetReferenceCell
//
// Description:		Returns the first portion of the dimension string (the
//					first cell in the sheet with any content) from which
//					all row and column references are based.
//
// Input Arguments:
//		dimensionString	= const wxString& containing the sheet's occupied range
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the reference cell address
//
//==========================================================================
wxString XlsxReader::GetReferenceCell(const wxString &dimensionString) const
{
	// Find the location of the colon
	int colonLocation = dimensionString.Find(':');

	// If it isn't found, return the orignal string (it wasn't a range)
	if (colonLocation == wxNOT_FOUND)
		return dimensionString;

	// Return everything up to the colon
	return dimensionString.Mid(0, colonLocation);
}

//==========================================================================
// Class:			XlsxReader
// Function:		GetSharedString
//
// Description:		Looks up the specified string index in the shared strings
//					XML file.
//
// Input Arguments:
//		index	= const unsigned int& specifying which string to use
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the reference cell address
//
//==========================================================================
wxString XlsxReader::GetSharedString(const unsigned int &index) const
{
	// Locate the desired string entry
	wxXmlNode *stringNode = sharedStrings->GetRoot()->GetChildren();
	unsigned int counter(0);
	while (stringNode)
	{
		// Keep looping until we reach the desired entry
		if (counter < index)
		{
			// Get the next child and increment the counter
			stringNode = stringNode->GetNext();
			counter++;
		}
		else
		{
			// Is this a string entry?
			if (stringNode->GetName().CompareTo(_T("si")) == 0)
			{
				wxXmlNode *textNode = stringNode->GetChildren();
				while (textNode)
				{
					// Is this text?
					if (textNode->GetName().CompareTo(_T("t")) == 0)
						return textNode->GetNodeContent();

					// Get the next node
					textNode = textNode->GetNext();
				}
			}
		}
	}

	// If we can't find anything, return an empty string
	return wxEmptyString;
}