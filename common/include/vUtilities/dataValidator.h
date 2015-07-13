/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataValidator.h
// Created:  12/12/2011
// Author:  K. Loux
// Description:  Class derived from wxTextValidator, used for converting to/from
//				 the various default units in the Convert class.  Also handles
//				 UndoRedoStack entries.
// History:
//	12/12/2011	- Created and debugged, K. Loux.

#ifndef DATA_VALIDATOR_H_
#define DATA_VALIDATOR_H_

// Standard C++ headers
#include <sstream>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/valtext.h>

// Local headers
#include "vUtilities/unitConverter.h"

template<class T>
class DataValidator : public wxTextValidator
{
public:
	enum NumberClass
	{
		ClassAll,// No restrictions (other than being a number)
		ClassPositive,
		ClassStrictlyPositive,
		ClassNegative,
		ClassStrictlyNegative,
		ClassInclusiveRange,
		ClassExclusiveRange,
		ClassMinimumInclusive,
		ClassMinimumExclusive,
		ClassMaximumInclusive,
		ClassMaximumExclusive,

		ClassCount
	};

	// Constructors
	DataValidator(T &data, const UnitConverter::UnitType &unit = UnitConverter::UnitTypeUnitless,
		const NumberClass &numberClass = ClassAll);
	DataValidator(T &data,  const T &min, const T &max,
		const NumberClass &numberClass = ClassInclusiveRange,
		const UnitConverter::UnitType &unit = UnitConverter::UnitTypeUnitless);
	
	// Destructor
	virtual ~DataValidator() {}
	
	// Mandatory overloads for validators
	virtual bool TransferToWindow() { return DoTransferToWindow(); }
	virtual bool TransferFromWindow() { return DoTransferFromWindow(); }
	virtual bool Validate(wxWindow *parent) { return DoValidate(parent); }

	void SetUnitType(const UnitConverter::UnitType& unit) { this->unit = unit; }

private:
	UnitConverter::UnitType unit;
	
	// Reference to the data we're protecting
	T &data;

	// Additional limits to place on the value
	const NumberClass numberClass;
	const T min, max;

	// These are separated from the mandatory overloads above in order to allow specialization
	bool DoTransferToWindow();
	bool DoTransferFromWindow();
	bool DoValidate(wxWindow *parent);
};

typedef DataValidator<double> UnitValidator;
typedef DataValidator<int> IntegerValidator;
typedef DataValidator<unsigned int> UnsignedValidator;

//==========================================================================
// Class:			DataValidator
// Function:		DataValidator
//
// Description:		Constructor for the DataValidator class.
//
// Input Arguments:
//		data		= double& pointer to the data we represent
//		unit		= const UnitConverter::UnitType& specifying the type of data we
//					  represent
//		numberClass	= const NumberClass& specifying additional limits
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template<class T>
DataValidator<T>::DataValidator(T &data, const UnitConverter::UnitType &unit,
	const NumberClass &numberClass) : wxTextValidator(wxFILTER_NUMERIC),
	unit(unit), data(data), numberClass(numberClass), min(min), max(max)
{
	assert(numberClass != ClassInclusiveRange && numberClass != ClassExclusiveRange);
}

//==========================================================================
// Class:			DataValidator
// Function:		DataValidator
//
// Description:		Constructor for the DataValidator class, for use with
//					enforcing numeric ranges.
//
// Input Arguments:
//		data		= double& pointer to the data we represent
//		min			= const double&, minimum acceptable value for the data
//		max			= const double&, maximum acceptable value for the data
//		numberClass	= const NumberClass&, specifying whether the range is inclusive or exclusive
//		unit		= const UnitConverter::UnitType& specifying the type of data we
//					  represent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template<class T>
DataValidator<T>::DataValidator(T &data, const T &min, const T &max,
	const NumberClass &numberClass, const UnitConverter::UnitType &unit)
	: wxTextValidator(wxFILTER_NUMERIC), unit(unit), data(data),
	numberClass(numberClass), min(min), max(max)
{
	assert(min < max || (numberClass != ClassInclusiveRange && numberClass != ClassExclusiveRange));
	assert(numberClass == ClassInclusiveRange || numberClass == ClassExclusiveRange);
}

//==========================================================================
// Class:			DataValidator
// Function:		DoTransferToWindow
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
template<class T>
bool DataValidator<T>::DoTransferToWindow()
{
	wxString value;
	value << data;
	dynamic_cast<wxTextEntry*>(m_validatorWindow)->SetValue(value);
	
	return true;
}

//==========================================================================
// Class:			DataValidator
// Function:		DoTransferFromWindow
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
template<class T>
bool DataValidator<T>::DoTransferFromWindow()
{
	T value;
	std::istringstream ss;

	ss.str(dynamic_cast<wxTextEntry*>(m_validatorWindow)->GetValue().ToUTF8().data());
	if ((ss >> value).fail())
		return false;
	
	return true;
}

//==========================================================================
// Class:			DataValidator
// Function:		DoValidate
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
template<class T>
bool DataValidator<T>::DoValidate(wxWindow * /*parent*/)
	// TODO:  This method should generate message boxes explaining the errors
{
	T value;
	std::istringstream ss;

	ss.str(dynamic_cast<wxTextEntry*>(m_validatorWindow)->GetValue().ToUTF8().data());
	if ((ss >> value).fail())
		return false;

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

	case ClassMinimumInclusive:
		if (value < min)
			return false;
		break;

	case ClassMinimumExclusive:
		if (value <= min)
			return false;
		break;

	case ClassMaximumInclusive:
		if (value > max)
			return false;
		break;

	case ClassMaximumExclusive:
		if (value >= max)
			return false;
		break;
	}

	return true;
}

#endif// DATA_VALIDATOR_H_