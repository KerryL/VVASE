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

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vUtilities/dataValidator.h"

//==========================================================================
// Class:			DataValidator
// Function:		DataValidator
//
// Description:		Constructor for the DataValidator class.
//
// Input Arguments:
//		_unit			= const Convert::UnitType& specifying the type of data we
//						  represent
//		_valPtr			= double* pointer to the data we represent
//		_numberClass	= const NumberClass& specifying additional limits
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DataValidator::DataValidator(const Convert::UnitType &_unit, double *_valPtr,
							const NumberClass &_numberClass)
							: wxTextValidator(wxFILTER_NUMERIC)
{
	assert(_numberClass != ClassInclusiveRange && _numberClass != ClassExclusiveRange);

	// Assign to the class members
	unit = _unit;
	if (_valPtr)
		valPtr = _valPtr;
	else
		valPtr = &m_value;
	numberClass = _numberClass;
}

//==========================================================================
// Class:			DataValidator
// Function:		DataValidator
//
// Description:		Constructor for the DataValidator class, for use with
//					enforcing numeric ranges.
//
// Input Arguments:
//		_unit			= const Convert::UnitType& specifying the type of data we
//						  represent
//		_min			= const double&, minimum acceptable value for the data
//		_max			= const double&, maximum acceptable value for the data
//		_valPtr			= double* pointer to the data we represent
//		_numberClass	= const NumberClass&, specifying whether the range is inclusive or exclusive
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DataValidator::DataValidator(const Convert::UnitType &_unit, const double &_min,
		const double &_max, double *_valPtr, const NumberClass &_numberClass)
{
	assert(_min < _max);
	assert(_numberClass == ClassInclusiveRange || _numberClass == ClassExclusiveRange);

	// Assign to the class members
	unit = _unit;
	if (_valPtr)
		valPtr = _valPtr;
	else
		valPtr = &m_value;
	numberClass = _numberClass;
	min = _min;
	max = _max;
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
bool DataValidator::TransferToWindow(void)
{
	/*if (!CheckValidator())
		return false;*/
	
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
	/*if (!CheckValidator())
		return false;*/
	
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
// Function:		Validate
//
// Description:		Checks to see if the control's contents are valid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool DataValidator::Validate(wxWindow *parent)
{
	/*if (!CheckValidator())
		return false;*/

	double value;
	if (!static_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().ToDouble(&value))
		return false;

	assert(numberClass >= ClassAll && numberClass < ClassCount);

	switch (numberClass)
	{
	default:
	case ClassAll:
		break;

	case ClassPositive:
		if (value < 0.0)
			return false;
		break;

	case ClassStrictlyPositive:
		if (value <= 0.0)
			return false;
		break;

	case ClassNegative:
		if (value > 0.0)
			return false;
		break;

	case ClassStrictlyNegative:
		if (value >= 0.0)
			return false;
		break;

	case ClassInclusiveRange:
		if (value < min || value > max)
			return false;
		break;

	case ClassExclusiveRange:
		if (value <= min || value >= max)
			return false;
		break;
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

	numberClass = dv.numberClass;

	min = dv.min;
	max = dv.max;

	m_value = dv.m_value;

	if (dv.valPtr == &dv.m_value)
		valPtr = &m_value;
	else
		valPtr = dv.valPtr;
	
	return *this;
}