/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integerValidator.cpp
// Created:  12/13/2011
// Author:  K. Loux
// Description:  Class derived from wxTextValidator, used for enforcing limits on
//				 integer value entries.
// History:
//	12/13/2011	- Created and debugged, K. Loux.

// Local headers
#include "vUtilities/integerValidator.h"

//==========================================================================
// Class:			IntegerValidator
// Function:		IntegerValidator
//
// Description:		Constructor for the IntegerValidator class.
//
// Input Arguments:
//		_valPtr			= int* pointer to the data we represent
//		_numberClass	= const NumberClass&, specifying additional limits to enforce
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IntegerValidator::IntegerValidator(int *_valPtr, const NumberClass &_numberClass)
							: wxTextValidator(wxFILTER_NUMERIC)
{
	assert(_numberClass != ClassInclusiveRange && _numberClass != ClassExclusiveRange);

	// Assign to the class members
	if (_valPtr)
		valPtr = _valPtr;
	else
		valPtr = &m_value;
	uValPtr = NULL;
	numberClass = _numberClass;
}

//==========================================================================
// Class:			IntegerValidator
// Function:		IntegerValidator
//
// Description:		Constructor for the IntegerValidator class, for use with
//					enforcing numeric ranges.
//
// Input Arguments:
//		_min			= const int&, minimum acceptable value for the data
//		_max			= const int&, maximum acceptable value for the data
//		_valPtr			= int* pointer to the data we represent
//		_numberClass	= const NumberClass&, specifying whether the range is inclusive or exclusive
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IntegerValidator::IntegerValidator(const int &_min, const int &_max,
								   int *_valPtr, const NumberClass &_numberClass)
{
	assert(_min < _max);
	assert(_numberClass == ClassInclusiveRange || _numberClass == ClassExclusiveRange);

	// Assign to the class members
	if (_valPtr)
		valPtr = _valPtr;
	else
		valPtr = &m_value;
	uValPtr = NULL;
	numberClass = _numberClass;
	min = _min;
	max = _max;
}

//==========================================================================
// Class:			IntegerValidator
// Function:		IntegerValidator
//
// Description:		Constructor for the IntegerValidator class (unsigned).
//
// Input Arguments:
//		_valPtr			= unsigned int* pointer to the data we represent
//		_numberClass	= const NumberClass&, specifying additional limits to enforce
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IntegerValidator::IntegerValidator(unsigned int *_valPtr, const NumberClass &_numberClass)
							: wxTextValidator(wxFILTER_NUMERIC)
{
	assert(_numberClass != ClassInclusiveRange && _numberClass != ClassExclusiveRange);

	// Assign to the class members
	if (_valPtr)
		uValPtr = _valPtr;
	else
		uValPtr = &m_uvalue;
	valPtr = NULL;
	numberClass = _numberClass;
}

//==========================================================================
// Class:			IntegerValidator
// Function:		IntegerValidator
//
// Description:		Constructor for the IntegerValidator class, for use with
//					enforcing numeric ranges (unsigned).
//
// Input Arguments:
//		_min			= const unsigned int&, minimum acceptable value for the data
//		_max			= const unsigned int&, maximum acceptable value for the data
//		_valPtr			= unsigned int* pointer to the data we represent
//		_numberClass	= const NumberClass&, specifying whether the range is inclusive or exclusive
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IntegerValidator::IntegerValidator(const unsigned int &_min, const unsigned int &_max,
								   unsigned int *_valPtr, const NumberClass &_numberClass)
{
	assert(_min < _max);
	assert(_numberClass == ClassInclusiveRange || _numberClass == ClassExclusiveRange);

	// Assign to the class members
	if (_valPtr)
		uValPtr = _valPtr;
	else
		uValPtr = &m_uvalue;
	valPtr = NULL;
	numberClass = _numberClass;
	umin = _min;
	umax = _max;
}

//==========================================================================
// Class:			IntegerValidator
// Function:		IntegerValidator
//
// Description:		Copy constructor for the IntegerValidator class.
//
// Input Arguments:
//		dv	= const IntegerValidator& to copy to this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IntegerValidator::IntegerValidator(const IntegerValidator& dv)
{
	// Make the assignment
	*this = dv;
}

//==========================================================================
// Class:			IntegerValidator
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
bool IntegerValidator::TransferToWindow(void)
{
	if (!CheckValidator())
		return false;
	
	if (valPtr)
	{
		wxString temp;
		temp.Printf("%i", *valPtr);
		static_cast<wxTextCtrl*>(m_validatorWindow)->SetValue(temp);
	}
	else if (uValPtr)
	{
		wxString temp;
		temp.Printf("%u", *uValPtr);
		static_cast<wxTextCtrl*>(m_validatorWindow)->SetValue(temp);
	}
	
	return true;
}

//==========================================================================
// Class:			IntegerValidator
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
bool IntegerValidator::TransferFromWindow(void)
{
	if (!CheckValidator())
		return false;
	
	if (valPtr)
	{
		long value;
		if (!static_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().ToLong(&value))
			return false;
		*valPtr = value;
	}
	else if (uValPtr)
	{
		unsigned long value;
		if (!static_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().ToULong(&value))
			return false;
		*uValPtr = value;
	}
	
	return true;
}

//==========================================================================
// Class:			IntegerValidator
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
bool IntegerValidator::Validate(wxWindow *parent)
{
	if (!CheckValidator())
		return false;
	// FIXME:  This needs work!
	long value;
	long uValue;
	if (!static_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().ToLong(&value))
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
// Class:			IntegerValidator
// Function:		operator=
//
// Description:		Assignment operator overload.
//
// Input Arguments:
//		dv	= const IntegerValidator& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		IntegerValidator& reference to this
//
//==========================================================================
IntegerValidator& IntegerValidator::operator=(const IntegerValidator& dv)
{
	// Check for self assignment
	if (this == &dv)
		return *this;
	
	// Make the assignments
	valPtr = dv.valPtr;
	uValPtr = dv.uValPtr;

	numberClass = dv.numberClass;

	min = dv.min;
	max = dv.max;
	umin = dv.umin;
	umax = dv.umax;

	m_value = dv.m_value;
	m_uvalue = dv.m_uvalue;

	if (dv.valPtr == &dv.m_value)
		valPtr = &m_value;
	else
		valPtr = dv.valPtr;

	if (dv.uValPtr == &dv.m_uvalue)
		uValPtr = &m_uvalue;
	else
		uValPtr = dv.uValPtr;
	
	return *this;
}