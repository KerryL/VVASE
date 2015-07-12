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

#ifndef _DATA_VALIDATOR_H_
#define _DATA_VALIDATOR_H_

// wxWidgets headers
#include <wx/valtext.h>

// Local headers
#include "vUtilities/unitConverter.h"

class DataValidator : public wxTextValidator
{
public:
	// Types of numeric validation
	enum NumberClass
	{
		ClassAll,// No restrictions (other than being a number)
		ClassPositive,
		ClassStrictlyPositive,
		ClassNegative,
		ClassStrictlyNegative,
		ClassInclusiveRange,
		ClassExclusiveRange,

		ClassCount
	};

	// Constructors
	DataValidator(const UnitConverter::UnitType &_unit, double *_valPtr = NULL,
		const NumberClass &_numberClass = ClassAll);
	DataValidator(const UnitConverter::UnitType &_unit, const double &_min,
		const double &_max, double *_valPtr = NULL, const NumberClass &_numberClass = ClassInclusiveRange);
	DataValidator(const DataValidator &dv);
	
	// Destructor
	virtual ~DataValidator() {};
	
	// Clone
	virtual DataValidator* Clone(void) const { return new DataValidator(*this); };
	
	// Mandatory overloads for validators
	virtual bool TransferToWindow(void);
	virtual bool TransferFromWindow(void);

	virtual bool Validate(wxWindow *parent);

	// Functions specific to our custom validator
	void SetUnitType(const UnitConverter::UnitType &_unit) { unit = _unit; };

private:
	UnitConverter::UnitType unit;
	
	// Reference to the data we're protecting
	double *valPtr;
	double m_value;

	// Additional limits to place on the value
	NumberClass numberClass;
	double min, max;
	
	// Operators
	DataValidator& operator=(const DataValidator& dv);
};

#endif// _DATA_VALIDATOR_H_