/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  xlsx_reader_class.cpp
// Created:  10/21/2010
// Author:  K. Loux
// Description:  Class for reading from Excel's XML-based files.  Contains functions
//				 intended for extracting columns of data.
// History:

// Standard C++ headers
#include <limits>

// wxWidgets headers
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

// Local headers
#include "vUtilities/xlsx_reader_class.h"

//==========================================================================
// Class:			XLSX_READER
// Function:		XLSX_READER
//
// Description:		Constructor for the XLSX_READER class.
//
// Input Arguments:
//		_PathAndFileName	= wxString describing of the .xlsx file to read.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
XLSX_READER::XLSX_READER(const wxString &_PathAndFileName)
{
	IsOkFlag = false;
	PathAndFileName = _PathAndFileName;
	Workbook = NULL;
	SharedStrings = NULL;

	// Make sure the file exists
	if (!wxFileExists(PathAndFileName))
		return;

	// Attempt to read relevant information from the file
	if (Initialize())
		IsOkFlag =  true;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		~XLSX_READER
//
// Description:		Destructor for the XLSX_READER class.
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
XLSX_READER::~XLSX_READER()
{
	// Delete dynamic memory
	Worksheets.Clear();

	delete Workbook;
	Workbook = NULL;

	delete SharedStrings;
	SharedStrings = NULL;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetNumberOfColumns
//
// Description:		Returns the numer of columns for the specified sheet.
//
// Input Arguments:
//		Sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of columns
//
//==========================================================================
unsigned int XLSX_READER::GetNumberOfColumns(const unsigned int &Sheet) const
{
	assert(Sheet < Sheets.size());

	// Get the number of columns from the specified sheet
	return ParseForColumnCount(GetDimensionString(*Worksheets[Sheet]));
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetNumberOfRows
//
// Description:		Returns the number of rows for the specified sheet.
//
// Input Arguments:
//		Sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of rows
//
//==========================================================================
unsigned int XLSX_READER::GetNumberOfRows(const unsigned int &Sheet) const
{
	assert(Sheet < Sheets.size());

	// Get the number of rows from the specified sheet
	return ParseForRowCount(GetDimensionString(*Worksheets[Sheet]));
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetSheetName
//
// Description:		Returns the name of the specified sheet.
//
// Input Arguments:
//		Sheet	= const unsigned int& identifying the sheet
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the sheet name
//
//==========================================================================
wxString XLSX_READER::GetSheetName(const unsigned int &Sheet) const
{
	assert(Sheet < Sheets.size());

	return Sheets[Sheet].first;
}

//==========================================================================
// Class:			XLSX_READER
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
unsigned int XLSX_READER::GetSelectedSheet(void) const
{
	// Check each sheet until we find the selected one
	int i;
	for (i = 0; i < Worksheets.GetCount(); i++)
	{
		if (SheetIsSelected(*Worksheets[i]))
			return i;
	}

	// If we can't find the selected sheet, return an invalid number
	return Worksheets.GetCount();
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetEntry
//
// Description:		Locates the wxZipEntry in the wxZipInputStream with the
//					specified name.
//
// Input Arguments:
//		ZipStream	= wxZipEntryStream& to look in
//		EntryString	= const wxString& containing the name of the entry we're
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
wxZipEntry *XLSX_READER::GetEntry(wxZipInputStream &ZipStream, const wxString &EntryString) const
{
	// Parse the file and record relevant information
	wxZipEntry *ZipPointer = NULL;
	while (ZipPointer = ZipStream.GetNextEntry(), ZipPointer != NULL)
	{
		// Look for the workbook pointer
		if (ZipPointer->GetInternalName().CompareTo(wxZipEntry::GetInternalName(EntryString)) == 0)
			return ZipPointer;

		delete ZipPointer;
		ZipPointer = NULL;
	}

	return NULL;
}

//==========================================================================
// Class:			XLSX_READER
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
wxZipInputStream *XLSX_READER::OpenFile(void) const
{
	// Open the archive
	wxFFileInputStream *FileInput = new wxFFileInputStream(PathAndFileName);
	if (FileInput->IsOk())
	{
		wxZipInputStream *ZipStream = new wxZipInputStream(FileInput);
		if (ZipStream->IsOk())
			return ZipStream;
	}

	return NULL;
}

//==========================================================================
// Class:			XLSX_READER
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
bool XLSX_READER::Initialize(void)
{
	// Open the archive
	wxZipInputStream *ZipStream = OpenFile();

	// Locate the workbook entry
	wxZipEntry *ZipPointer = GetEntry(*ZipStream, _T("xl\\workbook.xml"));
	if (ZipPointer == NULL)
		return false;

	// Open the workbook stream as an XML document
	Workbook = new wxXmlDocument(*ZipStream);
	if (!Workbook->IsOk())
		return false;

	// Delete the ZipPointer
	delete ZipPointer;
	ZipPointer = NULL;

	// Locate the shared strings entry
	// FIXME:  This only works because sharedStrings.xml is AFTER workbook.xml
	// (reverse alphabetical order?) - will this always work, or should the file
	// be closed and re-opened to ensure it is found?
	ZipPointer = GetEntry(*ZipStream, _T("xl\\sharedStrings.xml"));
	if (ZipPointer == NULL)
		return false;

	// Open the shared strings stream
	SharedStrings = new wxXmlDocument(*ZipStream);
	if (!SharedStrings->IsOk())
		return false;

	// Delete the ZipPointer
	delete ZipPointer;
	ZipPointer = NULL;

	// Locate the sheets
	wxXmlNode *Node = Workbook->GetRoot()->GetChildren();
	wxString TempName;
	while (Node)
	{
		// Does this node contain sheet information?
		if (Node->GetName().CompareTo(_T("sheets")) == 0)
		{
			wxXmlNode *SheetNode = Node->GetChildren();
			unsigned long SheetId;
			wxString IdString;
			while (SheetNode)
			{
				IdString = SheetNode->GetPropVal(_T("r:id"), _T("-1"));
				IdString.Mid(3, IdString.Length() - 3).ToULong(&SheetId);
				std::pair<wxString, unsigned int> SheetData(SheetNode->GetPropVal(_T("name"), _T("")), SheetId);
				Sheets.push_back(SheetData);

				// Get the next sheet
				SheetNode = SheetNode->GetNext();
			}
		}

		// Get the next child
		Node = Node->GetNext();
	}

	// Close the file
	delete ZipStream;
	ZipStream = NULL;

	// Now that we know how many sheets we're looking for, load the data from each sheet
	unsigned int Sheet;
	for (Sheet = 0; Sheet < Sheets.size(); Sheet++)
	{
		if (!LoadSheet(Sheet))
			return false;
	}

	return true;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		LoadSheet
//
// Description:		Loads the XML content of the worksheets and stores the
//					document objects.
//
// Input Arguments:
//		Sheet	= const unsigned int& specifying the sheet to load
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool XLSX_READER::LoadSheet(const unsigned int &Sheet)
{
	// Open the stream
	wxZipInputStream *ZipStream = OpenFile();

	// Locate the relevant entry point
	wxString SheetString;
	SheetString.Printf("xl\\worksheets\\sheet%i.xml", Sheets[Sheet].second);
	wxZipEntry *ZipPointer = GetEntry(*ZipStream, SheetString);

	// Open the stream as an XML file
	Worksheets.Add(new wxXmlDocument(*ZipStream));
	if (!Worksheets[Sheet]->IsOk())
		return false;

	// Delete the ZipPointer
	delete ZipPointer;
	ZipPointer = NULL;

	// Close the file
	delete ZipStream;
	ZipStream = NULL;

	return true;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetDimensionString
//
// Description:		Retrieves the string describing the range of this document
//					from the passed wxXmlDocument
//
// Input Arguments:
//		SheetDocument	= const wxXmlDocument&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the range of the document
//
//==========================================================================
wxString XLSX_READER::GetDimensionString(const wxXmlDocument &SheetDocument) const
{
	wxXmlNode *Node = SheetDocument.GetRoot()->GetChildren();
	while (Node)
	{
		// Does this node contain sheet information?
		if (Node->GetName().CompareTo(_T("dimension")) == 0)
			return Node->GetPropVal(_T("ref"), _T(""));

		// Get the next child
		Node = Node->GetNext();
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		SheetsSelected
//
// Description:		Looks for nodes and properties indicating that this sheet
//					was the active sheet when the document was saved.
//
// Input Arguments:
//		SheetDocument	= const wxXmlDocument&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for is selected, false otherwise
//
//==========================================================================
bool XLSX_READER::SheetIsSelected(const wxXmlDocument &SheetDocument) const
{
	// Locate the sheet view properties
	wxXmlNode *Node = SheetDocument.GetRoot()->GetChildren();
	while (Node)
	{
		// Does this node contain sheet information?
		if (Node->GetName().CompareTo(_T("sheetViews")) == 0)
		{
			wxXmlNode *ViewNode = Node->GetChildren();
			while (ViewNode)
			{
				if (ViewNode->GetName().CompareTo(_T("sheetView")) == 0)
				{
					if (ViewNode->HasProp(_T("tabSelected")))
					{
						long Selected = 0;
						ViewNode->GetPropVal(_T("tabSelected"), _T("")).ToLong(&Selected);
						return Selected == 1;
					}
				}

				// Get the next view node
				ViewNode = ViewNode->GetNext();
			}
		}

		// Get the next child
		Node = Node->GetNext();
	}

	return false;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		ParseForRowCount
//
// Description:		Parses a string describing a cell range to determine the
//					number of rows contained within the range.
//
// Input Arguments:
//		DimensionString	= const wxString& describing the range (i.e. "B4:ED56")
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of rows in the range
//
//==========================================================================
unsigned int XLSX_READER::ParseForRowCount(const wxString &DimensionString) const
{
	// If there is no :, the range consists of only one row
	int ColonLocation = DimensionString.Find(':');
	if (ColonLocation == wxNOT_FOUND)
		return 1;

	// Search for the first numeric character before the colon
	unsigned int i;
	for (i = 0; i < DimensionString.Length(); i++)
	{
		if (DimensionString.Mid(i, 1).IsNumber())
			break;
	}

	// Determine the first row containing data
	long FirstRow;
	DimensionString.Mid(i, ColonLocation - 1).ToLong(&FirstRow);

	// Find the last row containing data
	for (i = ColonLocation + 1; i < DimensionString.Length(); i++)
	{
		if (DimensionString.Mid(i, 1).IsNumber())
		{
			long LastRow;
			DimensionString.Mid(i, DimensionString.Length() - i).ToLong(&LastRow);
			
			// Compute the number of rows containing data
			return int(LastRow - FirstRow) + 1;
		}
	}

	return 0;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		ParseForColumnCount
//
// Description:		Parses a string describing a cell range to determine the
//					number of columns contained within the range.
//
// Input Arguments:
//		DimensionString	= const wxString& describing the range (i.e. "B4:ED56")
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the number of columns in the range
//
//==========================================================================
unsigned int XLSX_READER::ParseForColumnCount(const wxString &DimensionString) const
{
	// If there is no :, the range consists of only one column
	int ColonLocation = DimensionString.Find(':');
	if (ColonLocation == wxNOT_FOUND)
		return 1;

	// Search for the first numeric character before the colon
	unsigned int i;
	for (i = 0; i < DimensionString.Length(); i++)
	{
		if (DimensionString.Mid(i, 1).IsNumber())
			break;
	}

	// Determine the first column containing data
	long FirstColumn = ColumnNumberFromString(DimensionString.Mid(0, i));

	// Find the last column containing data
	for (i = ColonLocation + 1; i < DimensionString.Length(); i++)
	{
		if (DimensionString.Mid(i, 1).IsNumber())
			// Compute the number of columns containing data
			return ColumnNumberFromString(DimensionString.Mid(
			ColonLocation + 1, i - ColonLocation - 1)) - FirstColumn + 1;
	}

	return 0;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		ColumnNumberFromString
//
// Description:		Converts from Excel's column identifier string (i.e. 'A')
//					to a number representing the column location (starting
//					with column 'A' = 0).
//
// Input Arguments:
//		Column	= const wxString& containing the column identifier
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int representing the 0-based column location
//
//==========================================================================
unsigned int XLSX_READER::ColumnNumberFromString(const wxString &Column) const
{
	unsigned int i, ColumnNumber = 0;
	unsigned int factor = 1;
	for (i = Column.Length(); i >= 1; i--)
	{
		ColumnNumber += ((unsigned int)*Column.Mid(i - 1, 1).c_str() - 64) * factor;
		factor *= 26;
	}

	return ColumnNumber - 1;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetCellData
//
// Description:		Reads the data from the specified cell and returns a
//					wxString with the contents.
//
// Input Arguments:
//		Sheet	= const unsigned int&
//		Row		= const unsigned int&
//		Column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString with the contents of the specified cell
//
//==========================================================================
wxString XLSX_READER::GetCellData(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const
{
	assert(Sheet < (unsigned int)Worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *SheetDataNode = Worksheets[Sheet]->GetRoot()->GetChildren();
	wxString CellString = GetCellAddressString(Row, Column,
		GetReferenceCell(GetDimensionString(*Worksheets[Sheet])));
	wxString RowString;
	RowString.Printf("%i", Row + 1);
	while (SheetDataNode)
	{
		// Is this the sheet data?
		if (SheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *RowNode = SheetDataNode->GetChildren();
			while (RowNode)
			{
				// Is this a row?
				if (RowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (RowNode->GetPropVal(_T("r"), _T("")).CompareTo(RowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *ColumnNode = RowNode->GetChildren();

						while (ColumnNode)
						{
							// Is this the column we're looking for?
							if (ColumnNode->GetPropVal(_T("r"), _T("")).CompareTo(CellString) == 0)
							{
								// Is the value numeric?
								if (ColumnNode->HasProp(_T("t")))
								{
									// Check to see if we know what kind of data this is
									if (ColumnNode->GetPropVal(_T("t"), _T("")).CompareTo(_T("s")) == 0)// String
									{
										// The cell is numeric, but we can still handle this - return a string containing the value
										wxXmlNode *ValueNode = ColumnNode->GetChildren();
										while (ValueNode)
										{
											// Make sure we're looking at the correct node, then return the content
											if (ValueNode->GetName().CompareTo(_T("v")) == 0)
											{
												long Index;
												ValueNode->GetNodeContent().ToLong(&Index);
												return GetSharedString(Index);
											}

											// Get the next node
											ValueNode = ValueNode->GetNext();
										}
									}
									else
										return _T("Unknown data type");// FIXME!!!
								}
								else
								{
									// The cell is numeric, but we can still handle this - return a string containing the value
									wxXmlNode *ValueNode = ColumnNode->GetChildren();
									while (ValueNode)
									{
										// Make sure we're looking at the correct node, then return the content
										if (ValueNode->GetName().CompareTo(_T("v")) == 0)
											return ValueNode->GetNodeContent();

										// Get the next node
										ValueNode = ValueNode->GetNext();
									}
								}
							}

							// Get the next column
							ColumnNode = ColumnNode->GetNext();
						}
					}
				}

				// Get the next node
				RowNode = RowNode->GetNext();
			}
		}

		// Get the next child
		SheetDataNode = SheetDataNode->GetNext();
	}

	// If the cell wasn't found, return an empty string
	return wxEmptyString;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetCellData
//
// Description:		Returns the numeric contents of the specified cell.
//
// Input Arguments:
//		Sheet	= const unsigned int&
//		Row		= const unsigned int&
//		Column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double equal to the cell value
//
//==========================================================================
double XLSX_READER::GetNumericCellData(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const
{
	assert(Sheet < (unsigned int)Worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *SheetDataNode = Worksheets[Sheet]->GetRoot()->GetChildren();
	wxString CellString = GetCellAddressString(Row, Column,
		GetReferenceCell(GetDimensionString(*Worksheets[Sheet])));
	wxString RowString;
	RowString.Printf("%i", Row + 1);
	while (SheetDataNode)
	{
		// Is this the sheet data?
		if (SheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *RowNode = SheetDataNode->GetChildren();
			while (RowNode)
			{
				// Is this a row?
				if (RowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (RowNode->GetPropVal(_T("r"), _T("")).CompareTo(RowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *ColumnNode = RowNode->GetChildren();

						while (ColumnNode)
						{
							// Is this the column we're looking for?
							if (ColumnNode->GetPropVal(_T("r"), _T("")).CompareTo(CellString) == 0)
							{
								// Is the value numeric?
								if (ColumnNode->HasProp(_T("t")))
									// Value is NOT numeric - return QNaN
									return std::numeric_limits<double>::quiet_NaN();

								// Return the value of this cell
								wxXmlNode *ValueNode = ColumnNode->GetChildren();
								while (ValueNode)
								{
									// Make sure we're looking at the correct node, then return the content
									if (ValueNode->GetName().CompareTo(_T("v")) == 0)
									{
										double Number;
										if (ValueNode->GetNodeContent().ToDouble(&Number))
											return Number;

										// Conversion did not occur successfully - return QNaN
										return std::numeric_limits<double>::quiet_NaN();
									}

									// Get the next node
									ValueNode = ValueNode->GetNext();
								}
							}

							// Get the next column
							ColumnNode = ColumnNode->GetNext();
						}
					}
				}

				// Get the next node
				RowNode = RowNode->GetNext();
			}
		}

		// Get the next child
		SheetDataNode = SheetDataNode->GetNext();
	}

	// If the cell wasn't found, return zero
	return 0.0;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		CellIsNumeric
//
// Description:		Checks to see if a cell's content is numeric.
//
// Input Arguments:
//		Sheet	= const unsigned int&
//		Row		= const unsigned int&
//		Column	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the value is a number, false otherwise
//
//==========================================================================
bool XLSX_READER::CellIsNumeric(const unsigned int &Sheet,
		const unsigned int &Row, const unsigned int &Column) const
{
	assert(Sheet < (unsigned int)Worksheets.GetCount());

	// Locate the desired cell
	wxXmlNode *SheetDataNode = Worksheets[Sheet]->GetRoot()->GetChildren();
	wxString CellString = GetCellAddressString(Row, Column,
		GetReferenceCell(GetDimensionString(*Worksheets[Sheet])));
	wxString RowString;
	RowString.Printf("%i", Row + 1);
	while (SheetDataNode)
	{
		// Is this the sheet data?
		if (SheetDataNode->GetName().CompareTo(_T("sheetData")) == 0)
		{
			wxXmlNode *RowNode = SheetDataNode->GetChildren();
			while (RowNode)
			{
				// Is this a row?
				if (RowNode->GetName().CompareTo(_T("row")) == 0)
				{
					// Is this the row we're looking for
					if (RowNode->GetPropVal(_T("r"), _T("")).CompareTo(RowString) == 0)
					{
						// Now parse the columns
						wxXmlNode *ColumnNode = RowNode->GetChildren();

						while (ColumnNode)
						{
							// Is this the column we're looking for?
							if (ColumnNode->GetPropVal(_T("r"), _T("")).CompareTo(CellString) == 0)
							{
								// Is the value numeric?
								if (ColumnNode->HasProp(_T("t")))
									return false;
								else
									return true;
							}

							// Get the next column
							ColumnNode = ColumnNode->GetNext();
						}
					}
				}

				// Get the next node
				RowNode = RowNode->GetNext();
			}
		}

		// Get the next child
		SheetDataNode = SheetDataNode->GetNext();
	}

	// If the cell wasn't found, return true
	return true;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetCellAddressString
//
// Description:		Returns the cell's Excel-style address from the row and
//					column indecies.  For example, calling with (0, 0, "A1")
//					would return "A1".
//
// Input Arguments:
//		Row		= const unsigned int&
//		Column	= const unsigned int&
//		ReferenceCell	= const wxString& identifying the first cell with any
//						  content in the worksheet (may not necessarily be A1)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the cell's address string
//
//==========================================================================
wxString XLSX_READER::GetCellAddressString(const unsigned int &Row, const unsigned int Column,
	const wxString &ReferenceCell) const
{
	// Get the absolute location of the cell (including the reference offset)
	unsigned int i;
	for (i = 0; i < ReferenceCell.Length(); i++)
	{
		if (ReferenceCell.Mid(i, 1).IsNumber())
			break;
	}

	long ReferenceRow;
	ReferenceCell.Mid(i, ReferenceCell.Length() - i).ToLong(&ReferenceRow);
	unsigned int AbsoluteRow = Row + ReferenceRow;
	// +1 here because ColumnNumberFromString() returns 0-based and we want 1-based index
	unsigned int AbsoluteColumn = Column + ColumnNumberFromString(ReferenceCell.Mid(0, i)) + 1;

	// FIXME:  This only works for two-character or less addresses
	wxString ColumnString;
	if (AbsoluteColumn > 26)
	{
		ColumnString = wxChar(wxChar((AbsoluteColumn - 1) / 26) + 64);
		ColumnString.Append(wxChar((AbsoluteColumn - 1) % 26 + 65));
	}
	else
		ColumnString = wxChar((AbsoluteColumn - 1) % 26 + 65);

	wxString Address;
	Address.Printf("%i", AbsoluteRow);
	Address.Prepend(ColumnString);

	return Address;
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetReferenceCell
//
// Description:		Returns the first portion of the dimension string (the
//					first cell in the sheet with any content) from which
//					all row and column references are based.
//
// Input Arguments:
//		DimensionString	= const wxString& containing the sheet's occupied range
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the reference cell address
//
//==========================================================================
wxString XLSX_READER::GetReferenceCell(const wxString &DimensionString) const
{
	// Find the location of the colon
	int ColonLocation = DimensionString.Find(':');

	// If it isn't found, return the orignal string (it wasn't a range)
	if (ColonLocation == wxNOT_FOUND)
		return DimensionString;

	// Return everything up to the colon
	return DimensionString.Mid(0, ColonLocation);
}

//==========================================================================
// Class:			XLSX_READER
// Function:		GetSharedString
//
// Description:		Looks up the specified string index in the shared strings
//					XML file.
//
// Input Arguments:
//		Index	= const unsigned int& specifying which string to use
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the reference cell address
//
//==========================================================================
wxString XLSX_READER::GetSharedString(const unsigned int &Index) const
{
	// Locate the desired string entry
	wxXmlNode *StringNode = SharedStrings->GetRoot()->GetChildren();
	unsigned int Counter(0);
	while (StringNode)
	{
		// Keep looping until we reach the desired entry
		if (Counter < Index)
		{
			// Get the next child and increment the counter
			StringNode = StringNode->GetNext();
			Counter++;
		}
		else
		{
			// Is this a string entry?
			if (StringNode->GetName().CompareTo(_T("si")) == 0)
			{
				wxXmlNode *TextNode = StringNode->GetChildren();
				while (TextNode)
				{
					// Is this text?
					if (TextNode->GetName().CompareTo(_T("t")) == 0)
						return TextNode->GetNodeContent();

					// Get the next node
					TextNode = TextNode->GetNext();
				}
			}
		}
	}

	// If we can't find anything, return an empty string
	return wxEmptyString;
}