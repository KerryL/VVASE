/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataValidator.cpp
// Created:  12/12/2011
// Author:  K. Loux
// Description:  Class derived from wxTextValidator, used for converting to/from
//				 the various default units in the Convert class.  Also handles
//				 UndoRedoStack entries.
// History:
//	12/12/2011	- Created and debugged, K. Loux.

// Local headers
#include "vUtilities/dataValidator.h"

//==========================================================================
// Class:			DataValidator
// Function:		TransferToWindow
//
// Description:		Converts the referenced data to the user-specified type
//					and sends it to the window.
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
template<>
bool DataValidator<double>::TransferToWindow()
{
	dynamic_cast<wxTextEntry*>(m_validatorWindow)->SetValue(
		UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertOutput(data, unit)));
	
	return true;
}

//==========================================================================
// Class:			DataValidator
// Function:		TransferFromWindow
//
// Description:		Reads from the control and converts to our internal units.
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
template<>
bool DataValidator<double>::TransferFromWindow()
{
	double value;
	if (!dynamic_cast<wxTextEntry*>(m_validatorWindow)->GetValue().ToDouble(&value))
		return false;
		
	data = UnitConverter::GetInstance().ConvertInput(value, unit);
	
	return true;
}