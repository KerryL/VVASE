/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integerValidator.h
// Created:  12/13/2011
// Author:  K. Loux
// Description:  Class derived from wxTextValidator, used for enforcing limits on
//				 integer value entries.
// History:
//	12/13/2011	- Created and debugged, K. Loux.

#ifndef _INTEGER_VALIDATOR_H_
#define _INTEGER_VALIDATOR_H_

// wxWidgets headers
#include <wx/valtext.h>

// Local headers
#include "vUtilities/unitConverter.h"

class IntegerValidator : public wxTextValidator
{
public:
	// Types of numeric validation
	enum NumberClass
	{
		ClassAll,// No restrictions (other than being an integer)
		ClassPositive,
		ClassStrictlyPositive,
		ClassNegative,
		ClassStrictlyNegative,
		ClassRangeInclusive,
		ClassRangeExclusive,
		ClassMinimumInclusive,
		ClassMaximumInclusive,
		ClassMinimumExclusive,
		ClassMaximumExclusive,

		ClassCount
	};

	// Constructors
	IntegerValidator(int *_valPtr, const NumberClass &_numberClass = ClassAll);
	IntegerValidator(const int &_min, const int &_max, int *_valPtr,
		const NumberClass &_numberClass = ClassRangeInclusive);
	IntegerValidator(const int &_limit, int *_valPtr,
		const NumberClass &_numberClass = ClassMinimumInclusive);
	IntegerValidator(unsigned int *_valPtr, const NumberClass &_numberClass = ClassAll);
	IntegerValidator(const unsigned int &_min, const unsigned int &_max,
		unsigned int *_valPtr, const NumberClass &_numberClass = ClassRangeInclusive);
	IntegerValidator(const unsigned int &_limit, int unsigned *_valPtr,
		const NumberClass &_numberClass = ClassMinimumInclusive);
	IntegerValidator(const IntegerValidator &dv);
	
	// Destructor
	virtual ~IntegerValidator() {};
	
	// Clone
	virtual IntegerValidator* Clone(void) const { return new IntegerValidator(*this); };
	
	// Mandatory overloads for validators
	virtual bool TransferToWindow(void);
	virtual bool TransferFromWindow(void);

	virtual bool Validate(wxWindow *parent);

private:
	// Reference to the data we're protecting
	int *valPtr;
	int m_value;

	// We handle the signed and unsigned variety
	unsigned int *uValPtr;
	unsigned int m_uvalue;

	// Additional limits to place on the value
	NumberClass numberClass;
	int min, max, limit;
	unsigned int umin, umax, ulimit;
	
	// Operators
	IntegerValidator& operator=(const IntegerValidator& dv);
};

#endif// _INTEGER_VALIDATOR_H_
