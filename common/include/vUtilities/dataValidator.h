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
#include "vUtilities/convert.h"

class DataValidator : public wxTextValidator
{
public:
	// Constructors
	DataValidator(const Convert::UnitType &_unit, double *_valPtr = NULL);
	DataValidator(const DataValidator &dv);
	
	// Destructor
	virtual ~DataValidator() {};
	
	// Clone
	virtual DataValidator* Clone(void);
	
	// Mandatory overloads for validators
	virtual bool TransferToWindow(void);
	virtual bool TransferFromWindow(void);

private:
	// The type of data we're validating
	Convert::UnitType unit;
	
	// Reference to the data we're protecting
	double *valPtr;
	
	// Operators
	DataValidator& operator=(const DataValidator& dv);
};

#endif// _DATA_VALIDATOR_H_