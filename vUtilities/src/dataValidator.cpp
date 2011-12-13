/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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
// Function:		DataValidator
//
// Description:		Constructor for the DataValidator class.
//
// Input Arguments:
//		_unit	= const Convert::UnitType& specifying the type of data we
//				  represent
//		_valPtr	= double* pointer to the data we represent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DataValidator::DataValidator(const Convert::UnitType &_unit, double *_valPtr)
							: wxTextValidator(wxFILTER_NUMERIC)
{
	unit = _unit;
	valPtr = _valPtr;
}

//==========================================================================
// Class:			DataValidator
// Function:		DataValidator
//
// Description:		Copy constructor for the DataValidator class.
//
// Input Arguments:
//		dv	= const DataValidator& to copy to this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DataValidator::DataValidator(const DataValidator& dv)
{
	// Make the assignment
	*this = dv;
}

//==========================================================================
// Class:			DataValidator
// Function:		Clone
//
// Description:		Returns a pointer to a new validator.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		DataValidator*
//
//==========================================================================
DataValidator* DataValidator::Clone(void)
{
	return new DataValidator(*this);
}

//==========================================================================
// Class:			DataValidator
// Function:		Clone
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
bool DataValidator::TransferToWindow(void)
{
	if (!CheckValidator())
		return false;
	
	if (valPtr)
	{
		static_cast<wxTextCtrl*>(m_validatorWindow)->SetValue(
			Convert::GetInstance().FormatNumber(
			Convert::GetInstance().ConvertTo(*valPtr, unit)));
	}
	
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
bool DataValidator::TransferFromWindow(void)
{
	if (!CheckValidator())
		return false;
	
	if (valPtr)
	{
		double value;
		if (!static_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().ToDouble(&value))
			return false;
		
		*valPtr = Convert::GetInstance().Read(value, unit);
	}
	
	return true;
}

//==========================================================================
// Class:			DataValidator
// Function:		operator=
//
// Description:		Assignment operator overload.
//
// Input Arguments:
//		dv	= const DataValidator& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		DataValidator& reference to this
//
//==========================================================================
DataValidator& DataValidator::operator=(const DataValidator& dv)
{
	// Check for self assignment
	if (this == &dv)
		return *this;
	
	// Make the assignments
	unit = dv.unit;
	valPtr = dv.valPtr;
	
	return *this;
}